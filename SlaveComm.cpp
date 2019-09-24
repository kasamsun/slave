#include "SlaveComm.h"

#include <stdio.h>
#include "MsgPopupWnd.h"
#include "resource.h"
#include "Slave.h"
#include "SlaveWnd.h"
#include "NetStatDlg.h"
#include "PlayerDetailWnd.h"

#define CONNECTION_TIMERID		1210
#define CONNECTING_BLINK_RATE	300
#define SLVNETLOG_FILENAME		"SlvNet.log"

BEGIN_HANDLE_MSG(C_SlaveComm,C_MyWinsock)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_LBUTTONUP,OnLButtonUp)
	HANDLE_MSG(WM_RBUTTONUP,OnRButtonUp)
	HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)
	HANDLE_MSG(WM_TIMER,OnTimer)
END_HANDLE_MSG()


C_SlaveComm::C_SlaveComm(C_Slave *pSlave) : C_MyWinsock()
{
	m_pSlave = pSlave;
	m_pPlayer = NULL;
    m_bConnecting = FALSE;
    m_pMsgWnd = NULL;
	m_fpNetLog = NULL;
}

C_SlaveComm::~C_SlaveComm()
{
	if ( IsWindow(m_hWnd) ) KillTimer(m_hWnd,CONNECTION_TIMERID);
	delete m_pMsgWnd;
	CloseLogMessage();
}

void C_SlaveComm::CreateLogMessage()
{
	if ( m_fpNetLog == NULL )
	{
		// open in overwrite mode
		m_fpNetLog = fopen(SLVNETLOG_FILENAME,"w+");
	}
}

void C_SlaveComm::OpenLogMessage()
{
	if ( m_fpNetLog == NULL )
	{
		// open in append mode
		m_fpNetLog = fopen(SLVNETLOG_FILENAME,"a+");
	}
}

void C_SlaveComm::CloseLogMessage()
{
	if ( m_fpNetLog != NULL )
	{
		fclose(m_fpNetLog);
		m_fpNetLog = NULL;
	}
}

void C_SlaveComm::LogMessage(LPCTSTR lpszFormat, ...)
{
	if ( m_fpNetLog == NULL && m_pParentSock == NULL ) return;

	va_list Args;
	va_start(Args,lpszFormat);

	char szMsgBuf[256];
	vsprintf(szMsgBuf,lpszFormat,Args);
	va_end(Args);

	if ( m_pParentSock != NULL )
    {
    	((C_SlaveComm*)m_pParentSock)->LogMessage(szMsgBuf);
    }
    else
    {
		fputs(szMsgBuf,m_fpNetLog);
		fflush(m_fpNetLog);
    }
}

BOOL C_SlaveComm::CreateServerSocket(unsigned short nPortNum)
{
	m_bConnecting = FALSE;
	int nRetVal = C_MyWinsock::CreateServerSocket(nPortNum);
	InvalidateRect(m_hWnd,NULL,FALSE);
	LogMessage("CreateSrvSock(%d)=%d\n",nPortNum,nRetVal);
    return nRetVal;
}

BOOL C_SlaveComm::CreateClientSocket(unsigned short nPortNum,char *szDotAddr)
{
	m_bConnecting = TRUE;
	SetTimer(m_hWnd,CONNECTION_TIMERID,CONNECTING_BLINK_RATE,NULL);
	int nRetVal = C_MyWinsock::CreateClientSocket(nPortNum,szDotAddr);
	InvalidateRect(m_hWnd,NULL,FALSE);
	LogMessage("CreateCliSock(%d,%s)=%d\n",nPortNum,szDotAddr,nRetVal);
    return nRetVal;
}

int C_SlaveComm::Close()
{
	CloseAllSocket();
	if ( IsWindow(m_hWnd) )
    {
    	KillTimer(m_hWnd,CONNECTION_TIMERID);
   		InvalidateRect(m_hWnd,NULL,FALSE);
    }
	LogMessage("CloseSock\n");
	return TRUE;
}

void C_SlaveComm::PopupMessage(LPCTSTR pszMsg)
{
	if ( m_pParentSock == NULL )
	{
		if ( m_pMsgWnd != NULL )
			m_pMsgWnd->PopupMessage(pszMsg);
	}
	else
		((C_SlaveComm*)m_pParentSock)->PopupMessage(pszMsg);
}

int C_SlaveComm::CreateMsgWnd()
{
    m_pMsgWnd = new C_MsgPopupWnd(this,5,10000,8000);
	return 0;
}


