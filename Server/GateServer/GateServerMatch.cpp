#include "GateServer.h"


//
// ����ƥ��
//
void CGateServer::OnUpdateMatch(DWORD dwDeltaTime)
{
	//
	// 1. ���¼�ʱ���
	//
	static DWORD dwTime = 0;

	dwTime += dwDeltaTime;

	if (dwTime < 1000) {
		return;
	}

	dwTime = 0;

	//
	// 2. ƥ�����
	//
}
