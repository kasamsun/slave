#include "PlayerWnd.h"

#include <stdio.h>

#include "MsgPopupWnd.h"
#include "resource.h"
#include "Player.h"
#include "SlaveWnd.h"
#include "Slave.h"
#include "SlaveComm.h"

extern C_SlaveApp SlaveApp;

#define IDM_VIEW_INFO	9500
#define IDM_KICK_OUT	9501
#define IDM_PING_PLAYER 9502
#define IDM_PING_DEALER 9503

BEGIN_HANDLE_MSG(C_PlayerWnd,C_GenericClickWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_LBUTTONUP,OnLButtonUp)
	HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)
	HANDLE_MSG(WM_RBUTTONDOWN,OnRButtonDown)
	HANDLE_MSG(WM_COMMAND,OnCommand)

END_HANDLE_MSG()

C_PlayerWnd::C_PlayerWnd(C_Wnd *pParentWnd,int nShowType,int PosX,int PosY,C_Player *pPlayer)
		: C_GenericClickWnd(pParentWnd)
{
	m_bLPressed = FALSE;
    m_nDisplayMode = nShowType;

	m_hWnd = CreateWindow(m_szClassName,
                        "",
                        WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS,
                        PosX,PosY,
                        PLAYERWND_WIDTH*nShowType,PLAYERWND_HEIGHT*nShowType,
                        pParentWnd->GetHandle(),
                        NULL,
                        C_App::m_hInstance,
                        (LPSTR) this );
    if ( ! m_hWnd )
        exit( FALSE );

	m_pParentWnd = pParentWnd;
    m_pPlayer = pPlayer;

	SetActiveCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_POINT)) );
	SetClickCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_CLICK)) );
    SetDragCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_GRASP)) );

	m_Tooltip.Create(this);
	m_Tooltip.AddTool(m_hWnd,m_pPlayer->GetName());

    EnableKBMsgParentNotify(TRUE);
//    EnableDragWindow(TRUE);
//    EnableClickToTop(TRUE);
    m_pMsgWnd = new C_MsgPopupWnd(this,5,10000,8000);
}

C_PlayerWnd::~C_PlayerWnd()
{
	if ( IsWindow(m_hWnd) ) DestroyWindow(m_hWnd);
    if ( IsWindow(m_pMsgWnd->GetHandle()) ) DestroyWindow(m_pMsgWnd->GetHandle());
	delete m_pMsgWnd;
}

void C_PlayerWnd::ShowSize(int nMode)
{
	switch ( nMode )
    {
    case SINGLE_SIZE_PLAYERWND:
    case DOUBLE_SIZE_PLAYERWND:
		m_nDisplayMode = nMode;
    	SetWindowPos(m_hWnd,NULL,0,0,
        				PLAYERWND_WIDTH*nMode,PLAYERWND_HEIGHT*nMode,
                        SWP_NOREPOSITION|SWP_NOMOVE);
        return;
    default:
        return;
    }
}

UINT C_PlayerWnd::GetCharacterResID(int id)
{
	switch ( id )
    {
    case 0: return IDI_HEAD1;
    case 1: return IDI_HEAD2;
    case 2: return IDI_HEAD3;
    case 3: return IDI_HEAD4;
    case 4: return IDI_HEAD5;
    case 5: return IDI_HEAD6;
    case 6: return IDI_HEAD7;
    case 7: return IDI_HEAD8;
    case 8: return IDI_HEAD9;
    case 9: return IDI_HEAD10;
    case 10: return IDI_HEAD11;
    case 11: return IDI_HEAD12;
    case 12: return IDI_HEAD13;
    case 13: return IDI_HEAD14;
    case 14: return IDI_HEAD15;
    case 15: return IDI_HEAD16;
    case 16: return IDI_HEAD17;
    case 17: return IDI_HEAD18;
    case 18: return IDI_HEAD19;
    case 19: return IDI_HEAD20;
	case 128: return IDI_HEADCOM1;
	case 129: return IDI_HEADCOM2;
	case 130: return IDI_HEADCOM3;
	case 131: return IDI_HEADCOM4;
	case 132: return IDI_HEADCOM5;
	case 133: return IDI_HEADCOM6;
    default:
		if ( id >= 134 )
		{
			return IDI_ICON1 + id -26;
		}
		else if ( id >= 20 )
		{
			return IDI_ICON1 + id -20;
		}
		return IDI_NOBODY;
    }
}

int C_PlayerWnd::PlayerUpdated()
{
	m_Tooltip.UpdateTipText(m_hWnd,GetPlayer()->GetName());
	return 0;
}

int C_PlayerWnd::Chat(char *pszMsg)
{
	m_pMsgWnd->PopupMessage(pszMsg);
	return 0;
}


void C_PlayerWnd::Paint(HDC hDC,int x,int y)
{
    UINT nResID = GetCharacterResID(m_pPlayer->GetCharacterID());

	if ( m_nDisplayMode == SINGLE_SIZE_PLAYERWND )
	    DrawIcon(hDC,x,y,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID)));
    else
		DrawIconEx(hDC,x,y,
        			LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID)),
					PLAYERWND_WIDTH*2,PLAYERWND_HEIGHT*2,0,NULL,DI_NORMAL);
}

///////////////////// Event Handler /////////////////////////
LRESULT C_PlayerWnd::OnPaint(WPARAM ,LPARAM )
{
    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

    UINT nResID = GetCharacterResID(m_pPlayer->GetCharacterID());

	if ( m_nDisplayMode == SINGLE_SIZE_PLAYERWND )
	    DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID)));
    else
		DrawIconEx(m_hDC,0,0,
        			LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID)),
					PLAYERWND_WIDTH*2,PLAYERWND_HEIGHT*2,0,NULL,DI_NORMAL);

	EndPaint(m_hWnd,&ps);
	return 0;
}

