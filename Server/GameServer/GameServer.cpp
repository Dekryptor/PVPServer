#include "GameServer.h"


CGameServer::CGameServer(void)
	: m_curGames(0)
	, m_maxGames(0)
	, m_games(NULL)
	, m_pFreeGame(NULL)
	, m_pActiveGame(NULL)

	, m_timeOut(0)

	, m_hReportThread(NULL)
	, m_hUpdateThread(NULL)

	, m_dwUpdateCount(0)
	, m_dwUpdateTime(0)
	, m_dwUpdateTimeTotal(0)
	, m_dwRuntimeTotal(0)

	, m_dwRecvDataSize(0)
	, m_dwSendDataSize(0)
{
	m_nGateServerPort = 0;
	memset(m_szGateServerIP, 0, sizeof(m_szGateServerIP));
}

CGameServer::~CGameServer(void)
{

}

//
// ����������
//
BOOL CGameServer::Start(const char *ip, int port, int maxGames, int maxPlayers, int timeOut, const char *gateip, int gateport)
{
	//
	// 1. ���泬ʱ����
	//
	m_timeOut = max(5, timeOut);

	//
	// 2. ������ڷ�������ַ
	//
	m_nGateServerPort = gateport;
	strcpy(m_szGateServerIP, gateip);

	//
	// 3. ����������
	//
	if (AllocGames(maxGames) == FALSE) goto ERR;
	if (AllocPlayers(maxGames * maxPlayers) == FALSE) goto ERR;
	if (Listen(ip, port) == FALSE) goto ERR;
	if (CreateIOCP() == FALSE) goto ERR;
	if (CreateShutdownEvent() == FALSE) goto ERR;
	if (CreateListenThread() == FALSE) goto ERR;
	if (CreateTransferThreads() == FALSE) goto ERR;
	if (CreateReportThread() == FALSE) goto ERR;
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
void CGameServer::Stop(void)
{
	m_timeOut = 0;

	m_nGateServerPort = 0;
	memset(m_szGateServerIP, 0, sizeof(m_szGateServerIP));

	SetEvent(m_hShutdownEvent);

	Disconnect();
	DestroyIOCP();
	DestroyListenThread();
	DestroyTransferThreads();
	DestroyReportThread();
	DestroyUpdateThread();
	DestroyShutdownEvent();
	FreeGames();
	FreePlayers();
}

//
// ������Ϸ
//
BOOL CGameServer::AllocGames(int maxGames)
{
	//
	// 1. ������Ϸ�洢
	//
	m_curGames = 0;
	m_maxGames = maxGames;
	m_games = new CGame*[m_maxGames];

	//
	// 2. ������Ϸ����
	//
	for (int indexGame = 0; indexGame < m_maxGames; indexGame++) {
		m_games[indexGame] = new CGame(this);
		m_games[indexGame]->id = indexGame;
	}

	for (int indexGame = 0; indexGame < m_maxGames - 1; indexGame++) {
		m_games[indexGame]->pNext = m_games[indexGame + 1];
		m_games[indexGame + 1]->pNext = NULL;
	}

	m_pFreeGame = m_games[0];
	m_pActiveGame = NULL;

	return TRUE;
}

//
// �������
//
BOOL CGameServer::AllocPlayers(int maxPlayers)
{
	//
	// 1. ���������Ĵ洢
	//
	m_curContexts = 0;
	m_maxContexts = maxPlayers;
	m_contexts = new CIOContext*[m_maxContexts];

	//
	// 2. �����������
	//
	for (int indexContext = 0; indexContext < m_maxContexts; indexContext++) {
		m_contexts[indexContext] = new CPlayer(this);
		m_contexts[indexContext]->id = indexContext;
	}

	for (int indexContext = 0; indexContext < m_maxContexts - 1; indexContext++) {
		m_contexts[indexContext]->pNext = m_contexts[indexContext + 1];
		m_contexts[indexContext + 1]->pNext = NULL;
	}

	m_pFreeContext = m_contexts[0];
	m_pActiveContext = NULL;

	return TRUE;
}

//
// �����㱨�߳�
//
BOOL CGameServer::CreateReportThread(void)
{
	m_hReportThread = CreateThread(0, 0, ReportThread, (LPVOID)this, 0, NULL);
	return m_hReportThread != NULL ? TRUE : FALSE;
}

//
// ���������߳�
//
BOOL CGameServer::CreateUpdateThread(void)
{
	m_hUpdateThread = CreateThread(0, 0, UpdateThread, (LPVOID)this, 0, NULL);
	return m_hUpdateThread != NULL ? TRUE : FALSE;
}

//
// �ͷ���Ϸ
//
void CGameServer::FreeGames(void)
{
	if (m_games) {
		for (int indexGame = 0; indexGame < m_maxGames; indexGame++) {
			delete m_games[indexGame];
		}

		delete[] m_games;
	}

	m_curGames = 0;
	m_maxGames = 0;

	m_games = NULL;
	m_pFreeGame = NULL;
	m_pActiveGame = NULL;
}

//
// �ͷ����
//
void CGameServer::FreePlayers(void)
{
	if (m_contexts) {
		for (int indexContext = 0; indexContext < m_maxContexts; indexContext++) {
			delete m_contexts[indexContext];
		}

		delete[] m_contexts;
	}

	m_curContexts = 0;
	m_maxContexts = 0;

	m_contexts = NULL;
	m_pFreeContext = NULL;
	m_pActiveContext = NULL;
}

//
// ���ٻ㱨�߳�
//
void CGameServer::DestroyReportThread(void)
{
	if (m_hReportThread) {
		WaitForSingleObject(m_hReportThread, INFINITE);
		CloseHandle(m_hReportThread);
		m_hReportThread = NULL;
	}
}

//
// ���ٸ����߳�
//
void CGameServer::DestroyUpdateThread(void)
{
	if (m_hUpdateThread) {
		WaitForSingleObject(m_hUpdateThread, INFINITE);
		CloseHandle(m_hUpdateThread);
		m_hUpdateThread = NULL;
	}
}

//
// �����Ϸ
//
CGame* CGameServer::GetNextGame(void)
{
	CGame *pGame = NULL;

	if (m_pFreeGame) {
		//
		// 1. ���������
		//
		pGame = m_pFreeGame;

		pGame->pPrevActive = NULL;
		pGame->pNextActive = m_pActiveGame;

		if (m_pActiveGame) {
			m_pActiveGame->pPrevActive = pGame;
		}

		m_pActiveGame = pGame;

		//
		// 2. ������������
		//
		m_pFreeGame = m_pFreeGame->pNext;

		//
		// 3. ��¼��Ϸ��
		//
		m_curGames++;
	}

	return pGame;
}

//
// �ͷ���Ϸ
//
void CGameServer::ReleaseGame(CGame *pGame)
{
	//
	// 1. ������
	//
	pGame->Clear();

	//
	// 2. ������������
	//
	pGame->pNext = m_pFreeGame;
	m_pFreeGame = pGame;

	//
	// 3. ���������
	//
	if (pGame->pPrevActive) {
		pGame->pPrevActive->pNextActive = pGame->pNextActive;
	}

	if (pGame->pNextActive) {
		pGame->pNextActive->pPrevActive = pGame->pPrevActive;
	}

	if (m_pActiveGame == pGame) {
		m_pActiveGame = pGame->pNextActive;
	}

	//
	// 4. ��¼��Ϸ��
	//
	m_curGames--;
}

//
// ��ҵ�½
//
BOOL CGameServer::Login(CPlayer *pPlayer, DWORD guid)
{
	//
	// 1. ������ȫ���
	//
	if (guid == 0xffffffff) {
		return FALSE;
	}

	//
	// 2. ������
	//
	if (pPlayer->IsLogin()) {
		return FALSE;
	}

	//
	// 3. ����ע�����
	//
	GUIDMAP::const_iterator itPlayer = m_guidmap.find(guid);
	if (itPlayer != m_guidmap.end()) return FALSE;

	//
	// 4. ע�����
	//
	pPlayer->guid = guid;
	m_guidmap[guid] = pPlayer;

	//
	// 5. ������ұ�ʶ
	//
	pPlayer->SetFlags(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_LOGIN);

	return TRUE;
}

//
// ���ע��
//
BOOL CGameServer::Logout(CPlayer *pPlayer)
{
	//
	// 1. ������
	//
	if (pPlayer->IsLogin() == FALSE) {
		return FALSE;
	}

	//
	// 2. ����ע�����
	//
	GUIDMAP::const_iterator itPlayer = m_guidmap.find(pPlayer->guid);
	if (itPlayer == m_guidmap.end()) return FALSE;

	//
	// 3. ע�����
	//
	pPlayer->guid = 0xffffffff;
	m_guidmap.erase(itPlayer);

	//
	// 4. ������ұ�ʶ
	//
	pPlayer->SetFlags(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_NONE);

	return TRUE;
}

//
// ��ѯ���
//
CPlayer* CGameServer::QueryPlayer(DWORD guid)
{
	GUIDMAP::const_iterator itPlayer = m_guidmap.find(guid);
	return itPlayer != m_guidmap.end() ? itPlayer->second : NULL;
}

//
// ����ָ�����
//
void CGameServer::SendToPlayer(CPlayer *pPlayer, BYTE *pBuffer, size_t size)
{
	if (pPlayer && pPlayer->IsAlive() && pBuffer && size > 0) {
		pPlayer->sendBuffer.Lock();
		pPlayer->sendBuffer.PushData(pBuffer, size);
		pPlayer->sendBuffer.Unlock();

		m_dwSendDataSize += (DWORD)size;
	}
}

//
// �����������
//
void CGameServer::SendToPlayerAll(CGame *pGame, CPlayer *pIgnore, BYTE *pBuffer, size_t size, DWORD dwFilter)
{
	if (pGame) {
		if (CPlayer *pPlayer = pGame->pActivePlayer) {
			do {
				if (pPlayer != pIgnore) {
					if (pPlayer->GetFlags() & dwFilter) {
						SendToPlayer(pPlayer, pBuffer, size);
					}
				}
			} while (pPlayer = pPlayer->pNextPlayer);
		}
	}
}

//
// ����
//
void CGameServer::Host(CPlayer *pPlayer)
{
	if (pPlayer->pGame) {
		ProtoGameServer::Host responseHost;

		BYTE buffer[PACK_BUFFER_SIZE];
		CCacheBuffer writeBuffer(sizeof(buffer), buffer);

		//
		// 1. �������GUID
		//
		responseHost.set_guid(pPlayer->pGame->GetHostGUID());

		//
		// 2. ���л���Ϣ
		//
		Serializer(&writeBuffer, &responseHost, ProtoGameServer::RESPONSE_MSG::HOST);

		//
		// 3. �������
		//
		SendToPlayer(pPlayer, buffer, writeBuffer.GetActiveBufferSize());
	}
}

//
// ����
//
void CGameServer::Host(CGame *pGame)
{
	if (pGame) {
		ProtoGameServer::Host responseHost;

		BYTE buffer[PACK_BUFFER_SIZE];
		CCacheBuffer writeBuffer(sizeof(buffer), buffer);

		//
		// 1. �������GUID
		//
		responseHost.set_guid(pGame->GetHostGUID());

		//
		// 2. ���л���Ϣ
		//
		Serializer(&writeBuffer, &responseHost, ProtoGameServer::RESPONSE_MSG::HOST);

		//
		// 3. �������
		//
		SendToPlayerAll(pGame, NULL, buffer, writeBuffer.GetActiveBufferSize());
	}
}

//
// ���
//
void CGameServer::Monitor(void)
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
	printf("Games = %d/%d, Players = %d/%d\n", m_curGames, m_maxGames, m_curContexts, m_maxContexts);
}

