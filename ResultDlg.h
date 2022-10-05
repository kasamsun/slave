#ifndef _C_RESULTDLG_H_
#define _C_RESULTDLG_H_

#include "kswafx/Dialog.h"

class C_PlayerWnd;
class C_CoolStatic;

class C_ResultDlg : public C_Dialog
{
public:
	C_ResultDlg(C_Wnd * pParentWnd);
	~C_ResultDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);

private:
	C_PlayerWnd * m_pPlayerWnd[6];
	C_CoolStatic * m_pPosition[6];
};

#endif