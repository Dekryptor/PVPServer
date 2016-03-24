#include "GateServer.h"


CGateServer::CGateServer(void)
	: m_timeOut(0)
	, m_hUpdateThread(NULL)

	, m_dwUpdateCount(0)
	, m_dwUpdateTime(0)
	, m_dwUpdateTimeTotal(0)
	, m_dwRuntimeTotal(0)

	, m_dwRecvDataSize(0)
	, m_dwSendDataSize(0)
{

}

CGateServer::~CGateServer(void)
{

}

//
// ����������
//
BOOL CGateServer::Start(const char *ip, int port, int maxContexts, int timeOut)
{
	//
	// 1. ���泬ʱ����
	//
	m_timeOut = max(5, timeOut);

	//
	// 2. ����������
	//
	if (AllocContexts(maxContexts) == FALSE) return FALSE;
	if (Listen(ip, port) == FALSE) goto ERR;
	if (CreateIOCP() == FALSE) goto ERR;
	if (CreateShutdownEvent() == FALSE) goto ERR;
	if (CreateListenThread() == FALSE) goto ERR;
	if (CreateTransferThreads() == FALSE) goto ERR;
	if (CreateUpdateThread() == FALSE) goto ERR;

	goto RET;
ERR:
	WriteLog("Start fail err = %d", WSAGetLastError());
	return FALSE;
RET:
	return TRUE;
}

//
// ֹͣ������
//
void CGateServer::Stop(void)
{
	m_timeOut = 0;

	SetEvent(m_hShutdownEvent);

	Disconnect();
	DestroyIOCP();
	DestroyListenThread();
	DestroyTransferThreads();
	DestroyUpdateThread();
	DestroyShutdownEvent();
	FreeContexts();
}

//
// ���������߳�
//
BOOL CGateServer::CreateUpdateThread(void)
{
	m_hUpdateThread = CreateThread(0, 0, UpdateThread, (LPVOID)this, 0, NULL);
	return m_hUpdateThread != NULL ? TRUE : FALSE;
}

//
// ���ٸ����߳�
//
void CGateServer::DestroyUpdateThread(void)
{
	if (m_hUpdateThread) {
		WaitForSingleObject(m_hUpdateThread, INFINITE);
		CloseHandle(m_hUpdateThread);
		m_hUpdateThread = NULL;
	}
}

//
// ����ָ���ͻ���
//
void CGateServer::SendTo(CIOContext *pContext, BYTE *pBuffer, size_t size)
{
	if (pContext && pContext->IsAlive() && pBuffer && size > 0) {
		pContext->sendBuffer.Lock();
		pContext->sendBuffer.PushData(pBuffer, size);
		pContext->sendBuffer.Unlock();

		m_dwSendDataSize += (DWORD)size;
	}
}

//
// �ͻ��˶����ص�
//
void CGateServer::OnDisconnect(CIOContext *pContext)
{
	GameServerMap::const_iterator itGameServer = m_gameServerMap.find(pContext);
	if (itGameServer != m_gameServerMap.end()) m_gameServerMap.erase(itGameServer);

	CIOCPServer::OnDisconnect(pContext);
}

//
// ���·�����Ϣ
//
void CGateServer::OnUpdateSend(void)
{
	if (CIOContext *pContext = m_pActiveContext) {
		do {
			if (pContext->IsAlive()) {
				pContext->sendBuffer.Lock();
				{
					pContext->OnSendNext(NULL, 0, 0);
				}
				pContext->sendBuffer.Unlock();
			}
		} while (pContext = pContext->pNextActive);
	}
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
					WORD msg = 0;
					WORD fullSize = *(WORD *)pContext->recvBuffer.GetPopPointer();
					WORD bodySize = fullSize - sizeof(msg);

					if (pContext->recvBuffer.GetActiveBufferSize() < sizeof(fullSize) + fullSize) break;
					if (pContext->recvBuffer.PopData((BYTE *)&fullSize, sizeof(fullSize)) == FALSE) break;
					if (pContext->recvBuffer.PopData((BYTE *)&msg, sizeof(msg)) == FALSE)  break;

					m_dwRecvDataSize += sizeof(fullSize) + fullSize;

					switch (msg) {
					case ProtoGateClient::REQUEST_MSG::HEART:
						OnHeart(pContext, bodySize);
						OnHeartReset(pContext);
						break;

					case ProtoGateClient::REQUEST_MSG::LIST_GAME_SERVER:
						OnListGameServer(pContext, bodySize);
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

		if (pContext->dwHeartTime > (DWORD)(1000 * m_timeOut)) {
			WriteLog("%s: Heart TimeOut\n", pContext->ip);
			ReleaseIOContext(pContext, FALSE);
		}

		pContext = pNextContext;
	}
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
// �����Ϸ�������б�
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
	for (GameServerMap::const_iterator itGameServer = m_gameServerMap.begin(); itGameServer != m_gameServerMap.end(); ++itGameServer) {
		if (ProtoGateServer::ListGameServer_GameServer *pGameServer = responseListGameServer.add_servers()) {
			pGameServer->set_ip(itGameServer->second.ip);
			pGameServer->set_port(itGameServer->second.port);
			pGameServer->set_maxgames(itGameServer->second.maxGames);
			pGameServer->set_curgames(itGameServer->second.curGames);
		}
	}

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
// �����Ϸ������״̬
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
	strcpy(m_gameServerMap[pContext].ip, requestServerStatus.ip().c_str());
	m_gameServerMap[pContext].port = requestServerStatus.port();
	m_gameServerMap[pContext].maxGames = requestServerStatus.maxgames();
	m_gameServerMap[pContext].curGames = requestServerStatus.curgames();
}