//
// �����߳�
//
DWORD WINAPI CGameServer::UpdateThread(LPVOID lpParam)
{
	if (CGameServer *pServer = (CGameServer *)lpParam) {
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
				EnterCriticalSection(&pServer->m_sectionContext);
				{
					pServer->OnUpdateRecv(dwDeltaTime);

					// ��Ϸ����20FPS
					if (dwGameDeltaTime > 50) {
						pServer->OnUpdateGameLogic(dwGameDeltaTime / 1000.0f);
						dwGameDeltaTime = 0;
					}

					pServer->OnUpdateSend();
				}
				LeaveCriticalSection(&pServer->m_sectionContext);
				DWORD dwEnd = tick() / 1000;

				//
				// 2. ����ͳ����Ϣ
				//
				pServer->m_dwUpdateCount++;
				pServer->m_dwUpdateTimeTotal += dwEnd - dwBegin;
				pServer->m_dwUpdateTime = pServer->m_dwUpdateTimeTotal / pServer->m_dwUpdateCount;

				// �������1FPS
				if (dwReportDeltaTime > 1000) {
					EnterCriticalSection(&pServer->m_sectionContext);
					{
						pServer->Monitor();
					}
					LeaveCriticalSection(&pServer->m_sectionContext);

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
	sprintf(szFileName, "GameServer%d-%02d-%02d.log", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);

	if (FILE *pFile = fopen(szFileName, "a+")) {
		fprintf(pFile, "%02d:%02d:%02d: %s", timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, szString);
		fclose(pFile);
	}

	printf("%s", szString);
}
