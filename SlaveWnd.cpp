#include "SlaveWnd.h"

#include <stdio.h>
#include "kswafx/GenericClickWnd.h"
#include "kswafx/CoolDlg.h"
#include "kswafx/DCEx.h"
#include "kswafx/Tooltip.h"
#include "kswafx/CoolButton.h"

#include "resource.h"
#include "SlaveCard.h"
#include "Slave.h"
#include "PlayerDetailWnd.h"
#include "CardWnd.h"
#include "ChatEdit.h"
#include "AboutDlg.h"
#include "HelpDlg.h"
#include "PlayerWnd.h"
#include "SettingDlg.h"
#include "StartGameWnd.h"
#include "ResultDlg.h"
#include "JoinDlg.h"
#include "SlaveComm.h"
#include "SlaveAI.h"

// Note :: 1.  now it'll have leak , if u exit game when card is picking up.

#define START_BUTTON	0
#define JOIN_BUTTON		1
#define QUIT_BUTTON		2
#define SETTING_BUTTON	3
#define HELP_BUTTON		5
#define ABOUT_BUTTON	6
#define EXIT_BUTTON		7

#define PICKUPCARD_READY	1
#define PICKUPCARD_TIMERID	1000
#define AI_TIMERID			2000
#define AI_KINGEXC_TIMERID	2100
#define AI_QUEENEXC_TIMERID	2200
#define AI_KINGEXC_TIMER	1500	// 1.5 sec.
#define AI_QUEENEXC_TIMER	2500	// 2.5 sec.

extern HHOOK hCoolDlgHook;
extern C_SlaveApp SlaveApp;

BEGIN_HANDLE_MSG(C_SlaveWnd,C_GenericWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_NCLBUTTONDBLCLK,OnNCLButtonDblClk)
	HANDLE_MSG(WM_COMMAND,OnCommand)
	HANDLE_MSG(WM_CLOSE,OnClose)
	HANDLE_MSG(WM_DESTROY,OnDestroy)
	HANDLE_MSG(WM_CHAR,OnChar)
	HANDLE_MSG(WM_RBUTTONDOWN,OnRButtonDown)
	HANDLE_MSG(WM_TIMER,OnTimer)

	HANDLE_MSG(WM_GENERICCLICKWND_LBUTTONCLICK,OnCardWndLButtonClick)
	HANDLE_MSG(WM_GENERICCLICKWND_RBUTTONCLICK,OnCardWndRButtonClick)
	HANDLE_MSG(WM_PICKCARD_MOVED,OnPickCardReady)

END_HANDLE_MSG()

C_SlaveWnd::C_SlaveWnd(C_Slave * pSlave) : C_GenericWnd()
{
	RECT rc;

	SetRect(&rc,0,0,SLAVEWND_WIDTH,SLAVEWND_HEIGHT);
	AdjustWindowRect(&rc,WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX|WS_DLGFRAME|WS_CAPTION,NULL);

	m_hWnd = CreateWindow(
                    m_szClassName,
                    m_szClassName,
                    WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX|WS_DLGFRAME|
							WS_CAPTION|WS_CLIPSIBLINGS,
                    rc.left,
                    rc.top,
                    rc.right-rc.left,
                    rc.bottom-rc.top,
                    NULL,
                    NULL,
                    C_App::m_hInstance,
                    (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

	MoveToCenter();

	m_hdcBkMem = NULL;

	m_pSlave = pSlave;
	int i;
	for ( i=0 ;i<8; i++ )
        m_pPlayerLotStatus[i] = NULL;

	m_pStartGameWnd = NULL;
	m_bPickingUp = FALSE;
	for ( i=0 ; i<52 ; i++ )
		m_pPickCardWnd[i] = NULL;
	for ( i=0 ; i<4 ; i++ )
	{
		m_pSelCardWnd[i] = NULL;
		m_SelectedCard.nCard[i] = 0;
	}
	for ( i=0 ; i<2 ; i++ )
	{
		m_pExcCardWnd[i] = NULL;
	}

	m_nPickDelay = 800;
	// Create Command Button
	InitScreen();

}

C_SlaveWnd::~C_SlaveWnd( void )
{
	int i;
    for ( i = 0 ; i<MAX_SLAVE_BUTTON ; i++ )
        delete m_pButton[i];

	delete m_pStartGameWnd;
	delete m_pPassButton;
	delete m_pExchangeButton;
    delete m_pChatEdit;
    for ( i = 0 ; i<6 ; i++ )
	{
		delete m_pPlayerDetailWnd[i];
	}
    m_MyCardList.DeleteObjects();
	m_DropCardList.DeleteObjects();

   	C_CoolDlg::CoolDlgClearImage();
    C_CardWnd::ClearBackCardImage();
    C_CardWnd::ClearCardImage();

	CoolDlgTerminate();
}

void C_SlaveWnd::InitScreen()
{
	CreateSlaveFont();

	int i;
	RECT rcButton[MAX_SLAVE_BUTTON] = {
                        {10,440,42,472},
                        {50,440,82,472},
                        {90,440,122,472},
                        {130,440,162,472},
                        {540,440,572,472},
                        {580,440,612,472},
                        {620,440,652,472} };
    char szButtonCaption[MAX_SLAVE_BUTTON][8] = {
                        "Start","Join","Quit",
                        "Setting","Help",
                        "About","Exit" };
    UINT nButtonID[MAX_SLAVE_BUTTON] = {
                        ID_SV_START,ID_SV_JOIN,ID_SV_QUIT,
                        ID_SV_SETTING,ID_SV_HELP,
                        ID_SV_ABOUT,ID_SV_EXIT};
	int nButtonResID[MAX_SLAVE_BUTTON] = {
						IDI_START,IDI_JOIN,IDI_QUIT,IDI_SETTING,
						IDI_HELP,IDI_ABOUT,IDI_EXIT };

 
	m_Tooltip.Create(this);
	// Create Command Button
    for ( i = 0 ; i<MAX_SLAVE_BUTTON ; i++ )
    {
        m_pButton[i] = new C_CoolButton;
        m_pButton[i]->Create(szButtonCaption[i],NULL,
                        BS_PUSHBUTTON|BS_CENTER|BS_VCENTER|WS_VISIBLE|WS_CHILD|BS_ICON,
                        rcButton[i],this,nButtonID[i]);
        m_pButton[i]->SetFont(m_hFont,TRUE);
		m_pButton[i]->SetImage(IMAGE_ICON,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nButtonResID[i])));
		m_pButton[i]->EnableKBMsgParentNotify(TRUE);
		m_pButton[i]->EnableFocusOnMouseMove(TRUE);

        m_Tooltip.AddTool(m_pButton[i]->GetHandle(),szButtonCaption[i]);
    }

	// Create pass button
	RECT rcPass = {390,250,460,290};
	m_pPassButton = new C_CoolButton;
    m_pPassButton->Create("Pass",NULL,
                    BS_PUSHBUTTON|BS_CENTER|BS_VCENTER|WS_CHILD,
                    rcPass,this,IDI_PASS);
    m_pPassButton->SetFont(m_hFont,TRUE);
	m_pPassButton->EnableKBMsgParentNotify(TRUE);
	m_pPassButton->EnableFocusOnMouseMove(TRUE);
	SendMessage(m_pPassButton->GetHandle(),WM_SETFONT,(WPARAM)GetBigSlaveFont(),MAKELPARAM(TRUE,0));
	m_Tooltip.AddTool(m_pPassButton->GetHandle(),"Pass this round");

	// Create pass button
	SetRect(&rcPass,280,250,380,290);
	m_pExchangeButton = new C_CoolButton;
    m_pExchangeButton->Create("Exchange",NULL,
                    BS_PUSHBUTTON|BS_CENTER|BS_VCENTER|WS_CHILD,
                    rcPass,this,IDI_EXCHANGE);
    m_pExchangeButton->SetFont(m_hFont,TRUE);
	m_pExchangeButton->EnableKBMsgParentNotify(TRUE);
	m_pExchangeButton->EnableFocusOnMouseMove(TRUE);
	SendMessage(m_pExchangeButton->GetHandle(),WM_SETFONT,(WPARAM)GetBigSlaveFont(),MAKELPARAM(TRUE,0));
	m_Tooltip.AddTool(m_pExchangeButton->GetHandle(),"");

    // Create Chat Entry Box
    RECT rcChatEdit = {45,412,175,432};
    m_pChatEdit = new C_ChatEdit;
    m_pChatEdit->Create(rcChatEdit,this,ID_CHATEDIT);
    m_pChatEdit->SetFont(m_hFont,TRUE);
    m_Tooltip.AddTool(m_pChatEdit->GetHandle(),"Enter message here");

    // Create PlayerDetailWnd
    POINT ptPlayerDetail[6] = {{10,300},{10,154},{10,10},{470,10},{470,154},{470,300}};
    for ( i=0 ; i<6 ; i++ )
    {
		m_pPlayerDetailWnd[i] = new C_PlayerDetailWnd(this,SHOW_PLAYER_STATUS,
        							ptPlayerDetail[i].x,
                                    ptPlayerDetail[i].y);
	}

    // set all card image
	C_CardWnd::SetCardImage();

	// set bkg image of every component
	SetImage();

	// set delay time
	SlaveApp.OpenRegistry();
	SetGameSpeed(SlaveApp.GetAIDelay());
	SlaveApp.CloseRegistry();
}

