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
	virtual BOOL Match(int evaluation, const PlayerStatus &player, CIOContext *matchs[], int &indexMatch, int maxMatchs); // ƥ��


	// ����
protected:
	int m_maxMatchs;                                                                               // ���ƥ����
	float m_maxTimeOut;                                                                            // ƥ�䳬ʱ
	CIOContext **m_matchs;                                                                         // ƥ�伯��
};
