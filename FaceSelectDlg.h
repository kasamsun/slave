#ifndef _C_FACESELECTDLG_H_
#define _C_FACESELECTDLG_H_

#include "kswafx/Dialog.h"

class C_FaceSelectDlg : public C_Dialog
{
public:
	C_FaceSelectDlg(C_Wnd * pParentWnd,HWND hFace);
	~C_FaceSelectDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);

private:
	void SetIconSelection();
	HWND m_hFace;
	static int m_nPage;
};

#endif