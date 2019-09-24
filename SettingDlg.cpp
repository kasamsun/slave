#include "SettingDlg.h"

#include <stdio.h>

#include "resource.h"
#include "SlaveApp.h"
#include "SlaveWnd.h"
#include "Slave.h"
#include "SlaveComm.h"
#include "FaceSelectDlg.h"
#include "BmpPreviewDlg.h"
#include "ChatEdit.h"

extern C_SlaveApp SlaveApp;

BEGIN_HANDLE_MSG(C_SettingDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_SettingDlg::C_SettingDlg(C_SlaveWnd * pParentWnd)
{
	m_nSelectedSlot = 1;
	m_nSelectedFace = 1;
	m_nSelectedAISpeed = 1;
	Create(IDD_SETTING,pParentWnd);
}

C_SettingDlg::~C_SettingDlg()
{
}

void C_SettingDlg::UpdateCtlBkg(HWND hCtl)
{
	RECT rc;
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ClientToScreen(hCtl,(LPPOINT)&rc.right);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.right);
	InvalidateRect(GetParent(hCtl),&rc,FALSE);
}

LRESULT C_SettingDlg::OnInitDialog(WPARAM,LPARAM)
{
	// set save slot reference table
	m_nSlotID[0] = IDC_SLOT1;
	m_nSlotID[1] = IDC_SLOT2;
	m_nSlotID[2] = IDC_SLOT3;
	m_nSlotID[3] = IDC_SLOT4;
	m_nSlotID[4] = IDC_SLOT5;

	// set limit text to all entry box
	SendMessage(GetDlgItem(m_hWnd,IDC_NAME),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT1),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT2),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT3),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT4),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT5),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT6),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_BKGFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_CARDFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_DLGFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_PDFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_DEFCHATMSG),EM_SETLIMITTEXT,255,0);

	if ( ((C_SlaveWnd*)m_pParentWnd)->GetSlave()->GetGameMode() != IDLE_GAMEMODE )
	{
		// can't change player data , when game start/join
		EnableWindow(GetDlgItem(m_hWnd,IDC_NAME),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SELECTFACE),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_RESETSCORE),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT1),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT2),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT3),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT4),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT5),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_RESETCOMPSCORE),FALSE);
	}

	// load data from registry
    char tmpbuf[256];
	RECT rc;
	HWND hCtl;

    SlaveApp.OpenRegistry();

	// last active slot
	m_nSelectedSlot = (int)SlaveApp.GetPlayerSlot();
	hCtl = GetDlgItem(m_hWnd,m_nSlotID[m_nSelectedSlot-1]);
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	MoveWindow(GetDlgItem(m_hWnd,IDC_ARROWB),
				rc.left-32,rc.top-5,32,32,TRUE);
	// player name
	SlaveApp.GetName(m_nSelectedSlot,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_NAME),tmpbuf);
	// player character
	OnFaceSelect(SlaveApp.GetCharID(m_nSelectedSlot));
	// player statistic
    UINT nGamePlay,nKing,nSlave;
    SlaveApp.GetStat(m_nSelectedSlot,nGamePlay,nKing,nSlave);
	sprintf(tmpbuf,"%d",nSlave);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFSLAVE),tmpbuf);
	sprintf(tmpbuf,"%d",nKing);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFKING),tmpbuf);
	sprintf(tmpbuf,"%d",nGamePlay);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY),tmpbuf);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY2),tmpbuf);

	// opponent
    SlaveApp.GetOpponent(1,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT1),tmpbuf);
    SlaveApp.GetOpponent(2,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT2),tmpbuf);
    SlaveApp.GetOpponent(3,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT3),tmpbuf);
    SlaveApp.GetOpponent(4,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT4),tmpbuf);
    SlaveApp.GetOpponent(5,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT5),tmpbuf);
    SlaveApp.GetOpponent(6,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT6),tmpbuf);

	// environment
	if ( SlaveApp.GetUseCoolCtl() )
		SendMessage(GetDlgItem(m_hWnd,IDC_USECOOLCTL),BM_SETCHECK,TRUE,0);
	if ( SlaveApp.GetUseFading() )
		SendMessage(GetDlgItem(m_hWnd,IDC_USEFADEBKG),BM_SETCHECK,TRUE,0);
	if ( SlaveApp.GetUseCustomImg() )
		SendMessage(GetDlgItem(m_hWnd,IDC_USECUSTOMBKG),BM_SETCHECK,TRUE,0);

    SlaveApp.GetCustomBkg(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_BKGFILE),tmpbuf);
    SlaveApp.GetCustomCard(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_CARDFILE),tmpbuf);
    SlaveApp.GetCustomDlg(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_DLGFILE),tmpbuf);
    SlaveApp.GetCustomPD(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_PDFILE),tmpbuf);

	if ( SlaveApp.IsUseSound() )
		SendMessage(GetDlgItem(m_hWnd,IDC_USESOUND),BM_SETCHECK,TRUE,0);
	if ( SlaveApp.IsAlarmTurn() )
		SendMessage(GetDlgItem(m_hWnd,IDC_ALARMTURN),BM_SETCHECK,TRUE,0);

	// game speed

	switch ( SlaveApp.GetAIDelay() )
	{
	case 800: hCtl = GetDlgItem(m_hWnd,IDC_AIFAST); m_nSelectedAISpeed = 800; break;
	case 1200: hCtl = GetDlgItem(m_hWnd,IDC_AINORMAL); m_nSelectedAISpeed = 1200; break;
	case 1800: hCtl = GetDlgItem(m_hWnd,IDC_AISLOW); m_nSelectedAISpeed = 1800; break;
	default: hCtl = GetDlgItem(m_hWnd,IDC_AINORMAL); m_nSelectedAISpeed = 1200; break;
	}
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	MoveWindow(GetDlgItem(m_hWnd,IDC_ARROWB2),
					rc.left-32,rc.top-5,32,32,TRUE);

    SlaveApp.GetDefChatMsg(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_DEFCHATMSG),tmpbuf);

	SlaveApp.CloseRegistry();

	return TRUE;
}

