#include "Slave.h"

#include <stdio.h>
#include "SlaveCard.h"
#include "resource.h"
#include "SlaveWnd.h"
#include "SlaveComm.h"
#include "SlaveAI.h"
#include "SlvPing.h"
#include "PlayerDetailWnd.h"

extern C_SlaveApp SlaveApp;

C_Slave::C_Slave()
{
	m_nGameMode = IDLE_GAMEMODE;
    m_nPlayMode = IDLE_PLAYMODE;
 
	// default game setting
	m_nRunningPlayerID = MYID;

	int i;
	// default no of player
	m_GameSetting.nNoOfPlayer = 4;
	m_GameSetting.bTongBeat2 = TRUE;
	for ( i=0; i<6 ; i++ )
	{
		m_GameSetting.nPlayerID[i] = NO_PLAYER;
		m_IngameTable.nPlayerID[i] = NO_PLAYER;
	}
	m_nGameNum = 0;
	m_nIngameOffset = 0;
	m_IngameTable.nDiscardDirection = NORMAL_DIRECTION;
	m_IngameTable.nDiscardType = NONE_CARD;
	m_IngameTable.nLatestPlayerID = NO_PLAYER;
	for ( i=0; i<4 ; i++ )
		m_IngameTable.LatestCard.nCard[i] = 0;

	for ( i=0 ; i<6 ; i++ )
		m_FinishTable.nPlayerID[i] = 0;

	// create UI Object
    m_pSlaveWnd = new C_SlaveWnd(this);

    // create communication object
	m_pComm = new C_SlaveComm(this);
   	RECT rc = {500,405,532,437};
    m_pComm->Create("",0,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
    					rc,m_pSlaveWnd,NULL);
	m_pComm->CreateMsgWnd();

	m_pPing = new C_SlvPing(this);
}

C_Slave::~C_Slave()
{
	GetPlayerList()->DeleteObjects();
	delete m_pSlaveWnd;
    delete m_pComm;
    delete m_pPing;
}

int C_Slave::Initialize()
{
    // create last player
    SLAVEPLAYER SlavePlayer;

	UINT nLastPlayer;
	UINT nGamePlay,nKing,nSlave;

	char szName[32];
	memset(szName,0x00,sizeof(szName));

	// get last player from registry 
	SlaveApp.OpenRegistry();
	nLastPlayer = SlaveApp.GetPlayerSlot();
	SlavePlayer.nPlayerStatus = IAM_SPECTATOR;
    SlavePlayer.nCharacterID = (int)SlaveApp.GetCharID(nLastPlayer);
	SlaveApp.GetName(nLastPlayer,SlavePlayer.szPlayerName,sizeof(SlavePlayer.szPlayerName));
	SlaveApp.GetStat(nLastPlayer,nGamePlay,nKing,nSlave);
	SlavePlayer.nNoOfGamePlay = nGamePlay;
	SlavePlayer.nNoOfKing = nKing;
	SlavePlayer.nNoOfSlave = nSlave;

	if ( SlaveApp.GetUseNetLog() )
		m_pComm->CreateLogMessage();
	
	SlaveApp.CloseRegistry();

	AddMyPlayer(&SlavePlayer);
	// add AI Object to my player , for temporarily use 
	GetMyPlayer()->SetSlaveAI(new C_SlaveAI(GetMyPlayer()));

	// main communicating object bind to MyPlayer
	m_pComm->SetPlayer(GetMyPlayer());

    // update UI object only when finish initializing
	ShowWindow(m_pSlaveWnd->GetHandle(),SW_SHOW);
	UpdateWindow(m_pSlaveWnd->GetHandle());
	return TRUE;
}

int C_Slave::StartServer()
{
	m_nGameMode = SERVER_GAMEMODE;
	// reset player running id
	m_nRunningPlayerID = GetMyPlayer()->GetPlayerID();


	SlaveApp.OpenRegistry();
	// get port to activate
    DWORD dwPort;
	dwPort = (DWORD)SlaveApp.GetPort();
	SlaveApp.CloseRegistry();

    return m_pComm->CreateServerSocket((unsigned short)dwPort);
}

int C_Slave::StartClient(char * lpszJoinIP)
{
	m_nGameMode = CLIENT_GAMEMODE;
	// reset player running id
	m_nRunningPlayerID = GetMyPlayer()->GetPlayerID();

	// get Port for connecting
    DWORD dwPort;

	SlaveApp.OpenRegistry();
	dwPort = (DWORD)SlaveApp.GetPort();
	SlaveApp.CloseRegistry();

	if ( m_pComm->CreateClientSocket((unsigned short)dwPort,lpszJoinIP) )
		return TRUE;
	else
	{
		m_nGameMode = IDLE_GAMEMODE;
		return FALSE;
	}
}

int C_Slave::QuitGame()
{
	LockWindowUpdate(m_pSlaveWnd->GetHandle());

	m_nGameMode = IDLE_GAMEMODE;
	m_pComm->Close();
	m_pSlaveWnd->ShowIdleGame();
	SetPlayMode(IDLE_PLAYMODE);

	C_Player *pPlayer;
	int i;

	// remove all other player
	for ( i=0 ; i<GetPlayerList()->Count() ; i++ )
	{
		pPlayer = GetPlayerList()->Items(i);
		if ( GetMyPlayer()->GetPlayerID() != pPlayer->GetPlayerID() )
		{
			RemovePlayer(pPlayer->GetPlayerID());
			i--;
		}
		else
		{
			if ( pPlayer->GetStatus() == IAM_PLAYER )
			{
				m_pSlaveWnd->ClearPlayerUI(pPlayer->GetTableOrder());
                pPlayer->SetTableOrder(NOT_IN_TABLE);
				pPlayer->SetStatus(IAM_SPECTATOR);
			}
		}
	}

	// clear player setting
	for ( i=0 ; i<6 ; i++ )
    {
		m_GameSetting.nPlayerID[i] = NO_PLAYER;
        m_IngameTable.nPlayerID[i] = NO_PLAYER;
    }

    m_IngameTable.nLatestPlayerID = 0;
	m_IngameTable.nDiscardType = 0;
	for ( i=0 ; i<4 ; i++ )
		m_IngameTable.LatestCard.nCard[i] = 0;

	// v.0.92 clear player data
	GetMyPlayer()->GetCardList()->DeleteObjects();
	// v.1.00 
	GetMyPlayer()->SetPosition(IAM_PEOPLE);

	LockWindowUpdate(NULL);
	return 0;
}

