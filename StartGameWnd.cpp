#include "StartGameWnd.h"

#include <stdio.h>
#include "resource.h"
#include "SlaveWnd.h"
#include "kswafx/CoolButton.h"
#include "PlayerWnd.h"

#define EMPTY_LOT			-1
#define SETAS_SPECTATOR		0

#define NO_SELECTED_PLAYER	-1

#define IDC_3PGAME		1100
#define IDC_4PGAME		1101
#define IDC_5PGAME		1102
#define IDC_6PGAME		1103

BEGIN_HANDLE_MSG(C_StartGameWnd,C_GenericWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)
	HANDLE_MSG(WM_COMMAND,OnCommand)
	HANDLE_MSG(WM_RBUTTONDOWN,OnRButtonDown)

	HANDLE_MSG(WM_GENERICCLICKWND_BEGIN_DRAG,OnPlayerWndBeginDrag)
	HANDLE_MSG(WM_GENERICCLICKWND_END_DRAG,OnPlayerWndEndDrag)
	HANDLE_MSG(WM_GENERICCLICKWND_RBUTTONCLICK,OnPlayerWndRButtonClick)
END_HANDLE_MSG()

C_StartGameWnd::C_StartGameWnd(C_SlaveWnd * pSlaveWnd,LPSTART_GAME_SETTING pSetting)
			: C_GenericWnd(pSlaveWnd)
{
	m_hWnd = CreateWindow(
                    m_szClassName,
                    "",
                    WS_CHILD|WS_CLIPSIBLINGS,
                    (SLAVEWND_WIDTH-STARTGAMEWND_WIDTH)/2,30,
                    STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT,
                    pSlaveWnd->GetHandle(),
                    NULL,
                    C_App::m_hInstance,
                    (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

	m_pSlave = pSlaveWnd->GetSlave();
	m_pSlave->SetPlayMode(SETTING_PLAYMODE);
    m_bInitBkg = FALSE;
	m_nDragSlot = NO_SELECTED_PLAYER;

	// Create Command Button
	RECT rcButton = {170,280,260,330};
	RECT rcGroup = { 10,220,110,330 };
	RECT rcPlayer = { 20,240,100,260 };
	RECT rcTongBeat2 = { 120,230,265,250 };
	RECT rcCheckSuit = { 120,255,265,275 };

	// only server can press start game
	m_pContinueButton = new C_CoolButton;
	m_pGroup = new C_CoolButton;
	m_p3PlayerGame = new C_CoolButton;
	m_p4PlayerGame = new C_CoolButton;
	m_p5PlayerGame = new C_CoolButton;
	m_p6PlayerGame = new C_CoolButton;
	m_pTongBeat2 = new C_CoolButton;
	m_pCheckSuit = new C_CoolButton;

	m_Tooltip.Create(this);

	DWORD dwEnable;
	if ( m_pSlave->GetGameMode() == SERVER_GAMEMODE )
		dwEnable = 0;
	else
		dwEnable = WS_DISABLED;

	m_pContinueButton->Create("Start Game!",NULL,
			            BS_PUSHBUTTON|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcButton,this,IDOK);
	m_pContinueButton->SetFont(pSlaveWnd->GetSlaveFont(),FALSE);

	m_p3PlayerGame->Create("3 Players",NULL,
			            BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD|WS_GROUP|dwEnable,
				        rcPlayer,this,IDC_3PGAME);
	m_p3PlayerGame->SetFont(pSlaveWnd->GetSlaveFont(),FALSE);
	
	OffsetRect(&rcPlayer,0,20);
	m_p4PlayerGame->Create("4 Players",NULL,
			            BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcPlayer,this,IDC_4PGAME);
	m_p4PlayerGame->SetFont(pSlaveWnd->GetSlaveFont(),FALSE);
	m_p4PlayerGame->SetCheck(BST_CHECKED);		// first default  is 4 players

	OffsetRect(&rcPlayer,0,20);
	m_p5PlayerGame->Create("5 Players",NULL,
			            BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcPlayer,this,IDC_5PGAME);
	m_p5PlayerGame->SetFont(pSlaveWnd->GetSlaveFont(),FALSE);

	OffsetRect(&rcPlayer,0,20);
	m_p6PlayerGame->Create("6 Players",NULL,
			            BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcPlayer,this,IDC_6PGAME);
	m_p6PlayerGame->SetFont(pSlaveWnd->GetSlaveFont(),FALSE);

	m_pGroup->Create("No. of Player",NULL,
			            BS_GROUPBOX|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcGroup,this,-1);
	m_pGroup->SetFont(pSlaveWnd->GetSlaveFont(),FALSE);

	m_pTongBeat2->Create("3 cards can beat 2",NULL,
			            BS_AUTOCHECKBOX|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcTongBeat2,this,IDC_TONGBEAT2);
	m_pTongBeat2->SetFont(pSlaveWnd->GetSlaveFont(),FALSE);
	m_pTongBeat2->SetCheck(BST_CHECKED);
	m_Tooltip.AddTool(m_pTongBeat2->GetHandle(),"ไพ่ตองสามารถตบ 2 เดี่ยวได้");

	m_pCheckSuit->Create("Check suit (single discard)",NULL,
			            BS_AUTOCHECKBOX|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcCheckSuit,this,IDC_CHECKSUIT);
	m_pCheckSuit->SetFont(pSlaveWnd->GetSlaveFont(),FALSE);
	m_pCheckSuit->SetCheck(BST_UNCHECKED);
	m_Tooltip.AddTool(m_pCheckSuit->GetHandle(),"ในการลงไพ่เดี่ยว ต้องลงไพ่เลขเดียวกันหรือ ดอกเดียวกันเท่านั้น");

	int i;
   	for ( i=0 ; i<8 ; i++ )
		m_pPlayerWndLotStatus[i] = NULL;
	for ( i=0 ; i<6 ; i++ )
		m_nSelectedSlotID[i] = NO_SELECTED_PLAYER;
	for ( i=0 ; i<8 ; i++ )
		m_nLotStatus[i] = EMPTY_LOT;
	
	m_nNoOfPlayer = 4;		// 4 by default , same as default in ::Slave

	m_hrgnPos[0] = CreateRectRgn(18,60,50,132);
	m_hrgnPos[1] = CreateRectRgn(58,60,90,132);
	m_hrgnPos[2] = CreateRectRgn(98,60,130,132);
	m_hrgnPos[3] = CreateRectRgn(138,60,170,132);
	m_hrgnPos[4] = CreateRectRgn(178,60,210,132);
	m_hrgnPos[5] = CreateRectRgn(218,60,250,132);
    
	if ( pSetting == NULL )
	{
		// make default position
		for ( i=0 ; i < m_pSlave->GetPlayerList()->Count() ; i++ )
    		AddPlayer(m_pSlave->GetPlayerList()->Items(i));
	}
	else
		SetGame(pSetting);

    BringWindowToTop(m_hWnd);
    ShowWindow(m_hWnd,SW_SHOW);
}

C_StartGameWnd::~C_StartGameWnd( void )
{
	DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
    DeleteDC(m_hdcBkMem);

	if ( IsWindow(m_hWnd) ) DestroyWindow(m_hWnd);

    delete m_pContinueButton;
	delete m_pGroup;
	delete m_p3PlayerGame;
	delete m_p4PlayerGame;
	delete m_p5PlayerGame;
	delete m_p6PlayerGame;
	delete m_pTongBeat2;
	delete m_pCheckSuit;

	int i;
	for ( i=0 ; i<6 ; i++ )
		DeleteObject(m_hrgnPos[i]);
    for ( i=0 ; i<8 ; i++ )
        delete m_pPlayerWndLotStatus[i];
}

int C_StartGameWnd::AddPlayer(C_Player * pPlayer)
{
	POINT nPlayerPos[8] =
    { {44,145},{94,145},{144,145},{194,145},{44,185},{94,185},{144,185},{194,185} };

    if ( pPlayer == NULL ) return FALSE;

    // finding for empty slot
    int nEmptyLot;
    for ( nEmptyLot=0 ; nEmptyLot<8 ; nEmptyLot++ )
        if ( m_pPlayerWndLotStatus[nEmptyLot] == NULL )
            break;
    // if no slot left
    if ( nEmptyLot == 8 )
        return FALSE;

    // create UI object at empty slot
    C_PlayerWnd * pPlayerWnd;
   	pPlayerWnd = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,
									nPlayerPos[nEmptyLot].x,nPlayerPos[nEmptyLot].y,
									pPlayer );
	// only server can change position of player
	if ( m_pSlave->GetGameMode() == SERVER_GAMEMODE )
	{
		pPlayerWnd->EnableDragWindow(TRUE);
		pPlayerWnd->EnableMoveBkgWindow(TRUE);
		pPlayerWnd->EnableClickToTop(TRUE);
		pPlayerWnd->EnableClickNotify(TRUE);
		pPlayerWnd->Refresh();
	}
	m_pPlayerWndLotStatus[nEmptyLot] = pPlayerWnd;
	m_nLotStatus[nEmptyLot] = SETAS_SPECTATOR;
    return TRUE;
}

int C_StartGameWnd::RemovePlayer(C_Player * pPlayer)
{
    if ( pPlayer == NULL ) return FALSE;

    for ( int i = 0 ; i < 8 ; i++ )
    {
		if ( m_pPlayerWndLotStatus[i] != NULL )
    		if ( m_pPlayerWndLotStatus[i]->GetPlayer() == pPlayer )
    		{
	            delete m_pPlayerWndLotStatus[i];
		        m_pPlayerWndLotStatus[i] = NULL;
				// if to be set as player
				if ( m_nLotStatus[i] > SETAS_SPECTATOR )
				{
					// remove from selected player slot
					m_nSelectedSlotID[m_nLotStatus[i]-1] = NO_SELECTED_PLAYER;
				}
				m_nLotStatus[i] = EMPTY_LOT;
			    return TRUE;
			}
    }
    return FALSE;
}

// nSlotID  =>  0-7
// nOrder   =>  0-3
int C_StartGameWnd::SetPlayer(int nSlotID,int nOrder)
{
	POINT nPos[6] = { {18,60},{58,60},{98,60},{138,60},{178,60},{218,60} };

	if ( m_pPlayerWndLotStatus[nSlotID] != NULL &&
				m_nSelectedSlotID[nOrder] == NO_SELECTED_PLAYER )
	{
		// if player present and this order is empty
		// set pPlayer to be player # nOrder
		MoveWindow(m_pPlayerWndLotStatus[nSlotID]->GetHandle(),
					nPos[nOrder].x,nPos[nOrder].y,
					PLAYERWND_WIDTH,PLAYERWND_HEIGHT,TRUE);
		m_pPlayerWndLotStatus[nSlotID]->Refresh();
		m_nLotStatus[nSlotID] = nOrder+1;
		m_nSelectedSlotID[nOrder] = nSlotID;
		return TRUE;
	}
	return FALSE;
}

int C_StartGameWnd::ClearPlayer(int nOrder)
{
	POINT nPlayerPos[8] =
    { {44,145},{94,145},{144,145},{194,145},{44,185},{94,185},{144,185},{194,185} };

	int nSelSlot = m_nSelectedSlotID[nOrder];

	if ( nSelSlot == NO_SELECTED_PLAYER ) return FALSE;

	// check for selected player
	if ( m_nLotStatus[nSelSlot] > SETAS_SPECTATOR )
	{
		MoveWindow(m_pPlayerWndLotStatus[nSelSlot]->GetHandle(),
					nPlayerPos[nSelSlot].x,nPlayerPos[nSelSlot].y,
					PLAYERWND_WIDTH,PLAYERWND_HEIGHT,TRUE);
		m_pPlayerWndLotStatus[nSelSlot]->Refresh();
		m_nLotStatus[nSelSlot] = SETAS_SPECTATOR;
		m_nSelectedSlotID[nOrder] = NO_SELECTED_PLAYER;
		return TRUE;
	}
	return FALSE;
}

int C_StartGameWnd::MovePlayer(int nSlotID,int nOrder,int nOldOrder)
{
	int nSelSlot = m_nSelectedSlotID[nOldOrder];
	if ( nSelSlot == NO_SELECTED_PLAYER ) return FALSE;

	// must be player
	if ( m_nLotStatus[nSelSlot] > SETAS_SPECTATOR )
	{
		m_nLotStatus[nSelSlot] = SETAS_SPECTATOR;
		m_nSelectedSlotID[nOldOrder] = NO_SELECTED_PLAYER;
		// set at new order
		return SetPlayer(nSlotID,nOrder);
	}
	return FALSE;
}

int C_StartGameWnd::SetGame(LPSTART_GAME_SETTING pSetting)
{
	// set game by input setting
	int i,j;
	LockWindowUpdate(m_hWnd);

	// add player , to make it match with playerlist in jubmoo
	for ( i=0 ; i<m_pSlave->GetPlayerList()->Count() ; i++ )
	{
		for ( j=0 ; j<8 ; j++ )
		{
			if ( m_pPlayerWndLotStatus[j] != NULL )
				if ( m_pPlayerWndLotStatus[j]->GetPlayer()->GetPlayerID() ==
						m_pSlave->GetPlayerList()->Items(i)->GetPlayerID() )
				{
					// player already in start window
					break;
				}
		}
		if ( j==8 ) // player not found in start window
		{
			AddPlayer(m_pSlave->GetPlayerList()->Items(i));
		}
	}

	// remove set player
	for ( i=0 ; i<6 ; i++ )
		ClearPlayer(i);

	// reset player
	for ( i=0 ; i<8 ; i++ )
		if ( m_pPlayerWndLotStatus[i] != NULL )
		{
			for ( int j=0 ; j<6 ; j++ )
				if ( m_pPlayerWndLotStatus[i]->GetPlayer()->GetPlayerID() ==
						pSetting->nPlayerID[j] )
				{
					SetPlayer(i,j);
					continue;
				}
		}

	m_nNoOfPlayer = pSetting->nNoOfPlayer;

	m_p3PlayerGame->SetCheck(FALSE);
	m_p4PlayerGame->SetCheck(FALSE);
	m_p5PlayerGame->SetCheck(FALSE);
	m_p6PlayerGame->SetCheck(FALSE);
	switch ( pSetting->nNoOfPlayer )
	{
	case 3: m_p3PlayerGame->SetCheck(TRUE); break;
	case 4: m_p4PlayerGame->SetCheck(TRUE); break;
	case 5: m_p5PlayerGame->SetCheck(TRUE); break;
	case 6: m_p6PlayerGame->SetCheck(TRUE); break;
	}

	if ( pSetting->bTongBeat2 )
		m_pTongBeat2->SetCheck(BST_CHECKED);
	else
		m_pTongBeat2->SetCheck(BST_UNCHECKED);

	if ( pSetting->bCheckSuit )
		m_pCheckSuit->SetCheck(BST_CHECKED);
	else
		m_pCheckSuit->SetCheck(BST_UNCHECKED);

	RECT rc = { 138,100,250,132 };
	InvalidateRect(m_hWnd,&rc,FALSE);

	LockWindowUpdate(NULL);
	return 0;
}

int C_StartGameWnd::SendUpdateSetting()
{
	// get current setting and change to setting structure
	START_GAME_SETTING GameSetting;

	// empty slot must be more than 0
	// to distinct from normal player
	// that why player ID must start from MYID ( default is 1 )
	GameSetting.nNoOfPlayer = m_nNoOfPlayer;
	switch ( m_pTongBeat2->GetCheck() )
	{
	case BST_CHECKED:
		GameSetting.bTongBeat2 = TRUE;
		break;
	default:
		GameSetting.bTongBeat2 = FALSE;
		break;
	}
	switch ( m_pCheckSuit->GetCheck() )
	{
	case BST_CHECKED:
		GameSetting.bCheckSuit = TRUE;
		break;
	default:
		GameSetting.bCheckSuit = FALSE;
		break;
	}

	for ( int i=0 ; i<6 ; i++ )
	{
		if ( m_nSelectedSlotID[i] == NO_SELECTED_PLAYER )
			GameSetting.nPlayerID[i] = 0;
		else
		{
			if ( i >= GameSetting.nNoOfPlayer )
				GameSetting.nPlayerID[i] = 0;
			else
				GameSetting.nPlayerID[i] = m_pPlayerWndLotStatus[m_nSelectedSlotID[i]]->GetPlayer()->GetPlayerID();
		}
	}

	GetSlave()->UpdateSetting(&GameSetting);
	return 0;
}

void C_StartGameWnd::Refresh()
{
	m_bInitBkg = FALSE;
    RECT rMyRect;
    GetClientRect(m_hWnd,&rMyRect);
    ClientToScreen(m_hWnd,(LPPOINT)&rMyRect.left);
    ClientToScreen(m_hWnd,(LPPOINT)&rMyRect.right);
    ScreenToClient(m_pParentWnd->GetHandle(),(LPPOINT)&rMyRect.left);
    ScreenToClient(m_pParentWnd->GetHandle(),(LPPOINT)&rMyRect.right);
    InvalidateRect(m_pParentWnd->GetHandle(),&rMyRect,FALSE);

	DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));

    m_hDC = GetDC(m_hWnd);
    m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT);
    m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);
    BitBlt(m_hdcBkMem,0,0,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT,m_hDC,0,0,SRCCOPY);

    RECT rc;
    GetClientRect(m_hWnd,&rc);

	DrawEdge( m_hdcBkMem,&rc,EDGE_ETCHED,BF_RECT);
	DrawIcon(m_hdcBkMem,18,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS1)));
	DrawIcon(m_hdcBkMem,58,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS2)));
	DrawIcon(m_hdcBkMem,98,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS3)));
	DrawIcon(m_hdcBkMem,138,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS4)));
	DrawIcon(m_hdcBkMem,178,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS5)));
	DrawIcon(m_hdcBkMem,218,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS6)));

    ReleaseDC(m_hWnd,m_hDC);
}


