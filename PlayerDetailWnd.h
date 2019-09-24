#ifndef _C_PLAYERDETAILWND_H_
#define _C_PLAYERDETAILWND_H_

#define PLAYERDETAIL_WIDTH	180
#define PLAYERDETAIL_HEIGHT	100

enum { SHOW_PLAYER_STATUS=0, SHOW_PLAYER_INFO=1 };
enum { REFRESH_ALL,REFRESH_POSITION,REFRESH_DISCARDSTATUS,REFRESH_CARDLIST,REFRESH_HISTORY };

#include <GenericClickWnd.h>
class C_PlayerWnd;
class C_Player;
class C_Tooltip;

class C_PlayerDetailWnd : public C_GenericClickWnd
{
public:
	C_PlayerDetailWnd(C_Wnd *,int nDisplayMode,int PosX,int PosY);
	~C_PlayerDetailWnd();

	int GetDisplayMode() { return m_nDisplayMode; }
	void SetDisplayMode(int nMode);
	void SetImage(LPCSTR lpszImage = NULL);
	int ClearImage();
 
    C_Player * SetPlayer(C_Player *);
    C_Player * ClearPlayer();
	C_Player * GetPlayer() { return m_pPlayer; }
	void Refresh(int bFlag = REFRESH_ALL);

	void Float();
	void Sink();
	void ShowActive();
	void ShowInactive();
    int PutActiveImage();
	void FlashCaption(int bFlag);

	int GetPosX() { return m_nPosX; }
	int GetPosY() { return m_nPosY; }

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam,LPARAM lParam);
    LRESULT OnLButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonDown(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBkgnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam,LPARAM lParam);

private:
	static char szDisplayModeText[2][24];

	HDC m_hdcBkMem;
	HBITMAP m_hbmpBkMem,m_hbmpBkTmp;

	int PaintStatus(HDC);
    int PaintInfo(HDC);

    C_PlayerWnd * m_pPlayerWnd;
	C_Player * m_pPlayer;

    int m_nDisplayMode;
	C_Tooltip * m_pTooltip;
	int m_nPosX,m_nPosY;
	int m_nMoveTick;
	int m_bActivate;

	int m_nActiveImageIndex;
};

#endif