void C_SlaveWnd::SetImage()
{
	// ==============Set Background====================
    ClearImage();

    int bFading;
    int bCoolCtl;
    int bCustomImg;

    char szCustomBkg[48];
    memset(szCustomBkg,0x00,sizeof(szCustomBkg));
    char szCustomCard[48];
    memset(szCustomCard,0x00,sizeof(szCustomCard));
    char szCustomDlg[48];
    memset(szCustomDlg,0x00,sizeof(szCustomDlg));
    char szCustomPD[48];
    memset(szCustomPD,0x00,sizeof(szCustomPD));

	RECT rc;

    // read background source from registry
	SlaveApp.OpenRegistry();
    bFading = SlaveApp.GetUseFading();
    bCoolCtl = SlaveApp.GetUseCoolCtl();
    bCustomImg = SlaveApp.GetUseCustomImg();

   	C_CoolDlg::CoolDlgClearImage();
   	C_CoolDlg::CoolDlgEnableFading(bFading);

	if ( bCoolCtl )
    	CoolDlgAutoSubclass();
	else
    	CoolDlgTerminate();

    if ( bCustomImg )
    {
	    SlaveApp.GetCustomBkg(szCustomBkg,sizeof(szCustomBkg));
    	SlaveApp.GetCustomCard(szCustomCard,sizeof(szCustomCard));
	    SlaveApp.GetCustomDlg(szCustomDlg,sizeof(szCustomDlg));
	    SlaveApp.GetCustomPD(szCustomPD,sizeof(szCustomPD));
    }

	SlaveApp.CloseRegistry();

    // set other component background
    C_CardWnd::SetBackCardImage(szCustomCard);
    C_CardWnd::SetCardImage(szCustomCard);
    C_CoolDlg::CoolDlgSetImage(NULL,szCustomDlg);
	
	int i;
	for ( i=0; i < 6 ; i++ )
		m_pPlayerDetailWnd[i]->SetImage(szCustomPD);
    
	// set slavewnd background
	GetClientRect(m_hWnd,&rc);

	m_hDC = GetDC(m_hWnd);
    m_hdcBkMem = CreateCompatibleDC(m_hDC);
	m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,rc.right-rc.left,rc.bottom-rc.top);
    m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);

    int bLoadCustom = FALSE;
	if ( bCustomImg && szCustomBkg != NULL )
	{
		HDC hdcCustomBmp;
		HBITMAP hbmpTmpCustomBmp;
		HBITMAP hbmpCustomBmp = (HBITMAP)LoadImage(NULL,szCustomBkg,
							IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if ( hbmpCustomBmp != NULL )
		{
			int i;
		    hdcCustomBmp = CreateCompatibleDC(m_hDC);
			hbmpTmpCustomBmp = (HBITMAP)SelectObject(hdcCustomBmp,hbmpCustomBmp);

			BITMAP bm;
			GetObject(hbmpCustomBmp,sizeof(bm),&bm);
			for ( i = 0 ; i < SLAVEWND_WIDTH ; i+=bm.bmWidth )
				for ( int j = 0 ; j < SLAVEWND_HEIGHT ; j+=bm.bmHeight )
					BitBlt(m_hdcBkMem,i,j,bm.bmWidth,bm.bmHeight,
							hdcCustomBmp,0,0,SRCCOPY);

			DeleteObject(SelectObject(hdcCustomBmp,hbmpTmpCustomBmp));
			DeleteDC(hdcCustomBmp);
            bLoadCustom = TRUE;
        }
	}

    if ( !bLoadCustom )
    {
        if ( bFading )
        {
            // using default painting image , if not enter filename or load file fail
            C_DCEx * pDCEx = new C_DCEx(m_hdcBkMem);
            pDCEx->FillGradientRect(&rc,GetSysColor(COLOR_3DFACE),
                                        GetSysColor(COLOR_3DHILIGHT));
            delete pDCEx;
        }
        else
        {
            // paint solid background
            HBRUSH hb;
            hb = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
            HBRUSH hbtmp = (HBRUSH)SelectObject(m_hdcBkMem,hb);
            FillRect(m_hdcBkMem,&rc,hb);
            DeleteObject(SelectObject(m_hdcBkMem,hbtmp));
        }
    }

	// put small logo
	HDC hdcLogo = CreateCompatibleDC(m_hDC);
	HBITMAP hbmpLogo = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_SVLOGO));
	HBITMAP hbmpLogoTmp = (HBITMAP)SelectObject(hdcLogo,hbmpLogo);
    HFONT hfontTmp = (HFONT)SelectObject(hdcLogo,GetSlaveFont());
    // draw version number
    SetBkMode(hdcLogo,TRANSPARENT);
    TextOut(hdcLogo,98,19,SLAVE_VERSION,strlen(SLAVE_VERSION));
    SelectObject(hdcLogo,hfontTmp);

	BITMAP bm;
	GetObject(hbmpLogo,sizeof(BITMAP),&bm);
	HDC hdcLogoMsk = CreateCompatibleDC(m_hDC);
	C_DCEx * pDCEx = new C_DCEx(m_hdcBkMem);
	HBITMAP hbmpLogoMsk = pDCEx->CreateMaskBitmap(hdcLogo,bm.bmWidth,bm.bmHeight,TRANSPARENT_COLOR);
	HBITMAP hbmpLogoMskTmp = (HBITMAP)SelectObject(hdcLogoMsk,hbmpLogoMsk);
	pDCEx->TransparentBlt(536,404,bm.bmWidth,bm.bmHeight,hdcLogo,hdcLogoMsk,0,0);
	DeleteObject(SelectObject(hdcLogoMsk,hbmpLogoMskTmp));
	DeleteObject(SelectObject(hdcLogo,hbmpLogoTmp));
	DeleteDC(hdcLogoMsk);
	DeleteDC(hdcLogo);

	RECT rcShade;
	// put big logo
	HDC hdcBLogo = CreateCompatibleDC(m_hDC);
	HBITMAP hbmpBLogo = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_BIGLOGO));
	HBITMAP hbmpBLogoTmp = (HBITMAP)SelectObject(hdcBLogo,hbmpBLogo);
	GetObject(hbmpBLogo,sizeof(BITMAP),&bm);
	if ( m_pSlave->GetPlayMode() != IDLE_PLAYMODE )
	{
		SetRect(&rcShade,0,0,bm.bmWidth,bm.bmHeight);
		pDCEx->ShadeRect(hdcBLogo,&rcShade,TRANSPARENT_COLOR);
	}

	HDC hdcBLogoMsk = CreateCompatibleDC(m_hDC);
	HBITMAP hbmpBLogoMsk = pDCEx->CreateMaskBitmap(hdcBLogo,bm.bmWidth,bm.bmHeight,TRANSPARENT_COLOR);
	HBITMAP hbmpBLogoMskTmp = (HBITMAP)SelectObject(hdcBLogoMsk,hbmpBLogoMsk);

	pDCEx->TransparentBlt(210,40,bm.bmWidth,bm.bmHeight,hdcBLogo,hdcBLogoMsk,0,0);
	DeleteObject(SelectObject(hdcBLogoMsk,hbmpBLogoMskTmp));
	DeleteObject(SelectObject(hdcBLogo,hbmpBLogoTmp));
	DeleteDC(hdcBLogoMsk);
	DeleteDC(hdcBLogo);

	HRGN hrgn;
	for ( i=0 ; i<6 ; i++ )
	{
		int x = m_pPlayerDetailWnd[i]->GetPosX();
		int y = m_pPlayerDetailWnd[i]->GetPosY();
		hrgn = CreateRoundRectRgn(x,y,x+PLAYERDETAIL_WIDTH-1,y+PLAYERDETAIL_HEIGHT-1,24,24);
		ExtSelectClipRgn(m_hdcBkMem,hrgn,RGN_AND );
		SetRect(&rcShade,x,y,x+PLAYERDETAIL_WIDTH-1,y+PLAYERDETAIL_HEIGHT-1);
		pDCEx->ShadeRect(m_hdcBkMem,&rcShade,RGB(128,128,128));
		SelectClipRgn(m_hdcBkMem,NULL);
		DeleteObject(hrgn);
	}
	delete pDCEx;
	ReleaseDC(m_hWnd,m_hDC);
}

