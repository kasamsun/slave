#include "PlayerDetailWnd.h"

#include <stdio.h>
#include "kswafx/DCEx.h"
#include "kswafx/Tooltip.h"
#include "resource.h"
#include "SlaveWnd.h"
#include "PlayerWnd.h"
#include "Player.h"
#include "Slave.h"

#define PD_MOVE_TIMERID 1100
#define PD_MOVE_TIMER 50

#define PD_ACTIVE_PLAYER_TIMERID	2200
#define PD_ACTIVE_PLAYER_TIMER		250

#define SM_ICON_POSX	12
#define SM_ICON_POSY	12
#define LG_ICON_POSX	10
#define LG_ICON_POSY	12

char C_PlayerDetailWnd::szDisplayModeText[2][24] = {"Current Status",
													"Player Statistic"};

BEGIN_HANDLE_MSG(C_PlayerDetailWnd,C_GenericClickWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_LBUTTONDOWN,OnLButtonDown)
	HANDLE_MSG(WM_LBUTTONUP,OnLButtonUp)
	HANDLE_MSG(WM_RBUTTONDOWN,OnRButtonDown)
	HANDLE_MSG(WM_RBUTTONUP,OnRButtonUp)
	HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)
	HANDLE_MSG(WM_TIMER,OnTimer)

END_HANDLE_MSG()

C_PlayerDetailWnd::C_PlayerDetailWnd(C_Wnd *pParentWnd,int nDisplayMode,int PosX,int PosY)
		: C_GenericClickWnd(pParentWnd)
{
	m_hWnd = CreateWindow(
                            m_szClassName,
                            "",
                            WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS,
                            PosX,PosY,
                            PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,
                            pParentWnd->GetHandle(),
                            NULL,
                            C_App::m_hInstance,
                            (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

    //ShowWindow(m_hWnd,SW_SHOWNA);
	//UpdateWindow(m_hWnd);

    m_nDisplayMode = nDisplayMode;
	m_nPosX = PosX;
	m_nPosY = PosY;
    m_pPlayerWnd = NULL;
	m_pPlayer = NULL;
	m_hdcBkMem = NULL;
	m_nMoveTick = 0;
	m_bActivate = FALSE;
	m_nActiveImageIndex = 0;  // running 0,1,2
    // set default background
	SetImage();

	HRGN hrgn;
	hrgn = CreateRoundRectRgn(0,0,PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,
								24,24);
	SetWindowRgn(m_hWnd,hrgn,TRUE);

	SetActiveCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_POINT)) );
	SetClickCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_CLICK)) );
//	SetDragCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_GRASP)) );

    EnableKBMsgParentNotify(TRUE);

	m_pTooltip = new C_Tooltip;
	m_pTooltip->Create(this);
	m_pTooltip->AddTool(m_hWnd,szDisplayModeText[m_nDisplayMode]);
}

C_PlayerDetailWnd::~C_PlayerDetailWnd()
{
	if ( m_nMoveTick != 5 && m_nMoveTick != 0 )
		KillTimer(m_hWnd,PD_MOVE_TIMERID);
	ClearImage();
	delete m_pPlayerWnd;
	delete m_pTooltip;
	if ( m_bActivate )
		KillTimer(m_hWnd,PD_ACTIVE_PLAYER_TIMERID);
}

void C_PlayerDetailWnd::SetDisplayMode(int nMode)
{
	m_nDisplayMode = nMode;
    switch( m_nDisplayMode )
    {
    case SHOW_PLAYER_STATUS:
    	if ( m_pPlayerWnd != NULL )
        {
        	m_pPlayerWnd->ShowSize(DOUBLE_SIZE_PLAYERWND);
        	SetWindowPos(m_pPlayerWnd->GetHandle(),NULL,LG_ICON_POSX,LG_ICON_POSY,0,0,SWP_NOSIZE);
        }
        break;
    case SHOW_PLAYER_INFO:
    	if ( m_pPlayerWnd != NULL )
        {
        	m_pPlayerWnd->ShowSize(SINGLE_SIZE_PLAYERWND);
        	SetWindowPos(m_pPlayerWnd->GetHandle(),NULL,SM_ICON_POSX,SM_ICON_POSY,0,0,SWP_NOSIZE);
        }
        break;
    }
	m_pTooltip->UpdateTipText(m_hWnd,szDisplayModeText[m_nDisplayMode]);
    InvalidateRect(m_hWnd,NULL,FALSE);
}

