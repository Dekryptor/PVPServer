#include "GameServer.h"


CGameServer::CGameServer(void)
{

}

CGameServer::~CGameServer(void)
{

}

//
// ����������
//
BOOL CGameServer::Start(const char *ip, int port, int maxGames, int maxPlayers, int timeOut, const char *rootip, int rootport)
{
	return TRUE;
}

//
// ֹͣ������
//
void CGameServer::Stop(void)
{

}

//
// �������
//
BOOL CGameServer::AllocPlayers(int maxPlayers)
{
	return TRUE;
}

//
// ������Ϸ
//
BOOL CGameServer::AllocGames(int maxGames)
{
	return TRUE;
}

//
// �����㱨�߳�
//
BOOL CGameServer::CreateReportThread(void)
{
	return TRUE;
}

//
// ���������߳�
//
BOOL CGameServer::CreateUpdateThread(void)
{
	return TRUE;
}

//
// �ͷ����
//
void CGameServer::FreePlayers(void)
{

}

//
// �ͷ���Ϸ
//
void CGameServer::FreeGames(void)
{

}

//
// ���ٻ㱨�߳�
//
void CGameServer::DestroyReportThread(void)
{

}

//
// ���ٸ����߳�
//
void CGameServer::DestroyUpdateThread(void)
{

}

//
// �����Ϸ
//
CGame* CGameServer::GetNextGame(void)
{
	return NULL;
}

//
// �ͷ���Ϸ
//
void CGameServer::ReleaseGame(CGame *pGame)
{

}

//
// ��ѯ���
//
CPlayer* CGameServer::QueryPlayer(DWORD guid)
{
	return NULL;
}

//
// ע�����
//
BOOL CGameServer::RegisterPlayer(CPlayer *pPlayer, DWORD guid)
{
	return TRUE;
}

//
// ע�����
//
BOOL CGameServer::UnRegisterPlayer(CPlayer *pPlayer)
{
	return TRUE;
}

//
// ����ָ�����
//
void CGameServer::SendToPlayer(CPlayer *pPlayer, BYTE *pBuffer, DWORD size)
{

}

//
// �����������
//
void CGameServer::SendToPlayerAll(CGame *pGame, CPlayer *pPlayerIgnore, BYTE *pBuffer, DWORD size, DWORD dwFilter)
{

}

//
// �ͻ������ӻص�
//
void CGameServer::OnConnect(CIOContext *pIOContext, SOCKET acceptSocket)
{

}

//
// �ͻ��˶����ص�
//
void CGameServer::OnDisconnect(CIOContext *pIOContext)
{

}

//
// ���½�����Ϣ
//
void CGameServer::OnUpdateRecv(DWORD dwDeltaTime)
{

}

//
// ���·�����Ϣ
//
void CGameServer::OnUpdateSend(void)
{

}

//
// ������Ϸ
//
void CGameServer::OnUpdateGame(float deltaTime)
{

}

//
// ����
//
void CGameServer::OnHeart(CPlayer *pPlayer)
{

}

//
// ��������
//
void CGameServer::OnHeartReset(CPlayer *pPlayer)
{

}

//
// ��½
//
void CGameServer::OnLogin(CPlayer *pPlayer)
{

}

//
// ���ñ�ʶ
//
void CGameServer::OnFlags(CPlayer *pPlayer)
{

}

//
// ������Ϸ
//
void CGameServer::OnCreateGame(CPlayer *pPlayer)
{

}

//
// ������Ϸ
//
void CGameServer::OnDestroyGame(CPlayer *pPlayer)
{

}

//
// ������Ϸ
//
void CGameServer::OnEnterGame(CPlayer *pPlayer)
{

}

//
// �˳���Ϸ
//
void CGameServer::OnExitGame(CPlayer *pPlayer)
{

}

//
// ����ָ�����
//
void CGameServer::OnSendToPlayer(CPlayer *pPlayer, WORD packSize)
{

}

//
// �����������
//
void CGameServer::OnSendToPlayerAll(CPlayer *pPlayer, WORD packSize)
{

}

//
// �����������(������)
//
void CGameServer::OnSendToPlayerFilterAll(CPlayer *pPlayer, WORD packSize)
{

}

//
// ���
//
void CGameServer::Monitor(void)
{

}

//
// ����
//
void CGameServer::Report(BYTE *buffer, CCacheBuffer *pCacheBuffer)
{

}

//
// �㱨�߳�
//
DWORD WINAPI CGameServer::ReportThread(LPVOID lpParam)
{
	return 0L;
}

//
// �����߳�
//
DWORD WINAPI CGameServer::UpdateThread(LPVOID lpParam)
{
	return 0L;
}