int C_SlaveWnd::ClearImage()
{
	if ( m_hdcBkMem != NULL )
	{
		DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
		DeleteDC(m_hdcBkMem);
		m_hdcBkMem = NULL;
	}
	return TRUE;
}

void C_SlaveWnd::CreateSlaveFont()
{
	LOGFONT lf;
	m_hDC = GetDC(m_hWnd);

    // set font size 8 ---v
	lf.lfHeight = -MulDiv(8,GetDeviceCaps(m_hDC,LOGPIXELSY),72);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 0;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = 222;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;
	strcpy(lf.lfFaceName,"MS Sans Serif");
	m_hFont = CreateFontIndirect(&lf);

	lf.lfHeight = -MulDiv(14,GetDeviceCaps(m_hDC,LOGPIXELSY),72);
    m_hBigFont = CreateFontIndirect(&lf);

	ReleaseDC(m_hWnd,m_hDC);
}

void C_SlaveWnd::MoveToCenter()
{
	RECT swp;
	int width;
	int height;

	width = GetSystemMetrics(SM_CXFULLSCREEN);
	height = GetSystemMetrics(SM_CYFULLSCREEN)+
				GetSystemMetrics(SM_CYCAPTION);

	GetWindowRect(m_hWnd,&swp);

	if ( (height-(swp.bottom-swp.top))/2 < 0 )
    	MoveWindow(m_hWnd,(width-(swp.right-swp.left))/2,0,
					swp.right-swp.left,
					swp.bottom-swp.top,FALSE);
	else if ( (width-(swp.right-swp.left))/2 < 0 )
		MoveWindow(m_hWnd,0,(height-(swp.bottom-swp.top))/2,
					swp.right-swp.left,
					swp.bottom-swp.top,FALSE);
	else
		MoveWindow(m_hWnd,(width-(swp.right-swp.left))/2,
					(height-(swp.bottom-swp.top))/2,
					swp.right-swp.left,
					swp.bottom-swp.top,FALSE);
}

// ==========================================================
// Interface
// ==========================================================
int C_SlaveWnd::ShowSettingGame(LPSTART_GAME_SETTING pGameSetting)
{
	if ( m_pStartGameWnd == NULL )
    {
		ShowIdleGame();
		// new screen with this setting
		m_pStartGameWnd = new C_StartGameWnd(this,pGameSetting);
    }
	else
	{
		if ( pGameSetting != NULL )
		{
			// set current screen this setting
			m_pStartGameWnd->SetGame(pGameSetting);
		}
	}
	return 0;
}

int C_SlaveWnd::ShowStartGame()
{
	int i;
	// set player to table by current setting , and check for my player
	// must be on lower-left side
	LockWindowUpdate(m_hWnd);

	for ( i=0; i<6 ; i++ )
	{
    	if ( m_pSlave->GetGameSetting()->nPlayerID[i] == NO_PLAYER ) continue;

		C_Player *pPlayer = m_pSlave->GetPlayer(m_pSlave->GetGameSetting()->nPlayerID[i]);
        if ( pPlayer == NULL )
        {
        	m_pSlave->GetComm()->LogMessage("#Err08\n");
			continue;
        }
        int nPos = pPlayer->GetTableOrder();

		switch ( pPlayer->GetStatus() )
		{
		case IAM_SPECTATOR:
			pPlayer->SetStatus(IAM_PLAYER);
		    SetPlayerUI(nPos,pPlayer);
			break;
		case IAM_PLAYER: case IAM_COMPUTER:
		    SetPlayerUI(nPos,pPlayer);
			break;
		}
	}
	delete m_pStartGameWnd;
	m_pStartGameWnd = NULL;
	LockWindowUpdate(NULL);

	return 0;
}

int C_SlaveWnd::ShowGameResult()
{
	if ( m_pSlave->GetMyPlayer()->GetStatus() == IAM_PLAYER )
	{
		// v1.00
		if ( SlaveApp.IsUseSound() )
			PlaySound(MAKEINTRESOURCE(IDR_SND_CLAPPING),
					C_App::m_hInstance,
					SND_RESOURCE|SND_ASYNC);
		C_ResultDlg ResultDlg(this);
		ResultDlg.DoModal();
		// unlock window update ,  in RoundDlg Closing
		LockWindowUpdate(NULL);
	}
	return 0;
}

int C_SlaveWnd::ShowIdleGame()
{
 	KillTimer(m_hWnd,AI_TIMERID);
	KillTimer(m_hWnd,PICKUPCARD_TIMERID);

	for ( int j=0 ; j<52 ; j++ )
	{
		delete m_pPickCardWnd[j];
		m_pPickCardWnd[j] = NULL;
	}
	m_bPickingUp = FALSE;

	ClearDropCard();
	ClearCardOnHand();
	delete m_pStartGameWnd; m_pStartGameWnd = NULL;
	ShowWindow(m_pPassButton->GetHandle(),SW_HIDE);
	ShowWindow(m_pExchangeButton->GetHandle(),SW_HIDE);

	if ( GetSlave()->GetGameMode() == IDLE_GAMEMODE )
	{
		EnableWindow(m_pButton[START_BUTTON]->GetHandle(),TRUE);
		EnableWindow(m_pButton[JOIN_BUTTON]->GetHandle(),TRUE);
	}

	return 0;
}

int C_SlaveWnd::AddSpectatorUI(C_Player * pPlayer)
{
	static POINT nPlayerPos[8] =
    { {10,405},{180,440},{280,440},{380,440},{480,440},{230,440},{330,440},{430,440} };
	//  0 xxxx  1  5  2  6  3  7  4

    if ( pPlayer == NULL ) return FALSE;

    // finding for empty slot
    C_PlayerWnd * pPlayerWnd;
    int nEmptyLot;

    if ( pPlayer == GetSlave()->GetMyPlayer() )
	{
    	// my player create at first position
    	nEmptyLot = 0;
        if ( m_pPlayerLotStatus[0] != NULL )
        	return FALSE;
    }
    else
    {
        int i;
        for ( i=1 ; i<MAX_NO_OF_PLAYER ; i++ )
		{
        	if ( m_pPlayerLotStatus[i] == NULL )
            	break;
		}
		// if no slot left
		if ( i == MAX_NO_OF_PLAYER )
        	return FALSE;
        else
        	nEmptyLot = i;
    }
    // create UI object at empty slot
   	pPlayerWnd = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,
									nPlayerPos[nEmptyLot].x,nPlayerPos[nEmptyLot].y,
									pPlayer );
	pPlayer->SetUI(pPlayerWnd);
    m_pPlayerLotStatus[nEmptyLot] = pPlayer;

    // if setting up create too
    if ( m_pStartGameWnd != NULL )
    	m_pStartGameWnd->AddPlayer(pPlayer);
    return TRUE;
}

int C_SlaveWnd::RemoveSpectatorUI(C_Player * pPlayer)
{
	// important :	when add -> create player , create UI
    //              when remove -> remove UI , remove player

    if ( pPlayer == NULL ) return FALSE;

    for ( int i = 0 ; i < GetSlave()->GetPlayerList()->Count() ; i++ )
    {
    	C_Player * pPy = GetSlave()->GetPlayerList()->Items(i);
    	if ( pPy == pPlayer )
        {
		    // if setting up remove from setting wnd too
		    if ( m_pStartGameWnd != NULL )
    			m_pStartGameWnd->RemovePlayer(pPy);
			// if playing remove from table too
			for ( int k=0 ; k<6 ; k++ )
			{
				if ( m_pPlayerDetailWnd[k]->GetPlayer() == pPy )
				{
					ClearPlayerUI(k);
					break;
				}
			}

            for ( int j = 0 ; j < MAX_NO_OF_PLAYER ; j++ )
			{
            	if ( m_pPlayerLotStatus[j] == pPy )
                {
                	m_pPlayerLotStatus[j] = NULL;
                    break;
                }
			}
			if ( pPy->GetStatus() != IAM_COMPUTER )
			{
				delete pPy->GetUI();
				pPy->SetUI(NULL);
			}
            return TRUE;
        }
    }
    return FALSE;
}

int C_SlaveWnd::SetPlayerUI(int nPos,C_Player * pPlayer)
{
	// delete old UI
    RemoveSpectatorUI(pPlayer);

    // create new UI
	m_pPlayerDetailWnd[nPos]->SetPlayer(pPlayer);
    return TRUE;
}

