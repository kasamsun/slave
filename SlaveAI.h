#ifndef _C_SLAVEAI_H_
#define _C_SLAVEAI_H_

#include "SlaveApp.h"

#define AI_ERROR		-1

class C_Player;

typedef struct _CARD_VALUE
{
	int nDiscardType;
	DISCARD_CARD Card;
	double dValue;
} CARD_VALUE,*LPCARD_VALUE;

typedef MyList<CARD_VALUE> CARD_VALUE_LIST;

class C_SlaveAI
{
public:
	C_SlaveAI(C_Player * pPlayer);
	~C_SlaveAI();

	int Discard(int& nDiscardType,LPDISCARD_CARD pCardNum);
	int ExchangeCard(LPDISCARD_CARD pCardNum);

	C_Player * GetPlayer() { return m_pPlayer; }

	// ============== cheat =====================
	void ShowStrength();

protected:
    double CalcValue(int nDiscardType,LPDISCARD_CARD pRefCard);
	void RefreshCardValue();

	void RefreshCardGroup();
	void RefreshHiCard();
	int DiscardGroup(int nIndex,int& nDiscardType,LPDISCARD_CARD pCardNum);
	int CanSplitCard(int nDiscardType);

	C_Player *m_pPlayer;

	// ============= buffer =================
	int m_CardCount[13];
	int m_CardTable[13][4];
	CARD_VALUE_LIST m_CardValueList;
	
// ======================= old AI ============================	
	int m_nHiNum;			// power card  ( 2/3 of left cards )
	int m_nHiNum2;			// couple of power card
	int m_nSupNum;			// super power card ( 4/5 of left cards )
	int m_nSupNum2;			// couple of power card

	// =====================================
	int nHiCount;
	int nHiCount2;
	int nHiCount34;
	int nLowCount;
	int nLowCount2;
	int nSupCount;
	int nSupCount2;
};
#endif