LRESULT C_SettingDlg::OnCommand(WPARAM wParam,LPARAM)
{
	switch ( LOWORD(wParam))
	{
	case IDC_SLOT1: case IDC_SLOT2: case IDC_SLOT3:
	case IDC_SLOT4:	case IDC_SLOT5:
		OnChangeSaveSlot(LOWORD(wParam));
		break;
	case IDC_SELECTFACE: OnFaceSelect(); break;
	case IDC_RESETSCORE: OnResetScore(); break;
	case IDC_RESETCOMPSCORE: OnResetComputerScore(); break;
	case IDC_SELECTBKG: OnBmpSelect(IDC_BKGFILE); break;
	case IDC_SELECTCARD: OnBmpSelect(IDC_CARDFILE); break;
	case IDC_SELECTDLG: OnBmpSelect(IDC_DLGFILE); break;
	case IDC_SELECTPD: OnBmpSelect(IDC_PDFILE); break;
	case IDC_AIFAST: case IDC_AINORMAL: case IDC_AISLOW:
		OnChangeAISpeed(LOWORD(wParam));
		break;
	case IDOK:
		OnOK();
        EndDialog(m_hWnd,IDOK);
		break;
	case IDCANCEL:
		EndDialog(m_hWnd,IDCANCEL);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

void C_SettingDlg::OnChangeAISpeed(int nSlotID)
{
	HWND hCtl;
	RECT rc;

	switch ( nSlotID )
	{
	case IDC_AIFAST: m_nSelectedAISpeed = 800; break;
	case IDC_AINORMAL: m_nSelectedAISpeed = 1200; break;
	case IDC_AISLOW: m_nSelectedAISpeed = 1800; break;
	}
	hCtl = GetDlgItem(m_hWnd,nSlotID);
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	MoveWindow(GetDlgItem(m_hWnd,IDC_ARROWB2),
					rc.left-32,rc.top-5,32,32,TRUE);
	UpdateCtlBkg(GetDlgItem(m_hWnd,IDC_ARROWB2));
}

void C_SettingDlg::OnChangeSaveSlot(int nSlotID)
{
	RECT rc;
	HWND hCtl;
	char tmpbuf[32];

	int nSlot = 0;
	switch ( nSlotID )
	{
	case IDC_SLOT1: nSlot = 1; break;
	case IDC_SLOT2: nSlot = 2; break;
	case IDC_SLOT3: nSlot = 3; break;
	case IDC_SLOT4: nSlot = 4; break;
	case IDC_SLOT5: nSlot = 5; break;
	}

	if ( m_nSelectedSlot == nSlot ) return;

	m_nSelectedSlot = nSlot;
	LockWindowUpdate(m_hWnd);
	// arrow pointer move 
	hCtl = GetDlgItem(m_hWnd,m_nSlotID[nSlot-1]);
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	hCtl = GetDlgItem(m_hWnd,IDC_ARROWB);
	MoveWindow(hCtl,rc.left-32,rc.top-5,32,32,TRUE);
	UpdateCtlBkg(hCtl);

	//////////////////////////
	SlaveApp.OpenRegistry();
	// player data
	SlaveApp.GetName(m_nSelectedSlot,tmpbuf,sizeof(tmpbuf));
	hCtl = GetDlgItem(m_hWnd,IDC_NAME);
	SetWindowText(hCtl,tmpbuf);
	//
	OnFaceSelect(SlaveApp.GetCharID(m_nSelectedSlot));
	//
	UINT nGamePlay,nKing,nSlave;
	SlaveApp.GetStat(m_nSelectedSlot,nGamePlay,nKing,nSlave);
	sprintf(tmpbuf,"%d\0",nSlave);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFSLAVE);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);
	sprintf(tmpbuf,"%d\0",nKing);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFKING);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);
	sprintf(tmpbuf,"%d\0",nGamePlay);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY2);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);

	SlaveApp.CloseRegistry();
	///////////////////////////

	LockWindowUpdate(NULL);
}

