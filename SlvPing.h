#ifndef _C_SLVPING_H_
#define _C_SLVPING_H_

#include <IcmpPing.h>

// ==========================================
// IcmpPing need WSAStartup before use 
// ==========================================
class C_Slave;

class C_SlvPing : public C_IcmpPing
{
public:
	C_SlvPing(C_Slave *pSlave);

    void Ping(int nPlayerID,char * szIP);
protected:
	virtual void OnResponse(int nErrCode,LPIP_ECHO_REPLY pEchoReply);
private:
	C_Slave * m_pSlave;
	int m_nPlayerID;
};

#endif