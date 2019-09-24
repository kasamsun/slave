#ifndef _C_STARTGAMEWND_H_
#define _C_STARTGAMEWND_H_

#define STARTGAMEWND_WIDTH	270
#define STARTGAMEWND_HEIGHT	340

#include <GenericWnd.h>
#include "Slave.h"
#include <Tooltip.h>

class C_SlaveWnd;
class C_CoolButton;
class C_PlayerWnd;
class C_Player;

class C_StartGameWnd : public C_GenericWnd
{
public:
	C_StartGameWnd(C_SlaveWnd * pSlaveWnd,LPSTART_GAME_SETTING pSetting=NULL);
	~C_StartGameWnd();

	C_Slave * GetSlave() { return m_pSlave; }

	int AddPlayer(C_Player * pPlayer);
    int RemovePlayer(C_Player *pPlayer);

	int SetPlayer(int nPlayerID,int nOrder);
	int ClearPlayer(int nOrder);
	int MovePlayer(int nSlotID,int nOrder,int nOldOrder);

	int SetGame(LPSTART_GAME_SETTING pGameSetting);
	int SendUpdateSetting();

    void Refresh();

	// Message declaration
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBkgnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonDown(WPARAM wParam,LPARAM lParam);

	LRESULT OnPlayerWndBeginDrag(WPARAM wParam,LPARAM lParam);
	LRESULT OnPlayerWndEndDrag(WPARAM wParam,LPARAM lParam);
	LRESULT OnPlayerWndRButtonClick(WPARAM wParam,LPARAM lParam);
private:
	HDC m_hdcBkMem;
	HBITMAP m_hbmpBkMem,m_hbmpBkTmp;
	C_Tooltip m_Tooltip;

	BOOL m_bInitBkg;
	// Main jubmoo engine
	C_Slave * m_pSlave;

	C_PlayerWnd * m_pPlayerWndLotStatus[8];
	int m_nLotStatus[8];
	int m_nDragSlot;

	int m_nSelectedSlotID[6];
	int m_nNoOfPlayer;

	// command button
    C_CoolButton * m_pContinueButton;
	C_CoolButton * m_pGroup;
	C_CoolButton * m_p3PlayerGame;
	C_CoolButton * m_p4PlayerGame;
	C_CoolButton * m_p5PlayerGame;
	C_CoolButton * m_p6PlayerGame;
	C_CoolButton * m_pTongBeat2;
	C_CoolButton * m_pCheckSuit;

	HRGN m_hrgnPos[6];
	POINT m_pntLastPos;
};

#endif