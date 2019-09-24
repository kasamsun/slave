#ifndef _C_HELPDLG_H_
#define _C_HELPDLG_H_

#include <Dialog.h>

class C_HelpDlg : public C_Dialog
{
public:
	C_HelpDlg(C_Wnd * pParentWnd);
	~C_HelpDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);

private:
};

#endif