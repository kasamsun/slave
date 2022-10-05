#include "ResultDlg.h"

#include <stdio.h>
#include "kswafx/CoolStatic.h"
#include "resource.h"
#include "PlayerWnd.h"
#include "Slave.h"
#include "SlaveWnd.h"

BEGIN_HANDLE_MSG(C_ResultDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_ResultDlg::C_ResultDlg(C_Wnd * pParentWnd)
{
	int i;
	for ( i=0 ; i<6 ; i++ )
	{
		m_pPlayerWnd[i] = NULL;
		m_pPosition[i] = NULL;
	}
	Create(IDD_GAMERESULT,pParentWnd);
}

C_ResultDlg::~C_ResultDlg()
{
	int i;
	for ( i=0 ; i<6 ; i++ )
	{
		delete m_pPlayerWnd[i];
		delete m_pPosition[i];
	}
}

LRESULT C_ResultDlg::OnInitDialog(WPARAM ,LPARAM )
{
	int nNameResID[6] = {	IDC_POS1NAME,IDC_POS2NAME,IDC_POS3NAME,
							IDC_POS4NAME,IDC_POS5NAME,IDC_POS6NAME };
	int idRes[6];

	C_Slave * pSlave = ((C_SlaveWnd*)m_pParentWnd)->GetSlave();

	C_Player * pPlayer;
	int i;

	switch ( pSlave->GetGameSetting()->nNoOfPlayer )
	{
	case 3:
		idRes[0] = IDI_ISKING;
		idRes[1] = IDI_ISPEOPLE;
		idRes[2] = IDI_ISSLAVE;
		idRes[3] = 0;idRes[4] = 0;idRes[5] = 0;
		break;
	case 4:
		idRes[0] = IDI_ISKING;
		idRes[1] = IDI_ISQUEEN;
		idRes[2] = IDI_ISVSLAVE;
		idRes[3] = IDI_ISSLAVE;
		idRes[4] = 0;idRes[5] = 0;
		break;
	case 5:
		idRes[0] = IDI_ISKING;
		idRes[1] = IDI_ISQUEEN;
		idRes[2] = IDI_ISPEOPLE;
		idRes[3] = IDI_ISVSLAVE;
		idRes[4] = IDI_ISSLAVE;
		idRes[5] = 0;
		break;
	case 6:
		idRes[0] = IDI_ISKING;
		idRes[1] = IDI_ISQUEEN;
		idRes[2] = IDI_ISPEOPLE;
		idRes[3] = IDI_ISPEOPLE;
		idRes[4] = IDI_ISVSLAVE;
		idRes[5] = IDI_ISSLAVE;
		break;
	}

	for ( i=0 ; i<6 ; i++ )
	{
		RECT rc;
		// set score font
		HWND hCtl = GetDlgItem(m_hWnd,nNameResID[i]);
		HFONT hFont = ((C_SlaveWnd*)m_pParentWnd)->GetBigSlaveFont();
		SendMessage(hCtl,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE,0));
		GetClientRect(hCtl,&rc);
		ClientToScreen(hCtl,(LPPOINT)&rc.left);
		ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);

		// create player wnd
		pPlayer = pSlave->GetPlayer(pSlave->GetFinishTable()->nPlayerID[i]);
		if ( pPlayer != NULL )
		{
			SetWindowText(hCtl,pPlayer->GetName());
			int x = rc.left - 40;
			int y = rc.top - 5;
			m_pPlayerWnd[i] = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,
										x,y,pPlayer );
			m_pPosition[i] = new C_CoolStatic;
			RECT rcPos;
			rcPos.left = rc.right + 120;
			rcPos.top = rc.top;
			rcPos.right = rcPos.left + 32;
			rcPos.bottom = rcPos.top + 32;
			m_pPosition[i]->Create("",NULL,SS_ICON|WS_VISIBLE|WS_CHILD,rcPos,(C_Wnd*)this,-1);

			SendMessage(m_pPosition[i]->GetHandle(),STM_SETICON,
					(WPARAM)(HICON)LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(idRes[i])),
					0);    
		}
		else
		{
			SetWindowText(hCtl,"");
		}
	}

	return TRUE;
}

LRESULT C_ResultDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
	case IDOK:
		// lock update , prevent screen flashing when close
		LockWindowUpdate(GetParent(m_hWnd));
		EndDialog(m_hWnd,0);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

