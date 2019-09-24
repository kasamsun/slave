#include "SlaveAI.h"

#include "Slave.h"
#include "Player.h"
#include "SlaveCard.h"
#include <stdio.h>

C_SlaveAI::C_SlaveAI(C_Player * pPlayer)
{
	m_pPlayer = pPlayer;
	m_nHiNum = CARD_J;
	m_nHiNum2 = CARD_J;
	m_nSupNum = CARD_A;
	m_nSupNum2 = CARD_A;
}

C_SlaveAI::~C_SlaveAI()
{
	m_CardValueList.DeleteObjects();
}

void C_SlaveAI::RefreshCardValue()
{
	m_CardValueList.DeleteObjects();

	int i;
	int nLastCard = -99;
	// create card list
	CARD_LIST *pCard = m_pPlayer->GetCardList();
	for ( i=0 ; i<pCard->Count() ; i++ )
	{
		if ( GET_NUM(*(pCard->Items(i))) !=  GET_NUM(nLastCard) )
		{
			nLastCard = *(pCard->Items(i));
			LPCARD_VALUE pCardValue = new CARD_VALUE;
			pCardValue->nDiscardType = 1;
			pCardValue->Card.nCard[0] = *(pCard->Items(i));
			int j;
			for ( j=1; j<4 ; j++ )
				pCardValue->Card.nCard[j] = 0;
			pCardValue->dValue = 0.5;
			m_CardValueList.Add(pCardValue);
		}
		else
		{
			LPCARD_VALUE pCardValue = m_CardValueList.Items(m_CardValueList.Count()-1);
			pCardValue->Card.nCard[pCardValue->nDiscardType] = *(pCard->Items(i));
			pCardValue->nDiscardType++;
		}
	}

	// update card value
    for ( i=0 ; i<m_CardValueList.Count() ; i++ )
    {
		LPCARD_VALUE pCardValue = m_CardValueList.Items(i);
        pCardValue->dValue = CalcValue(pCardValue->nDiscardType,&pCardValue->Card);
    }

}

double C_SlaveAI::CalcValue(int nDiscardType,LPDISCARD_CARD pRefCard)
{
    if ( pRefCard->nCard[nDiscardType-1] == 0 )
    {
        TRACE("error in Calc Value [0]\n");
        return -1;
    }

	C_Slave * pSlave = m_pPlayer->GetSlave();

	int nNoOfGrpCard = 0;
    int nNoOfLess = 0;
	int i;

	for ( i=0 ; i<pSlave->GetGameSetting()->nNoOfPlayer ; i++ )
	{
		C_Player * pPlayer = pSlave->GetPlayerList()->Items(i);
		if ( pPlayer == m_pPlayer ) continue;
        if ( pPlayer->GetDiscardStatus() == PASS_TURN ||
                pPlayer->GetDiscardStatus() == CARD_OVER )
            continue;

		CARD_LIST * pCard = pPlayer->GetCardList();
		int nCardCount[13];
        int nCardTable[13][4];
		int j,k;
		for ( j=0 ; j<13 ; j++ )
			nCardCount[j] = 0;
    	for ( j=0 ; j<13 ; j++ )
	    	for ( k=0 ; k<4 ; k++ )
		    	nCardTable[j][k] = 0;
		for ( j=0 ; j<pCard->Count() ; j++ )
		{
			int nNum = GET_NUM( *pCard->Items(j) );
    		nCardTable[nNum][nCardCount[nNum]] = *pCard->Items(j);
			nCardCount[nNum]++;
		}

		for ( j=0 ; j<13 ; j++ )
		{
            if ( nCardCount[j] == nDiscardType )
            {
                if ( nCardTable[j][nDiscardType-1] < pRefCard->nCard[nDiscardType-1] )
                    nNoOfLess++;
				nNoOfGrpCard++;
            }
		}
	}

    if ( nNoOfGrpCard == 0 ) return 1L;
    return ((double)nNoOfLess/nNoOfGrpCard);
}

