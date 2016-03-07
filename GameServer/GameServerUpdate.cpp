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
	OnExitGame((CPlayer *)pIOContext, 0);
	Logout((CPlayer *)pIOContext);

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
					WORD fullSize = *(WORD *)pPlayer->recvBuffer.GetPopPointer();
					WORD bodySize = fullSize - sizeof(msg);

					if (pPlayer->recvBuffer.GetActiveBufferSize() < sizeof(fullSize) + fullSize) break;
					if (pPlayer->recvBuffer.PopData((BYTE *)&fullSize, sizeof(fullSize)) == FALSE) break;
					if (pPlayer->recvBuffer.PopData((BYTE *)&msg, sizeof(msg)) == FALSE)  break;

					m_dwRecvDataSize += sizeof(fullSize) + fullSize;

					switch (msg) {
					case Client::SERVER_MSG::HEART:
						OnHeart(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case Client::SERVER_MSG::FLAGS:
						OnFlags(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case Client::SERVER_MSG::LOGIN:
						OnLogin(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case Client::SERVER_MSG::CREATE_GAME:
						OnCreateGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case Client::SERVER_MSG::DESTROY_GAME:
						OnDestroyGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case Client::SERVER_MSG::ENTER_GAME:
						OnEnterGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case Client::SERVER_MSG::EXIT_GAME:
						OnExitGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

						/*
					case Client::SERVER_MSG::MODIFY_GAME_PASSWORD:
						OnModifyGamePassword(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;
						*/

					case Client::SERVER_MSG::SEND_TO_PLAYER:
						OnSendToPlayer(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case Client::SERVER_MSG::SEND_TO_PLAYER_ALL:
						OnSendToPlayerAll(pPlayer, bodySize);
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
// ������Ϸ�߼�
//
void CGameServer::OnUpdateGameLogic(float deltaTime)
{

}

//
// ��������
//
void CGameServer::OnHeartReset(CPlayer *pPlayer)
{
	pPlayer->dwHeartTime = 0;
}

//
// ����
//
void CGameServer::OnHeart(CPlayer *pPlayer, WORD size)
{
	::Client::Heart clientHeart;
	::Server::Heart serverHeart;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientHeart, size) == FALSE) {
		return;
	}

	//
	// 2. ����
	//
	serverHeart.set_timestamp(clientHeart.timestamp());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverHeart, ::Server::SERVER_MSG::HEART);

	//
	// 4. �������
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// ��ʶ
//
void CGameServer::OnFlags(CPlayer *pPlayer, WORD size)
{
	::Client::Flags clientFlags;
	::Server::Flags serverFlags;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientFlags, size) == FALSE) {
		return;
	}

	//
	// 2. ��ʶ
	//
	serverFlags.set_flags(pPlayer->GetFlags());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverFlags, ::Server::SERVER_MSG::FLAGS);

	//
	// 4. �������
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// ��½
//
void CGameServer::OnLogin(CPlayer *pPlayer, WORD size)
{
	::Client::Login clientLogin;
	::Server::Login serverLogin;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientLogin, size) == FALSE) {
		return;
	}

	//
	// 2. ��ҵ�½
	//
	int err = ERR_NONE;

	if (clientLogin.version() != GAME_SERVER_VERSION) {
		err = ERR_VERSION_INVALID; goto ERR;
	}

	if (pPlayer->GetFlags() != PLAYER_FLAGS_NONE) {
		err = ERR_PLAYER_STATE_LOGIN; goto ERR;
	}

	if (Login(pPlayer, clientLogin.guid()) == FALSE) {
		err = ERR_PLAYER_INVALID_GUID; goto ERR;
	}

	serverLogin.set_guid(pPlayer->guid);

	goto NEXT;
ERR:
NEXT:
	serverLogin.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverLogin, ::Server::SERVER_MSG::LOGIN);

	//
	// 4. �������
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// ������Ϸ
//
void CGameServer::OnCreateGame(CPlayer *pPlayer, WORD size)
{
	::Client::CreateGame clientCreateGame;
	::Server::CreateGame serverCreateGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientCreateGame, size) == FALSE) {
		return;
	}

	//
	// 2. ������Ϸ
	//
	int err = ERR_NONE;

	if (pPlayer->GetFlags() != PLAYER_FLAGS_LOGIN) {
		err = ERR_PLAYER_STATE_LOGIN; goto ERR;
	}

	if (CGame *pGame = GetNextGame()) {
		pGame->SetGame(clientCreateGame.password().c_str(), clientCreateGame.mode(), clientCreateGame.map(), clientCreateGame.maxplayers());
		pGame->AddPlayer(pPlayer, clientCreateGame.password().c_str(), TRUE);
	}
	else {
		err = ERR_SERVER_FULL; goto ERR;
	}

	goto NEXT;