int C_SlaveWnd::ClearPlayerUI(int nPos)
{
	// clear player and set to be spectator again
    C_Player * pPlayer = m_pPlayerDetailWnd[nPos]->ClearPlayer();

	if ( pPlayer == NULL ) return FALSE;

	if ( pPlayer->GetStatus() != IAM_COMPUTER )
		AddSpectatorUI( pPlayer );
    return TRUE;
}
 
void C_SlaveWnd::RefreshPlayersTable(int nPos)
{
	InvalidateRect(m_pPlayerDetailWnd[nPos]->GetHandle(),NULL,FALSE);
}

int C_SlaveWnd::SetCardOnHand()
{
	LockWindowUpdate(m_hWnd);

	m_MyCardList.DeleteObjects();

	int i,nIndex = 0;
	int j;
	int k=0;
	CARD_LIST * pCard = m_pSlave->GetMyPlayer()->GetCardList();

    // look at #13 card
    int nNewLineAt = 13;
    if ( pCard->Count() > 16 )
    {
        j = 0;
        for ( i=13 ; i<pCard->Count() ; i++ )
        {
            if ( GET_NUM(*(pCard->Items(12))) != GET_NUM(*(pCard->Items(i))) )
                break;
            j++;
        }
        if ( j>0 )
        {
            for ( i=11 ; i>=0 ; i-- )
            {
                nNewLineAt--;
                if ( GET_NUM(*(pCard->Items(12))) != GET_NUM(*(pCard->Items(i))) )
                    break;
            }
        }
    }

	for ( i=0 ; i<pCard->Count() ; i++ )
	{
		if ( i < nNewLineAt ) j = 0; else j = 30;
		if ( nIndex == nNewLineAt ) nIndex = 0;
		if ( j == 30 ) k = 8;

    	C_PickCardWnd * pCardWnd = new C_PickCardWnd(this,*(pCard->Items(i)),200+nIndex*15+k,300+j,FACE_UP);
		nIndex++;
		m_MyCardList.Add(pCardWnd);
		BringWindowToTop(pCardWnd->GetHandle());
		pCardWnd->EnableKBMsgParentNotify(TRUE);
		pCardWnd->EnableClickNotify(FALSE);

	}
	LockWindowUpdate(NULL);

	return 0;
}

int C_SlaveWnd::ClearCardOnHand()
{
	m_MyCardList.DeleteObjects();
	return 0;
}

int C_SlaveWnd::RearrangeCardOnHand()
{
	return 0;
}

int C_SlaveWnd::ClearDropCard()
{
	m_DropCardList.DeleteObjects();
	return 0;
}

int C_SlaveWnd::PickUpCard()
{
#ifdef _DEBUG
	if ( m_bPickingUp )
	{
		TRACE("error overlap calling of pickup card\n");
		// clear all last picking cards
		for ( int i=0 ; i<52 ; i++ )
		{
			if ( m_pPickCardWnd[i] != NULL )
			{
				delete m_pPickCardWnd[i];
				m_pPickCardWnd[i] = NULL;
			}
		}
	}
//	TRACE("pickupcard of jubmooWnd%d\n",m_nPickDelay);
#endif

	m_bPickingUp = 1;
	int nNoOfCard = m_DropCardList.Count();
	for ( int j=0 ; j<nNoOfCard ; j++ )
	{
		m_pPickCardWnd[j] = m_DropCardList.Items(0);
		m_DropCardList.Remove(0);
	}

	for ( int k=nNoOfCard ; k<52 ; k++ )
		m_pPickCardWnd[k] = NULL;

	// save nPos to move card , to cancel, if overlap calling of pickupcard
	m_nNoOfPick = nNoOfCard;
	SetTimer(m_hWnd,PICKUPCARD_TIMERID,m_nPickDelay,NULL);
	return 0;
}

LRESULT C_SlaveWnd::OnPickUpCardTimer()
{
	// 303 because pickcardwnd 'll move incorrectly when X Pos is the same
	POINT pntDesPos = {450,-(CARD_HEIGHT*2)};
	static int i = 0;

	if ( m_bPickingUp == 1 )
	{
//		TRACE("first pick\n");
		if ( m_nNoOfPick == 0 )
		{
			m_bPickingUp = 0;
			KillTimer(m_hWnd,PICKUPCARD_TIMERID);

			// after pickupcard call EndTurn to continue jubmoo engine
			if ( m_pSlave->GetGameMode() == SERVER_GAMEMODE )
				m_pSlave->EndTurn();
			return 0;
		}
		i=0;
		m_bPickingUp = 2;

	}

	if ( m_bPickingUp == 2 )
	{
		if ( SlaveApp.IsUseSound() )
			PlaySound(MAKEINTRESOURCE(IDR_SND_DISCARD1),
					C_App::m_hInstance,
					SND_RESOURCE|SND_ASYNC);
		m_pPickCardWnd[i]->MoveCardTo(pntDesPos.x,pntDesPos.y,
								PICKUPCARD_READY,FAST_MOVECARD);
		i++;
		if ( i==m_nNoOfPick )
		{
			i=0;
			m_bPickingUp = 0;
			KillTimer(m_hWnd,PICKUPCARD_TIMERID);

			// after pickupcard call EndTurn to continue jubmoo engine
			if ( m_pSlave->GetGameMode() == SERVER_GAMEMODE )
				m_pSlave->EndTurn();
		}
		else if ( i==1 )
		{
			KillTimer(m_hWnd,PICKUPCARD_TIMERID);
			SetTimer(m_hWnd,PICKUPCARD_TIMERID,100,NULL);
		}
	}
	return 0;
}