// =========================================================
// Event handler
// =========================================================
LRESULT C_StartGameWnd::OnPaint(WPARAM ,LPARAM )
{
	POINT nPlayerPos[8] =
    { {44,145},{94,145},{144,145},{194,145},{44,185},{94,185},{144,185},{194,185} };
	POINT nPos[6] = { {18,60},{58,60},{98,60},{138,60},{178,60},{218,60} };

    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

	BitBlt(m_hDC,0,0,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT,m_hdcBkMem,0,0,SRCCOPY);

	int i;
	for ( i=0 ; i<8 ; i++ )
		if ( m_nLotStatus[i] == SETAS_SPECTATOR && i != m_nDragSlot )
		{
			int nResID = m_pPlayerWndLotStatus[i]->GetCharacterResID(m_pPlayerWndLotStatus[i]->GetPlayer()->GetCharacterID());
			DrawIcon(m_hDC,nPlayerPos[i].x,nPlayerPos[i].y,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID)));
		}
	for ( i=0 ; i<6 ; i++ )
		if ( m_nSelectedSlotID[i] > NO_SELECTED_PLAYER && m_nSelectedSlotID[i] != m_nDragSlot )
		{
			int nResID = m_pPlayerWndLotStatus[m_nSelectedSlotID[i]]->GetCharacterResID(m_pPlayerWndLotStatus[m_nSelectedSlotID[i]]->GetPlayer()->GetCharacterID());
			DrawIcon(m_hDC,nPos[i].x,nPos[i].y,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID)));
		}

	if ( !m_p6PlayerGame->GetCheck() )
	{
		DrawIcon(m_hDC,nPos[5].x,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_CROSS)));
		if ( !m_p5PlayerGame->GetCheck() )
		{
			DrawIcon(m_hDC,nPos[4].x,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_CROSS)));
			if ( !m_p4PlayerGame->GetCheck() )
				DrawIcon(m_hDC,nPos[3].x,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_CROSS)));
		}
	}

	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT C_StartGameWnd::OnEraseBkgnd(WPARAM wParam,LPARAM )
{
    if ( !m_bInitBkg )
    {
        m_hDC = (HDC)wParam;
        m_hdcBkMem = CreateCompatibleDC(m_hDC);
        m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT);
        m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);
        BitBlt(m_hdcBkMem,0,0,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT,m_hDC,0,0,SRCCOPY);
		RECT rc;
		GetClientRect(m_hWnd,&rc);

		DrawEdge( m_hdcBkMem,&rc,EDGE_ETCHED,BF_RECT);
		DrawIcon(m_hdcBkMem,18,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS1)));
		DrawIcon(m_hdcBkMem,58,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS2)));
		DrawIcon(m_hdcBkMem,98,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS3)));
		DrawIcon(m_hdcBkMem,138,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS4)));
		DrawIcon(m_hdcBkMem,178,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS5)));
		DrawIcon(m_hdcBkMem,218,100,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS6)));
        m_bInitBkg = TRUE;
    }
	return 1;
}

