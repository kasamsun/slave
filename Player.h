#ifndef _C_PLAYER_H_
#define _C_PLAYER_H_

#include <MyTempl.h>
#include "SlaveApp.h"

#define MAX_PLAYER_NAME	16
#define NOT_IN_TABLE	-1

enum { IAM_PLAYER,IAM_SPECTATOR,IAM_COMPUTER };
enum { IAM_KING,IAM_QUEEN,IAM_PEOPLE,IAM_VICESLAVE,IAM_SLAVE };
enum { WAITFORDISCARD,DISCARDING,LAST_DISCARD,PASS_TURN,CARD_OVER };

typedef struct _SLAVEPLAYER
{
	int nPlayerStatus;
    int nCharacterID;
	char szPlayerName[MAX_PLAYER_NAME];

	int nNoOfGamePlay;
	int nNoOfKing;
	int nNoOfSlave;
} SLAVEPLAYER, *LPSLAVEPLAYER;

class C_Slave;
class C_PlayerWnd;
class C_SlaveAI;

class C_Player
{
public:
	C_Player();
	C_Player(C_Slave * pSlave,int nPlayerID,LPSLAVEPLAYER PlayerInfo);
	~C_Player();

    void operator=(C_Player&);

	int Chat(char *lpszMsg);
	int Discard(int nDiscardType,LPDISCARD_CARD pCardNum);
	int Pass();

	CARD_LIST * GetCardList() { return &m_Card; }

    int GetCharacterID() { return m_nCharacterID; }
    void SetCharacterID(int nCharID) { m_nCharacterID = nCharID; }
	int GetStatus() { return m_nPlayerStatus; }
    void SetStatus(int nPlayerStatus) { m_nPlayerStatus = nPlayerStatus; }
	char * GetName() { return m_szPlayerName; }
    void SetName(LPCSTR lpszName);

	int GetNoOfSlave() { return m_nNoOfSlave; }
    void SetNoOfSlave(int nNoOfSlave) { m_nNoOfSlave = nNoOfSlave; }
	int GetNoOfKing() { return m_nNoOfKing; }
    void SetNoOfKing(int nNoOfKing) { m_nNoOfKing = nNoOfKing; }
	int GetNoOfGamePlay() { return m_nNoOfGamePlay; }
    void SetNoOfGamePlay(int nNoOfGamePlay) { m_nNoOfGamePlay = nNoOfGamePlay; }

	int GetNoOfBeenKing() { return m_nNoOfBeenKing; }
    void SetNoOfBeenKing(int nNoOfBeenKing) { m_nNoOfBeenKing = nNoOfBeenKing; }
	int GetNoOfBeenQueen() { return m_nNoOfBeenQueen; }
    void SetNoOfBeenQueen(int nNoOfBeenQueen) { m_nNoOfBeenQueen = nNoOfBeenQueen; }

    int GetTableOrder() { return m_nTableOrder; }
    void SetTableOrder(int nTableOrder) { m_nTableOrder = nTableOrder; }

	int GetPosition() { return m_nPosition; }
	void SetPosition(int nPosition) { m_nPosition = nPosition; }
	int GetDiscardStatus() { return m_nDiscardStatus; }
	void SetDiscardStatus(int nDiscardStatus) { m_nDiscardStatus = nDiscardStatus; }

	C_Slave * GetSlave() { return m_pSlave; }
    int GetPlayerID() { return m_nPlayerID; }
	void SetPlayerID(int nPlayerID) { m_nPlayerID = nPlayerID; }

	int ActivateAI();
	C_SlaveAI * GetSlaveAI() { return m_pSlaveAI; }
	void SetSlaveAI(C_SlaveAI *pSlaveAI) { m_pSlaveAI = pSlaveAI; }

    C_PlayerWnd * GetUI() { return m_pPlayerWnd; }
    void SetUI(C_PlayerWnd *pPlayerWnd) { m_pPlayerWnd = pPlayerWnd; }

protected:

private:
	CARD_LIST m_Card;

	// personel data
	int m_nPlayerStatus;	// IAM_PLAYER,IAM_SPECTATOR,IAM_COMPUTER

    int m_nCharacterID;
	char m_szPlayerName[MAX_PLAYER_NAME];

	int m_nNoOfKing;
	int m_nNoOfSlave;
	int m_nNoOfGamePlay;

	int m_nNoOfBeenKing;	// have been king for xx times
	int m_nNoOfBeenQueen;	// have been queen for xx times

    // table position of this player ( if i'm player )
    int m_nTableOrder;

	int m_nPosition; // IAM_KING,IAM_QUEEN,IAM_PEOPLE,IAM_VICESLAVE,IAM_SLAVE
	int m_nDiscardStatus; // WAITFORDISCARD,DISCARDING,LAST_DISCARD,PASS_TURN,CARD_OVER

	C_Slave * m_pSlave;
    int m_nPlayerID;

    // AI Object
	C_SlaveAI * m_pSlaveAI;
    // UI Object
    C_PlayerWnd * m_pPlayerWnd;
};

#endif
