#include "GameServer.h"


//
// 客户端链接回调
//
void CGameServer::OnConnect(CIOContext *pContext, SOCKET acceptSocket)
{
	((CPlayer *)pContext)->SetFlags(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_NONE);
	CIOCPServer::OnConnect(pContext, acceptSocket);
}

//
// 客户端断链回调
//
void CGameServer::OnDisconnect(CIOContext *pContext)
{
	OnExitGame((CPlayer *)pContext, 0);
	Logout((CPlayer *)pContext);
	CIOCPServer::OnDisconnect(pContext);
}

//
// 更新接收消息
//
void CGameServer::OnUpdateRecv(DWORD dwDeltaTime)
{
	CPlayer *pPlayer = (CPlayer *)m_pActiveContext;

	while (pPlayer) {
		CPlayer *pNextPlayer = (CPlayer *)pPlayer->pNextActive;

		if (pPlayer->IsAlive()) {
			pPlayer->SwitchRecvBuffer();
			pPlayer->dwHeartTime += dwDeltaTime;

			while (TRUE) {
				WORD msg;
				WORD fullSize;
				WORD bodySize;

				if (pPlayer->GetRecvBuffer().GetData((BYTE *)&fullSize, sizeof(fullSize)) != sizeof(fullSize)) break;
				if (pPlayer->GetRecvBuffer().GetActiveBufferSize() < sizeof(fullSize) + fullSize) break;

				if (pPlayer->GetRecvBuffer().PopData((BYTE *)&fullSize, sizeof(fullSize)) != sizeof(fullSize)) break;
				if (pPlayer->GetRecvBuffer().PopData((BYTE *)&msg, sizeof(msg)) != sizeof(msg))  break;

				bodySize = fullSize - sizeof(msg);
				m_dwRecvDataSize += sizeof(fullSize) + fullSize;

				ResponseFuncs::const_iterator itFunc = m_responses.find(msg);
				if (itFunc == m_responses.end()) continue;

				(this->*itFunc->second)(pPlayer, bodySize);
				OnHeartReset(pPlayer);
			}
		}

		if (pPlayer->Check((DWORD)(1000 * m_timeOut)) == FALSE) {
			ReleaseContext(pPlayer, FALSE);
		}

		pPlayer = pNextPlayer;
	}
}

//
// 更新游戏逻辑
//
void CGameServer::OnUpdateGameLogic(float deltaTime)
{
	if (CGame *pGame = m_pActiveGame) {
		do {
			pGame->Update(deltaTime);
		} while (pGame = pGame->pNextActive);
	}
}

//
// 重置心跳
//
void CGameServer::OnHeartReset(CPlayer *pPlayer)
{
	pPlayer->dwHeartTime = 0;
}

//
// 心跳
//
void CGameServer::OnHeart(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::Heart requestHeart;
	ProtoGameServer::Heart responseHeart;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestHeart, size) == FALSE) {
		return;
	}

	//
	// 2. 心跳
	//
	responseHeart.set_timestamp(requestHeart.timestamp());

	//
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseHeart, ProtoGameServer::RESPONSE_MSG::HEART);

	//
	// 4. 发送玩家
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// 标识
//
void CGameServer::OnFlags(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::Flags requestFlags;
	ProtoGameServer::Flags responseFlags;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestFlags, size) == FALSE) {
		return;
	}

	//
	// 2. 标识
	//
	responseFlags.set_flags(pPlayer->GetFlags());

	//
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseFlags, ProtoGameServer::RESPONSE_MSG::FLAGS);

	//
	// 4. 发送玩家
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// 登陆
//
void CGameServer::OnLogin(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::Login requestLogin;
	ProtoGameServer::Login responseLogin;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestLogin, size) == FALSE) {
		return;
	}

	//
	// 2. 玩家登陆
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
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseLogin, ProtoGameServer::RESPONSE_MSG::LOGIN);

	//
	// 4. 发送玩家
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// 就绪
//
void CGameServer::OnReady(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::Ready requestReady;
	ProtoGameServer::Flags responseFlags;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestReady, size) == FALSE) {
		return;
	}

	//
	// 2. 标识
	//
	if (pPlayer->IsLogin() == FALSE) {
		return;
	}

	if (pPlayer->pGame == NULL) {
		return;
	}

	if (requestReady.ready()) {
		pPlayer->EnableFlag(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_READY);
	}
	else {
		pPlayer->DisableFlag(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_READY);
		pPlayer->DisableFlag(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_GAMING);
	}
	responseFlags.set_flags(pPlayer->GetFlags());

	//
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseFlags, ProtoGameServer::RESPONSE_MSG::FLAGS);

	//
	// 4. 发送玩家
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// 获得游戏列表
//
void CGameServer::OnListGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::ListGame requestListGame;
	ProtoGameServer::ListGame responseListGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestListGame, size) == FALSE) {
		return;
	}

	//
	// 2. 获得游戏列表
	//
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (pPlayer->IsLogin() == FALSE) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_LOGIN; goto ERR;
	}

	if (CGame *pGame = m_pActiveGame) {
		do {
			if (ProtoGameServer::ListGame_Game *pGameStatus = responseListGame.add_games()) {
				pGameStatus->set_private_(pGame->IsPrivate() ? true : false);
				pGameStatus->set_gameid(pGame->id);
				pGameStatus->set_mode(pGame->GetMode());
				pGameStatus->set_mapid(pGame->GetMapID());
				pGameStatus->set_curplayers(pGame->GetCurPlayers());
				pGameStatus->set_maxplayers(pGame->GetMaxPlayers());
				pGameStatus->set_evaluation(pGame->GetEvaluation());
			}
		} while (pGame = pGame->pNextActive);
	}

	goto NEXT;
