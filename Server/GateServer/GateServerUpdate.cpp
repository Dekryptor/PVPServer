#include "GateServer.h"


//
// �ͻ������ӻص�
//
void CGateServer::OnConnect(CIOContext *pContext, SOCKET acceptSocket)
{
	pContext->guid = 0xffffffff;
	pContext->dwUserData = 0xffffffff;
	CIOCPServer::OnConnect(pContext, acceptSocket);
}

//
// �ͻ��˶����ص�
//
void CGateServer::OnDisconnect(CIOContext *pContext)
{
	Logout(pContext);
	ClearGameServer(pContext);
	CIOCPServer::OnDisconnect(pContext);
}

//
// ���½�����Ϣ
//
void CGateServer::OnUpdateRecv(DWORD dwDeltaTime)
{
	CIOContext *pContext = m_pActiveContext;

	while (pContext) {
		CIOContext *pNextContext = pContext->pNextActive;

		if (pContext->IsAlive()) {
			pContext->recvBuffer.Lock();
			{
				pContext->dwHeartTime += dwDeltaTime;

				while (TRUE) {
					WORD msg;
					WORD fullSize;
					WORD bodySize;

					if (pContext->recvBuffer.GetData((BYTE *)&fullSize, sizeof(fullSize)) != sizeof(fullSize)) break;
					if (pContext->recvBuffer.GetActiveBufferSize() < sizeof(fullSize) + fullSize) break;

					if (pContext->recvBuffer.PopData((BYTE *)&fullSize, sizeof(fullSize)) != sizeof(fullSize)) break;
					if (pContext->recvBuffer.PopData((BYTE *)&msg, sizeof(msg)) != sizeof(msg))  break;

					bodySize = fullSize - sizeof(msg);
					m_dwRecvDataSize += sizeof(fullSize) + fullSize;

					switch (msg) {
					case ProtoGateClient::REQUEST_MSG::HEART:
						OnHeart(pContext, bodySize);
						OnHeartReset(pContext);
						break;

					case ProtoGateClient::REQUEST_MSG::LOGIN:
						OnLogin(pContext, bodySize);
						OnHeartReset(pContext);
						break;

					case ProtoGateClient::REQUEST_MSG::MATCH:
						OnMatch(pContext, bodySize);
						OnHeartReset(pContext);
						break;

					case ProtoGateClient::REQUEST_MSG::CANCEL_MATCH:
						OnCancelMatch(pContext, bodySize);
						OnHeartReset(pContext);
						break;

					case ProtoGateClient::REQUEST_MSG::LIST_GAME_SERVER:
						OnListGameServer(pContext, bodySize);
						OnHeartReset(pContext);
						break;

					case ProtoGateClient::REQUEST_MSG::SEND_TO_PLAYER:
						OnSendToPlayer(pContext, bodySize);
						OnHeartReset(pContext);
						break;

					case ProtoGameServer::REQUEST_MSG::SERVER_STATUS:
						OnGameServerStatus(pContext, bodySize);
						OnHeartReset(pContext);
						break;
					}
				}
			}
			pContext->recvBuffer.Unlock();
		}

		if (pContext->Check((DWORD)(1000 * m_timeOut)) == FALSE) {
			ReleaseContext(pContext, FALSE);
		}

		pContext = pNextContext;
	}
}

//
// ����ƥ��
//
void CGateServer::OnUpdateMatch(DWORD dwDeltaTime)
{

}

//
// ��������
//
void CGateServer::OnHeartReset(CIOContext *pContext)
{
	pContext->dwHeartTime = 0;
}

//
// ����
//
void CGateServer::OnHeart(CIOContext *pContext, WORD size)
{
	ProtoGateClient::Heart requestHeart;
	ProtoGateServer::Heart responseHeart;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pContext->recvBuffer, &requestHeart, size) == FALSE) {
		return;
	}

	//
	// 2. ����
	//
	responseHeart.set_timestamp(requestHeart.timestamp());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseHeart, ProtoGateServer::RESPONSE_MSG::HEART);

	//
	// 4. ����
	//
	SendTo(pContext, buffer, writeBuffer.GetActiveBufferSize());
}

