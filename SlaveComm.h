#ifndef _C_SLAVECOMM_H_
#define _C_SLAVECOMM_H_

#include "kswafx/MyWinsock.h"
#include "Slave.h"
#include <stdio.h>

// ==========================================================
// MSG_VERSION
//	0x0300	- First Released
//	0x0301  - add set game data message
//	0x0302  - add option - TongBeat2
//  0x0303  - Slave engine changed - end turn - check game over
//          - fix SendSetGameData, OnSetGameData
//          - add option - CheckSuit
//          - add SlvPing
// ==========================================================
#define MSG_VERSION				0x0303

#define ID_LOGON_MSG			0x0001
#define ID_LOGON_OK_MSG			0x0002
#define ID_LOGON_FAIL_MSG		0x0003

#define ID_CHAT_MSG				0x0010
#define ID_DISCARD_MSG			0x0011
#define ID_PASS_MSG             0x0012
#define ID_EXCHANGE_MSG			0x0013

#define ID_ADD_PLAYER_MSG		0x0020
#define ID_REMOVE_PLAYER_MSG	0x0021

#define ID_SET_GAME_MSG			0x0031
#define ID_START_GAME_MSG		0x0032
#define ID_RESET_GAME_MSG		0x0033

#define ID_SET_GAME_DATA_MSG	0x0040
#define ID_START_ROUND_MSG		0x0041
#define ID_END_TURN_MSG			0x0042
#define ID_END_ROUND_MSG		0x0043

// below is not implemented yet

// err code for logonfail handler
#define PLAYER_FULL_ERR			0x0001
#define INVALID_MSGVER_ERR		0x0002
#define KICK_OUT				0x0101

typedef struct _HEADER_MSG
{
	unsigned short Version;
	unsigned short MsgID;
} HEADER_MSG;

typedef struct _LOGON_MSG
{
	HEADER_MSG Header;
	char Name[16];
	unsigned short PlayerID;
	unsigned char Status;
	unsigned char CharacterID;
	unsigned short NoOfKing;
	unsigned short NoOfSlave;
	unsigned short NoOfGamePlay;
} LOGON_MSG;

typedef struct _LOGON_OK_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
} LOGON_OK_MSG;

typedef struct _LOGON_FAIL_MSG
{
	HEADER_MSG Header;
	unsigned short ErrCode;
} LOGON_FAIL_MSG;

typedef struct _CHAT_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
    char ChatMsg[41];					 // chatedit char limit = 40
} CHAT_MSG;

typedef struct _ADD_PLAYER_MSG
{
	HEADER_MSG Header;
	char Name[16];
	unsigned short PlayerID;
	unsigned char Status;
	unsigned char CharacterID;
	unsigned short NoOfKing;
	unsigned short NoOfSlave;
	unsigned short NoOfBeenKing;
	unsigned short NoOfBeenQueen;
	unsigned short NoOfGamePlay;
	unsigned char Position;
	unsigned char DiscardStatus;
} ADD_PLAYER_MSG;

typedef struct _REMOVE_PLAYER
{
	HEADER_MSG Header;
	unsigned short PlayerID;
} REMOVE_PLAYER_MSG;

typedef struct _SET_GAME_MSG
{
	HEADER_MSG Header;

	unsigned short PlayerID[6];
	unsigned char NoOfPlayer;
	unsigned char TongBeat2;
	unsigned char CheckSuit;
} SET_GAME_MSG, *LPSET_GAME_MSG;

typedef struct _START_GAME_MSG
{
	HEADER_MSG Header;

	unsigned short PlayerID[6];
	unsigned char NoOfPlayer;
	unsigned char TongBeat2;
	unsigned char CheckSuit;
} START_GAME_MSG, *LPSTART_GAME_MSG;

typedef struct _RESET_GAME_MSG
{
	HEADER_MSG Header;

} RESET_GAME_MSG, *LPRESET_GAME_MSG;

typedef struct _START_ROUND_MSG
{
	HEADER_MSG Header;

    unsigned short PlayerID[6];
    unsigned char PlayersCardLen[6];
	unsigned short ActivePlayerID;
	unsigned char DiscardDirection;

} START_ROUND_MSG, *LPSTART_ROUND_MSG;