int C_Slave::ResetGame()
{
	LockWindowUpdate(m_pSlaveWnd->GetHandle());
	// resetting game
	SetPlayMode(SETTING_PLAYMODE);

	C_Player *pPlayer;
	int i;

	// send set game message to all client
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
		// checking client list and player list, remove unknown player
		for ( i=0 ; i < GetPlayerList()->Count() ; i++ )
		{
			C_Player * pPlayer = GetPlayerList()->Items(i);
			if ( pPlayer->GetPlayerID() == GetMyPlayer()->GetPlayerID() )
				continue;
			if ( pPlayer->GetStatus() == IAM_COMPUTER )
				continue;
			int j;
			C_Player * pLostPlayer;
			for ( j=0; j < m_pComm->GetClientList()->Count() ; j++ )
			{
				pLostPlayer = ((C_SlaveComm *)m_pComm->GetClientList()->Items(j))->GetPlayer();
				if ( pLostPlayer->GetPlayerID() == pPlayer->GetPlayerID() )
					break;
			}
			if ( j == m_pComm->GetClientList()->Count() )
			{
				// player not found
				RemovePlayer(pLostPlayer->GetPlayerID());
			}
		}

    	// send msg to other players
        for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
            pSlaveComm->SendResetGame();
        }
    }

	// remove computer player , set player back to spectator

	// clear table
	for ( i=0 ; i<6 ; i++ )
	{
		int pid = GetGameSetting()->nPlayerID[i];
		if ( pid == NO_PLAYER ) continue;
		pPlayer = GetPlayer(pid);
        if ( pPlayer == NULL )
        {
        	m_pComm->LogMessage("#Err01\n");
            continue;
        }
        int nPos = pPlayer->GetTableOrder();
		switch ( pPlayer->GetStatus() )
		{
		case IAM_COMPUTER:
//			TRACE("clear UI %s %d,%d\n",pPlayer->GetName(),j,pPlayer->GetPlayerID());
            // change setting to empty
           	m_GameSetting.nPlayerID[i] = NO_PLAYER;
            m_IngameTable.nPlayerID[pPlayer->GetTableOrder()] = NO_PLAYER;
			m_pSlaveWnd->ClearPlayerUI(nPos);
			break;
		case IAM_PLAYER:
//			TRACE("clear UI %s %d,%d\n",pPlayer->GetName(),j,pPlayer->GetPlayerID());
			pPlayer->SetStatus(IAM_SPECTATOR);
			m_pSlaveWnd->ClearPlayerUI(nPos);
			break;
		}
		pPlayer->SetTableOrder(NOT_IN_TABLE);
		pPlayer->SetPosition(IAM_PEOPLE);
	}

	// remove computer player
	for ( i=0 ; i<GetPlayerList()->Count() ; i++ )
	{
		pPlayer = GetPlayerList()->Items(i);
		if ( pPlayer->GetStatus() == IAM_COMPUTER )
		{
//			TRACE("del %s %d,%d\n",pPlayer->GetName(),i,pPlayer->GetPlayerID());
			pPlayer->SetUI(NULL);
			delete pPlayer->GetSlaveAI();
			pPlayer->SetSlaveAI(NULL);
			delete pPlayer;
			GetPlayerList()->Remove(i);
			i--;
			continue;
		}
	}

	// clear table setting
	for ( i=0 ; i<6 ; i++ )
    {
        m_IngameTable.nPlayerID[i] = NO_PLAYER;
    }
//	m_IngameTable.nNoOfPlayer = 0;
	m_IngameTable.nDiscardType = NONE_CARD;
	m_IngameTable.nLatestPlayerID = 0;
	for ( i=0 ; i<4 ; i++ )
		m_IngameTable.LatestCard.nCard[i] = 0;

	GetUI()->ShowSettingGame(&m_GameSetting);

	LockWindowUpdate(NULL);
	return TRUE;
}

int C_Slave::StartGame()
{
	LockWindowUpdate(m_pSlaveWnd->GetHandle());

	// enter ingame playmode
	SetPlayMode(INGAME_PLAYMODE);

	int i;

	// send start game message to all client
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
		// generate computer player at empty slot
		SLAVEPLAYER AIPlayer;

		SlaveApp.OpenRegistry();
		int nComputerID = RANDOM(6)+1;
		for ( i=0 ; i<m_GameSetting.nNoOfPlayer ; i++ )
		{
			// 0 specify that is empty and ready to add ai player
			if ( m_GameSetting.nPlayerID[i] == NO_PLAYER )
			{
				nComputerID++; if ( nComputerID > 6 ) nComputerID = 1;
				AIPlayer.nPlayerStatus = IAM_COMPUTER;
				SlaveApp.GetOpponent(nComputerID,AIPlayer.szPlayerName,sizeof(AIPlayer.szPlayerName));
				// AI player res id is always start with 128;
				AIPlayer.nCharacterID = 127+nComputerID;
				UINT g,k,s;
				SlaveApp.GetOpponentStat(nComputerID,g,k,s);
				AIPlayer.nNoOfSlave = s;
				AIPlayer.nNoOfKing = k;
				AIPlayer.nNoOfGamePlay = g;
				C_Player * pNewPlayer = AddNewPlayer(&AIPlayer);
				m_GameSetting.nPlayerID[i] = pNewPlayer->GetPlayerID();
				pNewPlayer->SetSlaveAI( new C_SlaveAI(pNewPlayer) );
			}
		}
		SlaveApp.CloseRegistry();

		// send msg to other players
        for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
			pSlaveComm->SendStartGame(&m_GameSetting);
        }
    }

	// clear player data
	C_Player * pPlayer;
	for ( i=0 ; i<GetPlayerList()->Count() ; i++ )
	{
		pPlayer = GetPlayerList()->Items(i);
		pPlayer->SetNoOfBeenKing(0);
		pPlayer->SetNoOfBeenQueen(0);
	}

	// set player order , and start game
	m_nGameNum = 0;
    int nFirstOrder = 0;
	m_nActivePlayerOrder = 0;

	for ( i=0 ; i<6 ; i++ )
	{
		if ( m_GameSetting.nPlayerID[i] == GetMyPlayer()->GetPlayerID() )
		{
        	nFirstOrder = i;
			break;
		}
	}
    m_nActivePlayerOrder -= nFirstOrder;
	m_nIngameOffset = nFirstOrder;
    if ( m_nActivePlayerOrder<0 ) m_nActivePlayerOrder+=6;

    m_IngameTable.nNoOfPlayer = m_GameSetting.nNoOfPlayer;
    m_IngameTable.nDiscardType = NONE_CARD;
    m_IngameTable.nLatestPlayerID = NO_PLAYER;
    for ( i=0 ; i<4 ; i++ )
        m_IngameTable.LatestCard.nCard[i] = 0;
    for ( i=0 ; i<6 ; i++ )
    {
		C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[nFirstOrder]);
        m_IngameTable.nPlayerID[i] = m_GameSetting.nPlayerID[nFirstOrder];
        nFirstOrder++ ; if ( nFirstOrder == 6 ) nFirstOrder = 0;
		if ( pPlayer == NULL )
        {
        	m_pComm->LogMessage("#Err02\n");
	        m_IngameTable.nPlayerID[i] = NO_PLAYER;
            continue;
        }
	    pPlayer->SetTableOrder(i);
    }

	m_pSlaveWnd->ShowStartGame();

    // server continue start game, client just wait for start round
    if ( m_nGameMode == SERVER_GAMEMODE )
		StartRound();

	LockWindowUpdate(NULL);
	return TRUE;
}