void C_SlaveAI::ShowStrength()
{
	RefreshCardValue();

	int i;
	char szMsg[256];
	strcpy(szMsg,"");
	for ( i=0; i<m_CardValueList.Count() ; i++ )
	{
		LPCARD_VALUE pVal = m_CardValueList.Items(i);

		int j;
		char z[256];
		strcpy(z,"");
		for ( j=0 ; j<pVal->nDiscardType ;j++ )
		{
			char x[9],y[9];
			int nNum = GET_NUM(pVal->Card.nCard[j]);
			switch ( nNum )
			{
				case CARD_J:sprintf(x,"J");break;
				case CARD_Q:sprintf(x,"Q");break;
				case CARD_K:sprintf(x,"K");break;
				case CARD_A:sprintf(x,"A");break;
				case CARD_2:sprintf(x,"2");break;
				default:sprintf(x,"%d",nNum+3);break;
			}
			int nFace = GET_FACE(pVal->Card.nCard[j]);
			switch ( nFace )
			{
				case CLUB:sprintf(y,".c");break;
				case DIAMOND:sprintf(y,".d");break;
				case HEART:sprintf(y,".h");break;
				case SPADE:sprintf(y,".s");break;
			}
			strcat(z,x);strcat(z,y);strcat(z,"  ");
		}
        strcat(szMsg,z);
		sprintf(z," ---> %4.4f\n",pVal->dValue);
		strcat(szMsg,z);
	}
	//MessageBox(GetActiveWindow(),szMsg,"",MB_OK);
}

void C_SlaveAI::RefreshCardGroup()
{
	int i,j;
	for ( i=0 ; i<13 ; i++ )
		m_CardCount[i] = 0;
	for ( i=0 ; i<13 ; i++ )
		for ( j=0 ; j<4 ; j++ )
			m_CardTable[i][j] = 0;

	CARD_LIST * pCard = m_pPlayer->GetCardList();
	// count group of card
	for ( i=0 ; i<pCard->Count() ; i++ )
	{
		int nNum = GET_NUM( *pCard->Items(i) );
		m_CardTable[nNum][m_CardCount[nNum]] = *pCard->Items(i);
		m_CardCount[nNum]++;
	}
} 

void C_SlaveAI::RefreshHiCard()
{
	C_Slave * pSlave = m_pPlayer->GetSlave();
	CARD_LIST * pCard = m_pPlayer->GetCardList();

	int nCardTable[53];
	int nCardTable2[53];
	int nUndiscarded = 0;
	int nUndiscarded2 = 0;
	int i;

	// count group of card
	for ( i=0 ; i<53 ; i++ )
	{
		nCardTable[i] = 0;
		nCardTable2[i] = 0;
	}

	for ( i=0 ; i<pSlave->GetGameSetting()->nNoOfPlayer ; i++ )
	{
		C_Player * pPlayer = pSlave->GetPlayerList()->Items(i);
		if ( pPlayer == m_pPlayer ) continue;
		CARD_LIST * pCard = pPlayer->GetCardList();
		int nCardCount[13];
		int j;
		for ( j=0 ; j<13 ; j++ )
			nCardCount[j] = 0;
		for ( j=0 ; j<pCard->Count() ; j++ )
		{
			int nNum = GET_NUM( *pCard->Items(j) );
			nCardCount[nNum]++;
		}

		for ( j=0 ; j<pCard->Count() ; j++ )
		{
			switch ( nCardCount[GET_NUM(*(pCard->Items(j)))] )
			{
			case 1:
				nCardTable[*(pCard->Items(j))] = 1;
				nUndiscarded++;
				break;
			case 2:
				nCardTable2[*(pCard->Items(j))] = 1;
				nUndiscarded2++;
				break;
			}
		}
	}

	// hicard of 1,2 card that still undiscarded
	//TRACE("Undiscarded = %d , Undiscarded2 = %d\n",nUndiscarded,nUndiscarded2);
	int nHiCardPos = nUndiscarded*2/3;
	int nHiCardPos2 = nUndiscarded2*2/3;
	int nSupCardPos = nUndiscarded*4/5;
	int nSupCardPos2 = nUndiscarded2*4/5;

	m_nHiNum = CARD_J;
	m_nHiNum2 = CARD_J;
	m_nSupNum = CARD_A;
	m_nSupNum2 = CARD_A;

	int ntmp=0;
	for ( i=1 ; i<=52 ; i++ )
	{
		if ( nCardTable[i] == 1 )
		{
			ntmp++;
			if ( ntmp > nHiCardPos )
			{
				m_nHiNum = GET_NUM(i);
				break;
			}
		}
	}
	ntmp=0;
	for ( i=1 ; i<=52 ; i++ )
	{
		if ( nCardTable2[i] == 1 )
		{
			ntmp++;
			if ( ntmp > nHiCardPos2 )
			{
				m_nHiNum2 = GET_NUM(i);
				break;
			}
		}
	}
	ntmp=0;
	for ( i=1 ; i<=52 ; i++ )
	{
		if ( nCardTable[i] == 1 )
		{
			ntmp++;
			if ( ntmp > nSupCardPos )
			{
				m_nSupNum = GET_NUM(i);
				break;
			}
		}
	}
	ntmp=0;
	for ( i=1 ; i<=52 ; i++ )
	{
		if ( nCardTable2[i] == 1 )
		{
			ntmp++;
			if ( ntmp > nSupCardPos2 )
			{
				m_nSupNum2 = GET_NUM(i);
				break;
			}
		}
	}
}