int C_SlaveWnd::RequestDiscard()
{
	m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->ShowActive();

	int i;
	// if active player is my player, set card to autopopup
	if ( m_pSlave->GetActivePlayerID() == m_pSlave->GetMyPlayer()->GetPlayerID() )
	{
		// add cheat
		m_pSlave->GetMyPlayer()->GetSlaveAI()->ShowStrength();
		// ======================
		if ( SlaveApp.IsAlarmTurn() )
			if ( m_pSlave->GetIngameTable()->nDiscardType != 0 )
				PlaySound(MAKEINTRESOURCE(IDR_SND_MYTURN),
						C_App::m_hInstance,
						SND_RESOURCE|SND_ASYNC);

		for ( i=0 ; i<4 ; i++ )
			m_pSelCardWnd[i] = NULL;

		int nCountNum[13];
		for ( i=0 ; i<13 ; i++ )
			nCountNum[i] = 0;
		// count cards
		for ( i=0 ; i<GetMyCardList()->Count() ; i++ )
		{
			C_PickCardWnd * pCard = GetMyCardList()->Items(i);
			nCountNum[GET_NUM(pCard->GetCardNumber())]++;
		}
		int nLastNum = GET_NUM(m_pSlave->GetIngameTable()->LatestCard.nCard[0]);

		switch ( m_pSlave->GetIngameTable()->nDiscardType )
		{
		case NONE_CARD:
			// activate all cards
			for ( i=0 ; i<GetMyCardList()->Count() ; i++ )
		    {
				C_PickCardWnd * pCard = GetMyCardList()->Items(i);
	        	pCard->EnableClickNotify(TRUE);
				pCard->EnableAutoMoveUp(TRUE);
	        }
			break;
		case ONE_CARD:
			for ( i=0 ; i<GetMyCardList()->Count() ; i++ )
			{
				C_PickCardWnd * pCard = GetMyCardList()->Items(i);
				int nNum = GET_NUM(pCard->GetCardNumber());
				switch(nCountNum[nNum]) 
				{
				case 1: case 2: 
					if ( m_pSlave->GetGameSetting()->bCheckSuit )
					{
						// on single discard , check for suit and number
						if ( pCard->GetCardNumber() > m_pSlave->GetIngameTable()->LatestCard.nCard[0] )
						{
							if ( GET_NUM(pCard->GetCardNumber()) == GET_NUM(m_pSlave->GetIngameTable()->LatestCard.nCard[0]) ||
									GET_FACE(pCard->GetCardNumber()) == GET_FACE(m_pSlave->GetIngameTable()->LatestCard.nCard[0]) )
							{
	        					pCard->EnableClickNotify(TRUE);
								pCard->EnableAutoMoveUp(TRUE);
							}
						}
					}
					else
					{
						if ( pCard->GetCardNumber() > m_pSlave->GetIngameTable()->LatestCard.nCard[0] )
						{
	        				pCard->EnableClickNotify(TRUE);
							pCard->EnableAutoMoveUp(TRUE);
						}
					}
					break;
				case 3: case 4:
        			pCard->EnableClickNotify(TRUE);
					pCard->EnableAutoMoveUp(TRUE);
					break;
				}
			}
			ShowWindow(m_pPassButton->GetHandle(),SW_SHOW);
			break;
		case TWO_CARD:
			for ( i=0 ; i<GetMyCardList()->Count() ; i++ )
			{
				C_PickCardWnd * pCard = GetMyCardList()->Items(i);
				int nNum = GET_NUM(pCard->GetCardNumber());
				switch(nCountNum[nNum]) 
				{
				case 2:
					if ( nNum > nLastNum )
					{
	        			pCard->EnableClickNotify(TRUE);
						pCard->EnableAutoMoveUp(TRUE);
					}
					else if ( nNum == nLastNum )
					{
						if ( GET_FACE(m_pSlave->GetIngameTable()->LatestCard.nCard[1]) != SPADE )
						{
		        			pCard->EnableClickNotify(TRUE);
							pCard->EnableAutoMoveUp(TRUE);
						}
					}
					break;
				case 3:
					if ( nNum > nLastNum )
					{
	        			pCard->EnableClickNotify(TRUE);
						pCard->EnableAutoMoveUp(TRUE);
					}
					break;
				case 4:
        			pCard->EnableClickNotify(TRUE);
					pCard->EnableAutoMoveUp(TRUE);
					break;
				}
			}
			ShowWindow(m_pPassButton->GetHandle(),SW_SHOW);
			break;
		case THREE_CARD:
			for ( i=0 ; i<GetMyCardList()->Count() ; i++ )
			{
				C_PickCardWnd * pCard = GetMyCardList()->Items(i);
				int nNum = GET_NUM(pCard->GetCardNumber());
				switch(nCountNum[nNum]) 
				{
				case 3:
					if ( nNum > nLastNum )
					{
	        			pCard->EnableClickNotify(TRUE);
						pCard->EnableAutoMoveUp(TRUE);
					}
					break;
				case 4:
        			pCard->EnableClickNotify(TRUE);
					pCard->EnableAutoMoveUp(TRUE);
					break;
				}
			}
			ShowWindow(m_pPassButton->GetHandle(),SW_SHOW);
			break;
		case FOUR_CARD:
			for ( i=0 ; i<GetMyCardList()->Count() ; i++ )
			{
				C_PickCardWnd * pCard = GetMyCardList()->Items(i);
				int nNum = GET_NUM(pCard->GetCardNumber());
				switch(nCountNum[nNum]) 
				{
				case 4:
					if ( nNum > nLastNum )
					{
	        			pCard->EnableClickNotify(TRUE);
						pCard->EnableAutoMoveUp(TRUE);
					}
					break;
				}
			}
			ShowWindow(m_pPassButton->GetHandle(),SW_SHOW);
			break;
		}
		return TRUE;
	}

	// if computer, set timer to get input from active player
	C_Player * pPlayer = m_pSlave->GetPlayer(m_pSlave->GetActivePlayerID());
	if ( pPlayer == NULL )
	{
       	m_pSlave->GetComm()->LogMessage("#Err09\n");
		return FALSE;
    }

    if ( pPlayer->GetStatus() == IAM_COMPUTER && m_pSlave->GetGameMode() == SERVER_GAMEMODE)
	{
		// slave engine call this function to generate AI's discard
		SetTimer(m_hWnd,AI_TIMERID,m_nAIDelay,NULL);
	}
	return TRUE;
}

int C_SlaveWnd::DiscardSelectedCard()
{
	int nDiscardType = 0;
	DISCARD_CARD Card;
	int i;
	
	for ( i=0 ; i<4 ; i++ )
		Card.nCard[i] = 0;

	for ( i=0 ; i<4 ; i++ )
	{
		if ( m_pSelCardWnd[i] != NULL )
		{
			Card.nCard[nDiscardType] = m_pSelCardWnd[i]->GetCardNumber();
			nDiscardType++;
		}
	}
	if ( nDiscardType == 0 ) return FALSE;
	
	switch ( m_pSlave->IsValidDiscard(nDiscardType,&Card) )
	{
	case DISCARD_OK:
		m_pSlave->GetMyPlayer()->Discard(nDiscardType,&Card);
		return TRUE;
	case DISCARD_INVALID:
		MessageBox(m_hWnd,"Invalid card. Try again",C_SlaveApp::m_lpszAppName,MB_OK);
		// clear all selected card
		for ( i=0 ; i<4 ; i++ )
		{
			if ( m_pSelCardWnd[i] != NULL )
			{
				m_pSelCardWnd[i]->EnableAutoMoveUp(FALSE);
				m_pSelCardWnd[i]->FreezeCard(FALSE);
			}
			m_pSelCardWnd[i] = NULL;
		}
		RequestDiscard();
		break;
	case DISCARD_TONGCANTBEAT2:
		MessageBox(m_hWnd,"Only Tong(ตอง) of 2 can beat 2-card. Try again",C_SlaveApp::m_lpszAppName,MB_OK);
		// clear all selected card
		for ( i=0 ; i<4 ; i++ )
		{
			if ( m_pSelCardWnd[i] != NULL )
			{
				m_pSelCardWnd[i]->EnableAutoMoveUp(FALSE);
				m_pSelCardWnd[i]->FreezeCard(FALSE);
			}
			m_pSelCardWnd[i] = NULL;
		}
		RequestDiscard();
		break;
	case DISCARD_SUITINVALID:
		MessageBox(m_hWnd,"You must discard with the same suit/number. Try again",C_SlaveApp::m_lpszAppName,MB_OK);
		// clear all selected card
		for ( i=0 ; i<4 ; i++ )
		{
			if ( m_pSelCardWnd[i] != NULL )
			{
				m_pSelCardWnd[i]->EnableAutoMoveUp(FALSE);
				m_pSelCardWnd[i]->FreezeCard(FALSE);
			}
			m_pSelCardWnd[i] = NULL;
		}
		RequestDiscard();
		break;
	}
	return FALSE;
}

int C_SlaveWnd::Discard(int nPos,int nLastPos,
						int nDiscardType,LPDISCARD_CARD pCardNum,
						int nDiscardMode)
{
	if ( nDiscardMode != SUDDEN_DISCARDMODE )
	{
		m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->ShowInactive();
		if ( nLastPos >= 0 )
			m_pPlayerDetailWnd[nLastPos]->ShowInactive();
	}

	int i,j;

    C_PickCardWnd * pCardWnd[4] = {NULL,NULL,NULL,NULL};
	// if my player, just remove from mycardlist
    if ( m_pSlave->GetMyPlayer()->GetTableOrder() == nPos )
    {
		// disable all card
		for ( i=0 ; i<m_MyCardList.Count() ; i++ )
		{
			m_MyCardList.Items(i)->EnableAutoMoveUp(FALSE);
			m_MyCardList.Items(i)->EnableClickNotify(FALSE);
		}
		// get selected card
		for ( j=0 ; j<nDiscardType ; j++ )
		{
	    	for ( i=0 ; i<m_MyCardList.Count() ; i++ )
		    {
				if ( m_MyCardList.Items(i)->GetCardNumber() == pCardNum->nCard[j] )
				{
					pCardWnd[j] = m_MyCardList.Items(i);
					m_MyCardList.Remove(i);
					break;
				}
			}
        }
    }
    else
    {
		int nCardPosX,nCardPosY;

		if ( m_pSlave->GetActivePlayerOrder() < 3 )
		{
			nCardPosX = m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->GetPosX()-90;
			nCardPosY = m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->GetPosY();
		}
		else
		{
			nCardPosX = m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->GetPosX()+PLAYERDETAIL_WIDTH+20;
			nCardPosY = m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->GetPosY();
		}

		for ( j=0 ; j<nDiscardType ; j++ )
		{
			pCardWnd[j] = new C_PickCardWnd(this,pCardNum->nCard[j],nCardPosX,nCardPosY,FACE_UP);
		    pCardWnd[j]->EnableKBMsgParentNotify(TRUE);
			pCardWnd[j]->EnableClickNotify(FALSE);
		}
    }

	static int nOffsetX = 233;
	static int nOffsetY = 40;

	if ( nLastPos < 0 ) { nOffsetX = 233; nOffsetY = 40; }
	if ( nOffsetY > 130 ) { nOffsetX -= 110; nOffsetY -= 110; }

	nOffsetY += 15;
	nOffsetX += 15;

	for ( j=0 ; j<nDiscardType ; j++ )
	{
		if ( pCardWnd[j] != NULL )
		{
			m_DropCardList.Add(pCardWnd[j]);
			BringWindowToTop(pCardWnd[j]->GetHandle());

			if ( nDiscardMode == SUDDEN_DISCARDMODE )
				// suddenly move card , use to show drop card when client come in
				pCardWnd[j]->MoveCardTo(nOffsetX+j*18,nOffsetY,0,999);
			else
			{
				if ( SlaveApp.IsUseSound() )
				{
					if ( nDiscardType >= THREE_CARD )
						PlaySound(MAKEINTRESOURCE(IDR_SND_SWOOZ),
								C_App::m_hInstance,
								SND_RESOURCE|SND_ASYNC);
					else
						PlaySound(MAKEINTRESOURCE(IDR_SND_DISCARD2),
								C_App::m_hInstance,
								SND_RESOURCE|SND_ASYNC);
				}
				pCardWnd[j]->MoveCardTo(nOffsetX+j*18,nOffsetY,0,NORMAL_MOVECARD);
			}
		}
	}

	ShowWindow(m_pPassButton->GetHandle(),SW_HIDE);
//	m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->Refresh();
	m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->Refresh(REFRESH_DISCARDSTATUS);
	m_pPlayerDetailWnd[m_pSlave->GetActivePlayerOrder()]->Refresh(REFRESH_CARDLIST);
	return 0;
}