// StartRound ::: start from distribute card
int C_Slave::StartRound()
{
	int i;
	// send gamedata round message to all client
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// clear cards
        for ( i=0 ; i<GetPlayerList()->Count() ; i++ )
        {
            C_Player * pPlayer = GetPlayerList()->Items(i);
			pPlayer->GetCardList()->DeleteObjects();
        }

		// checking still have 'nNoOfPlayer' players in game
		for ( i=0 ; i<m_GameSetting.nNoOfPlayer ; i++ )
		{
			C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
			if ( pPlayer == NULL )
			{
	        	m_pComm->LogMessage("#Err03\n");
				// don't continue.
				return FALSE;
			}
		}

		// random distribute card to all players
		// m_IngameTable.nDiscardDirection and
		// m_nActivePlayerOrder is set here
		if ( !DistributeCard() )
        {
        	m_pComm->LogMessage("#Err07\n");
            return FALSE;
        }
    	// send msg to other players
        for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
			pSlaveComm->SendStartRound();
        }
	}

	if ( SlaveApp.IsUseSound() )
		PlaySound(MAKEINTRESOURCE(IDR_SND_SHUFFLE),
				C_App::m_hInstance,
				SND_RESOURCE|SND_ASYNC);

	// create card if my player is being player
	if ( GetMyPlayer()->GetStatus() == IAM_PLAYER )
	{
		GetUI()->SetCardOnHand();
	}

	// clear finish table
	for ( i=0 ; i<6 ; i++ )
		GetFinishTable()->nPlayerID[i] = 0;

	m_nGameNum++;

	if ( m_nGameNum > 1 )
	{
		// next game require exchange card
		if ( GetGameSetting()->nNoOfPlayer > 3 )
			m_nPlayMode = EXCHANGE_PLAYMODE2;
		else
			m_nPlayMode = EXCHANGE_PLAYMODE1;

		// clear player's discard status
		for ( i=0 ; i<m_GameSetting.nNoOfPlayer ; i++ )
		{
			C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
			if ( pPlayer != NULL )
				pPlayer->SetDiscardStatus(WAITFORDISCARD);
		}

		// update player on screen
		for ( i=0 ; i<6 ; i++ )
		{
//			m_pSlaveWnd->RefreshPlayersTable(i);
			GetUI()->GetPlayerDetailWnd(i)->Refresh(REFRESH_CARDLIST);
			GetUI()->GetPlayerDetailWnd(i)->Refresh(REFRESH_POSITION);
			GetUI()->GetPlayerDetailWnd(i)->Refresh(REFRESH_DISCARDSTATUS);
			GetUI()->GetPlayerDetailWnd(i)->Refresh(REFRESH_HISTORY);
		}

		GetUI()->RequestExchange();
	}
	else
		StartNewTurn();

	return TRUE;
}

// StartNewTurn :: start from first player discard
int C_Slave::StartNewTurn()
{
	int i;

	// set table data , when start
	m_IngameTable.nLatestPlayerID = NO_PLAYER;
	for ( i=0 ; i<4 ; i++ )
		m_IngameTable.LatestCard.nCard[i] = 0;
	m_IngameTable.nDiscardType = NONE_CARD;

	// clear player's discard status
	for ( i=0 ; i<m_GameSetting.nNoOfPlayer ; i++ )
	{
		C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
		if ( pPlayer != NULL )
		{
			if ( pPlayer->GetCardList()->Count() > 0 )
			{
				if ( GetActivePlayerID() == pPlayer->GetPlayerID() )
					pPlayer->SetDiscardStatus(DISCARDING);
				else
					pPlayer->SetDiscardStatus(WAITFORDISCARD);
			}
		}
	}

	// update player on screen
	for ( i=0 ; i<6 ; i++ )
	{
//		m_pSlaveWnd->RefreshPlayersTable(i);
		GetUI()->GetPlayerDetailWnd(i)->Refresh(REFRESH_CARDLIST);
		GetUI()->GetPlayerDetailWnd(i)->Refresh(REFRESH_DISCARDSTATUS);
	}
	m_pSlaveWnd->RefreshDiscardDirection();

	m_pSlaveWnd->RequestDiscard();
	return TRUE;
}

int C_Slave::StartTurn()
{
	int i;
	for ( i=0 ; i<m_GameSetting.nNoOfPlayer ; i++ )
	{
		C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
		if ( pPlayer != NULL )
		{
			if ( GetActivePlayerID() == pPlayer->GetPlayerID() )
				pPlayer->SetDiscardStatus(DISCARDING);
//			GetUI()->RefreshPlayersTable(pPlayer->GetTableOrder());
			GetUI()->GetPlayerDetailWnd(pPlayer->GetTableOrder())->Refresh(REFRESH_DISCARDSTATUS);
		}
	}
	m_pSlaveWnd->RequestDiscard();
	return 0;
}

int C_Slave::NextTurn()
{
	// if somebody left , can't continue
	int i;
	for ( i=0 ; i<GetGameSetting()->nNoOfPlayer ; i++ )
	{
		if ( GetGameSetting()->nPlayerID[i] == NO_PLAYER )
		{
			TRACE("#Err47\n");
			m_pComm->PopupMessage("/c1Somebody left. Try to restart");
			return FALSE;
		}
	}

	// if nobody can discard , turn 'll over
	if ( SetNextDiscarder() )
    {
		// do start next turn
    	StartTurn();
    }
    else
    {
        PickUpCard();
		// endturn will be called after UI's pickupcard
    }
	return TRUE;
}