typedef struct _END_TURN_MSG
{
	HEADER_MSG Header;

} END_TURN_MSG, *LPEND_TURN_MSG;

typedef struct _END_ROUND_MSG
{
	HEADER_MSG Header;

} END_ROUND_MSG, *LPEND_ROUND_MSG;

typedef struct _SET_GAME_DATA_MSG
{
	HEADER_MSG Header;

    unsigned short PlayerID[6];
    unsigned char PlayersCardLen[6];
	unsigned short ActivePlayerID;
    unsigned short GameNum;
	unsigned char NoOfPlayer;
	unsigned char TongBeat2;
	unsigned char CheckSuit;
	unsigned char DiscardDirection;
	unsigned char DiscardType;
	unsigned short LatestPlayerID;
	unsigned char LatestCard[4];
	unsigned char PlayMode;
	unsigned short FinishPlayerID[6];
} SET_GAME_DATA_MSG, *LPSET_GAME_DATA_MSG;

typedef struct _DISCARD_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
    unsigned char DiscardType;
    unsigned char CardNum[4];
} DISCARD_MSG, *LPDISCARD_MSG;

typedef struct _PASS_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
} PASS_MSG, *LPPASS_MSG;

typedef struct _EXCHANGE_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
	unsigned char Position;
    unsigned char CardNum[4];
} EXCHANGE_MSG, *LPEXCHANGE_MSG;


class C_Slave;
class C_Player;
class C_MsgPopupWnd;

class C_SlaveComm : public C_MyWinsock
{
public:
	C_SlaveComm(C_Slave *pSlave);
	~C_SlaveComm();

    // interface
    int SendLogon(C_Player *pPlayer);
    int SendLogonOK(int nPlayerID);
    int SendLogonFail(int nErrCode);
	int SendChat(int nPlayerID,LPCSTR lpszMsg);
    int SendAddPlayer(C_Player *pPlayer);
    int SendRemovePlayer(int nPlayerID);
	int SendSetGame(LPSTART_GAME_SETTING pSetGame);
	int SendStartGame(LPSTART_GAME_SETTING pSetGame);
    int SendResetGame();
	int SendStartRound();
	int SendEndTurn();
	int SendEndRound();
    int SendSetGameData();
    int SendDiscard(int nPlayerID,int nDiscardType,LPDISCARD_CARD pCardNum);
    int SendPass(int nPlayerID);
	int SendExchange(int nPlayerID,int nPosition,LPDISCARD_CARD pCardNum);

	// msg processing
	int OnLogon();
    int OnLogonOK();
    int OnLogonFail();
    int OnChat();
    int OnAddPlayer();
	int OnRemovePlayer();
	int OnSetGame();
	int OnStartGame();
    int OnResetGame();
	int OnStartRound();
	int OnEndTurn();
	int OnEndRound();
    int OnSetGameData();
    int OnDiscard();
    int OnPass();
	int OnExchange();

	BOOL CreateServerSocket(unsigned short nPortNum);
    BOOL CreateClientSocket(unsigned short nPortNum,char *szDotAddr);
	int Close();

	virtual void OnAccept(int nErrCode);
	virtual void OnConnect(int nErrCode);
	virtual void OnRecv(int nErrCode);
	virtual void OnSend(int nErrCode);
	virtual void OnClose(int nErrCode);
	virtual void OnOOB(int nErrCode) {};

	// Message declaration
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBkgnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam,LPARAM lParam);

    C_Player * GetPlayer() { return m_pPlayer; }
	void SetPlayer(C_Player *pPlayer) { m_pPlayer = pPlayer; }
	C_Slave * GetSlave() { return m_pSlave; }
	
	void PopupMessage(LPCTSTR pszMsg);
	int CreateMsgWnd();

	void CreateLogMessage();
	void OpenLogMessage();
	void CloseLogMessage();
	void LogMessage(LPCTSTR lpszFormat, ...);

	void CheckResponse();
private:
	C_Slave * m_pSlave;
	C_Player * m_pPlayer;
    int m_bConnecting;
    C_MsgPopupWnd * m_pMsgWnd;
	FILE * m_fpNetLog;
};

#endif