int C_SlaveAI::DiscardGroup(int nIndex,int& nDiscardType,LPDISCARD_CARD pCardNum)
{
	int j;
	for ( j=0 ; j<m_CardCount[nIndex] ; j++ )
		pCardNum->nCard[j] = m_CardTable[nIndex][j];
	nDiscardType = m_CardCount[nIndex];
	return TRUE;
}

int C_SlaveAI::CanSplitCard(int nDiscardType)
{
		//	if ( nLowCount >= nHiCount+nHiCount34 &&
		//			nLowCount2 >= nHiCount2+nHiCount34 )
		//	if ( nLowCount+nLowCount2 >= nHiCount+nHiCount2+nHiCount34 )

	if ( nDiscardType == 1 )
	{
		int i;
		int nSplitCount = 0;
		for ( i=m_nSupNum ; i<=CARD_2 ; i++ )
		{
			switch ( m_CardCount[i] )
			{
			case 2: nSplitCount++; break;
			case 3: nSplitCount++; break;
			case 4: nSplitCount+=2; break;
			}
		}
		if ( nSplitCount > 0 )
			;//MessageBox(GetActiveWindow(),"can split","",MB_OK);
		if ( nLowCount+nLowCount2 >= nHiCount+nHiCount2+nHiCount34+nSplitCount )
			return FALSE;
		for ( i=m_nSupNum ; i<=CARD_2 ; i++ )
		{
			switch ( m_CardCount[i] )
			{
			case 2: case 3: case 4: m_CardCount[i] = 1; break;
			}
		}		
		return TRUE;
	}
	else if ( nDiscardType == 2 )
	{
		int i;
		int nSplitCount = 0;
		for ( i=m_nSupNum ; i<=CARD_2 ; i++ )
		{
			switch ( m_CardCount[i] )
			{
			case 3: nSplitCount++; break;
			case 4: nSplitCount++; break;
			}
		}
		if ( nLowCount+nLowCount2 >= nHiCount+nHiCount2+nHiCount34+nSplitCount )
			return FALSE;
		for ( i=m_nSupNum ; i<=CARD_2 ; i++ )
		{
			switch ( m_CardCount[i] )
			{
			case 3: case 4: m_CardCount[i] = 2; break;
			}
		}		
		return TRUE;
	}

	return FALSE;
}

