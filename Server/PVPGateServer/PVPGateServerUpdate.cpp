#include "PVPGateServer.h"


//
// ����ƥ��
//
void CPVPGateServer::OnUpdateMatch(DWORD dwDeltaTime)
{
	static DWORD dwTime = 0;
	static const float timeOut = 100.0f;
	static const int maxGamePlayers = 6;
	static const DWORD dwMinEvaluation = 0;
	static const DWORD dwMaxEvaluation = 10000;

	//
	// 1. ���¼�ʱ���
	//
	dwTime += dwDeltaTime;

	if (dwTime < 1000) {
		return;
	}

	dwTime = 0;

	//
	// 2. ƥ�����
	//
	for (PlayerEvaluationMap::iterator itPlayerMap = m_evaluations.begin(); itPlayerMap != m_evaluations.end(); ++itPlayerMap) {
		for (std::map<DWORD, PlayerStatus>::iterator itPlayer = itPlayerMap->second.begin(); itPlayer != itPlayerMap->second.end(); ++itPlayer) {
			itPlayer->second.timeout += 1.0f;
		}
	}
}
