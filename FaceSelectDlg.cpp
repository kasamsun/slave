#include "FaceSelectDlg.h"

#include "resource.h"

BEGIN_HANDLE_MSG(C_FaceSelectDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()

int C_FaceSelectDlg::m_nPage = 0;

C_FaceSelectDlg::C_FaceSelectDlg(C_Wnd * pParentWnd,HWND hFace)
{
	m_hFace = hFace;
	Create(IDD_FACESELECT,pParentWnd);
}

C_FaceSelectDlg::~C_FaceSelectDlg()
{
}

LRESULT C_FaceSelectDlg::OnInitDialog(WPARAM,LPARAM)
{
	SetIconSelection();

	RECT rc;
	GetWindowRect(m_hFace,&rc);
	RECT myrc;
	GetWindowRect(m_hWnd,&myrc);
	MoveWindow(m_hWnd,rc.right,rc.top,
					myrc.right-myrc.left,myrc.bottom-myrc.top,TRUE);
	return TRUE;
}

void C_FaceSelectDlg::SetIconSelection()
{
	int nCtlID[20] = { IDC_HEAD1,IDC_HEAD2,IDC_HEAD3,IDC_HEAD4,
						IDC_HEAD5,IDC_HEAD6,IDC_HEAD7,IDC_HEAD8,
						IDC_HEAD9,IDC_HEAD10,IDC_HEAD11,IDC_HEAD12,
						IDC_HEAD13,IDC_HEAD14,IDC_HEAD15,IDC_HEAD16,
						IDC_HEAD17,IDC_HEAD18,IDC_HEAD19,IDC_HEAD20,
						};
	int nResID[20] = { IDI_HEAD1,IDI_HEAD2,IDI_HEAD3,IDI_HEAD4,
						IDI_HEAD5,IDI_HEAD6,IDI_HEAD7,IDI_HEAD8,
						IDI_HEAD9,IDI_HEAD10,IDI_HEAD11,IDI_HEAD12,
						IDI_HEAD13,IDI_HEAD14,IDI_HEAD15,IDI_HEAD16,
						IDI_HEAD17,IDI_HEAD18,IDI_HEAD19,IDI_HEAD20 };
	int resid;
	for ( int i=0 ;i<20 ;i++ )
	{
		if ( m_nPage == 0 )
		{
			resid = nResID[i];
			SendMessage(GetDlgItem(m_hWnd,nCtlID[i]),
				BM_SETIMAGE,IMAGE_ICON,
				(LPARAM)LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(resid)));
		}
		else
		{
			resid = IDI_ICON1+i+20*(m_nPage-1);
			if ( resid <= IDI_ICON229 )
				SendMessage(GetDlgItem(m_hWnd,nCtlID[i]),
					BM_SETIMAGE,IMAGE_ICON,
					(LPARAM)LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(resid)));
			else
				SendMessage(GetDlgItem(m_hWnd,nCtlID[i]),
					BM_SETIMAGE,IMAGE_ICON,NULL);
		}
	}
}

LRESULT C_FaceSelectDlg::OnCommand(WPARAM wParam,LPARAM)
{
	int nRetVal = 0;

	nRetVal = 20*m_nPage;

	switch ( LOWORD(wParam))
	{
	case IDOK:
		//MessageBox(NULL,"idok","",MB_OK);
		m_nPage++;
		if ( m_nPage == 13 ) m_nPage = 0;
		SetIconSelection();
		InvalidateRect(m_hWnd,NULL,false);
		break;
	case IDC_HEAD1: EndDialog(m_hWnd,(nRetVal + 0 >=128)?nRetVal+6:nRetVal); break;
	case IDC_HEAD2: EndDialog(m_hWnd,(nRetVal + 1 >=128)?nRetVal+7:nRetVal+1); break;
	case IDC_HEAD3: EndDialog(m_hWnd,(nRetVal + 2 >=128)?nRetVal+8:nRetVal+2); break;
	case IDC_HEAD4: EndDialog(m_hWnd,(nRetVal + 3 >=128)?nRetVal+9:nRetVal+3); break;
	case IDC_HEAD5: EndDialog(m_hWnd,(nRetVal + 4 >=128)?nRetVal+10:nRetVal+4); break;
	case IDC_HEAD6: EndDialog(m_hWnd,(nRetVal + 5 >=128)?nRetVal+11:nRetVal+5); break;
	case IDC_HEAD7: EndDialog(m_hWnd,(nRetVal + 6 >=128)?nRetVal+12:nRetVal+6); break;
	case IDC_HEAD8: EndDialog(m_hWnd,(nRetVal + 7 >=128)?nRetVal+13:nRetVal+7); break;
	case IDC_HEAD9: EndDialog(m_hWnd,(nRetVal + 8 >=128)?nRetVal+14:nRetVal+8); break;
	case IDC_HEAD10: EndDialog(m_hWnd,(nRetVal + 9 >=128)?nRetVal+15:nRetVal+9); break;
	case IDC_HEAD11: EndDialog(m_hWnd,(nRetVal + 10 >=128)?nRetVal+16:nRetVal+10); break;
	case IDC_HEAD12: EndDialog(m_hWnd,(nRetVal + 11 >=128)?nRetVal+17:nRetVal+11); break;
	case IDC_HEAD13: EndDialog(m_hWnd,(nRetVal + 12 >=128)?nRetVal+18:nRetVal+12); break;
	case IDC_HEAD14: EndDialog(m_hWnd,(nRetVal + 13 >=128)?nRetVal+19:nRetVal+13); break;
	case IDC_HEAD15: EndDialog(m_hWnd,(nRetVal + 14 >=128)?nRetVal+20:nRetVal+14); break;
	case IDC_HEAD16: EndDialog(m_hWnd,(nRetVal + 15 >=128)?nRetVal+21:nRetVal+15); break;
	case IDC_HEAD17: EndDialog(m_hWnd,(nRetVal + 16 >=128)?nRetVal+22:nRetVal+16); break;
	case IDC_HEAD18: EndDialog(m_hWnd,(nRetVal + 17 >=128)?nRetVal+23:nRetVal+17); break;
	case IDC_HEAD19: EndDialog(m_hWnd,(nRetVal + 18 >=128)?nRetVal+24:nRetVal+18); break;
	case IDC_HEAD20: EndDialog(m_hWnd,(nRetVal + 19 >=128)?nRetVal+25:nRetVal+19); break;
	case IDCANCEL:
		EndDialog(m_hWnd,-1);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

