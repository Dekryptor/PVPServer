#include "GameServer.h"


//
// �ͻ������ӻص�
//
void CGameServer::OnConnect(CIOContext *pContext, SOCKET acceptSocket)
{
	((CPlayer *)pContext)->SetFlags(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_NONE);
	CIOCPServer::OnConnect(pContext, acceptSocket);
}

//
// �ͻ��˶����ص�
//
void CGameServer::OnDisconnect(CIOContext *pContext)
{
	OnExitGame((CPlayer *)pContext, 0);
	Logout((CPlayer *)pContext);
	CIOCPServer::OnDisconnect(pContext);
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
					WORD msg;
					WORD fullSize;
					WORD bodySize;

					if (pPlayer->recvBuffer.GetData((BYTE *)&fullSize, sizeof(fullSize)) != sizeof(fullSize)) break;
					if (pPlayer->recvBuffer.GetActiveBufferSize() < sizeof(fullSize) + fullSize) break;

					if (pPlayer->recvBuffer.PopData((BYTE *)&fullSize, sizeof(fullSize)) != sizeof(fullSize)) break;
					if (pPlayer->recvBuffer.PopData((BYTE *)&msg, sizeof(msg)) != sizeof(msg))  break;

					bodySize = fullSize - sizeof(msg);
					m_dwRecvDataSize += sizeof(fullSize) + fullSize;

					switch (msg) {
					case ProtoGameClient::REQUEST_MSG::HEART:
						OnHeart(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::FLAGS:
						OnFlags(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::LOGIN:
						OnLogin(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::LIST_GAME:
						OnListGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::CREATE_GAME:
						OnCreateGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::DESTROY_GAME:
						OnDestroyGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::ENTER_GAME:
						OnEnterGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::EXIT_GAME:
						OnExitGame(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::SEND_TO_PLAYER:
						OnSendToPlayer(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					case ProtoGameClient::REQUEST_MSG::SEND_TO_PLAYER_ALL:
						OnSendToPlayerAll(pPlayer, bodySize);
						OnHeartReset(pPlayer);
						break;

					default:
						OnUpdateGameMessage(pPlayer, bodySize, msg);
						OnHeartReset(pPlayer);
						break;
					}
				}
			}
			pPlayer->recvBuffer.Unlock();
		}

		if (pPlayer->Check((DWORD)(1000 * m_timeOut)) == FALSE) {
			ReleaseContext(pPlayer, FALSE);
		}

		pPlayer = pNextPlayer;
	}
}

//
// ������Ϸ��Ϣ
//
void CGameServer::OnUpdateGameMessage(CPlayer *pPlayer, WORD size, WORD msg)
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
	ProtoGameClient::Heart requestHeart;
	ProtoGameServer::Heart responseHeart;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestHeart, size) == FALSE) {
		return;
	}

	//
	// 2. ����
	//
	responseHeart.set_timestamp(requestHeart.timestamp());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseHeart, ProtoGameServer::RESPONSE_MSG::HEART);

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
	ProtoGameClient::Flags requestFlags;
	ProtoGameServer::Flags responseFlags;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestFlags, size) == FALSE) {
		return;
	}

	//
	// 2. ��ʶ
	//
	responseFlags.set_flags(pPlayer->GetFlags());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseFlags, ProtoGameServer::RESPONSE_MSG::FLAGS);

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
	ProtoGameClient::Login requestLogin;
	ProtoGameServer::Login responseLogin;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestLogin, size) == FALSE) {
		return;
	}

	//
	// 2. ��ҵ�½
	//
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (requestLogin.version() != ProtoGameServer::VERSION_NUMBER::VERSION) {
		err = ProtoGameServer::ERROR_CODE::ERR_VERSION_INVALID; goto ERR;
	}

	if (pPlayer->GetFlags() != ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_NONE) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_NONE; goto ERR;
	}

	if (Login(pPlayer, requestLogin.guid()) == FALSE) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_INVALID_GUID; goto ERR;
	}

	responseLogin.set_guid(pPlayer->guid);

	goto NEXT;