LRESULT C_StartGameWnd::OnCommand(WPARAM wParam,LPARAM )
{
	RECT rcUpdNum = {18,100,250,132};

//		TRACE("option setting changed\n");
		// send changing to all other client
	switch ( LOWORD(wParam) )
    {
	case IDC_6PGAME:
		m_nNoOfPlayer = 6;
		InvalidateRect(m_hWnd,&rcUpdNum,FALSE);
		SendUpdateSetting();
		break;
	case IDC_5PGAME:
		ClearPlayer(5);
		m_nNoOfPlayer = 5;
		InvalidateRect(m_hWnd,&rcUpdNum,FALSE);
		SendUpdateSetting();
		break;
	case IDC_4PGAME:
		ClearPlayer(5);
		ClearPlayer(4);
		m_nNoOfPlayer = 4;
		InvalidateRect(m_hWnd,&rcUpdNum,FALSE);
		SendUpdateSetting();
		break;
	case IDC_3PGAME:
		ClearPlayer(5);
		ClearPlayer(4);
		ClearPlayer(3);
		m_nNoOfPlayer = 3;
		InvalidateRect(m_hWnd,&rcUpdNum,FALSE);
		SendUpdateSetting();
		break;
	case IDC_TONGBEAT2: case IDC_CHECKSUIT:
		SendUpdateSetting();
		break;
    case IDOK:
    	// validate player
		TRACE("TRACE Lotstatu %d,%d,%d,%d,%d,%d,%d,%d\n",
			m_nLotStatus[0],m_nLotStatus[1],m_nLotStatus[2],m_nLotStatus[3],
			m_nLotStatus[4],m_nLotStatus[5],m_nLotStatus[6],m_nLotStatus[7]);
		TRACE("TRACE selectslot %d,%d,%d,%d,%d,%d\n",
			m_nSelectedSlotID[0],m_nSelectedSlotID[1],m_nSelectedSlotID[2],
			m_nSelectedSlotID[3],m_nSelectedSlotID[4],m_nSelectedSlotID[5]);
		// notify to parent to start game
/*
//		// this version allow only human player to play........

		for ( int i=0 ; i<m_pSlave->GetGameSetting()->nNoOfPlayer ; i++ )
		{
			if ( m_pSlave->GetGameSetting()->nPlayerID[i] == NO_PLAYER )
			{
				MessageBox(m_hWnd,"This beta version can't create computer player.\n try next version soon",
							"Error",MB_OK|MB_ICONSTOP);
				return 0;
			}
		}
*/
// =====================================

		m_pSlave->StartGame();

    	break;
    }
    return 0;
}