// ================================================================
// Input image should be 180x100
// ================================================================
void C_PlayerDetailWnd::SetImage(LPCSTR lpszImage)
{
	// ==============Set Background====================
    ClearImage();

	RECT rc;
	GetClientRect(m_hWnd,&rc);
    int nWidth = rc.right-rc.left;
    int nHeight = rc.bottom-rc.top;

	m_hDC = GetDC(m_hWnd);
    m_hdcBkMem = CreateCompatibleDC(m_hDC);
	m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,rc.right-rc.left,rc.bottom-rc.top);
    m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);

	if ( lpszImage != NULL )
	{
		HDC hdcCustomBmp;
		HBITMAP hbmpTmpCustomBmp;
		HBITMAP hbmpCustomBmp = (HBITMAP)LoadImage(NULL,lpszImage,
							IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if ( hbmpCustomBmp != NULL )
		{
		    hdcCustomBmp = CreateCompatibleDC(m_hDC);
			hbmpTmpCustomBmp = (HBITMAP)SelectObject(hdcCustomBmp,hbmpCustomBmp);

			BITMAP bm;
			GetObject(hbmpCustomBmp,sizeof(bm),&bm);
			for ( int i = 0 ; i < nWidth ; i+=bm.bmWidth )
				for ( int j = 0 ; j < nHeight ; j+=bm.bmHeight )
					BitBlt(m_hdcBkMem,i,j,bm.bmWidth,bm.bmHeight,
							hdcCustomBmp,0,0,SRCCOPY);

			DeleteObject(SelectObject(hdcCustomBmp,hbmpTmpCustomBmp));
			DeleteDC(hdcCustomBmp);
			ReleaseDC(m_hWnd,m_hDC);
			return;
		}
	}
    // if not enter filename or load file fail , set to default image
    HDC hdcPaint = CreateCompatibleDC(m_hDC);
    HBITMAP hbmpPaint = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_PLAYERDETAIL));
    HBITMAP hbmpTmp = (HBITMAP)SelectObject(hdcPaint,hbmpPaint);
    BITMAP bm;
    GetObject(hbmpPaint,sizeof(bm),&bm);
    for ( int i = 0 ; i < nWidth ; i+=bm.bmWidth )
        for ( int j = 0 ; j < nHeight ; j+=bm.bmHeight )
            BitBlt(m_hdcBkMem,i,j,bm.bmWidth,bm.bmHeight,
                    hdcPaint,0,0,SRCCOPY);
    DeleteObject(SelectObject(hdcPaint,hbmpTmp));
    DeleteDC(hdcPaint);

	ReleaseDC(m_hWnd,m_hDC);
}

int C_PlayerDetailWnd::ClearImage()
{
	if ( m_hdcBkMem != NULL )
	{
		DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
		DeleteDC(m_hdcBkMem);
		m_hdcBkMem = NULL;
	}
	return TRUE;
}

C_Player * C_PlayerDetailWnd::SetPlayer(C_Player *pPlayer)
{
	// create PlayerWnd here and delete the old one
    if ( m_nDisplayMode == SHOW_PLAYER_STATUS )
    {
	   	m_pPlayerWnd = new C_PlayerWnd(this,DOUBLE_SIZE_PLAYERWND,LG_ICON_POSX,LG_ICON_POSY,pPlayer );
    }
    else
    {
	   	m_pPlayerWnd = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,SM_ICON_POSX,SM_ICON_POSY,pPlayer );
    }
    pPlayer->SetUI(m_pPlayerWnd);
    Refresh();
	m_pPlayer = pPlayer;
//	if ( pPlayer->GetJubmoo()->GetActivePlayerID() != pPlayer->GetPlayerID() )
	// change to always float
		Float();
    return pPlayer;
}