ERR:
NEXT:
	serverCreateGame.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverCreateGame, ::Server::SERVER_MSG::CREATE_GAME);

	//
	// 4. �������
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// ������Ϸ
//
void CGameServer::OnDestroyGame(CPlayer *pPlayer, WORD size)
{
	::Client::DestroyGame clientDestroyGame;
	::Server::DestroyGame serverDestroyGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientDestroyGame, size) == FALSE) {
		return;
	}

	//
	// 2. ���ټ��
	//
	int err = pPlayer->pGame ? ERR_NONE : ERR_PLAYER_OUT_GAME;
	serverDestroyGame.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverDestroyGame, ::Server::SERVER_MSG::DESTROY_GAME);

	//
	// 4. �������
	//
	if (err == ERR_NONE) {
		SendToPlayerAll(pPlayer->pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
	else {
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}

	//
	// 5. ������Ϸ
	//
	if (err == ERR_NONE) {
		ReleaseGame(pPlayer->pGame);
	}
}

//
// ������Ϸ
//
void CGameServer::OnEnterGame(CPlayer *pPlayer, WORD size)
{
	::Client::EnterGame clientEnterGame;
	::Server::EnterGame serverEnterGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientEnterGame, size) == FALSE) {
		return;
	}

	//
	// 2. ������Ϸ
	//
	int err = ERR_NONE;

	if (pPlayer->GetFlags() != PLAYER_FLAGS_LOGIN) {
		err = ERR_PLAYER_STATE_LOGIN; goto ERR;
	}

	if (clientEnterGame.gameid() < 0 || clientEnterGame.gameid() >= m_maxGames) {
		err = ERR_GAME_INVALID_ID; goto ERR;
	}

	err = m_games[clientEnterGame.gameid()]->AddPlayer(pPlayer, clientEnterGame.password().c_str(), FALSE);

	goto NEXT;
ERR:
NEXT:
	serverEnterGame.set_err(err);
	serverEnterGame.set_guid(pPlayer->guid);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverEnterGame, ::Server::SERVER_MSG::ENTER_GAME);

	//
	// 4. �������
	//
	if (err == ERR_NONE) {
		SendToPlayerAll(pPlayer->pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
	else {
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}
}

//
// �˳���Ϸ
//
void CGameServer::OnExitGame(CPlayer *pPlayer, WORD size)
{
	::Client::ExitGame clientExitGame;
	::Server::ExitGame serverExitGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientExitGame, size) == FALSE) {
		return;
	}

	//
	// 2. �˳���Ϸ
	//
	int err = pPlayer->pGame ? ERR_NONE : ERR_PLAYER_OUT_GAME;
	serverExitGame.set_err(err);
	serverExitGame.set_guid(pPlayer->guid);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverExitGame, ::Server::SERVER_MSG::EXIT_GAME);

	//
	// 4. �������
	//
	if (err == ERR_NONE) {
		SendToPlayerAll(pPlayer->pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
	else {
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}

	//
	// 5. �˳���Ϸ
	//
	if (err == ERR_NONE) {
		pPlayer->pGame->DelPlayer(pPlayer);
	}
}

/*
//
// �޸���Ϸ����
//
void CGameServer::OnModifyGamePassword(CPlayer *pPlayer, WORD size)
{
	::Client::ModifyGamePassword clientModifyGamePassword;
	::Server::ModifyGamePassword serverModifyGamePassword;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientModifyGamePassword, size) == FALSE) {
		return;
	}

	//
	// 2. �޸���Ϸ����
	//
	int err = ERR_NONE;

	if (err == ERR_NONE) err = pPlayer->pGame ? ERR_NONE : ERR_PLAYER_OUT_GAME;
	if (err == ERR_NONE) pPlayer->pGame->SetGame(clientModifyGamePassword.password().c_str(), pPlayer->pGame->GetMode(), pPlayer->pGame->GetMapID(), pPlayer->pGame->GetMaxPlayers());

	serverModifyGamePassword.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverModifyGamePassword, ::Server::SERVER_MSG::MODIFY_GAME_PASSWORD);

	//
	// 4. �������
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}
*/

//
// ����ָ�����
//
void CGameServer::OnSendToPlayer(CPlayer *pPlayer, WORD size)
{
	::Client::SendToPlayer clientSendToPlayer;
	::Server::SendToPlayer serverSendToPlayer;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientSendToPlayer, size) == FALSE) {
		return;
	}

	//
	// 2. ����Ŀ�����
	//
	CPlayer *pTarget = QueryPlayer(clientSendToPlayer.guid());
	if (pTarget == NULL) return;

	serverSendToPlayer.set_size(clientSendToPlayer.size());
	serverSendToPlayer.set_data(clientSendToPlayer.data().c_str(), clientSendToPlayer.size());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverSendToPlayer, ::Server::SERVER_MSG::SEND_TO_PLAYER);

	//
	// 4. ����Ŀ�����
	//
	SendToPlayer(pTarget, buffer, writeBuffer.GetActiveBufferSize());
}

//
// �����������
//
void CGameServer::OnSendToPlayerAll(CPlayer *pPlayer, WORD size)
{
	::Client::SendToPlayerAll clientSendToPlayerAll;
	::Server::SendToPlayer serverSendToPlayer;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &clientSendToPlayerAll, size) == FALSE) {
		return;
	}

	//
	// 2. ����Ŀ�����
	//
	if (pPlayer->pGame == NULL) {
		return;
	}

	serverSendToPlayer.set_size(clientSendToPlayerAll.size());
	serverSendToPlayer.set_data(clientSendToPlayerAll.data().c_str(), clientSendToPlayerAll.size());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &serverSendToPlayer, ::Server::SERVER_MSG::SEND_TO_PLAYER);

	//
	// 4. �����������
	//
	SendToPlayerAll(pPlayer->pGame, pPlayer, buffer, writeBuffer.GetActiveBufferSize(), clientSendToPlayerAll.filter());
}
