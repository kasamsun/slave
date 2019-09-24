#include "Player.h"

#include "SlaveCard.h"
#include "Slave.h"
#include "SlaveWnd.h"
#include "PlayerWnd.h"
#include "SlaveAI.h"
#include "SlaveApp.h"
#include "resource.h"

extern C_SlaveApp SlaveApp;

C_Player::C_Player()
{
	m_pSlave = NULL;
    m_nPlayerID = 0;
    m_pPlayerWnd = NULL;
	m_pSlaveAI = NULL;

   	m_nPlayerStatus = IAM_SPECTATOR;
    m_nCharacterID = 0;
	memset(m_szPlayerName,0x00,sizeof(m_szPlayerName));

	m_nNoOfSlave = 0;
	m_nNoOfKing = 0;
	m_nNoOfGamePlay = 0;

	m_nNoOfBeenKing = 0;
	m_nNoOfBeenQueen = 0;

	m_Card.Clear();
    m_nTableOrder = NOT_IN_TABLE;
	m_nPosition = IAM_PEOPLE;
	m_nDiscardStatus = WAITFORDISCARD;
}

C_Player::C_Player(C_Slave * pSlave,int nPlayerID,LPSLAVEPLAYER PlayerInfo)
{
	m_pSlave = pSlave;
    m_nPlayerID = nPlayerID;
    m_pPlayerWnd = NULL;
	m_pSlaveAI = NULL;

 	m_nPlayerStatus = PlayerInfo->nPlayerStatus;
    m_nCharacterID = PlayerInfo->nCharacterID;
	strcpy(m_szPlayerName,PlayerInfo->szPlayerName);

	m_nNoOfSlave = PlayerInfo->nNoOfSlave;
	m_nNoOfKing = PlayerInfo->nNoOfKing;
	m_nNoOfGamePlay = PlayerInfo->nNoOfGamePlay;

	m_nNoOfBeenKing = 0;
	m_nNoOfBeenQueen = 0;

    m_Card.Clear();
    m_nTableOrder = NOT_IN_TABLE;
	m_nPosition = IAM_PEOPLE;
	m_nDiscardStatus = WAITFORDISCARD;
}

C_Player::~C_Player()
{
    if ( m_Card.Count() > 0 )
		m_Card.DeleteObjects();

	if ( m_pSlaveAI != NULL ) delete m_pSlaveAI;

    // delete UI Object
    if ( m_pPlayerWnd == NULL ) return;
	if ( m_nPlayerStatus == IAM_SPECTATOR )
	{
		if ( IsWindow(m_pPlayerWnd->GetHandle()) )
   			DestroyWindow(m_pPlayerWnd->GetHandle());
		delete m_pPlayerWnd;
	}

}

void C_Player::operator=(C_Player& p)
{
	m_pSlave = p.m_pSlave;
    m_nPlayerID = p.m_nPlayerID;

    // operator = will not copy pointer member
    m_pPlayerWnd = NULL;
	m_pSlaveAI = NULL;

   	m_nPlayerStatus = p.m_nPlayerStatus;
    m_nCharacterID = p.m_nCharacterID;
	strcpy(m_szPlayerName,p.m_szPlayerName);

	m_nNoOfSlave = p.m_nNoOfSlave;
	m_nNoOfKing = p.m_nNoOfKing;
	m_nNoOfGamePlay = p.m_nNoOfGamePlay;

	m_nNoOfBeenKing = p.m_nNoOfBeenKing;
	m_nNoOfBeenQueen = p.m_nNoOfBeenQueen;

    int i;
    if ( m_Card.Count() > 0 )
		m_Card.DeleteObjects();
	for ( i=0 ; i<p.m_Card.Count() ; i++ )
	{
	    int nCard = *(p.m_Card.Items(i));
		m_Card.Add( new int( nCard ) );
	}

    m_nTableOrder = p.m_nTableOrder;
	m_nPosition = p.m_nPosition;
	m_nDiscardStatus = p.m_nDiscardStatus;
}

void C_Player::SetName(LPCSTR lpszName)
{
    if ( strlen(lpszName) > MAX_PLAYER_NAME-1 )
        strncpy(m_szPlayerName,lpszName,MAX_PLAYER_NAME-1);
    else
        strcpy(m_szPlayerName,lpszName);
    if ( m_pPlayerWnd != NULL )
    	m_pPlayerWnd->PlayerUpdated();
}


int C_Player::Chat(char *pszMsg)
{
    m_pPlayerWnd->Chat(pszMsg);
	m_pSlave->Chat(GetPlayerID(),pszMsg);
	return 0;
}