ERR:
NEXT:
	responseLogin.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseLogin, ProtoGameServer::RESPONSE_MSG::LOGIN);

	//
	// 4. �������
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// �����Ϸ�б�
//
void CGameServer::OnListGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::ListGame requestListGame;
	ProtoGameServer::ListGame responseListGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestListGame, size) == FALSE) {
		return;
	}

	//
	// 2. �����Ϸ�б�
	//
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (pPlayer->pGame != NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_INGAME; goto ERR;
	}

	if (pPlayer->GetFlags() != ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_LOGIN) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_LOGIN; goto ERR;
	}

	if (CGame *pGame = m_pActiveGame) {
		do {
			if (ProtoGameServer::ListGame_Game *pGameStatus = responseListGame.add_games()) {
				pGameStatus->set_private_(pGame->IsPrivate() ? true : false);
				pGameStatus->set_gameid(pGame->id);
				pGameStatus->set_mode(pGame->GetMode());
				pGameStatus->set_map(pGame->GetMapID());
				pGameStatus->set_maxplayers(pGame->GetMaxPlayers());
				pGameStatus->set_curplayers(pGame->GetCurPlayers());
				pGameStatus->set_evaluation(pGame->GetEvaluation());
			}
		} while (pGame = pGame->pNextActive);
	}

	goto NEXT;
ERR:
NEXT:
	responseListGame.set_err(err);

	 //
	 // 3. ���л���Ϣ
	 //
	 Serializer(&writeBuffer, &responseListGame, ProtoGameServer::RESPONSE_MSG::LIST_GAME);

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
	ProtoGameClient::CreateGame requestCreateGame;
	ProtoGameServer::CreateGame responseCreateGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestCreateGame, size) == FALSE) {
		return;
	}

	//
	// 2. ������Ϸ
	//
	CGame *pGame = NULL;
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (pPlayer->pGame != NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_INGAME; goto ERR;
	}

	if (pPlayer->GetFlags() != ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_LOGIN) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_LOGIN; goto ERR;
	}

	pGame = GetNextGame();

	if (pGame == NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_SERVER_FULL; goto ERR;
	}

	pGame->SetGame(requestCreateGame.password().c_str(), requestCreateGame.mode(), requestCreateGame.map(), requestCreateGame.maxplayers(), requestCreateGame.evaluation());
	pGame->AddPlayer(pPlayer, requestCreateGame.password().c_str(), TRUE);

	responseCreateGame.set_gameid(pGame->id);

	goto NEXT;
ERR:
NEXT:
	responseCreateGame.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseCreateGame, ProtoGameServer::RESPONSE_MSG::CREATE_GAME);

	//
	// 4. �������
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());

	//
	// 5. �����������
	//
	if (err == ProtoGameServer::ERROR_CODE::ERR_NONE) {
		Host(pPlayer);
	}
}

//
// ������Ϸ
//
void CGameServer::OnDestroyGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::DestroyGame requestDestroyGame;
	ProtoGameServer::DestroyGame responseDestroyGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestDestroyGame, size) == FALSE) {
		return;
	}

	//
	// 2. ���ټ��
	//
	responseDestroyGame.set_err(pPlayer->pGame ? ProtoGameServer::ERROR_CODE::ERR_NONE : ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_INGAME);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseDestroyGame, ProtoGameServer::RESPONSE_MSG::DESTROY_GAME);

	//
	// 4. �������
	//
	if (pPlayer->pGame) {
		SendToPlayerAll(pPlayer->pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
	else {
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}

	//
	// 5. ������Ϸ
	//
	if (pPlayer->pGame) {
		ReleaseGame(pPlayer->pGame);
	}
}

//
// ������Ϸ
//
void CGameServer::OnEnterGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::EnterGame requestEnterGame;
	ProtoGameServer::EnterGame responseEnterGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestEnterGame, size) == FALSE) {
		return;
	}

	//
	// 2. ������Ϸ
	//
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (pPlayer->pGame != NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_INGAME; goto ERR;
	}

	if (pPlayer->GetFlags() != ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_LOGIN) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_LOGIN; goto ERR;
	}

	if (requestEnterGame.gameid() < 0 || requestEnterGame.gameid() >= (DWORD)m_maxGames) {
		err = ProtoGameServer::ERROR_CODE::ERR_GAME_INVALID_ID; goto ERR;
	}

	err = (ProtoGameServer::ERROR_CODE)m_games[requestEnterGame.gameid()]->AddPlayer(pPlayer, requestEnterGame.password().c_str(), FALSE);
	if (err != ProtoGameServer::ERROR_CODE::ERR_NONE) goto ERR;

	responseEnterGame.set_gameid(pPlayer->pGame->id);
	responseEnterGame.set_guid(pPlayer->guid);

	goto NEXT;