int C_SlaveWnd::Pass(int nPos)
{
	int i;
	// disable all card
	for ( i=0 ; i<m_MyCardList.Count() ; i++ )
	{
		m_MyCardList.Items(i)->EnableAutoMoveUp(FALSE);
		m_MyCardList.Items(i)->EnableClickNotify(FALSE);
		m_MyCardList.Items(i)->FreezeCard(FALSE);
	}
	ShowWindow(m_pPassButton->GetHandle(),SW_HIDE);
	m_pPlayerDetailWnd[nPos]->ShowInactive();
	return 0;
}

int C_SlaveWnd::RequestExchange()
{
	switch ( m_pSlave->GetMyPlayer()->GetPosition() )
	{
	case IAM_KING:
	case IAM_QUEEN:
		if ( m_pSlave->GetMyPlayer()->GetPosition() == IAM_KING )
			m_Tooltip.UpdateTipText(m_pExchangeButton->GetHandle(),
									"Select 2 cards to change with Slave");
		else
			m_Tooltip.UpdateTipText(m_pExchangeButton->GetHandle(),
									"Select a card to change with Vice Slave");
		ShowWindow(m_pExchangeButton->GetHandle(),SW_SHOW);
		// enable all card
		m_pExcCardWnd[0] = NULL;
		m_pExcCardWnd[1] = NULL;
		for ( int i = 0 ; i<GetMyCardList()->Count() ; i++ )
		{
			C_PickCardWnd * pCard = GetMyCardList()->Items(i);
   			pCard->EnableClickNotify(TRUE);
			pCard->EnableAutoMoveUp(TRUE);
		}
		break;
	}

	// if computer, set timer to get input from active player
	if ( m_pSlave->GetGameMode() == SERVER_GAMEMODE )
	{
		// computer exchange the cards
		C_Player * pPlayer = m_pSlave->GetPlayerByPosition(IAM_KING);
		if ( pPlayer != NULL )
		{
			if ( pPlayer->GetStatus() == IAM_COMPUTER )
				SetTimer(m_hWnd,AI_KINGEXC_TIMERID,AI_KINGEXC_TIMER,NULL);
		}
		pPlayer = m_pSlave->GetPlayerByPosition(IAM_QUEEN);
		if ( pPlayer != NULL )
		{
			if ( pPlayer->GetStatus() == IAM_COMPUTER )
				SetTimer(m_hWnd,AI_QUEENEXC_TIMERID,AI_QUEENEXC_TIMER,NULL);
		}
	}

	return TRUE;
}

void C_SlaveWnd::RefreshDiscardDirection()
{
	RECT rc = {460,405,492,437};
	InvalidateRect(m_hWnd,&rc,FALSE);
}

// =====================================================================
// Event Handler
// =====================================================================
LRESULT C_SlaveWnd::OnPaint(WPARAM ,LPARAM )
{
    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

	RECT rc;
	GetClientRect(m_hWnd,&rc);
	BitBlt(m_hDC,0,0,rc.right-rc.left,rc.bottom-rc.top,m_hdcBkMem,0,0,SRCCOPY);

	if ( m_pSlave->GetIngameTable()->nDiscardDirection == NORMAL_DIRECTION )
		DrawIcon(m_hDC,460,405,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_DIRNORM)));
	else
		DrawIcon(m_hDC,460,405,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_DIRREV)));

	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT C_SlaveWnd::OnNCLButtonDblClk(WPARAM ,LPARAM )
{
	if ( !IsIconic(m_hWnd) )
		ShowWindow(m_hWnd,SW_MINIMIZE);
	return 0;
}

LRESULT C_SlaveWnd::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( wParam )
	{
    case ID_SV_START: OnStartButtonClick(); break;
    case ID_SV_JOIN: OnJoinButtonClick(); break;
    case ID_SV_QUIT: OnQuitButtonClick(); break;
    case ID_SV_SETTING:
    	{
        C_SettingDlg SettingDlg(this);
		if ( SettingDlg.DoModal() == IDCANCEL )
        	return 0;
        }
 	    InvalidateRect(m_hWnd,NULL,FALSE);
        break;
	case ID_SV_HELP:
		{
        C_HelpDlg HelpDlg(this);
        HelpDlg.DoModal();
        }
		break;
	case ID_SV_ABOUT:
		{
		C_AboutDlg AboutDlg(this);
		AboutDlg.DoModal();
#ifdef _DEBUG
		// test ///////////////////////////////////
		HDC hdc = GetDC(m_hWnd);
		char tmpbuf[99];
		sprintf(tmpbuf,"nDC Type %d nDC Dir %d Latest=%d,%d,%d,%d",
				m_pSlave->GetIngameTable()->nDiscardType,
				m_pSlave->GetIngameTable()->nDiscardDirection,
				m_pSlave->GetIngameTable()->LatestCard.nCard[0],
				m_pSlave->GetIngameTable()->LatestCard.nCard[1],
				m_pSlave->GetIngameTable()->LatestCard.nCard[2],
				m_pSlave->GetIngameTable()->LatestCard.nCard[3]
				);
		TextOut(hdc,200,0,tmpbuf,strlen(tmpbuf));
		sprintf(tmpbuf,"PID %d,%d,%d,%d,%d,%d NOOFP %d",
				m_pSlave->GetGameSetting()->nPlayerID[0],
				m_pSlave->GetGameSetting()->nPlayerID[1],
				m_pSlave->GetGameSetting()->nPlayerID[2],
				m_pSlave->GetGameSetting()->nPlayerID[3],
				m_pSlave->GetGameSetting()->nPlayerID[4],
				m_pSlave->GetGameSetting()->nPlayerID[5],
				m_pSlave->GetGameSetting()->nNoOfPlayer
				);
		TextOut(hdc,200,18,tmpbuf,strlen(tmpbuf));
		ReleaseDC(m_hWnd,hdc);
#endif
		}
		break;
	case ID_SV_EXIT:
        SendMessage(m_hWnd,WM_CLOSE,0,0);
		break;
	case IDI_PASS:			// User Click pass this round
		m_pSlave->GetMyPlayer()->Pass();
		break;
	case IDI_EXCHANGE:
		{
			DISCARD_CARD card;
            for ( int k=0 ; k<4 ; k++ )
                card.nCard[k] = 0;
			if ( m_pSlave->GetMyPlayer()->GetPosition() == IAM_KING )
			{
				if ( m_pExcCardWnd[0] != NULL && m_pExcCardWnd[1] != NULL )
				{
					card.nCard[0] = m_pExcCardWnd[0]->GetCardNumber();
					card.nCard[1] = m_pExcCardWnd[1]->GetCardNumber();
					m_pSlave->ExchangeCard(m_pSlave->GetMyPlayer()->GetPlayerID(),IAM_KING,&card);
					// diable all card on hand
					for ( int i = 0 ; i<GetMyCardList()->Count() ; i++ )
					{
						C_PickCardWnd * pCard = GetMyCardList()->Items(i);
			   			pCard->EnableClickNotify(FALSE);
						pCard->EnableAutoMoveUp(FALSE);
					}
					ShowWindow(m_pExchangeButton->GetHandle(),SW_HIDE);
					return 0;
				}
			}
			else if ( m_pSlave->GetMyPlayer()->GetPosition() == IAM_QUEEN )
			{
				if ( m_pExcCardWnd[0] != NULL )
				{
					card.nCard[0] = m_pExcCardWnd[0]->GetCardNumber();
					m_pSlave->ExchangeCard(m_pSlave->GetMyPlayer()->GetPlayerID(),IAM_QUEEN,&card);
					// diable all card on hand
					for ( int i = 0 ; i<GetMyCardList()->Count() ; i++ )
					{
						C_PickCardWnd * pCard = GetMyCardList()->Items(i);
			   			pCard->EnableClickNotify(FALSE);
						pCard->EnableAutoMoveUp(FALSE);
					}
					ShowWindow(m_pExchangeButton->GetHandle(),SW_HIDE);
					return 0;
				}
			}
		}
		break;
	}
	return 0;
}