void C_SettingDlg::OnFaceSelect(int id)
{
	if ( id < 0 )
	{
		C_FaceSelectDlg FaceSelect(this,GetDlgItem(m_hWnd,IDC_SELECTFACE));
		id = FaceSelect.DoModal();
		//char tmpbuf[99];
		//sprintf(tmpbuf,"%d",id);
		//MessageBox(NULL,tmpbuf,"",MB_OK);
		if ( id < 0 || id >= 255 ) return ;
	}

	int nResID;
	switch ( id )
    {
    case 0: nResID = IDI_HEAD1; break;
    case 1: nResID = IDI_HEAD2; break;
    case 2: nResID = IDI_HEAD3; break;
    case 3: nResID = IDI_HEAD4; break;
    case 4: nResID = IDI_HEAD5; break;
    case 5: nResID = IDI_HEAD6; break;
    case 6: nResID = IDI_HEAD7; break;
    case 7: nResID = IDI_HEAD8; break;
    case 8: nResID = IDI_HEAD9; break;
    case 9: nResID = IDI_HEAD10; break;
    case 10: nResID = IDI_HEAD11; break;
    case 11: nResID = IDI_HEAD12; break;
    case 12: nResID = IDI_HEAD13; break;
    case 13: nResID = IDI_HEAD14; break;
    case 14: nResID = IDI_HEAD15; break;
    case 15: nResID = IDI_HEAD16; break;
    case 16: nResID = IDI_HEAD17; break;
    case 17: nResID = IDI_HEAD18; break;
    case 18: nResID = IDI_HEAD19; break;
    case 19: nResID = IDI_HEAD20; break;
    case 128: nResID = IDI_HEADCOM1; break;
    case 129: nResID = IDI_HEADCOM2; break;
    case 130: nResID = IDI_HEADCOM3; break;
    case 131: nResID = IDI_HEADCOM4; break;
    case 132: nResID = IDI_HEADCOM5; break;
    case 133: nResID = IDI_HEADCOM6; break;
//    default: nResID = IDI_NOBODY; break;
    default:
		if ( id >= 134 )
		{
			nResID = IDI_ICON1 + id -26; break;
		}
		else if ( id >= 20 )
		{
			nResID = IDI_ICON1 + id -20; break;
		}
		nResID = IDI_NOBODY; break;
    }
	m_nSelectedFace = id;
	UpdateCtlBkg(GetDlgItem(m_hWnd,IDC_SELECTFACE));
	SendMessage(GetDlgItem(m_hWnd,IDC_SELECTFACE),
				BM_SETIMAGE,IMAGE_ICON,
				(LPARAM)LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID))
				);
}

void C_SettingDlg::OnBmpSelect(int ctlid)
{
	char szFilename[50];

	HWND hCtl = GetDlgItem(m_hWnd,ctlid);

	GetWindowText(hCtl,szFilename,sizeof(szFilename));

	C_BmpPreviewDlg BmpPreviewDlg(this,hCtl,szFilename);
	
	if ( BmpPreviewDlg.DoModal() == IDOK )
		SetWindowText(hCtl,szFilename);
}

void C_SettingDlg::OnResetScore()
{
	HWND hCtl;
	if ( MessageBox(m_hWnd,"Delete player's score?",
				C_SlaveApp::m_lpszAppName,
				MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
		return;
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFKING);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFSLAVE);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY2);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
}