int C_Slave::SetNextDiscarder()
{
	// check over turn
	int nCountWait=0;
	int nCountLast=0;
	int nCountPass=0;
	int nCountOver=0;

	int i;
	for ( i=0 ; i<GetGameSetting()->nNoOfPlayer ; i++ )
	{
		C_Player * p = GetPlayer(GetGameSetting()->nPlayerID[i]);
		if ( p != NULL )
		{
			switch ( p->GetDiscardStatus() )
			{
			case WAITFORDISCARD:
				nCountWait++;
				break;
			case LAST_DISCARD:
				nCountLast++;
				break;
			case PASS_TURN:
				nCountPass++;
				break;
			case CARD_OVER:
				nCountOver++;
				break;
			}
		}
	}

    int nLoop = 0;

	if ( 1 + nCountOver == GetGameSetting()->nNoOfPlayer )
	{
		// game is over
		return FALSE;
	}
	else if ( nCountWait == 0 && nCountLast == 1 )
	{
        nLoop = 0;
		// turn is over
		do
		{
			if ( m_IngameTable.nDiscardDirection == NORMAL_DIRECTION )
			{
	    		m_nActivePlayerOrder++;
				if ( m_nActivePlayerOrder > 5 ) m_nActivePlayerOrder = 0;
			}
			else
			{
				m_nActivePlayerOrder--;
				if ( m_nActivePlayerOrder < 0 ) m_nActivePlayerOrder = 5;
			}
			C_Player * p = GetPlayer(GetActivePlayerID());
			if ( p != NULL )
			{
				if ( p->GetDiscardStatus() == LAST_DISCARD )
					return FALSE;
			}
            // prevent infinite loop
            nLoop++;
            if ( nLoop > 64 ) return FALSE;
		} while ( 1 );
	}
	else if ( nCountWait == 0 && nCountLast == 0 )
	{
		for ( i = 0 ; i<6 ; i++ )
		{
			if ( GetIngameTable()->nLatestPlayerID == GetIngameTable()->nPlayerID[i] )
			{
				m_nActivePlayerOrder = i;
				break;
			}
		}

		// turn is over and need to reverse
		if ( GetIngameTable()->nDiscardDirection == NORMAL_DIRECTION )
			GetIngameTable()->nDiscardDirection = REVERSE_DIRECTION;
		else
			GetIngameTable()->nDiscardDirection = NORMAL_DIRECTION;
		GetUI()->RefreshDiscardDirection();

        nLoop = 0;
		do
		{
			if ( m_IngameTable.nDiscardDirection == NORMAL_DIRECTION )
			{
	    		m_nActivePlayerOrder++;
				if ( m_nActivePlayerOrder > 5 ) m_nActivePlayerOrder = 0;
			}
			else
			{
				m_nActivePlayerOrder--;
				if ( m_nActivePlayerOrder < 0 ) m_nActivePlayerOrder = 5;
			}
			C_Player * p = GetPlayer(GetActivePlayerID());
			if ( p != NULL )
			{
				if ( p->GetDiscardStatus() == PASS_TURN )
					return FALSE;
			}
            // prevent infinite loop
            nLoop++;
            if ( nLoop > 64 ) return FALSE;
		} while ( 1 );
	}

    nLoop = 0;
	do
	{
		if ( m_IngameTable.nDiscardDirection == NORMAL_DIRECTION )
		{
	    	m_nActivePlayerOrder++;
			if ( m_nActivePlayerOrder > 5 ) m_nActivePlayerOrder = 0;
		}
		else
		{
			m_nActivePlayerOrder--;
			if ( m_nActivePlayerOrder < 0 ) m_nActivePlayerOrder = 5;
		}
		C_Player * p = GetPlayer(GetActivePlayerID());
		if ( p != NULL )
		{
			if ( p->GetDiscardStatus() == WAITFORDISCARD )
				return TRUE;
		}
        // prevent infinite loop
        nLoop++;
        if ( nLoop > 64 ) return FALSE;
	} while ( 1 );

//	return TRUE;
}

int C_Slave::EndTurn()
{
	int i;

    for ( i=0 ; i<GetGameSetting()->nNoOfPlayer ; i++ )
    {
        if ( GetGameSetting()->nPlayerID[i] == NO_PLAYER )
        {
	    	// if this player has quited, just wait
		    m_pComm->LogMessage("#Err47\n");
			m_pComm->PopupMessage("/c1Somebody left. Try to restart");
        	return FALSE; // player may leave , so check it
        }
    }

	// send message to all client here
    for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
    {
        C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
		pSlaveComm->SendEndTurn();
    }

	// check over turn
	int nCountWait=0;
	int nCountLast=0;
	int nCountPass=0;
	int nCountOver=0;

	for ( i=0 ; i<GetGameSetting()->nNoOfPlayer ; i++ )
	{
		C_Player * p = GetPlayer(GetGameSetting()->nPlayerID[i]);
		if ( p != NULL )
		{
			switch ( p->GetDiscardStatus() )
			{
			case WAITFORDISCARD:
				nCountWait++;
				break;
			case LAST_DISCARD:
				nCountLast++;
				break;
			case PASS_TURN:
				nCountPass++;
				break;
			case CARD_OVER:
				nCountOver++;
				break;
			}
		}
	}

	if ( 1 + nCountOver == GetGameSetting()->nNoOfPlayer )
	{
		// game is over
		for ( i=0 ; i<GetGameSetting()->nNoOfPlayer ; i++ )
		{
			C_Player * p = GetPlayer(GetGameSetting()->nPlayerID[i]);
			if ( p->GetDiscardStatus() != CARD_OVER )
			{
				PutFinishPlayer(p->GetPlayerID(),NORMAL_ORDER);
				break;
			}
		}
		GetUI()->ShowGameResult();
		// server continue , client wait for endround msg
		if ( GetGameMode() == SERVER_GAMEMODE )
			EndRound();
		return TRUE;
	}

	StartNewTurn();
	return TRUE;
}

int C_Slave::EndRound()
{
	int i;
	// send message to all client here
    for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
    {
        C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
		pSlaveComm->SendEndRound();
    }

	// update player position
	int nStatus[6];
	switch ( GetGameSetting()->nNoOfPlayer )
	{
	case 3:
		nStatus[0] = IAM_KING;
		nStatus[1] = IAM_PEOPLE;
		nStatus[2] = IAM_SLAVE;
		nStatus[3] = 0;nStatus[4] = 0;nStatus[5] = 0;
		break;
	case 4:
		nStatus[0] = IAM_KING;
		nStatus[1] = IAM_QUEEN;
		nStatus[2] = IAM_VICESLAVE;
		nStatus[3] = IAM_SLAVE;
		nStatus[4] = 0;nStatus[5] = 0;
		break;
	case 5:
		nStatus[0] = IAM_KING;
		nStatus[1] = IAM_QUEEN;
		nStatus[2] = IAM_PEOPLE;
		nStatus[3] = IAM_VICESLAVE;
		nStatus[4] = IAM_SLAVE;
		nStatus[5] = 0;
		break;
	case 6:
		nStatus[0] = IAM_KING;
		nStatus[1] = IAM_QUEEN;
		nStatus[2] = IAM_PEOPLE;
		nStatus[3] = IAM_PEOPLE;
		nStatus[4] = IAM_VICESLAVE;
		nStatus[5] = IAM_SLAVE;
		break;
	}

	C_Player * pPlayer;
	for ( i=0 ; i < GetGameSetting()->nNoOfPlayer ; i++ )
	{
		pPlayer = GetPlayer(GetFinishTable()->nPlayerID[i]);
		if ( pPlayer != NULL )
		{
			// update position and score
			pPlayer->SetNoOfGamePlay(pPlayer->GetNoOfGamePlay()+1);
			switch ( nStatus[i] )
			{
			case IAM_KING:
				pPlayer->SetNoOfKing(pPlayer->GetNoOfKing()+1);
				if ( pPlayer->GetPosition() == IAM_KING )
					pPlayer->SetNoOfBeenKing(pPlayer->GetNoOfBeenKing()+1);
				else
					pPlayer->SetNoOfBeenKing(1);
				pPlayer->SetNoOfBeenQueen(0);
				break;
			case IAM_QUEEN:
				if ( pPlayer->GetPosition() == IAM_QUEEN )
					pPlayer->SetNoOfBeenQueen(pPlayer->GetNoOfBeenQueen()+1);
				else
					pPlayer->SetNoOfBeenQueen(1);
				pPlayer->SetNoOfBeenKing(0);
				break;
			case IAM_PEOPLE: case IAM_VICESLAVE:
				pPlayer->SetNoOfBeenKing(0);
				pPlayer->SetNoOfBeenQueen(0);
				break;
			case IAM_SLAVE:
				pPlayer->SetNoOfBeenKing(0);
				pPlayer->SetNoOfBeenQueen(0);
				pPlayer->SetNoOfSlave(pPlayer->GetNoOfSlave()+1);
				break;
			}
			pPlayer->SetPosition(nStatus[i]);
			// update player screen
			if ( pPlayer->GetTableOrder() != NOT_IN_TABLE )
				GetUI()->RefreshPlayersTable(pPlayer->GetTableOrder());
		}
	}

	// update my player data to registry
	SlaveApp.OpenRegistry();
	SlaveApp.SetStat(SlaveApp.GetPlayerSlot(),
						GetMyPlayer()->GetNoOfGamePlay(),
						GetMyPlayer()->GetNoOfKing(),
						GetMyPlayer()->GetNoOfSlave());
	// update computer player data
	if ( m_nGameMode == SERVER_GAMEMODE )
	{
		for ( int i=0 ; i<m_GameSetting.nNoOfPlayer ; i++ )
		{
			C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
			if ( pPlayer != NULL )
			{
				if ( pPlayer->GetStatus() == IAM_COMPUTER )
					SlaveApp.SetOpponentStat(pPlayer->GetCharacterID()-127,
						pPlayer->GetNoOfGamePlay(),
						pPlayer->GetNoOfKing(),
						pPlayer->GetNoOfSlave());
			}
		}
	}
	SlaveApp.CloseRegistry();

	m_nGameNum++;

	// server continue , client wait for startround msg
	if ( GetGameMode() == SERVER_GAMEMODE )
		StartRound();

	return TRUE;
}

