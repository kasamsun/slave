#ifndef _CARDNUM_H_
#define _CARDNUM_H_

// define card number for Slave Game

#define		CLUB			0
#define		DIAMOND			1
#define		HEART			2
#define		SPADE			3

#define		GET_FACE(_cardnum_)		(((_cardnum_)-1)%4)
#define		GET_NUM(_cardnum_)		(((_cardnum_)-1) / 4)

#define		CARD_3			0
#define		CARD_4			1
#define		CARD_5			2
#define		CARD_6			3
#define		CARD_7			4
#define		CARD_8			5
#define		CARD_9			6
#define		CARD_10			7
#define		CARD_J			8
#define		CARD_Q			9
#define		CARD_K			10
#define		CARD_A			11
#define		CARD_2			12

#define		CLUB_3			1
#define		CLUB_4			5
#define		CLUB_5			9
#define		CLUB_6	 		13
#define		CLUB_7			17
#define		CLUB_8			21
#define		CLUB_9			25
#define		CLUB_10			29
#define		CLUB_J			33
#define		CLUB_Q			37
#define		CLUB_K			41
#define		CLUB_A			45
#define		CLUB_2			49

#define		DIAMOND_3 		2
#define		DIAMOND_4 		6
#define		DIAMOND_5 		10
#define		DIAMOND_6  		14
#define		DIAMOND_7 		18
#define		DIAMOND_8 		22
#define		DIAMOND_9 		26
#define		DIAMOND_10		30
#define		DIAMOND_J		34
#define		DIAMOND_Q		38
#define		DIAMOND_K		42
#define		DIAMOND_A		46
#define		DIAMOND_2		50

#define		HEART_3			3
#define		HEART_4			7
#define		HEART_5			11
#define		HEART_6			15
#define		HEART_7			19
#define		HEART_8			23
#define		HEART_9			27
#define		HEART_10		31
#define		HEART_J			35
#define		HEART_Q			39
#define		HEART_K			43
#define		HEART_A			47
#define		HEART_2			51

#define		SPADE_3			4
#define		SPADE_4			8
#define		SPADE_5			12
#define		SPADE_6			16
#define		SPADE_7			20
#define		SPADE_8			24
#define		SPADE_9			28
#define		SPADE_10		32
#define		SPADE_J			36
#define		SPADE_Q			40
#define		SPADE_K			44
#define		SPADE_A			48
#define		SPADE_2			52

#endif