// ================= Communication API =====================
int C_SlaveComm::SendLogon(C_Player *pPlayer)
{
	LOGON_MSG Data;
	memset(&Data,0x00,sizeof(LOGON_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_LOGON_MSG;

	strcpy(Data.Name,pPlayer->GetName());
	Data.PlayerID = (unsigned short) pPlayer->GetPlayerID();
	Data.Status = (unsigned char) pPlayer->GetStatus();
	Data.CharacterID = (unsigned char) pPlayer->GetCharacterID();
	Data.NoOfKing = (unsigned short) pPlayer->GetNoOfKing();
	Data.NoOfSlave = (unsigned short) pPlayer->GetNoOfSlave();
	Data.NoOfGamePlay = (unsigned short) pPlayer->GetNoOfGamePlay();

	SocketSend((char*)&Data,sizeof(LOGON_MSG));
	LogMessage("SendLogon(%s)\n",pPlayer->GetName());
	return 0;
}

int C_SlaveComm::SendLogonOK(int nPlayerID)
{
	LOGON_OK_MSG Data;
	memset(&Data,0x00,sizeof(LOGON_OK_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_LOGON_OK_MSG;

	Data.PlayerID = (unsigned short) nPlayerID;

	SocketSend((char*)&Data,sizeof(LOGON_OK_MSG));
	LogMessage("SendLogonOK(%d)\n",nPlayerID);
	return 0;
}

int C_SlaveComm::SendLogonFail(int nErrCode)
{
	LOGON_FAIL_MSG Data;
	memset(&Data,0x00,sizeof(LOGON_FAIL_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_LOGON_FAIL_MSG;

	Data.ErrCode = (unsigned short) nErrCode;

	SocketSend((char*)&Data,sizeof(LOGON_FAIL_MSG));
	LogMessage("SendLogonFail(%d)\n",nErrCode);
	return 0;
}

int C_SlaveComm::SendChat(int nPlayerID,LPCSTR lpszMsg)
{
	CHAT_MSG Data;
	memset(&Data,0x00,sizeof(CHAT_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_CHAT_MSG;

	Data.PlayerID = (unsigned short) nPlayerID;
    strcpy(Data.ChatMsg,lpszMsg);

	SocketSend((char*)&Data,sizeof(CHAT_MSG));
	LogMessage("SendChat(%d,%s)\n",nPlayerID,lpszMsg);
	return 0;
}

int C_SlaveComm::SendAddPlayer(C_Player *pPlayer)
{
	ADD_PLAYER_MSG Data;
	memset(&Data,0x00,sizeof(ADD_PLAYER_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_ADD_PLAYER_MSG;

	strcpy(Data.Name,pPlayer->GetName());
	Data.PlayerID = (unsigned short) pPlayer->GetPlayerID();
	Data.Status = (unsigned char) pPlayer->GetStatus();
	Data.CharacterID = (unsigned char) pPlayer->GetCharacterID();
	Data.NoOfKing = (unsigned short) pPlayer->GetNoOfKing();
	Data.NoOfSlave = (unsigned short) pPlayer->GetNoOfSlave();
	Data.NoOfBeenKing = (unsigned short) pPlayer->GetNoOfBeenKing();
	Data.NoOfBeenQueen = (unsigned short) pPlayer->GetNoOfBeenQueen();
	Data.NoOfGamePlay = (unsigned short) pPlayer->GetNoOfGamePlay();
	Data.Position = (unsigned char) pPlayer->GetPosition();
	Data.DiscardStatus = (unsigned char) pPlayer->GetDiscardStatus();

	SocketSend((char*)&Data,sizeof(ADD_PLAYER_MSG));
	LogMessage("SendAddPlayer(%d,%s)\n",pPlayer->GetPlayerID(),pPlayer->GetName());
	return 0;
}

int C_SlaveComm::SendRemovePlayer(int nPlayerID)
{
	REMOVE_PLAYER_MSG Data;
	memset(&Data,0x00,sizeof(REMOVE_PLAYER_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_REMOVE_PLAYER_MSG;

	Data.PlayerID = (unsigned short)nPlayerID;

	SocketSend((char*)&Data,sizeof(REMOVE_PLAYER_MSG));
	LogMessage("SendRemovePlayer(%d)\n",nPlayerID);
	return 0;
}

int C_SlaveComm::SendSetGame(LPSTART_GAME_SETTING pSetGame)
{
	SET_GAME_MSG Data;
	memset(&Data,0x00,sizeof(SET_GAME_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_SET_GAME_MSG;

	for ( int i=0 ; i<6 ; i++ )
		Data.PlayerID[i] = (unsigned short)pSetGame->nPlayerID[i];
	Data.NoOfPlayer = (unsigned char)pSetGame->nNoOfPlayer;
	Data.TongBeat2 = (unsigned char)pSetGame->bTongBeat2;
	Data.CheckSuit = (unsigned char)pSetGame->bCheckSuit;

	SocketSend((char*)&Data,sizeof(SET_GAME_MSG));
	LogMessage("SendSetGame(%d,%d,%d,%d,%d,%d - %d - %d)\n",
				pSetGame->nPlayerID[0],
				pSetGame->nPlayerID[1],
				pSetGame->nPlayerID[2],
				pSetGame->nPlayerID[3],
				pSetGame->nPlayerID[4],
				pSetGame->nPlayerID[5],
				pSetGame->nNoOfPlayer,
				pSetGame->bTongBeat2);
	return 0;
}

int C_SlaveComm::SendStartGame(LPSTART_GAME_SETTING pSetGame)
{
	START_GAME_MSG Data;
	memset(&Data,0x00,sizeof(START_GAME_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_START_GAME_MSG;

	for ( int i=0 ; i<6 ; i++ )
		Data.PlayerID[i] = (unsigned short)pSetGame->nPlayerID[i];
	Data.NoOfPlayer = (unsigned char)pSetGame->nNoOfPlayer;
	Data.TongBeat2 = (unsigned char)pSetGame->bTongBeat2;
	Data.CheckSuit = (unsigned char)pSetGame->bCheckSuit;

	SocketSend((char*)&Data,sizeof(START_GAME_MSG));
	LogMessage("SendStartGame(%d,%d,%d,%d,%d,%d - %d - %d)\n",
				pSetGame->nPlayerID[0],
				pSetGame->nPlayerID[1],
				pSetGame->nPlayerID[2],
				pSetGame->nPlayerID[3],
				pSetGame->nPlayerID[4],
				pSetGame->nPlayerID[5],
				pSetGame->nNoOfPlayer,
				pSetGame->bTongBeat2);
	return 0;
}

int C_SlaveComm::SendResetGame()
{
	RESET_GAME_MSG Data;
	memset(&Data,0x00,sizeof(RESET_GAME_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_RESET_GAME_MSG;

	SocketSend((char*)&Data,sizeof(RESET_GAME_MSG));
	LogMessage("SendResetGame\n");
	return 0;
}

int C_SlaveComm::SendStartRound()
{
	START_ROUND_MSG Data;
	memset(&Data,0x00,sizeof(START_ROUND_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_START_ROUND_MSG;

	int i;
	for ( i=0 ; i<m_pSlave->GetGameSetting()->nNoOfPlayer; i++ )
	{
		C_Player * pPlayer = m_pSlave->GetPlayer(m_pSlave->GetGameSetting()->nPlayerID[i]);
		if ( pPlayer == NULL )
		{
			Data.PlayerID[i] = 0;
			Data.PlayersCardLen[i] = 0;
		}
		else
		{
			Data.PlayerID[i] = (unsigned short)m_pSlave->GetGameSetting()->nPlayerID[i];
			Data.PlayersCardLen[i] = (unsigned char)pPlayer->GetCardList()->Count();
		}
	}

	Data.DiscardDirection = (unsigned char)m_pSlave->GetIngameTable()->nDiscardDirection;
	Data.ActivePlayerID = (unsigned short)m_pSlave->GetActivePlayerID();

	// create card buffer
	int nBufLen = 0;
	for ( i=0 ; i<m_pSlave->GetGameSetting()->nNoOfPlayer ; i++ )
	{
		nBufLen += Data.PlayersCardLen[i];
	}
	char * pBuf = new char[sizeof(START_ROUND_MSG)+nBufLen];
	memcpy(pBuf,&Data,sizeof(START_ROUND_MSG));
	int idx = 0;

	for ( i=0 ; i<m_pSlave->GetGameSetting()->nNoOfPlayer ; i++ )
	{
		int nPlayerID = m_pSlave->GetGameSetting()->nPlayerID[i];
		if ( nPlayerID == NO_PLAYER )
			continue;
		C_Player * pPlayer = m_pSlave->GetPlayer(nPlayerID);
		if ( pPlayer == NULL )
        {
        	LogMessage("#Err21\n");
			continue;
        }
		for ( int j=0 ; j<pPlayer->GetCardList()->Count() ; j++ )
		{
			*(pBuf+sizeof(START_ROUND_MSG)+idx) = (unsigned char)*(pPlayer->GetCardList()->Items(j));
			idx++;
		}
	}

	SocketSend(pBuf,sizeof(START_ROUND_MSG)+nBufLen);
	LogMessage("SendStartRound\n");

	delete[] pBuf;
	return 0;
}

int C_SlaveComm::SendEndTurn()
{
	END_TURN_MSG Data;
	memset(&Data,0x00,sizeof(END_TURN_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_END_TURN_MSG;

	SocketSend((char*)&Data,sizeof(END_TURN_MSG));
	LogMessage("SendEndTurn\n");
	return 0;
}

int C_SlaveComm::SendEndRound()
{
	END_TURN_MSG Data;
	memset(&Data,0x00,sizeof(END_ROUND_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_END_ROUND_MSG;

	SocketSend((char*)&Data,sizeof(END_ROUND_MSG));
	LogMessage("SendEndRound\n");
	return 0;
}

int C_SlaveComm::SendSetGameData()
{
	SET_GAME_DATA_MSG Data;
	memset(&Data,0x00,sizeof(SET_GAME_DATA_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_SET_GAME_DATA_MSG;

	int i,j;
    Data.GameNum = (unsigned short)m_pSlave->GetGameNumber();
	Data.PlayMode = (unsigned char)m_pSlave->GetPlayMode();
	Data.ActivePlayerID = (unsigned short)m_pSlave->GetActivePlayerID();
	Data.NoOfPlayer = (unsigned char)m_pSlave->GetGameSetting()->nNoOfPlayer;
	Data.TongBeat2 = (unsigned char)m_pSlave->GetGameSetting()->bTongBeat2;
	Data.CheckSuit = (unsigned char)m_pSlave->GetGameSetting()->bCheckSuit;
	Data.DiscardType = (unsigned char)m_pSlave->GetIngameTable()->nDiscardType;
	Data.DiscardDirection = (unsigned char)m_pSlave->GetIngameTable()->nDiscardDirection;
	Data.LatestPlayerID = (unsigned short)m_pSlave->GetIngameTable()->nLatestPlayerID;

	for ( i=0 ; i<4 ; i++ )
		Data.LatestCard[i] = (unsigned char)m_pSlave->GetIngameTable()->LatestCard.nCard[i];

    for ( i=0 ; i<6 ; i++ )
    {
		Data.PlayerID[i] = (unsigned short)m_pSlave->GetGameSetting()->nPlayerID[i];
		Data.FinishPlayerID[i] = (unsigned short)m_pSlave->GetFinishTable()->nPlayerID[i];
		C_Player * p = m_pSlave->GetPlayer(Data.PlayerID[i]);
		if ( p == NULL )
			Data.PlayersCardLen[i] = 0;
		else
			Data.PlayersCardLen[i] = (unsigned char)(p->GetCardList()->Count());
    }

	// create card buffer
	int nBufLen = 0;
	for ( i=0 ; i<6 ; i++ )
	{
		nBufLen += Data.PlayersCardLen[i];
	}
	char * pBuf = new char[nBufLen];
	int idx = 0;
    for ( i=0 ; i<6 ; i++ )
    {
		C_Player * p = m_pSlave->GetPlayer(Data.PlayerID[i]);
		if ( p != NULL )
		{
			for ( j=0 ; j<p->GetCardList()->Count() ; j++ )
			{
				*(pBuf+idx) = (unsigned char)*(p->GetCardList()->Items(j));
				idx++;
			}
		}
    }

	char * pSumBuf = new char[sizeof(SET_GAME_DATA_MSG)+nBufLen];
	memcpy(pSumBuf,&Data,sizeof(SET_GAME_DATA_MSG));
	memcpy(pSumBuf+sizeof(SET_GAME_DATA_MSG),pBuf,nBufLen);
	SocketSend(pSumBuf,sizeof(SET_GAME_DATA_MSG)+nBufLen);
	
//	SocketSend((char*)&Data,sizeof(SET_GAME_DATA_MSG));
//	SocketSend(pBuf,nBufLen);
	delete[] pBuf;
	delete[] pSumBuf;
	LogMessage("SendSetGameData\n");
	return 0;
}

int C_SlaveComm::SendDiscard(int nPlayerID,int nDiscardType,LPDISCARD_CARD pCardNum)
{
	DISCARD_MSG Data;
	memset(&Data,0x00,sizeof(DISCARD_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_DISCARD_MSG;

	Data.PlayerID = (unsigned short) nPlayerID;
	Data.DiscardType = (unsigned char) nDiscardType;
    for ( int i=0 ; i<4 ; i++ )
	{
		if ( i>=nDiscardType )
			Data.CardNum[i] = 0;
		else
	        Data.CardNum[i] = (unsigned char) pCardNum->nCard[i];
	}

	SocketSend((char*)&Data,sizeof(DISCARD_MSG));
	LogMessage("SendDiscard(%d - %d,%d,%d,%d)\n",nPlayerID,
                pCardNum->nCard[0],
                pCardNum->nCard[1],
                pCardNum->nCard[2],
                pCardNum->nCard[3]);
	return 0;
}

int C_SlaveComm::SendPass(int nPlayerID)
{
	PASS_MSG Data;
	memset(&Data,0x00,sizeof(PASS_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_PASS_MSG;

	Data.PlayerID = (unsigned short) nPlayerID;

	SocketSend((char*)&Data,sizeof(PASS_MSG));
	LogMessage("SendPass(%d)\n",nPlayerID);
	return 0;
}

int C_SlaveComm::SendExchange(int nPlayerID,int nPosition,LPDISCARD_CARD pCard)
{
	EXCHANGE_MSG Data;
	memset(&Data,0x00,sizeof(EXCHANGE_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_EXCHANGE_MSG;

	Data.PlayerID = (unsigned short) nPlayerID;
	Data.Position = (unsigned char) nPosition;
	for ( int i = 0 ; i<4 ; i++ )
		Data.CardNum[i] = (unsigned char) pCard->nCard[i];

	SocketSend((char*)&Data,sizeof(EXCHANGE_MSG));
	LogMessage("SendExchange(%d)\n",nPosition);
	return 0;
}
// ==============================================================
//         game event handler
// ==============================================================

// Client -> Server
// first logon to server
int C_SlaveComm::OnLogon()
{
	LOGON_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(LOGON_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(LOGON_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnLogon msglen err %d\n",nRetVal);
		LogMessage("OnLogon msglen err %d\n",nRetVal);
		return FALSE;
	}

	char tmpbuf[64];
    // if no space left for additional player , close this
    if ( m_pSlave->GetComm()->GetClientList()->Count() > MAX_NO_OF_PLAYER-1 )
    {
		LogMessage("OnLogon playerfull\n");
		SendLogonFail(PLAYER_FULL_ERR);
		sprintf(tmpbuf,"/c4%s connection being deny",Data.Name);
		PopupMessage(tmpbuf);
		return FALSE;
    }

	sprintf(tmpbuf,"/c4%s come to join",Data.Name);
	PopupMessage(tmpbuf);

    SLAVEPLAYER SlvPlayer;
	SlvPlayer.nPlayerStatus = Data.Status;
    SlvPlayer.nCharacterID = Data.CharacterID;
	strcpy(SlvPlayer.szPlayerName,Data.Name);
	SlvPlayer.nNoOfKing = Data.NoOfKing;
	SlvPlayer.nNoOfSlave = Data.NoOfSlave;
	SlvPlayer.nNoOfGamePlay = Data.NoOfGamePlay;

    // create player 
	C_Player * pPlayer = m_pSlave->AddNewPlayer(&SlvPlayer);
	int nPlayerID = pPlayer->GetPlayerID();

	// if there are available space for new player
    // send Player ID back to client
	TRACE("[%s] logon, assigned id %d\n",Data.Name,nPlayerID);
	LogMessage("OnLogon assign %d to %s\n",nPlayerID,Data.Name);

    SendLogonOK(nPlayerID);

	// also send addplayer to other client
    for ( int i = 0 ; i < m_pSlave->GetPlayerList()->Count(); i++ )
    	if ( m_pSlave->GetPlayerList()->Items(i)->GetPlayerID() != nPlayerID )
        	SendAddPlayer(m_pSlave->GetPlayerList()->Items(i));

	if ( m_pSlave->GetPlayMode() == SETTING_PLAYMODE )
		SendSetGame(m_pSlave->GetGameSetting());
	else if ( m_pSlave->GetPlayMode() == INGAME_PLAYMODE ||
			m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE1 ||
			m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE2 )
	{
		SendStartGame(m_pSlave->GetGameSetting());
		// and game data too ......................
		SendSetGameData();
	}

	return TRUE;
}

// Server -> Client
// response of logon msg, return player ID of current game
int C_SlaveComm::OnLogonOK()
{
	LOGON_OK_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(LOGON_OK_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(LOGON_OK_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnLogonOK msglen err %d\n",nRetVal);
		LogMessage("OnLogonOK msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("logonok pid=%d\n",(int)Data.PlayerID);
	LogMessage("OnLogonOK %d\n",(int)Data.PlayerID);

	// set my Player ID given from server
	m_pSlave->GetMyPlayer()->SetPlayerID((int)Data.PlayerID);
	PopupMessage("/c4Connection accepted. enjoy");
	return TRUE;
}

// Server -> Client
// logon fail with errcode
int C_SlaveComm::OnLogonFail()
{
	LOGON_FAIL_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(LOGON_FAIL_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(LOGON_FAIL_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnLogonFail msglen err %d\n",nRetVal);
		LogMessage("OnLogonFail msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("logonfail ok [%d]\n",(int)Data.ErrCode);
	LogMessage("OnLogonFail %d\n",(int)Data.ErrCode);

	m_pSlave->QuitGame();
	switch ( Data.ErrCode )
	{
	case PLAYER_FULL_ERR:
		PopupMessage("/c12Slave Server is now full");
		break;
	case INVALID_MSGVER_ERR:
		PopupMessage("/c12You use the wrong version with dealer");
		PopupMessage("/c12Get the latest from my homepage");
		break;
	case KICK_OUT:
		PopupMessage("/c12You've been kicked out");
		break;
	}
	return TRUE;
}

// Server -> Client , Client -> Server
// tell about who is speaking and what they speak
int C_SlaveComm::OnChat()
{
	CHAT_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(CHAT_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(CHAT_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnChat msglen err %d\n",nRetVal);
		LogMessage("OnChat msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("chat ok [%s]\n",(char*)Data.ChatMsg);

	// other player send this msg , do that
	C_Player * pPlayer = m_pSlave->GetPlayer((int)Data.PlayerID);
	if ( pPlayer != NULL )
	{
		LogMessage("OnChat %d,%s\n",(int)Data.PlayerID,Data.ChatMsg);
		pPlayer->Chat((char*)Data.ChatMsg);
	}
	else
		LogMessage("OnChat fail togetplayer %d\n",(int)Data.PlayerID);
	return TRUE;
}

// Server -> Client
// Server notify client about player who have to be created on client
int C_SlaveComm::OnAddPlayer()
{
	ADD_PLAYER_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(ADD_PLAYER_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(ADD_PLAYER_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnAddPlayer msglen err %d\n",nRetVal);
		LogMessage("OnAddPlayer msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("add player [%s]\n",Data.Name);

	SLAVEPLAYER SlvPlayer;
	SlvPlayer.nPlayerStatus = Data.Status;
	SlvPlayer.nCharacterID = Data.CharacterID;
	strcpy(SlvPlayer.szPlayerName,Data.Name);
	SlvPlayer.nNoOfKing = Data.NoOfKing;
	SlvPlayer.nNoOfSlave = Data.NoOfSlave;
	SlvPlayer.nNoOfGamePlay = Data.NoOfGamePlay;

    // create player with ID
	C_Player * pPlayer = m_pSlave->AddNewPlayer(&SlvPlayer,Data.PlayerID);
	pPlayer->SetDiscardStatus(Data.DiscardStatus);
	pPlayer->SetPosition(Data.Position);
	pPlayer->SetNoOfBeenKing(Data.NoOfBeenKing);
	pPlayer->SetNoOfBeenQueen(Data.NoOfBeenQueen);
	LogMessage("OnAddPlayer addplayer %d\n",(int)Data.PlayerID);
	return TRUE;
}

// Server -> Client
// Server notify client about player who have to be removed on client
int C_SlaveComm::OnRemovePlayer()
{
	REMOVE_PLAYER_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(REMOVE_PLAYER_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(REMOVE_PLAYER_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnRemovePlayer msglen err %d\n",nRetVal);
		LogMessage("OnRemovePlayer msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("remove player [%d]\n",(int)Data.PlayerID);

    // remove player
	if ( m_pSlave->RemovePlayer((int)Data.PlayerID) )
		LogMessage("OnRemovePlayer %d\n",(int)Data.PlayerID);
	else
		LogMessage("OnRemovePlayer fail togetpid %d\n",(int)Data.PlayerID);

	return TRUE;
}

// Server -> Client
// Server notify client that is now setting up game, information also
int C_SlaveComm::OnSetGame()
{
	SET_GAME_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(SET_GAME_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(SET_GAME_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnSetGame msglen err %d\n",nRetVal);
		LogMessage("OnSetGame msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("Set game %d players\n",(int)Data.NoOfPlayer);

    // setting game
	START_GAME_SETTING GameSetting;
	for ( int i=0 ; i<6 ; i++ )
		GameSetting.nPlayerID[i] = Data.PlayerID[i];
	GameSetting.nNoOfPlayer = Data.NoOfPlayer;
	GameSetting.bTongBeat2 = Data.TongBeat2;
	GameSetting.bCheckSuit = Data.CheckSuit;

	// this message intend to update config changing to all clients.
	m_pSlave->UpdateSetting(&GameSetting);
	LogMessage("OnSetGame %d,%d\n",(int)Data.NoOfPlayer,(int)Data.TongBeat2);
	return TRUE;
}

// Server -> Client
// Server notify client about info of start game setting
int C_SlaveComm::OnStartGame()
{
	SET_GAME_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(START_GAME_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(START_GAME_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnStartGame msglen err %d\n",nRetVal);
		LogMessage("OnStartGame msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("start game\n");

    // start game
	START_GAME_SETTING GameSetting;
	for ( int i=0 ; i<6 ; i++ )
		GameSetting.nPlayerID[i] = Data.PlayerID[i];
	GameSetting.nNoOfPlayer = Data.NoOfPlayer;
	GameSetting.bTongBeat2 = Data.TongBeat2;
	GameSetting.bCheckSuit = Data.CheckSuit;
	
	m_pSlave->UpdateSetting(&GameSetting);
	m_pSlave->StartGame();
	PopupMessage("/c4 Start!");
	LogMessage("OnStartNewGame %d,%d,%d,%d,%d,%d - %d - %d\n",
				(int)Data.PlayerID[0],
				(int)Data.PlayerID[1],
				(int)Data.PlayerID[2],
				(int)Data.PlayerID[3],
				(int)Data.PlayerID[4],
				(int)Data.PlayerID[5],
				(int)Data.NoOfPlayer,
				(int)Data.TongBeat2);
	return TRUE;
}

// Server -> Client
// Server notify client that is now setting up game, information also
int C_SlaveComm::OnResetGame()
{
	RESET_GAME_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(RESET_GAME_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(RESET_GAME_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnResetGame msglen err %d\n",nRetVal);
		LogMessage("OnResetGame msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("reset game\n");

	m_pSlave->ResetGame();
	PopupMessage("/c4Game Reset");
	LogMessage("OnResetGame\n");
	return TRUE;
}

// Server -> Client
// Server send start round data , cards of each player
int C_SlaveComm::OnStartRound()
{
	START_ROUND_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(START_ROUND_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(START_ROUND_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnStartRound msglen err %d\n",nRetVal);
		LogMessage("OnStartRound msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("start round ok\n");
	int nCardBufLen = 0;
	int i,j;
	for ( i=0 ; i<m_pSlave->GetGameSetting()->nNoOfPlayer ; i++ )
		nCardBufLen += Data.PlayersCardLen[i];

	char * pBuf = new char[nCardBufLen];
	nRetVal = 0;
	if ( nCardBufLen > 0 )
	{
		while ( nRetVal == 0 )
			nRetVal = SocketRecv((char*)pBuf,nCardBufLen);
	}
	if ( nRetVal != nCardBufLen )
	{
		TRACE("OnStartRound msglen2 err %d\n",nRetVal);
		LogMessage("OnStartRound msglen2 err %d\n",nRetVal);
		return FALSE;
	}

	int idx = 0;
	for ( i=0 ; i<m_pSlave->GetGameSetting()->nNoOfPlayer ; i++ )
	{
		int nPlayerID = (int)Data.PlayerID[i];
		if ( nPlayerID == NO_PLAYER )
			continue;
		C_Player * pPlayer = m_pSlave->GetPlayer(nPlayerID);
		if ( pPlayer == NULL )
        {
        	LogMessage("#Err22\n");
			continue;
        }
   		pPlayer->GetCardList()->DeleteObjects();
		for ( j=0 ; j<Data.PlayersCardLen[i] ; j++ )
		{
			int * nCard = new int( *(pBuf+idx) );
			pPlayer->GetCardList()->Add(nCard);
			idx++;
		}
	}

	m_pSlave->GetIngameTable()->nDiscardDirection = (int)Data.DiscardDirection;
	for ( i=0 ; i<6 ; i++ )
		if ( m_pSlave->GetIngameTable()->nPlayerID[i] == (int)Data.ActivePlayerID )
		{
			m_pSlave->SetActivePlayerOrder(i);
			break;
		}

    m_pSlave->StartRound();
	LogMessage("OnStartRound\n");
	return TRUE;
}

// Server -> Client
// Server notify client to end turn
int C_SlaveComm::OnEndTurn()
{
	END_TURN_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(END_TURN_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(END_TURN_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnEndTurn msglen err %d\n",nRetVal);
		LogMessage("OnEndTurn msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("end turn\n");

	m_pSlave->EndTurn();
	LogMessage("OnEndTurn\n");
	return TRUE;
}

// Server -> Client
// Server notify client to end turn
int C_SlaveComm::OnEndRound()
{
	END_ROUND_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(END_ROUND_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(END_ROUND_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnEndRound msglen err %d\n",nRetVal);
		LogMessage("OnEndRound msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("end round\n");

	m_pSlave->EndRound();
	LogMessage("OnEndRound\n");
	return TRUE;
}

// Server -> Client
// Server send current game data
int C_SlaveComm::OnSetGameData()
{
	SET_GAME_DATA_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(SET_GAME_DATA_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(SET_GAME_DATA_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnSetGameData msglen err %d\n",nRetVal);
		LogMessage("OnSetGameData msglen err %d\n",nRetVal);
		return FALSE;
	}

	int nCardBufLen = 0;
	int i,j;
	for ( i=0 ; i<6 ; i++ )
	{
		nCardBufLen += Data.PlayersCardLen[i];
	}

	char * pBuf = new char[nCardBufLen];
	nRetVal = 0;
	if ( nCardBufLen > 0 )
	{
		while ( nRetVal == 0 )
			nRetVal = SocketRecv((char*)pBuf,nCardBufLen);
	}
	if ( nRetVal != nCardBufLen )
	{
		TRACE("OnSetGameData msglen2 err %d\n",nRetVal);
		LogMessage("OnSetGameData msglen2 err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("set game data ok\n");

	// set player's card
	int idx = 0;
	for ( i=0 ; i<6 ; i++ )
	{
		C_Player * p = m_pSlave->GetPlayer(Data.PlayerID[i]);
		if ( p != NULL )
		{
			for ( j=0 ; j<Data.PlayersCardLen[i] ; j++ )
			{
				int * nCard = new int( *(pBuf+idx) );
				p->GetCardList()->Add(nCard);
				idx++;
			}
		}
	}

	// set game data
	m_pSlave->SetGameNumber((int)Data.GameNum);
	m_pSlave->SetPlayMode((int)Data.PlayMode);
	m_pSlave->GetGameSetting()->nNoOfPlayer = (int)Data.NoOfPlayer;
	m_pSlave->GetGameSetting()->bTongBeat2 = (int)Data.TongBeat2;
	m_pSlave->GetGameSetting()->bCheckSuit = (int)Data.CheckSuit;
	m_pSlave->GetIngameTable()->nNoOfPlayer = (int)Data.NoOfPlayer;
	m_pSlave->GetIngameTable()->nDiscardType = (int)Data.DiscardType;
	m_pSlave->GetIngameTable()->nDiscardDirection = (int)Data.DiscardDirection;
	m_pSlave->GetIngameTable()->nLatestPlayerID = (int)Data.LatestPlayerID;
	for ( i=0 ; i<4 ; i++ )
		m_pSlave->GetIngameTable()->LatestCard.nCard[i] = (int)Data.LatestCard[i];

	for ( i=0 ; i<6 ; i++ )
		m_pSlave->GetFinishTable()->nPlayerID[i] = Data.FinishPlayerID[i];

	for ( i=0 ; i<6 ; i++ )
	{
		if ( Data.ActivePlayerID == (unsigned short)m_pSlave->GetIngameTable()->nPlayerID[i] )
        {
        	// assume must be found
			m_pSlave->SetActivePlayerOrder(i);
        	break;
        }
	}

	int nCardDropped=0;
	DISCARD_CARD card;
	for ( i=0 ; i<4 ; i++ )
	{
		card.nCard[i] = (int)Data.LatestCard[i];

		if ( Data.LatestCard[i] != 0 )
			nCardDropped++;
	}

	if ( nCardDropped > 0 )
	{
		C_Player * p = m_pSlave->GetPlayer(m_pSlave->GetIngameTable()->nLatestPlayerID);
		if ( p!=NULL )
			m_pSlave->GetUI()->Discard(p->GetTableOrder(),-1,
						nCardDropped,&card,SUDDEN_DISCARDMODE);
	}

	if ( m_pSlave->GetPlayMode() == INGAME_PLAYMODE )
	{
		C_Player * p = m_pSlave->GetPlayer(m_pSlave->GetActivePlayerID());
		if ( p != NULL )
			if ( p->GetDiscardStatus() != CARD_OVER )
				m_pSlave->GetUI()->GetPlayerDetailWnd(p->GetTableOrder())->ShowActive();
	}

	LogMessage("OnSetGameData\n");
	return TRUE;
}

// Server -> Client , Client -> Server
// Discard action
int C_SlaveComm::OnDiscard()
{
	DISCARD_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(DISCARD_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(DISCARD_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnDiscard msglen err %d\n",nRetVal);
		LogMessage("OnDiscard msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("discard ok pid[%d],card[%d,%d,%d,%d]\n",(int)Data.PlayerID,
			(int)Data.CardNum[0],(int)Data.CardNum[1],
			(int)Data.CardNum[2],(int)Data.CardNum[3] );

	// other player send this msg , do that
	C_Player * pPlayer = m_pSlave->GetPlayer((int)Data.PlayerID);
	if ( pPlayer != NULL )
	{
		if ( m_pSlave->GetPlayMode() == INGAME_PLAYMODE )
		{
			DISCARD_CARD Card;
			for ( int i=0 ; i<4 ; i++ )
				Card.nCard[i] = (int)Data.CardNum[i];
			pPlayer->Discard((int)Data.DiscardType,&Card);
			LogMessage("OnDiscard %d,%d - %d,%d,%d,%d\n",(int)Data.PlayerID,(int)Data.DiscardType,
						(int)Data.CardNum[0],
						(int)Data.CardNum[1],
						(int)Data.CardNum[2],
						(int)Data.CardNum[3] );
		}
		else
			LogMessage("### err com#01");
	}
	else
		LogMessage("OnDiscard fail togetpid %d\n",(int)Data.PlayerID);

	return TRUE;
}

// Server -> Client , Client -> Server
// Pass action
int C_SlaveComm::OnPass()
{
	PASS_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(PASS_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(PASS_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnPass msglen err %d\n",nRetVal);
		LogMessage("OnPass msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("pass ok pid[%d]\n",(int)Data.PlayerID);

	// other player send this msg , do that
	C_Player * pPlayer = m_pSlave->GetPlayer((int)Data.PlayerID);
	if ( pPlayer != NULL )
	{
		if ( m_pSlave->GetPlayMode() == INGAME_PLAYMODE )
		{
			pPlayer->Pass();
			LogMessage("OnPass %d\n",(int)Data.PlayerID);
		}
		else
			LogMessage("### err com#02");
	}
	else
		LogMessage("OnDiscard fail togetpid %d\n",(int)Data.PlayerID);

	return TRUE;
}

// Server -> Client , Client -> Server
// Exchange action
int C_SlaveComm::OnExchange()
{
	EXCHANGE_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(EXCHANGE_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(EXCHANGE_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnExchange msglen err %d\n",nRetVal);
		LogMessage("OnExchange msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("exchange ok position[%d]\n",(int)Data.Position);

	// other player send this msg , do that
	if ( m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE1 ||
			m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE2 )
	{
		DISCARD_CARD card;
		for ( int i = 0 ; i<4 ; i++ )
			card.nCard[i] = (int)Data.CardNum[i];

		m_pSlave->ExchangeCard((int)Data.PlayerID,(int)Data.Position,&card);
	}
	else
		LogMessage("### err com#03");

	return TRUE;
}

// ===========================================================
//  base communication event handler
// ===========================================================
void C_SlaveComm::OnAccept(int nErrCode)
{
	if ( nErrCode == WSAENETDOWN )
	{
		TRACE("## Error On Accept\n");
		return;
	}

	TRACE("Incoming Connection\n");
	// child create
	C_SlaveComm * pActSock = new C_SlaveComm(m_pSlave);
	RECT rc = {0,0,10,10};
	pActSock->Create("",0,WS_CHILD,
						rc,(C_Wnd*)m_pSlave->GetUI(),this);

	if ( AcceptSocket(pActSock) )
	{
		// accept ok
		// checking connection from the same ip
		//
		// continue if yes
		TRACE("ready to send/receive data\n");
	}
	else
	{
		delete pActSock;
		TRACE("can't accept\n");
	}
}

void C_SlaveComm::OnConnect(int nErrCode)
{
    m_bConnecting = FALSE;
	KillTimer(m_hWnd,CONNECTION_TIMERID);
	switch ( nErrCode )
	{
	case WSAEADDRINUSE:		//The specified address is already in use.
	case WSAEADDRNOTAVAIL:	//The specified address is not available from the local machine.
	case WSAEAFNOSUPPORT:	//Addresses in the specified family cannot be used with this socket.
	case WSAECONNREFUSED:	//The attempt to connect was forcefully rejected.
	case WSAENETUNREACH:	//The network cannot be reached from this host at this time.
	case WSAEFAULT:			//The namelen argument is incorrect.
	case WSAEINVAL:			//The socket is already bound to an address.
	case WSAEISCONN:		//The socket is already connected.
	case WSAEMFILE:			//No more file descriptors are available.
	case WSAENOBUFS:		//No buffer space is available. The socket cannot be connected.
	case WSAENOTCONN:		//The socket is not connected.
	case WSAETIMEDOUT:		//Attempt to connect timed out without establishing a connection.
		{
		char tmpbuf[50];
		TRACE("error on connect\n");
		InvalidateRect(m_hWnd,NULL,FALSE);
        // now command quit game 'll blink wnd by unnecessary.
		m_pSlave->QuitGame();
		sprintf(tmpbuf,"/c12Can't join to dealer [err#%d]",nErrCode);
		PopupMessage(tmpbuf);
		}
		return;
	}

	// connect to server OK , send my player's info.
    InvalidateRect(m_hWnd,NULL,FALSE);
	SendLogon(m_pSlave->GetMyPlayer());
}

void C_SlaveComm::OnRecv(int nErrCode)
{
	if ( nErrCode == WSAENETDOWN )
	{
		TRACE("   @:~(   Error On Recv\n");
		return;
	}

	HEADER_MSG Header;
	memset(&Header,0x00,sizeof(HEADER_MSG));
	int nRetVal;

	nRetVal = SocketRecv((char*)&Header,sizeof(HEADER_MSG));
	// recv unspecify msg length
	if ( nRetVal != sizeof(HEADER_MSG) ) return;

	TRACE("header %04x ,msg %d  | ",Header.Version,Header.MsgID);

	if ( Header.Version == MSG_VERSION )
	{
		switch ( Header.MsgID )
		{
		case ID_LOGON_MSG: OnLogon(); break;
		case ID_LOGON_OK_MSG: OnLogonOK(); break;
		case ID_LOGON_FAIL_MSG: OnLogonFail(); break;
		case ID_CHAT_MSG: OnChat(); break;
		case ID_ADD_PLAYER_MSG: OnAddPlayer(); break;
		case ID_REMOVE_PLAYER_MSG: OnRemovePlayer(); break;
		case ID_SET_GAME_MSG: OnSetGame(); break;
		case ID_START_GAME_MSG: OnStartGame(); break;
		case ID_RESET_GAME_MSG: OnResetGame(); break;
		case ID_START_ROUND_MSG: OnStartRound(); break;
		case ID_END_TURN_MSG: OnEndTurn(); break;
		case ID_END_ROUND_MSG: OnEndRound(); break;
		case ID_SET_GAME_DATA_MSG: OnSetGameData(); break;
		case ID_DISCARD_MSG: OnDiscard(); break;
		case ID_PASS_MSG: OnPass(); break;
		case ID_EXCHANGE_MSG: OnExchange(); break;
		default:
			break;
		}
	}
	else
	{
		// message version mismatch handler
		if ( Header.MsgID == ID_LOGON_MSG )
		{
            LOGON_MSG Data;
//            int nRetVal;

            SocketRecv((char*)&Data + sizeof(HEADER_MSG),
                                sizeof(LOGON_MSG)-sizeof(HEADER_MSG));
			// version number mismatch.

			char tmpbuf[50];
			sprintf(tmpbuf,"/c12%s's version mismatch",Data.Name);
			PopupMessage(tmpbuf);
			sprintf(tmpbuf,"/c12%s connection being deny",Data.Name);
			PopupMessage(tmpbuf);
			TRACE("deny incoming connect ** version mismatch **\n");

            {
            	// send logonfail with the same version of incoming connection
            	LOGON_FAIL_MSG Data;
                memset(&Data,0x00,sizeof(LOGON_FAIL_MSG));

                Data.Header.Version = Header.Version;
                Data.Header.MsgID = ID_LOGON_FAIL_MSG;

                Data.ErrCode = (unsigned short) INVALID_MSGVER_ERR;

                SocketSend((char*)&Data,sizeof(LOGON_FAIL_MSG));
            }
		}
	}

}

void C_SlaveComm::OnSend(int nErrCode)
{
	if ( nErrCode == WSAENETDOWN )
	{
		TRACE("Error On Send\n");
		return;
	}
}

void C_SlaveComm::OnClose(int nErrCode)
{
	switch ( nErrCode )
	{
	case WSAENETDOWN:		//The network subsystem has failed.
	case WSAECONNRESET:		//The connection was reset by the remote side.
	case WSAECONNABORTED:	//The connection was terminated due to a time-out or other failure.
		TRACE("Error On Close [%d]\n",nErrCode);
		CloseSocket();
		break;
	}

	KillTimer(m_hWnd,CONNECTION_TIMERID);
	m_bConnecting = FALSE;
	m_fdSock = INVALID_SOCKET;
    if ( IsWindow(m_hWnd) ) InvalidateRect(m_hWnd,NULL,FALSE);
	if ( m_pParentSock == NULL )
	{
		// parent socket , go back to idle mode
		// both server and client mode
//        MessageBox(m_hWnd,"close event","",MB_OK);
		CloseSocket();
	    if ( IsWindow(m_hWnd) ) InvalidateRect(m_hWnd,NULL,FALSE);
		TRACE("Close Server Socket\n");
		m_pSlave->QuitGame();
		PopupMessage("/c4Quit from the game");
	}
	else
	{
		// passive close
		CloseSocket();
	    if ( IsWindow(m_hWnd) ) InvalidateRect(m_hWnd,NULL,FALSE);
		// child socket only in server mode
		// remove this player out
		TRACE("Close Client Socket\n");
		if ( m_pPlayer != NULL )
		{
			char tmpbuf[50];
			sprintf(tmpbuf,"/c4%s quit. bye",m_pPlayer->GetName());
			PopupMessage(tmpbuf);
			m_pSlave->RemovePlayer(m_pPlayer->GetPlayerID());
		}
		// this child socket is automaticly deleted by parent socket
	}
}

// ===================================================
// Event handler
// ===================================================
LRESULT C_SlaveComm::OnPaint(WPARAM,LPARAM)
{
	static bBlink = TRUE;
	PAINTSTRUCT ps;
	m_hDC = BeginPaint( m_hWnd, &ps );

	if ( m_fdSock == INVALID_SOCKET )
		DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_SVOFFLINE)));
	else
	{
		if ( m_bConnecting )
		{
			if ( bBlink )
				DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_SVCONNECT)));
			else
				DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_SVCONNECT1)));
			bBlink = !bBlink;
		}
		else
			DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_SVONLINE)));
	}
	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT C_SlaveComm::OnLButtonUp(WPARAM,LPARAM)
{
	PopupMessage(NULL);
	return 0;
}

LRESULT C_SlaveComm::OnRButtonUp(WPARAM,LPARAM)
{
	C_NetStatDlg NetStatDlg(this);
	NetStatDlg.DoModal();
	return 0;
}

LRESULT C_SlaveComm::OnEraseBkgnd(WPARAM,LPARAM)
{
	return 1;
}

LRESULT C_SlaveComm::OnTimer(WPARAM,LPARAM)
{
	RECT rc;
	GetClientRect(GetHandle(),&rc);
	ClientToScreen(GetHandle(),(LPPOINT)&rc.left);
	ClientToScreen(GetHandle(),(LPPOINT)&rc.right);
	ScreenToClient(GetParent(GetHandle()),(LPPOINT)&rc.left);
	ScreenToClient(GetParent(GetHandle()),(LPPOINT)&rc.right);
	InvalidateRect(GetParent(GetHandle()),&rc,FALSE);
//	InvalidateRect(m_hWnd,NULL,FALSE);
	return 0;
}