int C_Slave::PickUpCard()
{
	// UI->pickupcard will break slave engine for awhile until picking finish
	m_pSlaveWnd->PickUpCard();

	// set table data , when start
	m_IngameTable.nLatestPlayerID = NO_PLAYER;
	for ( int i=0 ; i<4 ; i++ )
		m_IngameTable.LatestCard.nCard[i] = 0;
	m_IngameTable.nDiscardType = NONE_CARD;

	return TRUE;
}

// ==============================================
//  remark : Player ID of my player  ** alway
//			 start from MYID , MYID 'll be lowest possible ID
//           if server, other client start id from MYID+1
//			 if client, my id upon what server give
// ==============================================
int C_Slave::AddMyPlayer(LPSLAVEPLAYER PlayerInfo)
{
    m_pMyPlayer = new C_Player(this,MYID,PlayerInfo);
	GetPlayerList()->Add(m_pMyPlayer);

    // create UI of my player
    m_pSlaveWnd->AddSpectatorUI(m_pMyPlayer);
	return 0;
}


// ==============================================
//  parameter : nPlayerID default is -1
//  return : Player ID of this player
// ==============================================
C_Player * C_Slave::AddNewPlayer(LPSLAVEPLAYER PlayerInfo,int nPlayerID)
{
	m_nRunningPlayerID++;

    C_Player * pPlayer;
    if ( nPlayerID < 0 )
    	// server manipulate running player id
	    pPlayer = new C_Player(this,m_nRunningPlayerID,PlayerInfo);
    else
    	// create player with player ID
	    pPlayer = new C_Player(this,nPlayerID,PlayerInfo);
	GetPlayerList()->Add(pPlayer);

    // create UI of player
	if ( PlayerInfo->nPlayerStatus != IAM_COMPUTER )
	{
	    LockWindowUpdate(m_pSlaveWnd->GetHandle());
		m_pSlaveWnd->AddSpectatorUI(pPlayer);
		LockWindowUpdate(NULL);
	}

	// dispatch message to other client
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// send msg to every other player , except owner
        for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
        	// if server do , send to all client
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
        	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
            {
	    		pSlaveComm->SendAddPlayer(pPlayer);
            }
            else
            {
				// SlaveComm::SetPlayer is here , so every previous event 
				// that can be occur must check NULL , v.0.91 fix

				// if new player log on ,SlaveComm.m_pPlayer is still NULL, so set it also
				if ( pSlaveComm->GetPlayer() == NULL )
				{
					pSlaveComm->SetPlayer(pPlayer);
				}
            	// if others do , send to all other client , except owner
				if ( pSlaveComm->GetPlayer()->GetPlayerID() != pPlayer->GetPlayerID() )
		    		pSlaveComm->SendAddPlayer(pPlayer);
            }
        }
    }
	return pPlayer;
}