C_Player * C_PlayerDetailWnd::ClearPlayer()
{
	// delete PlayerWnd here and create the new one at SlaveWnd
	m_bActivate = FALSE;
    if ( m_pPlayerWnd != NULL )
    {
		FlashCaption(FALSE);

        C_Player * pPlayer = m_pPlayerWnd->GetPlayer();
        delete m_pPlayerWnd;
        m_pPlayerWnd = NULL;
		m_pPlayer = NULL;
        Refresh();
		Sink();

		// clear timer
		ShowInactive();
	    return pPlayer;
    }
	Sink();
    return NULL;
}

void C_PlayerDetailWnd::Refresh(int bFlag)
{
	RECT rc;
	switch ( bFlag )
	{
	case REFRESH_ALL:
		InvalidateRect(m_hWnd,NULL,FALSE);
		break;
	case REFRESH_POSITION:
		if ( m_nDisplayMode == SHOW_PLAYER_STATUS )
		{
			rc.left = 90;rc.right = 122;
			rc.top = 44;rc.bottom = 76;
		}
		else
		{
			rc.left = 48;rc.right = 64;
			rc.top = 24;rc.bottom = 40;
		}
		InvalidateRect(m_hWnd,&rc,FALSE);
		break;
	case REFRESH_DISCARDSTATUS:
		if ( m_nDisplayMode == SHOW_PLAYER_STATUS )
		{
			rc.left = 132;rc.right = 164;
			rc.top = 44;rc.bottom = 76;
		}
		else
		{
			rc.left = 24;rc.right = 56;
			rc.top = 64;rc.bottom = 96;
		}
		InvalidateRect(m_hWnd,&rc,FALSE);
		break;
	case REFRESH_CARDLIST:
		if ( m_nDisplayMode == SHOW_PLAYER_STATUS )
		{
			rc.left = 10;rc.right = 136;
			rc.top = 77;rc.bottom = 93;
			InvalidateRect(m_hWnd,&rc,FALSE);
		}
		break;
	case REFRESH_HISTORY:
		if ( m_nDisplayMode != SHOW_PLAYER_STATUS )
		{
			rc.left = 88;rc.right = 180;
			rc.top = 10;rc.bottom = 90;
			InvalidateRect(m_hWnd,&rc,FALSE);
		}
		break;
	}
}

int C_PlayerDetailWnd::PaintStatus(HDC hDC)
{
    if ( m_pPlayerWnd != NULL )
    {
        RECT rc;
        char * lpszName;
        HFONT htmpFont;

	    lpszName = m_pPlayerWnd->GetPlayer()->GetName();
        htmpFont = (HFONT)SelectObject(hDC,((C_SlaveWnd*)m_pParentWnd)->GetBigSlaveFont());
        rc.left = 75;	rc.right = 175;
        rc.top = 10;	rc.bottom = 45;
        DrawText(hDC,lpszName,strlen(lpszName),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        SelectObject(hDC,htmpFont);

		// paint no of card
		int nNumCard = m_pPlayerWnd->GetPlayer()->GetCardList()->Count();
		for ( int i=0 ; i<nNumCard ; i++ )
			DrawIconEx(hDC,10+(i*6),77,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_SMCARD)),
				16,16,0,NULL,DI_NORMAL);
		int nResPosition;
		switch ( m_pPlayerWnd->GetPlayer()->GetPosition() )
		{
		case IAM_KING: nResPosition = IDI_ISKING; break;
		case IAM_QUEEN: nResPosition = IDI_ISQUEEN; break;
		case IAM_PEOPLE: nResPosition = IDI_ISPEOPLE; break;
		case IAM_VICESLAVE: nResPosition = IDI_ISVSLAVE; break;
		case IAM_SLAVE: nResPosition = IDI_ISSLAVE; break;
		}
		DrawIcon(hDC,90,44,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResPosition)));

		switch ( m_pPlayerWnd->GetPlayer()->GetDiscardStatus() )
		{
		case WAITFORDISCARD: nResPosition = IDI_DSWAIT; break;
		case DISCARDING:
			switch (m_nActiveImageIndex)
			{
			case 0: nResPosition = IDI_DSDISCARD; break;
			case 1: nResPosition = IDI_DSDISCARD2; break;
			case 2: nResPosition = IDI_DSDISCARD3; break;
			}
			break;
		case LAST_DISCARD: nResPosition = IDI_DSLAST; break;
		case PASS_TURN: nResPosition = IDI_DSPASS; break;
		case CARD_OVER: nResPosition = 0; break;
		}
		if ( nResPosition > 0 )
			DrawIcon(hDC,132,44,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResPosition)));
    }
    else
	    DrawIconEx(hDC,LG_ICON_POSX,LG_ICON_POSY,
                LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOBODY)),
                64,64,0,NULL,DI_NORMAL);
	return 0;
}