LRESULT C_PlayerWnd::OnLButtonUp(WPARAM wParam,LPARAM lParam)
{
	if ( m_bLPressed )
		m_pMsgWnd->PopupMessage();
	return C_GenericClickWnd::OnLButtonUp(wParam,lParam);
}

LRESULT C_PlayerWnd::OnEraseBkgnd(WPARAM ,LPARAM )
{
	return 1;
}

LRESULT C_PlayerWnd::OnRButtonDown(WPARAM wParam,LPARAM lParam)
{
	LRESULT lRetVal = C_GenericClickWnd::OnRButtonDown(wParam,lParam);

	if ( m_pPlayer->GetUI() == this )
	{
		// create menu
		HMENU hMenu = CreatePopupMenu();

		AppendMenu(hMenu,MF_STRING,IDM_VIEW_INFO,"Player Info");
		char szKickOut[] = "Kick Out";
		char szPingPlayer[] = "Ping Me";
		char szPingDealer[] = "Ping Dealer";

		C_Slave * pSlv = m_pPlayer->GetSlave();
		if ( pSlv->GetGameMode() == SERVER_GAMEMODE && 
				m_pPlayer->GetStatus() != IAM_COMPUTER )
		{
        	if ( pSlv->GetMyPlayer() != m_pPlayer )
            {
				AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
				AppendMenu(hMenu,MF_STRING,IDM_KICK_OUT,szKickOut);
				AppendMenu(hMenu,MF_STRING,IDM_PING_PLAYER,szPingPlayer);
            }
		}
		else if ( pSlv->GetGameMode() == CLIENT_GAMEMODE &&
				pSlv->GetMyPlayer() == m_pPlayer )
		{
			AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
			AppendMenu(hMenu,MF_STRING,IDM_PING_DEALER,szPingDealer);
		}
		
		POINT p;
		GetCursorPos(&p);
		TrackPopupMenuEx(hMenu,TPM_VERTICAL,p.x,p.y,m_hWnd,NULL);
		// clear cursor to original
		PostMessage(m_hWnd,WM_KILLFOCUS,0,0);
		DestroyMenu(hMenu);
	}	
	return lRetVal;
}

LRESULT C_PlayerWnd::OnCommand(WPARAM wParam,LPARAM )
{
	char tmpbuf[64];
	C_SlaveComm * pComm = NULL;

	switch(wParam)
	{
	case IDM_VIEW_INFO:
		sprintf(tmpbuf,"/sys Play%d King%d Slave%d",
				m_pPlayer->GetNoOfGamePlay(),
				m_pPlayer->GetNoOfKing(),
				m_pPlayer->GetNoOfSlave()
				);
		m_pMsgWnd->PopupMessage(tmpbuf);
		break;
	case IDM_KICK_OUT:
		{
		sprintf(tmpbuf,"Kick out %s really?",m_pPlayer->GetName());
		if ( MessageBox(m_hWnd,tmpbuf,C_SlaveApp::m_lpszAppName,
    				MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) == IDCANCEL )
			break;
		// kick out process
		pComm = m_pPlayer->GetSlave()->GetComm();
		int nForceRemove=-1;
        for ( int i=0 ; i < pComm->GetClientList()->Count() ; i++ )
        {
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)pComm->GetClientList()->Items(i);
			if ( pSlaveComm->GetPlayer() == m_pPlayer )
			{
				sprintf(tmpbuf,"/c12kick out %s",m_pPlayer->GetName());
				pComm->PopupMessage(tmpbuf);
				// just store index , force remove later 
				nForceRemove = i;
			}
			else
			{
				sprintf(tmpbuf,"/sys %s kick out %s",
						m_pPlayer->GetSlave()->GetMyPlayer()->GetName(),
						m_pPlayer->GetName());
				pSlaveComm->SendChat(m_pPlayer->GetSlave()->GetMyPlayer()->GetPlayerID(),tmpbuf);
			}
        }
		if ( nForceRemove >= 0 )
		{
			// 0.92 fix here , moved from above loop
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)pComm->GetClientList()->Items(nForceRemove);
			pSlaveComm->SendLogonFail(KICK_OUT);
			pSlaveComm->GetSlave()->RemovePlayer(m_pPlayer->GetPlayerID());
			pSlaveComm->Close();
			pComm->GetClientList()->Remove(nForceRemove);
			delete pSlaveComm;
		}
		}
		break;
	case IDM_PING_PLAYER:
		{
		// kick out process
		pComm = m_pPlayer->GetSlave()->GetComm();
        for ( int i=0 ; i < pComm->GetClientList()->Count() ; i++ )
        {
            C_SlaveComm * pSlaveComm = (C_SlaveComm *)pComm->GetClientList()->Items(i);
			if ( pSlaveComm->GetPlayer() == m_pPlayer )
			{
				sprintf(tmpbuf,"/c12Ping %s",m_pPlayer->GetName());
				pComm->PopupMessage(tmpbuf);
				m_pPlayer->GetSlave()->CheckResponse(m_pPlayer->GetPlayerID(),pSlaveComm);
			}
			else
			{
				sprintf(tmpbuf,"/sys Ping %s",m_pPlayer->GetName());
				pSlaveComm->SendChat(m_pPlayer->GetSlave()->GetMyPlayer()->GetPlayerID(),tmpbuf);
			}
        }
		}
		break;
	case IDM_PING_DEALER:
		pComm = m_pPlayer->GetSlave()->GetComm();
		sprintf(tmpbuf,"/c12Ping Dealer");
		pComm->PopupMessage(tmpbuf);
		m_pPlayer->GetSlave()->CheckResponse(m_pPlayer->GetPlayerID(),pComm);
		break;
	}
	return 0;
}