// ===============================================
//  return : TRUE - Remove successful
//           FALSE - Remove fail
// ===============================================
int C_Slave::RemovePlayer(int nPlayerID)
{
	for ( int i = 0 ; i<GetPlayerList()->Count() ; i++ )
	{
		if ( GetPlayerList()->Items(i)->GetPlayerID() == nPlayerID )
		{
        	// remove process
			C_Player * p = GetPlayerList()->Items(i);
			// remove game setting flag if he is player
            for ( int k=0 ; k<6 ; k++ )
                if ( m_GameSetting.nPlayerID[k] == p->GetPlayerID() )
                {
					// clear table
					int nTableOrder = p->GetTableOrder();
                    m_GameSetting.nPlayerID[k] = NO_PLAYER;
					if ( nTableOrder >= 0 )
						m_IngameTable.nPlayerID[nTableOrder] = NO_PLAYER;
                    break;
                }

		    LockWindowUpdate(m_pSlaveWnd->GetHandle());
			GetUI()->RemoveSpectatorUI(p);
			delete p;
			GetPlayerList()->Remove(i);

		    LockWindowUpdate(NULL);

			// dispatch message to other client
			if ( m_nGameMode == SERVER_GAMEMODE )
			{
    			// send msg to every other player , except owner
				for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
				{
        			// if server do , send to all client
					C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
        			if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
					{
	    				pSlaveComm->SendRemovePlayer(nPlayerID);
					}
					else
					{
            			// if others do , send to all other client , except owner
						if ( pSlaveComm->GetPlayer()->GetPlayerID() != nPlayerID )
		    				pSlaveComm->SendRemovePlayer(nPlayerID);
					}
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

int C_Slave::UpdateSetting(LPSTART_GAME_SETTING pGameSetting)
{
	for ( int i=0 ; i<6 ; i++ )
		m_GameSetting.nPlayerID[i] = pGameSetting->nPlayerID[i];
	m_GameSetting.nNoOfPlayer = pGameSetting->nNoOfPlayer;
	m_GameSetting.bTongBeat2 = pGameSetting->bTongBeat2;
	m_GameSetting.bCheckSuit = pGameSetting->bCheckSuit;

	if ( m_nGameMode ==	SERVER_GAMEMODE )
	{
		// server only can make change setting
		// just send msg to every other clientr
		for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
		{
			TRACE("send setting to client\n");
			C_SlaveComm * pSlaveComm = (C_SlaveComm*)m_pComm->GetClientList()->Items(i);
    		pSlaveComm->SendSetGame(&m_GameSetting);
		}
	}
	else if ( m_nGameMode == CLIENT_GAMEMODE )
	{
		// client get setting from server and just do it.
		GetUI()->ShowSettingGame(pGameSetting);
	}
	return 0;
}

void C_Slave::SetPlayMode(int nMode)
{
	if ( nMode == m_nPlayMode ) return;
	int nLastPlayMode = m_nPlayMode;
	m_nPlayMode = nMode;

	switch( nMode )
	{
	case IDLE_PLAYMODE:
		GetUI()->SetImage();
		break;
	case INGAME_PLAYMODE:
	case SETTING_PLAYMODE:
		if ( nLastPlayMode == IDLE_PLAYMODE )
			GetUI()->SetImage();
		break;
	}
}

C_Player * C_Slave::GetPlayer(int nPlayerID)
{
	for ( int i = 0 ; i < GetPlayerList()->Count() ; i++ )
	{
		if ( GetPlayerList()->Items(i)->GetPlayerID() == nPlayerID )
			return GetPlayerList()->Items(i);
	}
	return NULL;
}

// GetPlayerByPosition use only with KING,SLAVE,QUEEN,VSLAVE
// not PEOPLE 
C_Player * C_Slave::GetPlayerByPosition(int nPosition)
{
	for ( int i = 0 ; i < GetPlayerList()->Count() ; i++ )
	{
		if ( GetPlayerList()->Items(i)->GetPosition() == nPosition )
			return GetPlayerList()->Items(i);
	}
	return NULL;
}

int C_Slave::Discard(int nPlayerID,int nDiscardType,LPDISCARD_CARD pCardNum)
{
	C_Player * pPlayer = GetPlayer(GetIngameTable()->nLatestPlayerID);
	if ( pPlayer != NULL )
	{
		if ( pPlayer->GetDiscardStatus() != CARD_OVER )
			pPlayer->SetDiscardStatus(WAITFORDISCARD);
	}

    // update table data
    m_IngameTable.nDiscardType = nDiscardType;
    m_IngameTable.nLatestPlayerID = nPlayerID;

    for ( int j=0 ; j<4 ; j++ )
    {
        if ( j >= nDiscardType )
            m_IngameTable.LatestCard.nCard[j] = 0;
        else
            m_IngameTable.LatestCard.nCard[j] = pCardNum->nCard[j];
    }

	// check player finish
	pPlayer = GetPlayer(nPlayerID);
	if ( pPlayer != NULL )
	{
		if ( pPlayer->GetDiscardStatus() == CARD_OVER )
			PutFinishPlayer(nPlayerID,NORMAL_ORDER);
	}

    // dispatch this action to other player here
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// send msg to every other player , except owner
        for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
        	// if server discard , send to all client
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
        	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
            {
				// send discard
	    		pSlaveComm->SendDiscard(nPlayerID,nDiscardType,pCardNum);
            }
            else
            {
            	// if others discard , send to all other client , except owner
				if ( pSlaveComm->GetPlayer() != NULL )
					if ( pSlaveComm->GetPlayer()->GetPlayerID() != nPlayerID )
		    			pSlaveComm->SendDiscard(nPlayerID,nDiscardType,pCardNum);
            }
        }
    }
    // action that can use by client must check ID to prevent recursive
    else if ( m_nGameMode == CLIENT_GAMEMODE && nPlayerID == m_pMyPlayer->GetPlayerID() )
    {
		// client send discard to server
    	m_pComm->SendDiscard(nPlayerID,nDiscardType,pCardNum);
    }

	NextTurn();
	return TRUE;
}

int C_Slave::Pass(int nPlayerID)
{
    // dispatch this action to other player here
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// send msg to every other player , except owner
        for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
        	// if server discard , send to all client
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
        	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
            {
				// send discard
	    		pSlaveComm->SendPass(nPlayerID);
            }
            else
            {
            	// if others discard , send to all other client , except owner
				if ( pSlaveComm->GetPlayer() != NULL )
					if ( pSlaveComm->GetPlayer()->GetPlayerID() != nPlayerID )
			    		pSlaveComm->SendPass(nPlayerID);
            }
        }
    }
    // action that can use by client must check ID to prevent recursive
    else if ( m_nGameMode == CLIENT_GAMEMODE && nPlayerID == m_pMyPlayer->GetPlayerID() )
    {
		// client send discard to server
    	m_pComm->SendPass(nPlayerID);
    }

	NextTurn();
	return TRUE;
}

int C_Slave::ExchangeCard(int nPlayerID,int nPosition,LPDISCARD_CARD pCard)
{
    // dispatch this action to other player here
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// send msg to every other player , except owner
        for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
        	// if server chat , send to all client
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
        	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
            {
	    		pSlaveComm->SendExchange(nPlayerID,nPosition,pCard);
            }
            else
            {
            	// if others chat , send to all other client , except owner
				if ( pSlaveComm->GetPlayer() != NULL )
					if ( pSlaveComm->GetPlayer()->GetPlayerID() != nPlayerID )
			    		pSlaveComm->SendExchange(nPlayerID,nPosition,pCard);
            }
        }
    }
    // action that can use by client must check ID to prevent recursive
    else if ( m_nGameMode == CLIENT_GAMEMODE && nPlayerID == m_pMyPlayer->GetPlayerID() )
    {
		// client send exchange to server
    	m_pComm->SendExchange(nPlayerID,nPosition,pCard);
    }

	
	int i,j;
	// KING ~ SLAVE , QUEEN ~ VSLAVE
	if ( nPosition == IAM_KING )
	{
		C_Player * pKing = GetPlayerByPosition(IAM_KING);
		C_Player * pSlave = GetPlayerByPosition(IAM_SLAVE);

		if ( pKing == NULL || pSlave == NULL ) return FALSE;

		// remove pcard from king 
		for ( j=0 ; j<2 ; j++ )
		{
			for ( i=0 ; i<pKing->GetCardList()->Count() ; i++ )
			{
				int ncard = *(pKing->GetCardList()->Items(i));
				if ( ncard == pCard->nCard[j] )
				{
					delete pKing->GetCardList()->Items(i);
					pKing->GetCardList()->Remove(i);
					break;
				}
			}
		}
		// remove max card from slave
		int nMaxCard[2] = {0,0};
		for ( j=1 ; j>=0 ; j-- )
		{
			int nMaxCardPos = pSlave->GetCardList()->Count()-1;
			nMaxCard[j] = *(pSlave->GetCardList()->Items(nMaxCardPos));
			delete pSlave->GetCardList()->Items(nMaxCardPos);
			pSlave->GetCardList()->Remove(nMaxCardPos);
		}

		// put card to slave
		for ( j=0 ; j<2 ; j++ )
		{
			int k;
			for ( k=0 ; k < pSlave->GetCardList()->Count() ; k++ )
			{
				if ( pCard->nCard[j] < *(pSlave->GetCardList()->Items(k)) )
				{
					int * x = new int(pCard->nCard[j]);
					pSlave->GetCardList()->Insert(k,x);
					break;
				}
			}
			if ( k == pSlave->GetCardList()->Count() )
			{
				int * x = new int(pCard->nCard[j]);
				pSlave->GetCardList()->Add(x);
			}
		}

		// put maxcard to king
		for ( j=0 ; j<2 ; j++ )
		{
			int k;
			for ( k=0 ; k < pKing->GetCardList()->Count() ; k++ )
			{
				if ( nMaxCard[j] < *(pKing->GetCardList()->Items(k)) )
				{
					int * x = new int(nMaxCard[j]);
					pKing->GetCardList()->Insert(k,x);
					break;
				}
			}
			if ( k == pKing->GetCardList()->Count() )
			{
				int * x = new int(nMaxCard[j]);
				pKing->GetCardList()->Add(x);
			}
		}
		if ( GetMyPlayer()->GetPosition() == IAM_KING ||
			GetMyPlayer()->GetPosition() == IAM_SLAVE )
		{
			GetUI()->ClearCardOnHand();
			GetUI()->SetCardOnHand();
		}
	}
	else // IAM_QUEEN
	{
		C_Player * pQueen = GetPlayerByPosition(IAM_QUEEN);
		C_Player * pVSlave = GetPlayerByPosition(IAM_VICESLAVE);

		if ( pQueen == NULL || pVSlave == NULL ) return FALSE;

		// remove pcard from queen 
		for ( i=0 ; i<pQueen->GetCardList()->Count() ; i++ )
		{
			int ncard = *(pQueen->GetCardList()->Items(i));
			if ( ncard == pCard->nCard[0] )
			{
				delete pQueen->GetCardList()->Items(i);
				pQueen->GetCardList()->Remove(i);
				break;
			}
		}
		// remove max card from vice slave
		int nMaxCardPos = pVSlave->GetCardList()->Count()-1;
		int nMaxCard = *(pVSlave->GetCardList()->Items(nMaxCardPos));
		delete pVSlave->GetCardList()->Items(nMaxCardPos);
		pVSlave->GetCardList()->Remove(nMaxCardPos);

		// put card to vice slave
		int k;
		for ( k=0 ; k < pVSlave->GetCardList()->Count() ; k++ )
		{
			if ( pCard->nCard[0] < *(pVSlave->GetCardList()->Items(k)) )
			{
				int * x = new int(pCard->nCard[0]);
				pVSlave->GetCardList()->Insert(k,x);
				break;
			}
		}
		if ( k == pVSlave->GetCardList()->Count() )
		{
			int * x = new int(pCard->nCard[0]);
			pVSlave->GetCardList()->Add(x);
		}

		// put maxcard to queen
		for ( k=0 ; k < pQueen->GetCardList()->Count() ; k++ )
		{
			if ( nMaxCard < *(pQueen->GetCardList()->Items(k)) )
			{
				int * x = new int(nMaxCard);
				pQueen->GetCardList()->Insert(k,x);
				break;
			}
		}
		if ( k == pQueen->GetCardList()->Count() )
		{
			int * x = new int(nMaxCard);
			pQueen->GetCardList()->Add(x);
		}
		if ( GetMyPlayer()->GetPosition() == IAM_QUEEN ||
			GetMyPlayer()->GetPosition() == IAM_VICESLAVE )
		{
			GetUI()->ClearCardOnHand();
			GetUI()->SetCardOnHand();
		}
	}

	// remove pCard from nPosition , add to ~nPosition
	// remove max card from ~nPosition , add to nPosition
	if ( nPosition == IAM_KING )
	{
		C_Player * pKing = GetPlayerByPosition(IAM_KING);
		C_Player * pSlave = GetPlayerByPosition(IAM_SLAVE);
		if ( pKing != NULL && pSlave != NULL )
		{
			char tmpbuf[64];
			sprintf(tmpbuf,"/c1%s changes 2 cards with %s",pKing->GetName(),pSlave->GetName());
			m_pComm->PopupMessage(tmpbuf);
		}
	}
	else // IAM_QUEEN
	{
		C_Player * pQueen = GetPlayerByPosition(IAM_QUEEN);
		C_Player * pVSlave = GetPlayerByPosition(IAM_VICESLAVE);
		if ( pQueen != NULL && pVSlave != NULL )
		{
			char tmpbuf[64];
			sprintf(tmpbuf,"/c1%s changes a card with %s",pQueen->GetName(),pVSlave->GetName());
			m_pComm->PopupMessage(tmpbuf);
		}
	}

	if ( m_nPlayMode == EXCHANGE_PLAYMODE2 )
		m_nPlayMode = EXCHANGE_PLAYMODE1;
	else
	{
		m_nPlayMode = INGAME_PLAYMODE;
		StartNewTurn();
	}
	return TRUE;
}