int C_PlayerDetailWnd::PaintInfo(HDC hDC)
{
    if ( m_pPlayerWnd != NULL )
    {
        RECT rc;
        char * lpszName;
        HFONT htmpFont;

	    lpszName = m_pPlayerWnd->GetPlayer()->GetName();
        htmpFont = (HFONT)SelectObject(hDC,((C_SlaveWnd*)m_pParentWnd)->GetSlaveFont());
        rc.left = 5;	rc.right = 75;
        rc.top = 44;	rc.bottom = 62;
        DrawText(hDC,lpszName,strlen(lpszName),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
        char tmpbuf[8];
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfBeenKing());
        TextOut(hDC,105,20,tmpbuf,strlen(tmpbuf));
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfBeenQueen());
        TextOut(hDC,158,20,tmpbuf,strlen(tmpbuf));
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfKing());
        TextOut(hDC,105,55,tmpbuf,strlen(tmpbuf));
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfSlave());
        TextOut(hDC,158,55,tmpbuf,strlen(tmpbuf));

		MoveToEx(hDC,105,68,NULL);
		LineTo(hDC,123,68);
		MoveToEx(hDC,158,68,NULL);
		LineTo(hDC,176,68);
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfGamePlay());
        TextOut(hDC,105,68,tmpbuf,strlen(tmpbuf));
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfGamePlay());
        TextOut(hDC,158,68,tmpbuf,strlen(tmpbuf));
        SelectObject(m_hDC,htmpFont);

		int nResPosition;
		switch ( m_pPlayerWnd->GetPlayer()->GetPosition() )
		{
		case IAM_KING: nResPosition = IDI_ISKING; break;
		case IAM_QUEEN: nResPosition = IDI_ISQUEEN; break;
		case IAM_PEOPLE: nResPosition = IDI_ISPEOPLE; break;
		case IAM_VICESLAVE: nResPosition = IDI_ISVSLAVE; break;
		case IAM_SLAVE: nResPosition = IDI_ISSLAVE; break;
		}
		DrawIconEx(hDC,48,24,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResPosition)),
				16,16,0,NULL,DI_NORMAL);

		switch ( m_pPlayerWnd->GetPlayer()->GetDiscardStatus() )
		{
		case WAITFORDISCARD: nResPosition = IDI_DSWAIT; break;
		case DISCARDING:
			switch (m_nActiveImageIndex)
			{
			case 0: nResPosition = IDI_DSDISCARD; break;
			case 1: nResPosition = IDI_DSDISCARD2; break;
			case 2: nResPosition = IDI_DSDISCARD3; break;
			}
			break;
		case LAST_DISCARD: nResPosition = IDI_DSLAST; break;
		case PASS_TURN: nResPosition = IDI_DSPASS; break;
		case CARD_OVER: nResPosition = 0; break;
		}
		if ( nResPosition > 0 )
			DrawIcon(hDC,24,64,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResPosition)));

    }
    else
        DrawIcon(hDC,SM_ICON_POSX,SM_ICON_POSY,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOBODY)));
	DrawIcon(hDC,72,10,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOOFBEENKING)));
    DrawIcon(hDC,124,10,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOOFBEENQUEEN)));
    DrawIcon(hDC,72,50,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_ISKING)));
    DrawIcon(hDC,124,50,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_ISSLAVE)));
	return 0;
}