ERR:
NEXT:
	responseEnterGame.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseEnterGame, ProtoGameServer::RESPONSE_MSG::ENTER_GAME);

	//
	// 4. �������
	//
	if (err == ProtoGameServer::ERROR_CODE::ERR_NONE && pPlayer->pGame) {
		SendToPlayerAll(pPlayer->pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
	else {
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}

	//
	// 5. �����������
	//
	if (err == ProtoGameServer::ERROR_CODE::ERR_NONE) {
		Host(pPlayer);
	}
}

//
// �˳���Ϸ
//
void CGameServer::OnExitGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::ExitGame requestExitGame;
	ProtoGameServer::ExitGame responseExitGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestExitGame, size) == FALSE) {
		return;
	}

	//
	// 2. �˳���Ϸ
	//
	responseExitGame.set_err(pPlayer->pGame ? ProtoGameServer::ERROR_CODE::ERR_NONE : ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_INGAME);
	responseExitGame.set_guid(pPlayer->guid);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseExitGame, ProtoGameServer::RESPONSE_MSG::EXIT_GAME);

	//
	// 4. �������
	//
	if (pPlayer->pGame) {
		SendToPlayerAll(pPlayer->pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
	else {
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}

	//
	// 5. �˳���Ϸ
	//
	if (CGame *pGame = pPlayer->pGame) {
		DWORD dwLastHostGUID = pGame->GetHostGUID();

		pGame->DelPlayer(pPlayer);

		if (pGame->IsEmpty()) {
			ReleaseGame(pGame);
		}
		else if (dwLastHostGUID != pGame->GetHostGUID()) {
			Host(pGame);
		}
	}
}

//
// ����ָ�����
//
void CGameServer::OnSendToPlayer(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::SendToPlayer requestSendToPlayer;
	ProtoGameServer::SendToPlayer responseSendToPlayer;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestSendToPlayer, size) == FALSE) {
		return;
	}

	//
	// 2. ����Ŀ�����
	//
	if (pPlayer->pGame == NULL) {
		return;
	}

	CPlayer *pTarget = QueryPlayer(requestSendToPlayer.guid());

	if (pTarget == NULL) {
		return;
	}

	responseSendToPlayer.set_size(requestSendToPlayer.size());
	responseSendToPlayer.set_data(requestSendToPlayer.data().c_str(), requestSendToPlayer.size());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseSendToPlayer, ProtoGameServer::RESPONSE_MSG::SEND_TO_PLAYER);

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
	ProtoGameClient::SendToPlayerAll requestSendToPlayerAll;
	ProtoGameServer::SendToPlayer responseSendToPlayer;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pPlayer->recvBuffer, &requestSendToPlayerAll, size) == FALSE) {
		return;
	}

	//
	// 2. ����Ŀ�����
	//
	if (pPlayer->pGame == NULL) {
		return;
	}

	responseSendToPlayer.set_size(requestSendToPlayerAll.size());
	responseSendToPlayer.set_data(requestSendToPlayerAll.data().c_str(), requestSendToPlayerAll.size());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseSendToPlayer, ProtoGameServer::RESPONSE_MSG::SEND_TO_PLAYER);

	//
	// 4. �����������
	//
	SendToPlayerAll(pPlayer->pGame, pPlayer, buffer, writeBuffer.GetActiveBufferSize(), requestSendToPlayerAll.filter());
}