int C_Slave::Chat(int nPlayerID,char *pszMsg)
{
    // dispatch this action to other player here
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// send msg to every other player , except owner
        for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
        	// if server chat , send to all client
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)m_pComm->GetClientList()->Items(i);
        	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
            {
	    		pSlaveComm->SendChat(nPlayerID,pszMsg);
            }
            else
            {
            	// if others chat , send to all other client , except owner
				if ( pSlaveComm->GetPlayer() != NULL )
					if ( pSlaveComm->GetPlayer()->GetPlayerID() != nPlayerID )
			    		pSlaveComm->SendChat(nPlayerID,pszMsg);
            }
        }
    }
    // action that can use by client must check ID to prevent recursive
    else if ( m_nGameMode == CLIENT_GAMEMODE && nPlayerID == m_pMyPlayer->GetPlayerID() )
    {
    	m_pComm->SendChat(nPlayerID,pszMsg);
    }
	return TRUE;
}

int C_Slave::PutFinishPlayer(int nPlayerID,int nType)
{
	int i;
	switch (nType)
	{
	case NORMAL_ORDER:
		for ( i=0 ; i<6 ; i++ )
		{
			if ( GetFinishTable()->nPlayerID[i] == 0 )
			{
				GetFinishTable()->nPlayerID[i] = nPlayerID;
				break;
			}
		}
		break;
	case PUT_LASTPLAYER:
		for ( i=GetGameSetting()->nNoOfPlayer-1 ; i>=0 ; i-- )
		{
			if ( GetFinishTable()->nPlayerID[i] == 0 )
			{
				GetFinishTable()->nPlayerID[i] = nPlayerID;
				break;
			}
		}
		break;
	}
	return TRUE;
}

