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
// ���·�����Ϣ
//
void CGameServer::OnUpdateSend(void)
{
	if (CPlayer *pPlayer = (CPlayer *)m_pActiveContext) {
		do {
			if (pPlayer->IsAlive()) {
				pPlayer->sendBuffer.Lock();
				{
					pPlayer->OnSendNext(NULL, 0, 0);
				}
				pPlayer->sendBuffer.Unlock();
			}
		} while (pPlayer = (CPlayer *)pPlayer->pNextActive);
	}
}

//
// ���½�����Ϣ
//
void CGameServer::OnUpdateRecv(DWORD dwDeltaTime)
{
	CPlayer *pPlayer = (CPlayer *)m_pActiveContext;

	while (pPlayer) {
		CPlayer *pNextPlayer = (CPlayer *)pPlayer->pNextActive;

		if (pPlayer->IsAlive()) {
			pPlayer->recvBuffer.Lock();
			{
				pPlayer->dwHeartTime += dwDeltaTime;

				while (TRUE) {
					WORD msg = 0;
					WORD size = *(WORD *)pPlayer->recvBuffer.GetPopPointer();

					if (pPlayer->recvBuffer.GetActiveBufferSize() < sizeof(size) + size) break;
					if (!pPlayer->recvBuffer.PopData((BYTE *)&size, sizeof(size))) break;
					if (!pPlayer->recvBuffer.PopData((BYTE *)&msg, sizeof(msg)))  break;

					m_dwRecvDataSize += size;

					switch (msg) {
					case GAMESERVER_MSG_C2S_HEART:
						OnHeart(pPlayer);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_LOGIN:
						OnLogin(pPlayer);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_FLAGS:
						OnFlags(pPlayer);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_CREATE_GAME:
						OnCreateGame(pPlayer);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_DESTROY_GAME:
						OnDestroyGame(pPlayer);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_ENTER_GAME:
						OnEnterGame(pPlayer);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_EXIT_GAME:
						OnExitGame(pPlayer);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_MODIFY_GAME_PASSWORD:
						OnModifyGamePassword(pPlayer);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_SENDTO_PLAYER:
						OnSendToPlayer(pPlayer, size);
						OnHeartReset(pPlayer);
						break;

					case GAMESERVER_MSG_C2S_SENDTO_PLAYER_ALL:
						OnSendToPlayerAll(pPlayer, size);
						OnHeartReset(pPlayer);
						break;

					default:
						OnUpdateGameMessage(pPlayer, msg);
						OnHeartReset(pPlayer);
						break;
					}
				}
			}
			pPlayer->recvBuffer.Unlock();
		}

		if (pPlayer->dwHeartTime > (DWORD)(1000 * m_timeOut)) {
			WriteLog("%s: Heart TimeOut\n", pPlayer->ip);
			ReleaseIOContext(pPlayer, FALSE);
		}

		pPlayer = pNextPlayer;
	}
}

//
// ������Ϸ��Ϣ
//
void CGameServer::OnUpdateGameMessage(CPlayer *pPlayer, WORD msg)
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
// �޸���Ϸ����
//
void CGameServer::OnModifyGamePassword(CPlayer *pPlayer)
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