void C_PlayerDetailWnd::Float()
{
	if ( m_nMoveTick > 0 ) return;
	if ( m_nMoveTick < 0 )
		m_nMoveTick = -m_nMoveTick;
	else
		m_nMoveTick = 0;
	m_nMoveTick++;
	SetTimer(m_hWnd,PD_MOVE_TIMERID,PD_MOVE_TIMER,NULL);
	int i = ((m_nMoveTick>0)?m_nMoveTick:-m_nMoveTick);
	MoveWindow(m_hWnd,m_nPosX-i,m_nPosY-i,PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,TRUE);
}

void C_PlayerDetailWnd::Sink()
{
	if ( m_nMoveTick <= 0 ) return ;
	m_nMoveTick = -m_nMoveTick;
	m_nMoveTick++;
	SetTimer(m_hWnd,PD_MOVE_TIMERID,PD_MOVE_TIMER,NULL);
	int i = ((m_nMoveTick>0)?m_nMoveTick:-m_nMoveTick);
	MoveWindow(m_hWnd,m_nPosX-i,m_nPosY-i,PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,TRUE);
}

void C_PlayerDetailWnd::ShowActive()
{
	if ( !m_bActivate )
	{
		m_bActivate = TRUE;
		SetTimer(m_hWnd,PD_ACTIVE_PLAYER_TIMERID,PD_ACTIVE_PLAYER_TIMER,NULL);

		FlashCaption(TRUE);
	}
    PutActiveImage();
}

void C_PlayerDetailWnd::ShowInactive()
{
	FlashCaption(FALSE);

	KillTimer(m_hWnd,PD_ACTIVE_PLAYER_TIMERID);
	m_bActivate = FALSE;
	RECT rc;
	if ( m_nDisplayMode == SHOW_PLAYER_STATUS )
	{
		rc.left = 132;rc.right = 164;
		rc.top = 44;rc.bottom = 76;
	}
	else
	{
		rc.left = 24;rc.right = 56;
		rc.top = 64;rc.bottom = 96;
	}
	InvalidateRect(m_hWnd,&rc,FALSE);
}

int C_PlayerDetailWnd::PutActiveImage()
{
    int nResPosition;

    switch (m_nActiveImageIndex)
    {
    case 0: nResPosition = IDI_DSDISCARD; break;
    case 1: nResPosition = IDI_DSDISCARD2; break;
    case 2: nResPosition = IDI_DSDISCARD3; break;
    }

    POINT p;
	if ( m_nDisplayMode == SHOW_PLAYER_STATUS )
	{
        p.x = 132; p.y = 44;
	}
	else
	{
        p.x = 24; p.y = 64;
	}

    m_hDC = GetDC(m_hWnd);
    HDC hdc = CreateCompatibleDC(m_hDC);
    HBITMAP hbmp = CreateCompatibleBitmap(m_hDC,32,32);

    HBITMAP hbmpTmp = (HBITMAP)SelectObject(hdc,hbmp);
    BitBlt(hdc,0,0,32,32,m_hdcBkMem,p.x,p.y,SRCCOPY);
    DrawIcon(hdc,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResPosition)));
    BitBlt(m_hDC,p.x,p.y,32,32,hdc,0,0,SRCCOPY);

    DeleteObject(SelectObject(hdc,hbmpTmp));
    DeleteDC(hdc);

    ReleaseDC(m_hWnd,m_hDC);

    return 0;
}

void C_PlayerDetailWnd::FlashCaption(int bFlag)
{
    C_SlaveWnd * pSlaveWnd = ((C_SlaveWnd*)m_pParentWnd);
    if ( m_pPlayer != NULL )
        if ( m_pPlayer->GetPlayerID() == pSlaveWnd->GetSlave()->GetMyPlayer()->GetPlayerID() )
		{
			if ( pSlaveWnd->GetHandle() != GetActiveWindow() || !bFlag )
			{
				//if ( bFlag )
				int nRet = FlashWindow(m_pParentWnd->GetHandle(),TRUE);
				if ( !bFlag && nRet == 0 )
					FlashWindow(m_pParentWnd->GetHandle(),TRUE);
				//else
				//	SetActiveWindow(m_pParentWnd->GetHandle());
			}
		}
}