int C_Slave::IsValidDiscard(int nDiscardType,LPDISCARD_CARD pCardNum)
{
    if ( nDiscardType == 0 ) return DISCARD_INVALID;

    switch ( GetIngameTable()->nDiscardType )
    {
    case ONE_CARD:
        if ( nDiscardType == TWO_CARD )
            return DISCARD_INVALID;
        break;
    case TWO_CARD:
        if ( nDiscardType == ONE_CARD || nDiscardType == THREE_CARD )
            return DISCARD_INVALID;
        break;
    case THREE_CARD:
        if ( nDiscardType == ONE_CARD || nDiscardType == TWO_CARD )
            return DISCARD_INVALID;
        break;
    case FOUR_CARD:
        if ( nDiscardType == ONE_CARD || nDiscardType == TWO_CARD || nDiscardType == THREE_CARD )
            return DISCARD_INVALID;
        break;
    }

    int i;
    int nNum = 0;

    if ( nDiscardType == ONE_CARD )
	{
		// check for suit ( if it's set )
		if ( GetGameSetting()->bCheckSuit == TRUE && 
				GetIngameTable()->nDiscardType == ONE_CARD )
		{
			if ( pCardNum->nCard[0] < GetIngameTable()->LatestCard.nCard[0] )
				return DISCARD_INVALID;

			// is same num/suit ?
			if ( GET_FACE(pCardNum->nCard[0]) != GET_FACE(GetIngameTable()->LatestCard.nCard[0]) ) 
			{
				if ( GET_NUM(pCardNum->nCard[0]) != GET_NUM(GetIngameTable()->LatestCard.nCard[0]) )
					return DISCARD_SUITINVALID;
			}
		}
		return DISCARD_OK;
	}

	// check for 2 or more discard
    nNum = GET_NUM(pCardNum->nCard[0]);

    for ( i=1 ; i<nDiscardType ; i++ )
    {
        if ( GET_NUM(pCardNum->nCard[i]) != nNum )
            return DISCARD_INVALID;
    }

	// check for Tong Beat 2
	if ( GetGameSetting()->bTongBeat2 == FALSE )
	{
		if ( GetIngameTable()->nDiscardType == ONE_CARD && 
				nDiscardType == THREE_CARD )
		{
			if ( GET_NUM(GetIngameTable()->LatestCard.nCard[0]) == CARD_2 &&
						GET_NUM(pCardNum->nCard[0]) != CARD_2 )
				return DISCARD_TONGCANTBEAT2;
		}
	}

	
	return DISCARD_OK;
}

int C_Slave::DistributeCard()
{
	// distribute card to all players
	// all players must have no card
	CARD_LIST card;

	int i,j;

	for ( i=1 ; i<=52 ; i++ )
	{
		int * c = new int(i);
		card.Add(c);
	}

	C_Player * pPlayer[6];
	for ( i=0; i<6 ; i++ )
	{
		pPlayer[i] = GetPlayer(m_GameSetting.nPlayerID[i]);
	}
	int nIndex = 0;
	int nDir = NORMAL_DIRECTION;

	if ( m_nGameNum == 0 ) 
	{
		m_IngameTable.nDiscardDirection = NORMAL_DIRECTION;
		GetUI()->RefreshDiscardDirection();
	}
	else
	{
		// slave start first
		int nStart=0;
		for ( i = 0 ; i<GetGameSetting()->nNoOfPlayer ; i++ )
		{
			C_Player * p = GetPlayer(GetGameSetting()->nPlayerID[i]);
			if ( p->GetPosition() == IAM_SLAVE )
			{
				m_nActivePlayerOrder = p->GetTableOrder();
				nStart = i;
				break;
			}
		}
		
		int nDisK=0;
		int nDisQ=0;

		int n = nStart;
		for ( i=0 ; i<GetGameSetting()->nNoOfPlayer ; i++ )
		{
			n++ ;
			if ( n >= GetGameSetting()->nNoOfPlayer ) n = 0;

			nDisK++;
			C_Player * p = GetPlayer(GetGameSetting()->nPlayerID[n]);
			if ( p != NULL )
			{
				if ( p->GetPosition() == IAM_KING )
				break;
			}
		}
		n = nStart;
		if ( GetGameSetting()->nNoOfPlayer > 3 )
		{
			for ( i=0 ; i<GetGameSetting()->nNoOfPlayer ; i++ )
			{
				n++ ;
				if ( n >= GetGameSetting()->nNoOfPlayer ) n = 0;

				nDisQ++;
				C_Player * p = GetPlayer(GetGameSetting()->nPlayerID[n]);
				if ( p != NULL )
				{
					if ( p->GetPosition() == IAM_QUEEN )
					break;
				}
			}
		}
		// find direction from slave to king

		double x = (double)nDisK;
		double z = (double)nDisQ;
		double y = ((double)(GetGameSetting()->nNoOfPlayer))/2;

		if ( x > y)
			m_IngameTable.nDiscardDirection = NORMAL_DIRECTION;
		else if ( x == y )
		{
			if ( z > y )
				m_IngameTable.nDiscardDirection = NORMAL_DIRECTION;
			else
				m_IngameTable.nDiscardDirection = REVERSE_DIRECTION;
		}
		else
			m_IngameTable.nDiscardDirection = REVERSE_DIRECTION;
		GetUI()->RefreshDiscardDirection();
	}

	// distribute card
	while ( card.Count() > 0 )
	{
		if ( pPlayer[nIndex] != NULL )
		{
			j = RANDOM( card.Count() );
			int k;
			for ( k=0 ; k < pPlayer[nIndex]->GetCardList()->Count() ; k++ )
			{
				if ( *(card.Items(j)) < *(pPlayer[nIndex]->GetCardList()->Items(k)) )
				{
					pPlayer[nIndex]->GetCardList()->Insert(k,card.Items(j));
					card.Remove(j);
					break;
				}
			}
			if ( k == pPlayer[nIndex]->GetCardList()->Count() )
			{
				pPlayer[nIndex]->GetCardList()->Add(card.Items(j));
				card.Remove(j);
			}
		}
		if ( nDir == NORMAL_DIRECTION )
		{
			nIndex++ ; if ( nIndex >= 6 ) nIndex = 0;
		}
		else
		{
			nIndex-- ; if ( nIndex < 0 ) nIndex = 6;
		}
	}

	card.Clear();

	if ( m_nGameNum == 0 )
	{
		m_nActivePlayerOrder = 0;
		// find for who get 3 club is first player
		for (;;)
		{
	    	m_nActivePlayerOrder++;
			if ( m_nActivePlayerOrder > 5 ) m_nActivePlayerOrder = 0;
			C_Player * p = GetPlayer(GetActivePlayerID());
			if ( p != NULL )
				if ( *(p->GetCardList()->Items(0)) == CLUB_3 )
					break;
		}
	}

	return TRUE;
}

void C_Slave::CheckResponse(int nPlayerID,C_SlaveComm * pComm)
{
	char szIPBuf[64];
	UINT nPort;

	if ( pComm->GetPeerName(szIPBuf,&nPort) )
		m_pPing->Ping(nPlayerID,szIPBuf);
}