LRESULT C_SlaveWnd::OnClose(WPARAM wParam,LPARAM lParam)
{
	if ( MessageBox(m_hWnd,"Exit Slave really?",C_SlaveApp::m_lpszAppName,
    				MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) == IDOK )
	{
		KillTimer(m_hWnd,AI_TIMERID);
		KillTimer(m_hWnd,PICKUPCARD_TIMERID);

		return DefWindowProc( m_hWnd, WM_CLOSE, wParam, lParam );
	}
	return 0;
}

LRESULT C_SlaveWnd::OnDestroy(WPARAM ,LPARAM )
{
	DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
	DeleteDC(m_hdcBkMem);
	DeleteObject(m_hFont);
	DeleteObject(m_hBigFont);

	PostQuitMessage( 0 );
	return 0;
}

LRESULT C_SlaveWnd::OnChar(WPARAM wParam,LPARAM lParam)
{
	// notify all WM_CHAR to chatedit
	SendMessage(m_pChatEdit->GetHandle(),WM_CHAR,wParam,lParam);
    return DefWindowProc(m_hWnd,WM_CHAR,wParam,lParam);
}

LRESULT C_SlaveWnd::OnRButtonDown(WPARAM ,LPARAM )
{
	// create menu
	HMENU hMenu = CreatePopupMenu();
 
	switch ( m_pSlave->GetGameMode() )
	{
	case IDLE_GAMEMODE:
		AppendMenu(hMenu,MF_STRING,ID_SV_START,"Start");
		AppendMenu(hMenu,MF_STRING,ID_SV_JOIN,"Join");
		break;
	case SERVER_GAMEMODE:
	case CLIENT_GAMEMODE:
		AppendMenu(hMenu,MF_STRING,ID_SV_QUIT,"Quit");
		break;
	}
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,ID_SV_SETTING,"Setting");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,ID_SV_HELP,"Help");
	AppendMenu(hMenu,MF_STRING,ID_SV_ABOUT,"About");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,ID_SV_EXIT,"Exit");
	
	POINT p;
	GetCursorPos(&p);
	TrackPopupMenuEx(hMenu,TPM_VERTICAL,p.x,p.y,m_hWnd,NULL);
	DestroyMenu(hMenu);
	
	return 0;
}

LRESULT C_SlaveWnd::OnTimer(WPARAM wParam,LPARAM )
{
	switch (wParam)
	{
	case PICKUPCARD_TIMERID:
		return OnPickUpCardTimer();
    case AI_TIMERID:
   		return OnAIDiscardRequest();
	case AI_KINGEXC_TIMERID:
		{
			KillTimer(m_hWnd,AI_KINGEXC_TIMERID);
			C_Player * pPlayer = m_pSlave->GetPlayerByPosition(IAM_KING);
			pPlayer->ActivateAI();
		}
		break;
	case AI_QUEENEXC_TIMERID:
		{
			KillTimer(m_hWnd,AI_QUEENEXC_TIMERID);
			C_Player * pPlayer = m_pSlave->GetPlayerByPosition(IAM_QUEEN);
			pPlayer->ActivateAI();
		}
		break;
	}
	return 0;
}

// ==============================================================
// User defined handler
// ==============================================================
LRESULT C_SlaveWnd::OnCardWndLButtonClick(WPARAM ,LPARAM lParam)
{
	// if now picking card , can't drop the card
	if ( m_bPickingUp ) return 0;

	if ( HIBYTE(GetKeyState(VK_RBUTTON)) ) return 0;

	// click notify off , after discard
	C_PickCardWnd * pCardWnd = (C_PickCardWnd*)lParam;
	int nCardNum = pCardWnd->GetCardNumber();

	if ( !pCardWnd->IsMovedUp() ) return 0;

	if ( m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE1 || 
			m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE2 )
	{

		int i;
		int n = 0;
		if ( m_pSlave->GetMyPlayer()->GetPosition() == IAM_KING )
			n = 2;
		else if ( m_pSlave->GetMyPlayer()->GetPosition() == IAM_QUEEN )
			n = 1;

		for ( i=0 ; i<n ; i++ )
			if ( m_pExcCardWnd[i] == NULL )
			{
				m_pExcCardWnd[i] = pCardWnd;
				pCardWnd->FreezeCard();
				break;
			}

		return 0;
	}

    // count this num on hand
	int nCount=0;
	int i;
	for ( i=0 ; i<GetMyCardList()->Count() ; i++ )
	{
		if ( GET_NUM(GetMyCardList()->Items(i)->GetCardNumber()) ==
				GET_NUM(nCardNum) )
			nCount++;
	}
    // check last selected card, should be same , if not -> clear it
    for ( i=0 ; i<4 ; i++ )
    {
        if ( m_pSelCardWnd[i] != NULL )
        {
            if ( GET_NUM(m_pSelCardWnd[i]->GetCardNumber()) != GET_NUM(nCardNum) )
            {
                m_pSelCardWnd[i]->EnableAutoMoveUp(FALSE);
                m_pSelCardWnd[i]->FreezeCard(FALSE);
                m_pSelCardWnd[i]->EnableAutoMoveUp(TRUE);
    			m_pSelCardWnd[i] = NULL;
			}
        }
    }

	switch ( m_pSlave->GetIngameTable()->nDiscardType )
	{
	case NONE_CARD:
		{
			// lead card
			// if select in single card , drop it
			if ( nCount > 1 )
			{
				// if already selected , drop it
				for ( i=0 ; i<4 ; i++ )
				{
					if ( m_pSelCardWnd[i] == pCardWnd )
						// discard
						return DiscardSelectedCard();
				}
				m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
				pCardWnd->FreezeCard();
				// disable all other number
				for ( i=0 ; i<GetMyCardList()->Count() ; i++ )
				{
					C_PickCardWnd * p = GetMyCardList()->Items(i);
					if ( GET_NUM(p->GetCardNumber()) != GET_NUM(nCardNum) )
					{
						p->EnableClickNotify(FALSE);
						p->EnableAutoMoveUp(FALSE);
					}
				}
			}
			else
			{
				m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
				return DiscardSelectedCard();
			}
		}
		break;
	case ONE_CARD:	// return selected card immediately
		if ( nCount == 1 || nCount == 2 )
		{
			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			return DiscardSelectedCard();
		}
		else if ( nCount == 3 )
		{
			int bAlreadyClick = FALSE;

			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] == pCardWnd )
					bAlreadyClick = TRUE;

			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			// if already 3 card selected , drop it
			int nSelected=0;
			for ( i=0 ; i<4 ; i++ )
			{
				if ( m_pSelCardWnd[i] != NULL )
					nSelected++;
			}

			if ( nSelected == 3 )
				// discard
				return DiscardSelectedCard();

			if ( nSelected == 1 )
				if ( bAlreadyClick )
					if ( nCardNum > m_pSlave->GetIngameTable()->LatestCard.nCard[0] )
						return DiscardSelectedCard();

			return 0;
		}
		else if ( nCount == 4 )
		{
			int nSelected=0;
			int bAlreadyClick = FALSE;

			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] == pCardWnd )
					bAlreadyClick = TRUE;

			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] != NULL )
					nSelected++;

			if ( nSelected == 4 )
				// discard
				return DiscardSelectedCard();

			if ( nSelected == 1 )
				if ( bAlreadyClick )
					if ( nCardNum > m_pSlave->GetIngameTable()->LatestCard.nCard[0] )
						// discard
						return DiscardSelectedCard();

			if ( nSelected == 3 )
				if ( bAlreadyClick )
					// discard
					return DiscardSelectedCard();

			return 0;
		}
		break;
	case TWO_CARD:
		if ( nCount == 2 )
		{
			int nSelected = 0;
			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] != NULL )
					nSelected++;
			if ( nSelected == 2 )
				return DiscardSelectedCard();
		}
		else if ( nCount == 3 )
		{
			int nSelected=0;

			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] != NULL )
					nSelected++;

			if ( nSelected == 2 )
				return DiscardSelectedCard();

		}
		else if ( nCount == 4 )
		{
			int nSelected=0;
			int bAlreadyClick = FALSE;

			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] == pCardWnd )
					bAlreadyClick = TRUE;

			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] != NULL )
					nSelected++;

			if ( nSelected == 4 )
				// discard
				return DiscardSelectedCard();

			if ( nSelected == 2 )
				if ( bAlreadyClick )
					// discard
					if ( nCardNum > m_pSlave->GetIngameTable()->LatestCard.nCard[0] )
						return DiscardSelectedCard();
		}
		break;
	case THREE_CARD:
		if ( nCount == 3 )
		{
			int nSelected = 0;
			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] != NULL )
					nSelected++;
			if ( nSelected == 3 )
				return DiscardSelectedCard();
		}
		else if ( nCount == 4 )
		{
			int nSelected=0;
			int bAlreadyClick = FALSE;

			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] == pCardWnd )
					bAlreadyClick = TRUE;

			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] != NULL )
					nSelected++;

			if ( nSelected == 4 )
				// discard
				return DiscardSelectedCard();

			if ( nSelected == 3 )
				if ( bAlreadyClick )
					// discard
					if ( nCardNum > m_pSlave->GetIngameTable()->LatestCard.nCard[0] )
						return DiscardSelectedCard();
		}
		break;
	case FOUR_CARD:
		if ( nCount == 4 )
		{
			int nSelected = 0;
			m_pSelCardWnd[GET_FACE(nCardNum)] = pCardWnd;
			pCardWnd->FreezeCard();
			for ( i=0 ; i<4 ; i++ )
				if ( m_pSelCardWnd[i] != NULL )
					nSelected++;
			if ( nSelected == 4 )
				return DiscardSelectedCard();
		}
		break;
	}

	return 0;
}