ERR:
NEXT:
	responseListGame.set_err(err);

	 //
	 // 3. 序列化消息
	 //
	 Serializer(&writeBuffer, &responseListGame, ProtoGameServer::RESPONSE_MSG::LIST_GAME);

	 //
	 // 4. 发送玩家
	 //
	 SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// 创建游戏
//
void CGameServer::OnCreateGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::CreateGame requestCreateGame;
	ProtoGameServer::CreateGame responseCreateGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestCreateGame, size) == FALSE) {
		return;
	}

	//
	// 2. 创建游戏
	//
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (pPlayer->IsLogin() == FALSE) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_LOGIN; goto ERR;
	}

	if (pPlayer->pGame != NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_INGAME; goto ERR;
	}

	CGame *pGame = GetNextGame(requestCreateGame.gameid());

	if (pGame == NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_SERVER_FULL; goto ERR;
	}

	pGame->SetGame(requestCreateGame.password().c_str(), requestCreateGame.mode(), requestCreateGame.mapid(), requestCreateGame.maxplayers(), requestCreateGame.evaluation());
	pGame->AddPlayer(pPlayer, requestCreateGame.password().c_str(), TRUE);

	responseCreateGame.set_host(pGame->GetHostGUID());
	responseCreateGame.set_gameid(pGame->id);

	goto NEXT;
ERR:
NEXT:
	responseCreateGame.set_err(err);

	//
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseCreateGame, ProtoGameServer::RESPONSE_MSG::CREATE_GAME);

	//
	// 4. 发送玩家
	//
	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// 销毁游戏
//
void CGameServer::OnDestroyGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::DestroyGame requestDestroyGame;
	ProtoGameServer::DestroyGame responseDestroyGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestDestroyGame, size) == FALSE) {
		return;
	}

	//
	// 2. 销毁检查
	//
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (pPlayer->IsLogin() == FALSE) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_LOGIN; goto ERR;
	}

	if (pPlayer->pGame == NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_INGAME; goto ERR;
	}

	if (pPlayer->pGame->GetHostGUID() != pPlayer->guid) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_NOT_HOST; goto ERR;
	}

	goto NEXT;
ERR:
NEXT:
	responseDestroyGame.set_err(err);

	//
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseDestroyGame, ProtoGameServer::RESPONSE_MSG::DESTROY_GAME);

	//
	// 4. 发送玩家
	//
	if (err == ProtoGameServer::ERROR_CODE::ERR_NONE && pPlayer->pGame) {
		SendToPlayerAll(pPlayer->pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
	else {
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}

	//
	// 5. 销毁游戏
	//
	if (err == ProtoGameServer::ERROR_CODE::ERR_NONE) {
		ReleaseGame(pPlayer->pGame);
	}
}

//
// 进入游戏
//
void CGameServer::OnEnterGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::EnterGame requestEnterGame;
	ProtoGameServer::EnterGame responseEnterGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestEnterGame, size) == FALSE) {
		return;
	}

	//
	// 2. 进入游戏
	//
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (pPlayer->IsLogin() == FALSE) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_LOGIN; goto ERR;
	}

	if (pPlayer->pGame != NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_INGAME; goto ERR;
	}

	if (requestEnterGame.gameid() < 0 || requestEnterGame.gameid() >= m_maxGames) {
		err = ProtoGameServer::ERROR_CODE::ERR_GAME_INVALID_ID; goto ERR;
	}

	err = (ProtoGameServer::ERROR_CODE)m_games[requestEnterGame.gameid()]->AddPlayer(pPlayer, requestEnterGame.password().c_str(), FALSE);
	if (err != ProtoGameServer::ERROR_CODE::ERR_NONE) goto ERR;

	responseEnterGame.set_guid(pPlayer->guid);
	responseEnterGame.set_host(pPlayer->pGame->GetHostGUID());
	responseEnterGame.set_gameid(pPlayer->pGame->id);

	goto NEXT;