//
// ��½
//
void CGateServer::OnLogin(CIOContext *pContext, WORD size)
{
	ProtoGateClient::Login requestLogin;
	ProtoGateServer::Login responseLogin;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pContext->recvBuffer, &requestLogin, size) == FALSE) {
		return;
	}

	//
	// 2. ��½
	//
	ProtoGateServer::ERROR_CODE err = ProtoGateServer::ERROR_CODE::ERR_NONE;

	if (requestLogin.version() != ProtoGateServer::VERSION_NUMBER::VERSION) {
		err = ProtoGateServer::ERROR_CODE::ERR_VERSION_INVALID; goto ERR;
	}

	if (pContext->guid != 0xffffffff) {
		err = ProtoGateServer::ERROR_CODE::ERR_PLAYER_NOT_NONE; goto ERR;
	}

	if (Login(pContext, requestLogin.guid()) == FALSE) {
		err = ProtoGateServer::ERROR_CODE::ERR_PLAYER_INVALID_GUID; goto ERR;
	}

	responseLogin.set_guid(pContext->guid);

	goto NEXT;
ERR:
NEXT:
	responseLogin.set_err(err);

	 //
	 // 3. ���л���Ϣ
	 //
	 Serializer(&writeBuffer, &responseLogin, ProtoGateServer::RESPONSE_MSG::LOGIN);

	 //
	 // 4. ����
	 //
	 SendTo(pContext, buffer, writeBuffer.GetActiveBufferSize());
}

//
// ƥ��
//
void CGateServer::OnMatch(CIOContext *pContext, WORD size)
{
	ProtoGateClient::Match requestMatch;
	ProtoGateServer::Match responseMatch;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pContext->recvBuffer, &requestMatch, size) == FALSE) {
		return;
	}

	//
	// 2. ���
	//
	ProtoGateServer::ERROR_CODE err = ProtoGateServer::ERROR_CODE::ERR_NONE;

	if (pContext->guid == 0xffffffff) {
		err = ProtoGateServer::ERROR_CODE::ERR_PLAYER_NOT_LOGIN; goto ERR;
	}

	goto NEXT;
ERR:
	responseMatch.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseMatch, ProtoGateServer::RESPONSE_MSG::MATCH);

	//
	// 4. ����
	//
	SendTo(pContext, buffer, writeBuffer.GetActiveBufferSize());

	return;
NEXT:

	//
	// 5. ��ӵ���ƥ����Ҽ���
	//
	std::map<DWORD, PlayerStatus>::const_iterator itPlayer = m_evaluations[requestMatch.evaluation()].find(pContext->guid);
	if (itPlayer != m_evaluations[requestMatch.evaluation()].end()) return;

	m_evaluations[requestMatch.evaluation()][pContext->guid].pContext = pContext;
	m_evaluations[requestMatch.evaluation()][pContext->guid].timeout = 0.0f;
	m_evaluations[requestMatch.evaluation()][pContext->guid].minEvaluation = requestMatch.evaluation();
	m_evaluations[requestMatch.evaluation()][pContext->guid].maxEvaluation = requestMatch.evaluation();

	pContext->dwUserData = requestMatch.evaluation();
}

//
// ȡ��ƥ��
//
void CGateServer::OnCancelMatch(CIOContext *pContext, WORD size)
{
	ProtoGateClient::CancelMatch requestCancelMatch;
	ProtoGateServer::CancelMatch responseCancelMatch;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pContext->recvBuffer, &requestCancelMatch, size) == FALSE) {
		return;
	}

	//
	// 2. ȡ��ƥ��
	//
	PlayerEvaluationMap::iterator itPlayerMap = m_evaluations.find(pContext->dwUserData);
	if (itPlayerMap != m_evaluations.end()) {
		std::map<DWORD, PlayerStatus>::iterator itPlayer = itPlayerMap->second.find(pContext->guid);
		if (itPlayer != itPlayerMap->second.end()) itPlayerMap->second.erase(itPlayer);
		if (itPlayerMap->second.empty()) m_evaluations.erase(itPlayerMap);
	}

	pContext->dwUserData = 0xffffffff;

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseCancelMatch, ProtoGateServer::RESPONSE_MSG::CANCEL_MATCH);

	//
	// 4. ����
	//
	SendTo(pContext, buffer, writeBuffer.GetActiveBufferSize());
}

//
// ��Ϸ�������б�
//
void CGateServer::OnListGameServer(CIOContext *pContext, WORD size)
{
	ProtoGateClient::ListGameServer requestListGameServer;
	ProtoGateServer::ListGameServer responseListGameServer;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pContext->recvBuffer, &requestListGameServer, size) == FALSE) {
		return;
	}

	//
	// 2. �����Ϸ�������б�
	//
	ProtoGateServer::ERROR_CODE err = ProtoGateServer::ERROR_CODE::ERR_NONE;

	if (pContext->guid == 0xffffffff) {
		err = ProtoGateServer::ERROR_CODE::ERR_PLAYER_NOT_LOGIN; goto ERR;
	}

	for (GameServerMap::const_iterator itGameServer = m_servers.begin(); itGameServer != m_servers.end(); ++itGameServer) {
		if (ProtoGateServer::ListGameServer_GameServer *pGameServer = responseListGameServer.add_servers()) {
			pGameServer->set_ip(itGameServer->second.ip);
			pGameServer->set_port(itGameServer->second.port);
			pGameServer->set_curgames(itGameServer->second.curGames);
			pGameServer->set_maxgames(itGameServer->second.maxGames);
		}
	}

	goto NEXT;
