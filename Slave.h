#ifndef _C_SLAVE_H_
#define _C_SLAVE_H_

#include "SlaveApp.h"
#include "Player.h"

#define MYID					1
#define NO_PLAYER				0

#define NO_OF_PLAYER			6
#define MAX_NO_OF_PLAYER		8

typedef MyList<C_Player> PLAYER_LIST;

enum { SERVER_GAMEMODE,CLIENT_GAMEMODE,IDLE_GAMEMODE };
enum { INGAME_PLAYMODE,SETTING_PLAYMODE,IDLE_PLAYMODE,EXCHANGE_PLAYMODE1,EXCHANGE_PLAYMODE2 };
enum { NORMAL_DIRECTION,REVERSE_DIRECTION };
// Finish table 
enum { NORMAL_ORDER,PUT_LASTPLAYER };
// isvalid discard
enum {DISCARD_OK,DISCARD_INVALID,DISCARD_TONGCANTBEAT2,DISCARD_SUITINVALID};

typedef struct _START_GAME_SETTTING
{
	int nPlayerID[6];
	int nNoOfPlayer;
	int bTongBeat2;
	int bCheckSuit;
} START_GAME_SETTING,*LPSTART_GAME_SETTING;

typedef struct _INGAME_TABLE
{
	int nPlayerID[6];
	int nNoOfPlayer;
	int nLatestPlayerID;
	int nDiscardType;
	DISCARD_CARD LatestCard;
	int nDiscardDirection;
} INGAME_TABLE,*LPINGAME_TABLE;

typedef struct _FINISH_TABLE
{
	int nPlayerID[6];
} FINISH_TABLE,*LPFINISH_TABLE;

class C_SlaveWnd;
class C_SlaveComm;
class C_SlvPing;

class C_Slave
{
public:
	C_Slave();
	~C_Slave();

    int Initialize();

    int StartServer();
    int StartClient(char * lpszJoinIP);
	int QuitGame();
	int ResetGame();
	int StartGame();
	int StartRound();
	int StartNewTurn();
	int StartTurn();
    int NextTurn();
	int EndTurn();
	int EndRound();

	int SetNextDiscarder();

	int DistributeCard();
	int PutFinishPlayer(int nPlayerID,int nType);

	int AddMyPlayer(LPSLAVEPLAYER PlayerInfo);
	C_Player * AddNewPlayer(LPSLAVEPLAYER PlayerInfo,int nPlayerID=-1);
	int RemovePlayer(int nPlayerID);

	int PickUpCard();
	int Pass(int nPlayerID);
	int Discard(int nPlayerID,int nDiscardType,LPDISCARD_CARD pCardNum);
	int Chat(int nPlayerID,char *pszMsg);
	int ExchangeCard(int nPlayerID,int nPosition,LPDISCARD_CARD pCard);

	int IsValidDiscard(int nDiscardType,LPDISCARD_CARD pCardNum);

    int GetGameMode() { return m_nGameMode; }
    void SetGameMode(int nMode) { m_nGameMode = nMode; }
	int GetPlayMode() { return m_nPlayMode;	}
	void SetPlayMode(int nMode);
	int GetGameNumber() { return m_nGameNum; }
    void SetGameNumber(int nGameNum) { m_nGameNum = nGameNum; }
	
	int GetActivePlayerID() { return m_IngameTable.nPlayerID[m_nActivePlayerOrder]; }
	int GetActivePlayerOrder() { return m_nActivePlayerOrder; }
	void SetActivePlayerOrder(int nActivePlayerOrder) { m_nActivePlayerOrder = nActivePlayerOrder; }

	int UpdateSetting(LPSTART_GAME_SETTING pGameSetting);
	LPSTART_GAME_SETTING GetGameSetting() { return &m_GameSetting; }
    LPINGAME_TABLE GetIngameTable() { return &m_IngameTable; }
	LPFINISH_TABLE GetFinishTable() { return &m_FinishTable; }

	PLAYER_LIST * GetPlayerList() { return &m_PlayerList; }
	C_Player * GetPlayer(int nPlayerID);
	C_Player * GetPlayerByPosition(int nPosition);

    C_Player * GetMyPlayer() { return m_pMyPlayer; }

    C_SlaveWnd * GetUI() { return m_pSlaveWnd; }
    C_SlaveComm * GetComm() { return m_pComm; }

	void CheckResponse(int nPlayerID,C_SlaveComm *);

private:

	// UI Object
	C_SlaveWnd * m_pSlaveWnd;

    // game info
	int m_nRunningPlayerID;
    int m_nGameMode;
	int m_nPlayMode;
	int m_nGameNum;				// 0->xx
	int m_nActivePlayerOrder;	// 0-3 (index of m_IngameTable)
	int m_nIngameOffset;

	START_GAME_SETTING m_GameSetting;
    INGAME_TABLE m_IngameTable;
	FINISH_TABLE m_FinishTable;	// keep order of player

	PLAYER_LIST m_PlayerList;
    C_Player * m_pMyPlayer;

    C_SlaveComm * m_pComm;
	C_SlvPing * m_pPing;

};

#endif