ERR:
NEXT:
	responseEnterGame.set_err(err);

	//
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseEnterGame, ProtoGameServer::RESPONSE_MSG::ENTER_GAME);

	//
	// 4. 发送玩家
	//
	if (err == ProtoGameServer::ERROR_CODE::ERR_NONE && pPlayer->pGame) {
		SendToPlayerAll(pPlayer->pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
	else {
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}
}

//
// 退出游戏
//
void CGameServer::OnExitGame(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::ExitGame requestExitGame;
	ProtoGameServer::ExitGame responseExitGame;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestExitGame, size) == FALSE) {
		return;
	}

	//
	// 2. 退出游戏
	//
	ProtoGameServer::ERROR_CODE err = ProtoGameServer::ERROR_CODE::ERR_NONE;

	if (pPlayer->IsLogin() == FALSE) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_LOGIN; goto ERR;
	}

	if (pPlayer->pGame == NULL) {
		err = ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_INGAME; goto ERR;
	}

	CGame *pGame = pPlayer->pGame;

	err = (ProtoGameServer::ERROR_CODE)pGame->DelPlayer(pPlayer);
	if (err != ProtoGameServer::ERROR_CODE::ERR_NONE) goto ERR;

	if (pGame->IsEmpty()) {
		ReleaseGame(pGame);
		pGame = NULL;
	}

	responseExitGame.set_guid(pPlayer->guid);
	responseExitGame.set_host(pGame ? pGame->GetHostGUID() : 0xffffffff);

	goto NEXT;
ERR:
NEXT:
	responseExitGame.set_err(err);

	//
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseExitGame, ProtoGameServer::RESPONSE_MSG::EXIT_GAME);

	//
	// 4. 发送玩家
	//
	if (err == ProtoGameServer::ERROR_CODE::ERR_NONE && pGame) {
		SendToPlayerAll(pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}

	SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
}

//
// 发送指定玩家
//
void CGameServer::OnSendToPlayer(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::SendToPlayer requestSendToPlayer;
	ProtoGameServer::SendToPlayer responseSendToPlayer;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestSendToPlayer, size) == FALSE) {
		return;
	}

	//
	// 2. 查找目标玩家
	//
	if (pPlayer->IsLogin() == FALSE) {
		return;
	}

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
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseSendToPlayer, ProtoGameServer::RESPONSE_MSG::SEND_TO_PLAYER);

	//
	// 4. 发送目标玩家
	//
	SendToPlayer(pTarget, buffer, writeBuffer.GetActiveBufferSize());
}

//
// 发送所有玩家
//
void CGameServer::OnSendToPlayerAll(CPlayer *pPlayer, WORD size)
{
	ProtoGameClient::SendToPlayerAll requestSendToPlayerAll;
	ProtoGameServer::SendToPlayer responseSendToPlayer;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. 解析消息
	//
	if (Parser(&pPlayer->GetRecvBuffer(), &requestSendToPlayerAll, size) == FALSE) {
		return;
	}

	//
	// 2. 查找目标玩家
	//
	if (pPlayer->IsLogin() == FALSE) {
		return;
	}

	if (pPlayer->pGame == NULL) {
		return;
	}

	responseSendToPlayer.set_size(requestSendToPlayerAll.size());
	responseSendToPlayer.set_data(requestSendToPlayerAll.data().c_str(), requestSendToPlayerAll.size());

	//
	// 3. 序列化消息
	//
	Serializer(&writeBuffer, &responseSendToPlayer, ProtoGameServer::RESPONSE_MSG::SEND_TO_PLAYER);

	//
	// 4. 发送所有玩家
	//
	SendToPlayerAll(pPlayer->pGame, pPlayer, buffer, writeBuffer.GetActiveBufferSize(), requestSendToPlayerAll.filter());
}