ERR:
NEXT:
	responseListGameServer.set_err(err);

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseListGameServer, ProtoGateServer::RESPONSE_MSG::LIST_GAME_SERVER);

	//
	// 4. ����
	//
	SendTo(pContext, buffer, writeBuffer.GetActiveBufferSize());
}

//
// ����ָ�����
//
void CGateServer::OnSendToPlayer(CIOContext *pContext, WORD size)
{
	ProtoGateClient::SendToPlayer requestSendToPlayer;
	ProtoGateServer::SendToPlayer responseSendToPlayer;

	BYTE buffer[PACK_BUFFER_SIZE];
	CCacheBuffer writeBuffer(sizeof(buffer), buffer);

	//
	// 1. ������Ϣ
	//
	if (Parser(&pContext->recvBuffer, &requestSendToPlayer, size) == FALSE) {
		return;
	}

	//
	// 2. ת��Э��
	//
	if (pContext->guid == 0xffffffff) {
		return;
	}

	responseSendToPlayer.set_size(requestSendToPlayer.size());
	responseSendToPlayer.set_data(requestSendToPlayer.data().c_str(), requestSendToPlayer.size());

	//
	// 3. ���л���Ϣ
	//
	Serializer(&writeBuffer, &responseSendToPlayer, ProtoGateServer::RESPONSE_MSG::SEND_TO_PLAYER);

	//
	// 4. ����ָ�����
	//
	if (requestSendToPlayer.guids_size() > 0) {
		for (int index = 0; index < requestSendToPlayer.guids_size(); index++) {
			if (CIOContext *pContextSendTo = QueryContext(requestSendToPlayer.guids(index))) {
				if (pContext != pContextSendTo) {
					SendTo(pContextSendTo, buffer, writeBuffer.GetActiveBufferSize());
				}
			}
		}
	}
	else {
		for (PlayerMap::const_iterator itPlayer = m_players.begin(); itPlayer != m_players.end(); ++itPlayer) {
			if (pContext != itPlayer->second) {
				SendTo(itPlayer->second, buffer, writeBuffer.GetActiveBufferSize());
			}
		}
	}
}

//
// ��Ϸ������״̬
//
void CGateServer::OnGameServerStatus(CIOContext *pContext, WORD size)
{
	ProtoGameServer::ServerStatus requestServerStatus;

	//
	// 1. ������Ϣ
	//
	if (Parser(&pContext->recvBuffer, &requestServerStatus, size) == FALSE) {
		return;
	}

	//
	// 2. ������Ϸ�������б�
	//
	strcpy(m_servers[pContext].ip, requestServerStatus.ip().c_str());
	m_servers[pContext].port = requestServerStatus.port();
	m_servers[pContext].curGames = requestServerStatus.curgames();
	m_servers[pContext].maxGames = requestServerStatus.maxgames();

	m_servers[pContext].games.clear();
	for (int index = 0; index < requestServerStatus.games_size(); index++) {
		GameStatus gameStatus;
		gameStatus.bEmpty = requestServerStatus.games(index).empty() ? TRUE : FALSE;
		gameStatus.id = requestServerStatus.games(index).gameid();
		gameStatus.mode = requestServerStatus.games(index).mode();
		gameStatus.mapid = requestServerStatus.games(index).mapid();
		gameStatus.evaluation = requestServerStatus.games(index).evaluation();
		m_servers[pContext].games.push_back(gameStatus);
	}
}

//
// �����־
//
void WriteLog(const char *szFmt, ...)
{
	va_list argList;
	static char szString[1024 * 1024];

	va_start(argList, szFmt);
	vsprintf(szString, szFmt, argList);

	time_t timeRaw;
	struct tm *timeInfo;
	time(&timeRaw);
	timeInfo = localtime(&timeRaw);

	char szFileName[260];
	sprintf(szFileName, "GateServer%d-%02d-%02d.log", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);

	if (FILE *pFile = fopen(szFileName, "a+")) {
		fprintf(pFile, "%02d:%02d:%02d: %s", timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, szString);
		fclose(pFile);
	}

	printf("%s", szString);
}
