#pragma once


#include "GateServer.h"


class _ServerExport CPVPGateServer : public CGateServer
{
	// ����/��������
public:
	CPVPGateServer(void);
	virtual ~CPVPGateServer(void);


	// ����
protected:
	virtual void OnUpdateMatch(DWORD dwDeltaTime);                                                 // ����ƥ��


	// ����
protected:
};