LRESULT C_StartGameWnd::OnRButtonDown(WPARAM wParam,LPARAM lParam)
{
	return ((C_SlaveWnd*)m_pParentWnd)->OnRButtonDown(wParam,lParam);
}

LRESULT C_StartGameWnd::OnPlayerWndBeginDrag(WPARAM wParam,LPARAM lParam)
{
    for ( int i = 0 ; i < 8 ; i++ )
    {
		if ( m_pPlayerWndLotStatus[i] != NULL )
    		if ( m_pPlayerWndLotStatus[i] == (C_PlayerWnd*)lParam )
				m_nDragSlot = i;
    }
	m_pntLastPos.x = (int)LOWORD(wParam);
	m_pntLastPos.y = (int)HIWORD(wParam);
	return 0;
}

LRESULT C_StartGameWnd::OnPlayerWndEndDrag(WPARAM wParam,LPARAM lParam)
{
	// check end point
	int nX = (int)LOWORD(wParam)+16;
	int nY = (int)HIWORD(wParam)+16;

	int nOrder = -1;

	if ( PtInRegion(m_hrgnPos[0],nX,nY) )
		nOrder = 0;
	else if ( PtInRegion(m_hrgnPos[1],nX,nY) )
		nOrder = 1;
	else if ( PtInRegion(m_hrgnPos[2],nX,nY) )
		nOrder = 2;
	else if ( PtInRegion(m_hrgnPos[3],nX,nY) )
		nOrder = 3;
	else if ( PtInRegion(m_hrgnPos[4],nX,nY) )
		nOrder = 4;
	else if ( PtInRegion(m_hrgnPos[5],nX,nY) )
		nOrder = 5;

	if ( nOrder < 0 || m_nSelectedSlotID[nOrder] > NO_SELECTED_PLAYER ||
			nOrder >= m_nNoOfPlayer )
	{
//		TRACE("error moving %d\n",nOrder);
		MoveWindow( ((C_Wnd*)lParam)->GetHandle(),
				m_pntLastPos.x,m_pntLastPos.y,
				PLAYERWND_WIDTH,PLAYERWND_HEIGHT,
				TRUE);
		((C_GenericClickWnd*)lParam)->Refresh();
	}
	else
	{
		// then set new player
		if ( m_nLotStatus[m_nDragSlot] > SETAS_SPECTATOR )
		{
//			TRACE("Clear from %d\n",m_nLotStatus[m_nDragSlot]-1);
			MovePlayer(m_nDragSlot,nOrder,m_nLotStatus[m_nDragSlot]-1);
		}
		else
		{
			SetPlayer(m_nDragSlot,nOrder);
		}

		// send changing to all other client
		SendUpdateSetting();
	}
	m_nDragSlot = NO_SELECTED_PLAYER;
 	return 0;
}

LRESULT C_StartGameWnd::OnPlayerWndRButtonClick(WPARAM ,LPARAM lParam)
{
	// only server do that,
    if ( m_pSlave->GetGameMode() != SERVER_GAMEMODE )
    	return 0;

	// if player , clear him
	int nSlot;
	for ( nSlot = 0 ; nSlot < 8 ; nSlot++ )
	{
		if ( m_pPlayerWndLotStatus[nSlot] == (C_PlayerWnd*)lParam )
			break;
	}
	if ( m_nLotStatus[nSlot] > SETAS_SPECTATOR )
	{
		ClearPlayer(m_nLotStatus[nSlot]-1);

		// send changing to all other client
		SendUpdateSetting();
	}

	return 0;
}