//
// ���
//
void CGateServer::Monitor(void)
{
	//
	// 1. ����Ļ
	//
	system("cls");

	//
	// 2. ��ʾ������״̬
	//
	printf("Recv Data = %dB (%2.2fMb/s)\n", m_dwRecvDataSize, 8.0f*m_dwRecvDataSize / 1024.0f / 1024.0f);
	printf("Send Data = %dB (%2.2fMb/s)\n", m_dwSendDataSize, 8.0f*m_dwSendDataSize / 1024.0f / 1024.0f);
	printf("Update Time = %dms\n", m_dwUpdateTime);
	printf("Run Time = %2.2d:%2.2d:%2.2d\n", m_dwRuntimeTotal / 3600, m_dwRuntimeTotal / 60 - (m_dwRuntimeTotal / 3600) * 60, m_dwRuntimeTotal - (m_dwRuntimeTotal / 60) * 60);
}

//
// �����߳�
//
DWORD WINAPI CGateServer::UpdateThread(LPVOID lpParam)
{
	if (CGateServer *pServer = (CGateServer *)lpParam) {
		while (WAIT_OBJECT_0 != WaitForSingleObject(pServer->m_hShutdownEvent, 0)) {
			DWORD dwLastTime = tick() / 1000;
			static DWORD dwDeltaTime = 0;
			static DWORD dwGameDeltaTime = 0;
			static DWORD dwReportDeltaTime = 0;
			{
				//
				// 1. ���·�����
				//
				DWORD dwBegin = tick() / 1000;
				EnterCriticalSection(&pServer->m_sectionIOContext);
				{
					pServer->OnUpdateRecv(dwDeltaTime);
					pServer->OnUpdateSend();
				}
				LeaveCriticalSection(&pServer->m_sectionIOContext);
				DWORD dwEnd = tick() / 1000;

				//
				// 2. ����ͳ����Ϣ
				//
				pServer->m_dwUpdateCount++;
				pServer->m_dwUpdateTimeTotal += dwEnd - dwBegin;
				pServer->m_dwUpdateTime = pServer->m_dwUpdateTimeTotal / pServer->m_dwUpdateCount;

				// �������1FPS
				if (dwReportDeltaTime > 1000) {
					EnterCriticalSection(&pServer->m_sectionIOContext);
					{
						pServer->Monitor();
					}
					LeaveCriticalSection(&pServer->m_sectionIOContext);

					pServer->m_dwRuntimeTotal++;
					pServer->m_dwUpdateCount = 0;
					pServer->m_dwUpdateTime = 0;
					pServer->m_dwUpdateTimeTotal = 0;
					pServer->m_dwRecvDataSize = 0;
					pServer->m_dwSendDataSize = 0;

					dwReportDeltaTime = 0;
				}

				//
				// 3. �ͷ�ʱ��Ƭ
				//
				Sleep(1);
			}
			dwDeltaTime = tick() / 1000 - dwLastTime;
			dwDeltaTime = dwDeltaTime < 1000 ? dwDeltaTime : 0;
			dwGameDeltaTime += dwDeltaTime;
			dwReportDeltaTime += dwDeltaTime;
		}
	}

	return 0L;
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