LRESULT C_SlaveWnd::OnCardWndRButtonClick(WPARAM ,LPARAM lParam)
{
	if ( HIBYTE(GetKeyState(VK_LBUTTON)) ) return 0;

	// click notify off , after discard
	C_PickCardWnd * pCardWnd = (C_PickCardWnd*)lParam;

	int i;
	if ( m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE1 ||
			m_pSlave->GetPlayMode() == EXCHANGE_PLAYMODE2 )
	{
		for ( i=0 ; i<2 ; i++ )
		{
			if ( m_pExcCardWnd[i] == pCardWnd )
			{
				for ( int j=0 ; j<2 ; j++ )
				{
					if ( m_pExcCardWnd[j] != NULL )
					{
						m_pExcCardWnd[j]->EnableAutoMoveUp(FALSE);
						m_pExcCardWnd[j]->FreezeCard(FALSE);
						m_pExcCardWnd[j]->EnableAutoMoveUp(TRUE);
					}
					m_pExcCardWnd[j] = NULL;
				}
				break;
			}
		}
		return 0;
	}

	for ( i=0 ; i<4 ; i++ )
	{
		if ( m_pSelCardWnd[i] == pCardWnd )
		{
			for ( int j=0 ; j<4 ; j++ )
			{
				if ( m_pSelCardWnd[j] != NULL )
				{
					m_pSelCardWnd[j]->EnableAutoMoveUp(FALSE);
					m_pSelCardWnd[j]->FreezeCard(FALSE);
					m_pSelCardWnd[j]->EnableAutoMoveUp(TRUE);
				}
				m_pSelCardWnd[j] = NULL;
			}
			RequestDiscard();
			break;
		}
	}
	return 0;
}

LRESULT C_SlaveWnd::OnPickCardReady(WPARAM wParam,LPARAM lParam)
{
	C_PickCardWnd * pCardWnd = (C_PickCardWnd*)lParam;
	switch ( wParam )
	{
	case PICKUPCARD_READY:
		for ( int i=0 ; i<52 ; i++ )
		{
			if ( pCardWnd == m_pPickCardWnd[i] )
			{
				//TRACE("delete pick card %d\n",i);
				delete pCardWnd;
				m_pPickCardWnd[i] = NULL;
				break;
			}
		}
		break;
	}
	return 0;
}

LRESULT C_SlaveWnd::OnStartButtonClick()
{
    // ============ Start waiting for connection =========
    if ( GetSlave()->GetGameMode() != IDLE_GAMEMODE ) return 0;

    int nRetVal = GetSlave()->StartServer();
    if ( nRetVal == FALSE )
    	MessageBox(GetActiveWindow(),"Can't start network server. You still can play alone.",
                C_SlaveApp::m_lpszAppName,MB_OK|MB_ICONERROR);

	ShowSettingGame();
	EnableWindow(m_pButton[START_BUTTON]->GetHandle(),FALSE);
	EnableWindow(m_pButton[JOIN_BUTTON]->GetHandle(),FALSE);
	return 0;
}

LRESULT C_SlaveWnd::OnJoinButtonClick()
{
	if ( GetSlave()->GetGameMode() != IDLE_GAMEMODE ) return 0;

	char lpszJoinIP[64];

	SlaveApp.OpenRegistry();
	SlaveApp.GetJoinIP(lpszJoinIP,sizeof(lpszJoinIP));
	SlaveApp.CloseRegistry();

	C_JoinDlg JoinDlg(this,lpszJoinIP,sizeof(lpszJoinIP));
	if ( JoinDlg.DoModal() == IDOK )
	{
		// temporarily turn off cooldlg , it'll cause app error
		// when system dialog appear to ask for connection
		int bUseCoolDlg = FALSE;
		if ( hCoolDlgHook != NULL )
		{
			CoolDlgTerminate();
			bUseCoolDlg = TRUE;
		}
	    if ( !GetSlave()->StartClient(lpszJoinIP) )
		{
			if ( bUseCoolDlg )
				CoolDlgAutoSubclass();
			// connect fail
			GetSlave()->GetComm()->PopupMessage("/c12Joining to invalid address");
			return 0;
		}
		else
			if ( bUseCoolDlg )
				CoolDlgAutoSubclass();

		EnableWindow(m_pButton[START_BUTTON]->GetHandle(),FALSE);
		EnableWindow(m_pButton[JOIN_BUTTON]->GetHandle(),FALSE);
		SlaveApp.OpenRegistry();
		SlaveApp.SetJoinIP(lpszJoinIP);
		SlaveApp.CloseRegistry();
	}
    return 0;
}

LRESULT C_SlaveWnd::OnQuitButtonClick()
{
	switch ( GetSlave()->GetGameMode() )
	{
	case IDLE_GAMEMODE:
		return 0;
	case SERVER_GAMEMODE:
		// if playing , quit is to setting , else is to idle
		if ( GetSlave()->GetPlayMode() == INGAME_PLAYMODE ||
				GetSlave()->GetPlayMode() == EXCHANGE_PLAYMODE1 ||
				GetSlave()->GetPlayMode() == EXCHANGE_PLAYMODE2 )
		{
			if ( MessageBox(m_hWnd,"Quit game and Go back to game setting again?",C_SlaveApp::m_lpszAppName,
						MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
				return 0;
			// quit to setting again
//			ShowWindow(m_pContinueButton->GetHandle(),SW_HIDE);
			GetSlave()->ResetGame();
			return 0;
		}
		else if ( GetSlave()->GetPlayMode() == SETTING_PLAYMODE )
		{
			if ( MessageBox(m_hWnd,"Quit game really?",C_SlaveApp::m_lpszAppName,
						MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
				return 0;
		}
		break;
	case CLIENT_GAMEMODE:
		if ( GetSlave()->GetPlayMode() == INGAME_PLAYMODE ||
				GetSlave()->GetPlayMode() == SETTING_PLAYMODE )
			if ( MessageBox(m_hWnd,"Quit game really?",C_SlaveApp::m_lpszAppName,
    					MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
				return 0;
		break;
	}

	// do quit here

	// if already idle, can quit again, it may help to clear unwant things
    LockWindowUpdate(m_hWnd);

	// remove player from table
	for ( int i=0 ; i<6 ; i++ )
		ClearPlayerUI(i);

	GetSlave()->QuitGame();

	LockWindowUpdate(NULL);

	return 0;
}

LRESULT C_SlaveWnd::OnAIDiscardRequest()
{
	KillTimer(m_hWnd,AI_TIMERID);

	C_Player * pPlayer = m_pSlave->GetPlayer(m_pSlave->GetActivePlayerID());

    // verify
	if ( pPlayer == NULL )
    {
       	m_pSlave->GetComm()->LogMessage("#Err10\n");
        return FALSE;
    }

	if ( pPlayer->GetStatus() != IAM_COMPUTER )
    {
       	m_pSlave->GetComm()->LogMessage("#Err11\n");
        return FALSE;
    }

	pPlayer->ActivateAI();

	return TRUE;
}

