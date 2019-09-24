#ifndef _C_SLAVEWND_H_
#define _C_SLAVEWND_H_

#include <GenericWnd.h>
#include <MyTempl.h>
#include <Tooltip.h>
#include "PickCardWnd.h"
#include "Slave.h"

#define SLAVEWND_WIDTH		660
#define SLAVEWND_HEIGHT		480

enum { BUTTON_START,BUTTON_JOIN,BUTTON_QUIT,BUTTON_SETTING,
        BUTTON_HELP,BUTTON_ABOUT,BUTTON_EXIT };

#define MAX_SLAVE_BUTTON   	7
#define SUDDEN_DISCARDMODE	1

typedef MyList<C_PickCardWnd> MYCARD_LIST;
typedef MyList<C_PickCardWnd> DROPCARD_LIST;

class C_Slave;
class C_CoolButton;
class C_PlayerDetailWnd;
class C_Player;
class C_ChatEdit;
class C_SettingDlg;
class C_StartGameWnd;

class C_SlaveWnd : public C_GenericWnd
{
	friend C_SettingDlg;
public:
	C_SlaveWnd(C_Slave * pSlave);
	~C_SlaveWnd();

	C_Slave * GetSlave() { return m_pSlave; }

	void InitScreen();
	void SetImage();
    int ClearImage();
	void CreateSlaveFont();
    HFONT GetSlaveFont() { return m_hFont; }
    HFONT GetBigSlaveFont() { return m_hBigFont; }
	C_PlayerDetailWnd * GetPlayerDetailWnd(int nOrder) { return m_pPlayerDetailWnd[nOrder]; }
	void SetGameSpeed(int nAIDelay) { m_nAIDelay = nAIDelay; }

	// Utility functions
	void MoveToCenter();

    // Interface
	int ShowSettingGame(LPSTART_GAME_SETTING pGameSetting=NULL);
	int ShowStartGame();
	int ShowGameResult();
	int ShowIdleGame();
	int AddSpectatorUI(C_Player * pPlayer);
	int RemoveSpectatorUI(C_Player * pPlayer);
	int SetPlayerUI(int nPos,C_Player * pPlayer);
	int ClearPlayerUI(int nPos);
	void RefreshPlayersTable(int nPos);
	int SetCardOnHand();
	int ClearCardOnHand();
    int RearrangeCardOnHand();
	int ClearDropCard();
	void RefreshDiscardDirection();

	int RequestDiscard();
	int Discard(int nPos,int nLastPos,int nDiscardType,
				LPDISCARD_CARD pCardNum,int nDiscardMode=0);
	int Pass(int nPos);
	int PickUpCard();
	LRESULT OnPickUpCardTimer();

	int RequestExchange();

	MYCARD_LIST * GetMyCardList() { return &m_MyCardList; }
	// Message declaration
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnNCLButtonDblClk(WPARAM wParam,LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam,LPARAM lParam);
	LRESULT OnClose(WPARAM wParam,LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam,LPARAM lParam);
	LRESULT OnChar(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonDown(WPARAM wParam,LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam,LPARAM lParam);

	// user message handler
    LRESULT OnStartButtonClick();
    LRESULT OnJoinButtonClick();
    LRESULT OnQuitButtonClick();
	LRESULT OnAIDiscardRequest();
	LRESULT OnCardWndLButtonClick(WPARAM wParam,LPARAM lParam);
	LRESULT OnCardWndRButtonClick(WPARAM wParam,LPARAM lParam);
	LRESULT OnPickCardReady(WPARAM wParam,LPARAM lParam);

private:
	int DiscardSelectedCard();

	HDC m_hdcBkMem;
	HBITMAP m_hbmpBkMem,m_hbmpBkTmp;
	HFONT m_hFont,m_hBigFont;
	C_Tooltip m_Tooltip;

	// Main slave engine
	C_Slave * m_pSlave;

	// Command Button
    C_CoolButton * m_pButton[MAX_SLAVE_BUTTON];
	// player label
    C_PlayerDetailWnd * m_pPlayerDetailWnd[6];
    // player lot status
    C_Player * m_pPlayerLotStatus[8];
	// pass button
    C_CoolButton * m_pPassButton;
	// exchange button
    C_CoolButton * m_pExchangeButton;

	C_StartGameWnd * m_pStartGameWnd;

	// input windows
    C_ChatEdit * m_pChatEdit;
	MYCARD_LIST m_MyCardList;
	DROPCARD_LIST m_DropCardList;

	DISCARD_CARD m_SelectedCard; // card to be discard

	int m_bPickingUp;	// flag is to check picking up , prevent drop card
	// picking card to be moving
	C_PickCardWnd * m_pPickCardWnd[52];
//	int m_nPickToPos;
	int m_nNoOfPick;

	C_PickCardWnd * m_pSelCardWnd[4];	// store selected card *
	C_PickCardWnd * m_pExcCardWnd[2];	// store exchanged card *

	// timer
	int m_nPickDelay;
	int m_nAIDelay;
};

#endif