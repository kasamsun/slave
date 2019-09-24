#include "HelpDlg.h"

#include "resource.h"
#include "SlaveCard.h"
#include "SlaveWnd.h"
#include "Slave.h"
#include "SlaveApp.h"

#include <stdio.h>

extern C_SlaveApp SlaveApp;

BEGIN_HANDLE_MSG(C_HelpDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_HelpDlg::C_HelpDlg(C_Wnd * pParentWnd)
{
	Create(IDD_HELP,pParentWnd);
}

C_HelpDlg::~C_HelpDlg()
{
}

LRESULT C_HelpDlg::OnInitDialog(WPARAM,LPARAM)
{
	return TRUE;
}

LRESULT C_HelpDlg::OnCommand(WPARAM wParam,LPARAM)
{
	switch ( LOWORD(wParam))
	{
	case IDOK:
		EndDialog(m_hWnd,0);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