int C_SlaveAI::Discard(int& nDiscardType,LPDISCARD_CARD pCardNum)
{
	C_Slave * pSlave = m_pPlayer->GetSlave();
	CARD_LIST * pCard = m_pPlayer->GetCardList();
	DISCARD_CARD * pLatestCard = &(pSlave->GetIngameTable()->LatestCard);

	int i;
	for ( i = 0 ; i<4 ; i++ )
		pCardNum->nCard[i] = 0;

	RefreshCardGroup();
	RefreshHiCard();

#ifdef _DEBUG
	ShowStrength();

    char mycardbuf[100];
    char tmpbuf[256];
    strcpy(mycardbuf,"");

	for ( i=0 ; i<pCard->Count() ; i++ )
	{
		int nNum = GET_NUM( *pCard->Items(i) );
        char x[9],y[9];
        switch ( nNum )
        {
            case CARD_J:sprintf(x,"J");break;
            case CARD_Q:sprintf(x,"Q");break;
            case CARD_K:sprintf(x,"K");break;
            case CARD_A:sprintf(x,"A");break;
            case CARD_2:sprintf(x,"2");break;
            default:sprintf(x,"%d",nNum+3);break;
        }
        int nFace = GET_FACE( *pCard->Items(i) );
        switch ( nFace )
        {
            case CLUB:sprintf(y,".c");break;
            case DIAMOND:sprintf(y,".d");break;
            case HEART:sprintf(y,".h");break;
            case SPADE:sprintf(y,".s");break;
        }
        strcat(mycardbuf,x);strcat(mycardbuf,y);strcat(mycardbuf,"  ");
	}
	{
		// show hi card , hi num
		char x[16];
		strcat(mycardbuf," hi=");
		switch ( m_nHiNum )
		{
			case CARD_J:sprintf(x,"J");break;
			case CARD_Q:sprintf(x,"Q");break;
			case CARD_K:sprintf(x,"K");break;
			case CARD_A:sprintf(x,"A");break;
			case CARD_2:sprintf(x,"2");break;
			default:sprintf(x,"%d",m_nHiNum+3);break;
		}
		strcat(mycardbuf,x);

		strcat(mycardbuf," hi2=");
		switch ( m_nHiNum2 )
		{
			case CARD_J:sprintf(x,"J");break;
			case CARD_Q:sprintf(x,"Q");break;
			case CARD_K:sprintf(x,"K");break;
			case CARD_A:sprintf(x,"A");break;
			case CARD_2:sprintf(x,"2");break;
			default:sprintf(x,"%d",m_nHiNum2+3);break;
		}
		strcat(mycardbuf,x);

		strcat(mycardbuf," sup=");
		switch ( m_nSupNum )
		{
			case CARD_J:sprintf(x,"J");break;
			case CARD_Q:sprintf(x,"Q");break;
			case CARD_K:sprintf(x,"K");break;
			case CARD_A:sprintf(x,"A");break;
			case CARD_2:sprintf(x,"2");break;
			default:sprintf(x,"%d",m_nSupNum+3);break;
		}
		strcat(mycardbuf,x);

		strcat(mycardbuf," sup2=");
		switch ( m_nSupNum2 )
		{
			case CARD_J:sprintf(x,"J");break;
			case CARD_Q:sprintf(x,"Q");break;
			case CARD_K:sprintf(x,"K");break;
			case CARD_A:sprintf(x,"A");break;
			case CARD_2:sprintf(x,"2");break;
			default:sprintf(x,"%d",m_nSupNum2+3);break;
		}
		strcat(mycardbuf,x);
	}
    //MessageBox(GetActiveWindow(),mycardbuf,"",MB_OK);
#endif


// ============================ AI ===============================
// ============================ AI ===============================
// ============================ AI ===============================
// ============================ AI ===============================

	if ( pSlave->GetIngameTable()->nDiscardType == NONE_CARD )
	{
		// lead ================================================

		// look for 'block lead'
		int nCheck = 0;
		int nCheckOver = 0;
		int nTmp = 0;
		for ( i=0 ; i<pSlave->GetGameSetting()->nNoOfPlayer ; i++ )
		{
			C_Player * pPlayer = pSlave->GetPlayer(pSlave->GetGameSetting()->nPlayerID[i]);
			if ( pPlayer != NULL )//&& pPlayer != pSlave->GetMyPlayer() )
			{
				if ( pPlayer->GetDiscardStatus() != CARD_OVER )
				{
					if ( pPlayer->GetCardList()->Count() <= 2 )
						nCheck++;
				}
				else
				{
					nCheckOver++;
				}
			}
		}
		if ( m_pPlayer->GetCardList()->Count() <= 2 ) nCheck--;
		if ( m_pPlayer->GetDiscardStatus() == CARD_OVER ) nCheckOver--;

		if ( (m_pPlayer->GetPosition() == IAM_KING && nCheck > 0) ||
				( nCheckOver+2 == pSlave->GetGameSetting()->nNoOfPlayer && nCheck == 1 ))
		{
			// enter 'block lead'
//            MessageBox(GetActiveWindow(),"Enter Block Lead","",MB_OK);
			for ( i=CARD_2 ; i>=CARD_3 ; i-- )
			{
				if ( m_CardCount[i] == 4 || m_CardCount[i] == 3 )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			for ( i=CARD_2 ; i>=CARD_3 ; i-- )
			{
				if ( m_CardCount[i] == 2 )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			for ( i=CARD_2 ; i>=CARD_3 ; i-- )
			{
				if ( m_CardCount[i] == 1 )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			// case else , discard left most
			nDiscardType = ONE_CARD;
			pCardNum->nCard[0] = *(pCard->Items(0));
			return TRUE;
		}

		// normal lead
//        MessageBox(GetActiveWindow(),"normal lead","",MB_OK);
		for ( i=CARD_3 ; i<=CARD_2 ; i++ )
		{
			// sometimes  should be selected 1# or 2# discarding
			if ( m_CardCount[i] == 1 || m_CardCount[i] == 2 )
				return DiscardGroup(i,nDiscardType,pCardNum);
		}
		for ( i=CARD_3 ; i<=CARD_2 ; i++ )
		{
			if ( m_CardCount[i] == 3 )
				return DiscardGroup(i,nDiscardType,pCardNum);
		}
		for ( i=CARD_3 ; i<=CARD_2 ; i++ )
		{
			if ( m_CardCount[i] == 4 )
				return DiscardGroup(i,nDiscardType,pCardNum);
		}
		// case else , discard left most
//		MessageBox(GetActiveWindow(),"case else , discard left most","",MB_OK);
		nDiscardType = ONE_CARD;
		pCardNum->nCard[0] = *(pCard->Items(0));
        return TRUE;
	}
	else
	{
		// not lead ============================================

		// if single/pair of low/mid card can discard , do it
		switch (pSlave->GetIngameTable()->nDiscardType)
		{
		case ONE_CARD:
			for ( i=CARD_3 ; i<m_nHiNum ; i++ )
			{
				if ( m_CardCount[i] == 1 &&
						m_CardTable[i][0] > pSlave->GetIngameTable()->LatestCard.nCard[0] )
				{
					if ( pSlave->GetGameSetting()->bCheckSuit )
					{
						if ( GET_NUM(m_CardTable[i][0]) == GET_NUM(pSlave->GetIngameTable()->LatestCard.nCard[0]) ||
								GET_FACE(m_CardTable[i][0]) == GET_FACE(pSlave->GetIngameTable()->LatestCard.nCard[0]) )
							return DiscardGroup(i,nDiscardType,pCardNum);
					}
					else
					{
						return DiscardGroup(i,nDiscardType,pCardNum);
					}
				}
			}
			break;
		case TWO_CARD:
			for ( i=CARD_3 ; i<m_nHiNum2 ; i++ )
			{
				if ( m_CardCount[i] == 2 &&
						m_CardTable[i][1] > pSlave->GetIngameTable()->LatestCard.nCard[1] )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			break;
		}

		nHiCount = 0;
		nHiCount2 = 0;
		nHiCount34 = 0;
		nLowCount = 0;
		nLowCount2 = 0;
		nSupCount = 0;
		nSupCount2 = 0;
		// count card group
		for ( i=CARD_3 ; i<=CARD_2 ; i++ )
		{
			switch (m_CardCount[i])
			{
			case 3:case 4: nHiCount34++; break;
			case 1: if ( i<m_nHiNum ) nLowCount++; else nHiCount++;
					if ( i>=m_nSupNum ) nSupCount++; break;
			case 2: if ( i<m_nHiNum2 ) nLowCount2++; else nHiCount2++;
					if ( i>=m_nSupNum2 ) nSupCount2++; break;
			}
		}

//        sprintf(tmpbuf,"%s\nlow/hi=%d/%d   , 2low/2hi=%d,%d  , 34hi=%d\nno small card can be play",
//					mycardbuf,nLowCount,nHiCount,nLowCount2,nHiCount2,nHiCount34);
//        MessageBox(GetActiveWindow(),tmpbuf,"",MB_OK);

		if ( nLowCount+nLowCount2 == 0 || 
				nLowCount+nLowCount2+nHiCount+nHiCount2+nHiCount34 == 1)
		{
			// if last card in hand , always fight
			;
			// or no low card in hand , always fight
//	        sprintf(tmpbuf,"%s\nalways fight , no way out",mycardbuf);
//		    MessageBox(GetActiveWindow(),tmpbuf,"",MB_OK);

		}
		else
		{
			// no low/mid card can be discarded
			// is it worth to use hi card in hand , 
			
			if ( nLowCount >= nHiCount+nHiCount34 &&
					nLowCount2 >= nHiCount2+nHiCount34 )
			{
				if ( pSlave->GetIngameTable()->nDiscardType == 1 &&
						nLowCount <= nHiCount+nHiCount34+nSupCount )
				{
					// fight with my super power , make a chance to lead
//					MessageBox(GetActiveWindow(),"i have super card ,fight it 1","",MB_OK);
				}
				else if ( pSlave->GetIngameTable()->nDiscardType == 2 &&
						nLowCount2 <= nHiCount2+nHiCount34+nSupCount )
				{
					// fight with my super power , make a chance to lead
//					MessageBox(GetActiveWindow(),"i have super card ,fight it 2","",MB_OK);
				}
//				else if ( CanSplitCard(pSlave->GetIngameTable()->nDiscardType) )
//				{
//					// can split super card?
//					MessageBox(GetActiveWindow(),"can split super card to fight","",MB_OK);
//				}
				else
				{
					;
//			        sprintf(tmpbuf,"%s\nGIVE UP1\nlowcard %d >= %d hicard\nlowcard2 %d >= %d hicard2",mycardbuf,nLowCount,nHiCount,nLowCount2,nHiCount2);
//				    MessageBox(GetActiveWindow(),tmpbuf,"",MB_OK);
					return FALSE;
				}
			}
			if ( nLowCount+nLowCount2 >= nHiCount+nHiCount2+nHiCount34 )
			{
				if ( nLowCount+nLowCount2 <= nHiCount+nHiCount2+nHiCount34+nSupCount )
				{
					// fight with my super power , make a chance to lead
//					MessageBox(GetActiveWindow(),"i have super card ,fight it 3","",MB_OK);
				}
				else if ( nLowCount+nLowCount2 <= nHiCount+nHiCount2+nHiCount34+nSupCount2 )
				{
					// fight with my super power , make a chance to lead
//					MessageBox(GetActiveWindow(),"i have super card ,fight it 4","",MB_OK);
				}
				else if ( (nLowCount+nLowCount+nHiCount+nHiCount2+nHiCount34 == 2) &&
							(nSupCount > 0 || nSupCount2 > 0 || nHiCount34 > 0) )
				{
					// fight with my super power , end it with my low card
//					MessageBox(GetActiveWindow(),"fight with my highest , and last with my low","",MB_OK);
				}
//				else if ( CanSplitCard(pSlave->GetIngameTable()->nDiscardType) )
//				{
//					// can split super card?
//					MessageBox(GetActiveWindow(),"can split super card to fight","",MB_OK);
//				}
				else
				{
					;
//			        sprintf(tmpbuf,"%s\nGIVE UP2\nsumlow %d >= %d sumhigh or no any super card %d,%d,%d",
//							mycardbuf,nLowCount+nLowCount2,nHiCount+nHiCount2+nHiCount34,nSupCount,nSupCount2,nHiCount34);
//				    MessageBox(GetActiveWindow(),tmpbuf,"",MB_OK);
					return FALSE;
				}
			}
		}

		switch (pSlave->GetIngameTable()->nDiscardType)
		{
		case ONE_CARD:
			for ( i=CARD_3 ; i<=CARD_2 ; i++ )
			{
				if ( m_CardCount[i] == 1 && 
						m_CardTable[i][0] > pSlave->GetIngameTable()->LatestCard.nCard[0] )
				{
					if ( pSlave->GetGameSetting()->bCheckSuit )
					{
						if ( GET_NUM(m_CardTable[i][0]) == GET_NUM(pSlave->GetIngameTable()->LatestCard.nCard[0]) ||
								GET_FACE(m_CardTable[i][0]) == GET_FACE(pSlave->GetIngameTable()->LatestCard.nCard[0]) )
							return DiscardGroup(i,nDiscardType,pCardNum);
					}
					else
					{
						return DiscardGroup(i,nDiscardType,pCardNum);
					}
				}
			}
			if ( GET_NUM(pSlave->GetIngameTable()->LatestCard.nCard[0]) != CARD_2 && 
					pSlave->GetGameSetting()->bTongBeat2 )
			{
				// tong can beat 2 , so find it
				for ( i=CARD_3 ; i<=CARD_2 ; i++ )
				{
					if ( m_CardCount[i] == 3 )
						return DiscardGroup(i,nDiscardType,pCardNum);
				}
			}
			else if ( GET_NUM(pSlave->GetIngameTable()->LatestCard.nCard[0]) == CARD_2 &&
					pSlave->GetGameSetting()->bTongBeat2 == FALSE &&
					m_CardCount[CARD_2] == 3 )
			{
				// Tong can't beat 2 except tong 2
				return DiscardGroup(CARD_2,nDiscardType,pCardNum);
			}
			for ( i=CARD_3 ; i<=CARD_2 ; i++ )
			{
				if ( m_CardCount[i] == 4 )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			return FALSE;
		case TWO_CARD:
			for ( i=CARD_3 ; i<=CARD_2 ; i++ )
			{
				if ( m_CardCount[i] == 2 && 
						m_CardTable[i][1] > pSlave->GetIngameTable()->LatestCard.nCard[1] )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			for ( i=CARD_3 ; i<=CARD_2 ; i++ )
			{
				if ( m_CardCount[i] == 4 )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			return FALSE;
		case THREE_CARD:
			for ( i=CARD_3 ; i<=CARD_2 ; i++ )
			{
				if ( m_CardCount[i] == 3 &&
						m_CardTable[i][0] > pSlave->GetIngameTable()->LatestCard.nCard[0] )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			for ( i=CARD_3 ; i<=CARD_2 ; i++ )
			{
				if ( m_CardCount[i] == 4 )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			return FALSE;
		case FOUR_CARD:
			for ( i=CARD_3 ; i<=CARD_2 ; i++ )
			{
				if ( m_CardCount[i] == 4 &&
						m_CardTable[i][0] > pSlave->GetIngameTable()->LatestCard.nCard[0] )
					return DiscardGroup(i,nDiscardType,pCardNum);
			}
			return FALSE;
		}
	}

	return TRUE;
}

int C_SlaveAI::ExchangeCard(LPDISCARD_CARD pCardNum)
{
	int i;
	for ( i = 0 ; i<4 ; i++ )
		pCardNum->nCard[i] = 0;

	RefreshCardGroup();

	CARD_LIST * pCardList = m_pPlayer->GetCardList();
	switch ( m_pPlayer->GetPosition() )
	{
	case IAM_KING:
		// get 2 most useless cards
		{
			int nCount = 0;
			for ( i = 0 ; i < CARD_J ; i++ )
			{
				if ( m_CardCount[i] == 1 )
				{
					pCardNum->nCard[nCount] = m_CardTable[i][0];
					nCount++;
				}
				if ( nCount == 2 )
					return TRUE;
			}
			pCardNum->nCard[0] = *pCardList->Items(0);
			pCardNum->nCard[1] = *pCardList->Items(1);
		}
		break;
	case IAM_QUEEN:
		// get one most useless card
		{
			for ( i = 0 ; i < CARD_J ; i++ )
			{
				if ( m_CardCount[i] == 1 )
				{
					pCardNum->nCard[0] = m_CardTable[i][0];
					return TRUE;
				}
			}
			pCardNum->nCard[0] = *pCardList->Items(0);
		}
		break;
	}

	return TRUE;
}