///////////////////// Event Handler /////////////////////////
LRESULT C_PlayerDetailWnd::OnPaint(WPARAM ,LPARAM )
{
    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

    // Paint Background
	RECT rc;
	GetClientRect(m_hWnd,&rc);
	BitBlt(m_hDC,0,0,rc.right-rc.left,rc.bottom-rc.top,m_hdcBkMem,0,0,SRCCOPY);

	SetBkMode(m_hDC,TRANSPARENT);
    switch( m_nDisplayMode )
    {
    case SHOW_PLAYER_STATUS: PaintStatus(m_hDC); break;
    case SHOW_PLAYER_INFO: PaintInfo(m_hDC); break;
    }
	
	EndPaint(m_hWnd,&ps);
	return 0;
}

LRESULT C_PlayerDetailWnd::OnLButtonDown(WPARAM wParam,LPARAM lParam)
{
	Sink();
	return C_GenericClickWnd::OnLButtonDown(wParam,lParam);
}

LRESULT C_PlayerDetailWnd::OnLButtonUp(WPARAM wParam,LPARAM lParam)
{
	if ( m_pPlayer != NULL )
		Float();

	if ( m_bMouseAbove && m_bLPressed )
	{
        switch ( m_nDisplayMode )
        {
        case SHOW_PLAYER_STATUS:
            SetDisplayMode(SHOW_PLAYER_INFO);
            break;
        case SHOW_PLAYER_INFO:
            SetDisplayMode(SHOW_PLAYER_STATUS);
            break;
        }
    }
	return C_GenericClickWnd::OnLButtonUp(wParam,lParam);
}

LRESULT C_PlayerDetailWnd::OnRButtonDown(WPARAM wParam,LPARAM lParam)
{
	Sink();
	return C_GenericClickWnd::OnRButtonDown(wParam,lParam);
}

LRESULT C_PlayerDetailWnd::OnRButtonUp(WPARAM wParam,LPARAM lParam)
{
	if ( m_pPlayer != NULL )
		Float();

	if ( m_bMouseAbove && m_bRPressed )
	{
		int nDisplayMode; 
        switch ( m_nDisplayMode )
        {
        case SHOW_PLAYER_STATUS:
            nDisplayMode = SHOW_PLAYER_INFO;
            break;
        case SHOW_PLAYER_INFO:
            nDisplayMode = SHOW_PLAYER_STATUS;
            break;
        }
		for ( int i = 0 ; i<6 ; i++ )
		{
			((C_SlaveWnd*)m_pParentWnd)->GetPlayerDetailWnd(i)->SetDisplayMode(nDisplayMode);
		}
    }
	return C_GenericClickWnd::OnRButtonUp(wParam,lParam);
}

LRESULT C_PlayerDetailWnd::OnEraseBkgnd(WPARAM ,LPARAM )
{
	return 1;
}

LRESULT C_PlayerDetailWnd::OnTimer(WPARAM wParam,LPARAM )
{
	if ( wParam == PD_ACTIVE_PLAYER_TIMERID )
	{
		m_nActiveImageIndex++;
		if ( m_nActiveImageIndex > 2 ) m_nActiveImageIndex = 0;
        PutActiveImage();

		FlashCaption(TRUE);
		return 0;
	}

//	TRACE("#%d",m_nMoveTick);
	m_nMoveTick++;
	if ( m_nMoveTick >= 5 || m_nMoveTick == 0 )
	{
		if ( m_nMoveTick > 5 ) m_nMoveTick = 5;
		KillTimer(m_hWnd,PD_MOVE_TIMERID);
	}
	int i = ((m_nMoveTick>0)?m_nMoveTick:-m_nMoveTick);
	MoveWindow(m_hWnd,m_nPosX-i,m_nPosY-i,PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,TRUE);

    if ( m_nMoveTick == 0 )
    	if ( m_pPlayer != NULL )
            	Float();

	return 0;
}
