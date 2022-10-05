#ifndef _SLAVEAPP_H_
#define _SLAVEAPP_H_

#include "kswafx/MainAfx.h"
#include <time.h>
#include "kswafx/MyTempl.h"

// =======================================================
//	SLAVE_BUILDDATE 
#define SLAVE_BUILDDATE	"12/8/2001"
#define SLAVE_VERSION		"1.02"
//
// =======================================================

/////////// global define ///////////////////////////////
#define JUBMOO_REGISTRY "SOFTWARE\\Slave"

typedef int	CARDNUM;
typedef MyList<CARDNUM> CARD_LIST;
typedef struct _DISCARD_CARD
{
	CARDNUM nCard[4];
} DISCARD_CARD, *LPDISCARD_CARD;

// discard type
enum { NONE_CARD,ONE_CARD,TWO_CARD,THREE_CARD,FOUR_CARD };

#define RANDOM(_k_)		(int)( rand()%(_k_) )
#define RANDOMIZE()     srand((unsigned) time(NULL))

#define TRANSPARENT_COLOR RGB(255,0,255)

#include "kswafx/RegistryKey.h"

class C_SlaveApp : public C_App
{
public:
	static char m_lpszAppName[16];

	C_SlaveApp();
    ~C_SlaveApp();

    int Initialize();
    void CheckSlaveRegistry();

    void OpenRegistry();
    void CloseRegistry();

    UINT GetAIDelay();
    void SetAIDelay(UINT nAIDelay);

    void GetCustomBkg(LPSTR lpszPath,int nBufLen);
    void SetCustomBkg(LPCSTR lpszPath);
    void GetCustomCard(LPSTR lpszPath,int nBufLen);
    void SetCustomCard(LPCSTR lpszPath);
    void GetCustomDlg(LPSTR lpszPath,int nBufLen);
    void SetCustomDlg(LPCSTR lpszPath);
    void GetCustomPD(LPSTR lpszPath,int nBufLen);
    void SetCustomPD(LPCSTR lpszPath);

    void GetDefChatMsg(LPSTR lpszPath,int nBufLen);
    void SetDefChatMsg(LPCSTR lpszPath);

    void GetJoinIP(LPSTR lpszPath,int nBufLen);
    void SetJoinIP(LPCSTR lpszPath);
    UINT GetPort();
    void SetPort(UINT nPort);

    void GetOpponent(int nOrder,LPSTR lpszPath,int nBufLen);
    void SetOpponent(int nOrder,LPCSTR lpszPath);
    void GetOpponentStat(int nOrder,UINT &nGamePlay,UINT &nKing,UINT &nSlave);
    void SetOpponentStat(int nOrder,UINT nGamePlay,UINT nKing,UINT nSlave);

    UINT GetCharID(int nOrder);
    void SetCharID(int nOrder,UINT nCharID);
    void GetName(int nOrder,LPSTR lpszPath,int nBufLen);
    void SetName(int nOrder,LPCSTR lpszPath);
    void GetStat(int nOrder,UINT &nGamePlay,UINT &nKing,UINT &nSlave);
    void SetStat(int nOrder,UINT nGamePlay,UINT nKing,UINT nSlave);

    int GetUseCoolCtl();
    void SetUseCoolCtl(int bUseCoolCtl);
    int GetUseCustomImg();
    void SetUseCustomImg(int bUseCustomImg);
    int GetUseFading();
    void SetUseFading(int bUseFading);
    int GetUseNetLog();
    void SetUseNetLog(int bUseNetLog);

	int GetPlayerSlot() { return m_nPlayerSlot; }
	int UpdatePlayerSlot(int nSlot);

	int IsUseSound() { return m_bUseSound; }
	void UseSound(int bUseSound) { m_bUseSound = bUseSound; }
	int IsAlarmTurn() { return m_bAlarmTurn; }
	void AlarmTurn(int bAlarmTurn) { m_bAlarmTurn = bAlarmTurn; }

protected:
    UINT GetLastPlayer();
    void SetLastPlayer(UINT nLastPlayer);

	int GetUseSound();
	void SetUseSound(int bUseSound);
	int GetAlarmTurn();
	void SetAlarmTurn(int bAlarmTurn);

private:
	C_RegistryKey m_RegKey;
	int m_nPlayerSlot;
	int m_bUseSound;
	int m_bAlarmTurn;
};


#endif