int C_Player::Discard(int nDiscardType,LPDISCARD_CARD pCardNum)
{
	// remove card from card list
	int nCount = 0;

	for ( int j=0 ; j<nDiscardType ; j++ )
	{
		for ( int i=0 ; i<GetCardList()->Count() ; i++ )
		{
			int ncard = *(GetCardList()->Items(i));
			if ( ncard == pCardNum->nCard[nCount] )
			{
				nCount++;
				delete GetCardList()->Items(i);
				GetCardList()->Remove(i);
				break;
			}
		}
	}

	if ( nCount != nDiscardType )
	{
		// error
		TRACE("### ERR can't discard, card missing\n");
#ifdef _DEBUG
		MessageBox(GetActiveWindow(),"Can't discard , card missing","",MB_OK);
#endif
		return FALSE;
	}

	int bIsFallen = FALSE;

	if ( GetCardList()->Count() > 0 )
		SetDiscardStatus(LAST_DISCARD);
	else
	{
		if ( m_pSlave->GetGameNumber() > 1 )
		{
			// i any one can beat king
			if ( GetPosition() != IAM_KING && m_pSlave->GetFinishTable()->nPlayerID[0] == 0 )
			{
				C_Player * pPlayer = m_pSlave->GetPlayerByPosition(IAM_KING);
				if ( pPlayer != NULL )
				{
					bIsFallen = TRUE;
					pPlayer->SetDiscardStatus(CARD_OVER);
					pPlayer->GetCardList()->DeleteObjects();
					if ( pPlayer == m_pSlave->GetMyPlayer() )
						m_pSlave->GetUI()->ClearCardOnHand();
					m_pSlave->GetUI()->RefreshPlayersTable(pPlayer->GetTableOrder());
					m_pSlave->PutFinishPlayer(pPlayer->GetPlayerID(),PUT_LASTPLAYER);
				}
			}
			if ( GetPosition() != IAM_QUEEN && m_pSlave->GetFinishTable()->nPlayerID[0] == 0 )
			{
				C_Player * pPlayer = m_pSlave->GetPlayerByPosition(IAM_QUEEN);
				if ( pPlayer != NULL )
				{
					if ( pPlayer->GetNoOfBeenQueen() >= 3 )
					{
						bIsFallen = TRUE;
						pPlayer->SetDiscardStatus(CARD_OVER);
						pPlayer->GetCardList()->DeleteObjects();
						if ( pPlayer == m_pSlave->GetMyPlayer() )
							m_pSlave->GetUI()->ClearCardOnHand();
						m_pSlave->GetUI()->RefreshPlayersTable(pPlayer->GetTableOrder());
						m_pSlave->PutFinishPlayer(pPlayer->GetPlayerID(),PUT_LASTPLAYER);
					}
				}
			}
		}
		SetDiscardStatus(CARD_OVER);
	}

	// must do UI's discard first , because in Slave's discard
	// will call next turn that 'll call pickup card
	// it cause pickup undiscard card
	int nLastPos = -1;
	C_Player * pPlayer = m_pSlave->GetPlayer(m_pSlave->GetIngameTable()->nLatestPlayerID);
	if ( pPlayer != NULL )
		nLastPos = 0;

	m_pSlave->GetUI()->Discard(GetTableOrder(),nLastPos,nDiscardType,pCardNum);
	m_pSlave->Discard(GetPlayerID(),nDiscardType,pCardNum);
	// if king , queen fall
	if ( bIsFallen )
		if ( SlaveApp.IsUseSound() )
			PlaySound(MAKEINTRESOURCE(IDR_SND_BOING),
					C_App::m_hInstance,
					SND_RESOURCE|SND_ASYNC);
	return TRUE;
}

int C_Player::Pass()
{
	SetDiscardStatus(PASS_TURN);
	m_pSlave->GetUI()->Pass(GetTableOrder());
	m_pSlave->Pass(GetPlayerID());
	return TRUE;
}

int C_Player::ActivateAI()
{
	if ( m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE1 ||
			m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE2 )
	{
		DISCARD_CARD Card;
		m_pSlaveAI->ExchangeCard(&Card);
		return m_pSlave->ExchangeCard(GetPlayerID(),GetPosition(),&Card);
	}
	else
	{
		int nDiscardType;
		DISCARD_CARD Card;
		if ( m_pSlaveAI->Discard(nDiscardType,&Card) )
			return Discard(nDiscardType,&Card);
		else
			return Pass();
	}
}