void C_SettingDlg::OnResetComputerScore()
{
	if ( MessageBox(m_hWnd,"Delete computer's score?",
				C_SlaveApp::m_lpszAppName,
				MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
		return;
	SlaveApp.OpenRegistry();
	for ( int i=1 ; i<=6 ; i++ )
		SlaveApp.SetOpponentStat(i,0,0,0);
	SlaveApp.CloseRegistry();
}

void C_SettingDlg::OnOK()
{
	C_SlaveWnd * pSlvWnd = (C_SlaveWnd*)m_pParentWnd;

	char tmpbuf[256];

	/////////////////////////
	SlaveApp.OpenRegistry();

    // save all value to registry
	SlaveApp.UpdatePlayerSlot(m_nSelectedSlot);

    GetWindowText(GetDlgItem(m_hWnd,IDC_NAME),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetName(m_nSelectedSlot,tmpbuf);

	SlaveApp.SetCharID(m_nSelectedSlot,m_nSelectedFace);

	UINT nGamePlay,nKing,nSlave;

    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFSLAVE),tmpbuf,sizeof(tmpbuf));
	nSlave = atoi(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFKING),tmpbuf,sizeof(tmpbuf));
	nKing = atoi(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY),tmpbuf,sizeof(tmpbuf));
	nGamePlay = atoi(tmpbuf);
	SlaveApp.SetStat(m_nSelectedSlot,nGamePlay,nKing,nSlave);

    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT1),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetOpponent(1,tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT2),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetOpponent(2,tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT3),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetOpponent(3,tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT4),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetOpponent(4,tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT5),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetOpponent(5,tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT6),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetOpponent(6,tmpbuf);

    GetWindowText(GetDlgItem(m_hWnd,IDC_BKGFILE),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetCustomBkg(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_CARDFILE),tmpbuf,sizeof(tmpbuf));
    SlaveApp.SetCustomCard(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_DLGFILE),tmpbuf,sizeof(tmpbuf));
    SlaveApp.SetCustomDlg(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_PDFILE),tmpbuf,sizeof(tmpbuf));
    SlaveApp.SetCustomPD(tmpbuf);

    if ( SendMessage(GetDlgItem(m_hWnd,IDC_USECOOLCTL),BM_GETCHECK,0,0) == BST_CHECKED )
    	SlaveApp.SetUseCoolCtl(TRUE);
    else
    	SlaveApp.SetUseCoolCtl(FALSE);

    if ( SendMessage(GetDlgItem(m_hWnd,IDC_USEFADEBKG),BM_GETCHECK,0,0) == BST_CHECKED )
    	SlaveApp.SetUseFading(TRUE);
    else
    	SlaveApp.SetUseFading(FALSE);

    if ( SendMessage(GetDlgItem(m_hWnd,IDC_USECUSTOMBKG),BM_GETCHECK,0,0) == BST_CHECKED )
    	SlaveApp.SetUseCustomImg(TRUE);
    else
    	SlaveApp.SetUseCustomImg(FALSE);

    if ( SendMessage(GetDlgItem(m_hWnd,IDC_USESOUND),BM_GETCHECK,0,0) == BST_CHECKED )
    	SlaveApp.UseSound(TRUE);
    else
    	SlaveApp.UseSound(FALSE);
    if ( SendMessage(GetDlgItem(m_hWnd,IDC_ALARMTURN),BM_GETCHECK,0,0) == BST_CHECKED )
    	SlaveApp.AlarmTurn(TRUE);
    else
    	SlaveApp.AlarmTurn(FALSE);

	SlaveApp.SetAIDelay(m_nSelectedAISpeed);
	pSlvWnd->SetGameSpeed(m_nSelectedAISpeed);

	GetWindowText(GetDlgItem(m_hWnd,IDC_DEFCHATMSG),tmpbuf,sizeof(tmpbuf));
	SlaveApp.SetDefChatMsg(tmpbuf);

	SlaveApp.CloseRegistry();

    // set img follow this setting
    pSlvWnd->GetSlave()->GetMyPlayer()->SetCharacterID(m_nSelectedFace);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NAME),tmpbuf,sizeof(tmpbuf));
    pSlvWnd->GetSlave()->GetMyPlayer()->SetName(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFSLAVE),tmpbuf,sizeof(tmpbuf));
    pSlvWnd->GetSlave()->GetMyPlayer()->SetNoOfSlave(atoi(tmpbuf));
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFKING),tmpbuf,sizeof(tmpbuf));
    pSlvWnd->GetSlave()->GetMyPlayer()->SetNoOfKing(atoi(tmpbuf));
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY),tmpbuf,sizeof(tmpbuf));
    pSlvWnd->GetSlave()->GetMyPlayer()->SetNoOfGamePlay(atoi(tmpbuf));
   	pSlvWnd->SetImage();
//	if ( pSlvWnd->m_pStartGameWnd != NULL ) pSlvWnd->m_pStartGameWnd->Refresh();
	pSlvWnd->m_pChatEdit->SetDefChatMsg();
}
