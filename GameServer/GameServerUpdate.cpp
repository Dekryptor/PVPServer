#include "GameServer.h"


//
// �ͻ������ӻص�
//
void CGameServer::OnConnect(CIOContext *pIOContext, SOCKET acceptSocket)
{
	((CPlayer *)pIOContext)->SetFlags(PLAYER_FLAGS_NONE);
	CIOCPServer::OnConnect(pIOContext, acceptSocket);
}

//
// �ͻ��˶����ص�
//
void CGameServer::OnDisconnect(CIOContext *pIOContext)
{
	OnExitGame((CPlayer *)pIOContext);
	UnRegisterPlayer((CPlayer *)pIOContext);

	CIOCPServer::OnDisconnect(pIOContext);
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
