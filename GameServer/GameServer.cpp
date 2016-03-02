#include "GameServer.h"


CGameServer::CGameServer(void)
	: m_maxGames(0)
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
	m_nRootServerPort = 0;
	memset(m_szRootServerIP, 0, sizeof(m_szRootServerIP));
}

CGameServer::~CGameServer(void)
{

}

//
// ����������
//
BOOL CGameServer::Start(const char *ip, int port, int maxGames, int maxPlayers, int timeOut, const char *rootip, int rootport)
{
	//
	// 1. ���泬ʱ����
	//
	m_timeOut = max(1000, timeOut);

	//
	// 2. ������ڷ�������ַ
	//
	m_nRootServerPort = rootport;
	strcpy(m_szRootServerIP, rootip);

	//
	// 3. ����������
	//
	if (AllocGames(maxGames) == FALSE) return FALSE;
	if (AllocPlayers(maxGames * maxPlayers) == FALSE) return FALSE;
	if (Listen(ip, port) == FALSE) return FALSE;
	if (CreateIOCP() == FALSE) return FALSE;
	if (CreateShutdownEvent() == FALSE) return FALSE;
	if (CreateWorkThreads() == FALSE) return FALSE;
	if (CreateListenThread() == FALSE) return FALSE;
	if (CreateReportThread() == FALSE) return FALSE;
	if (CreateUpdateThread() == FALSE) return FALSE;

	return TRUE;
}

//
// ֹͣ������
//
void CGameServer::Stop(void)
{
	m_timeOut = 0;

	m_nRootServerPort = 0;
	memset(m_szRootServerIP, 0, sizeof(m_szRootServerIP));

	SetEvent(m_hShutdownEvent);

	Disconnect();
	DestroyIOCP();
	DestroyWorkThreads();
	DestroyListenThread();
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
	m_maxGames = maxGames;
	m_games = new CGame*[m_maxGames];

	//
	// 2. ������Ϸ����
	//
	for (int indexGame = 0; indexGame < m_maxGames; indexGame++) {
		m_games[indexGame] = new CGame(this);
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
	// 1. ����IO�����Ĵ洢
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

		delete [] m_games;
	}

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

		delete [] m_contexts;
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
	}

	return pGame;
}

//
// �ͷ���Ϸ
//
void CGameServer::ReleaseGame(CGame *pGame)
{
	//
	// 1. ������������
	//
	pGame->pNext = m_pFreeGame;
	m_pFreeGame = pGame;

	//
	// 2. ���������
	//
	if (pGame->pPrevActive) {
		pGame->pPrevActive->pNextActive = pGame->pNextActive;
	}

	if (pGame->pNextActive) {
		pGame->pNextActive->pPrevActive = pGame->pPrevActive;
	}

	if (m_pActiveGame == pGame) {
		m_pActiveGame =  pGame->pNextActive;
	}
}

//
// ��ѯ���
//
CPlayer* CGameServer::QueryPlayer(DWORD guid)
{
	GUIDMAP::const_iterator itPlayer = m_guidmap.find(guid);
	return itPlayer != m_guidmap.end() ? (CPlayer *)GetIOContextByIndex(itPlayer->second) : NULL;
}

//
// ע�����
//
BOOL CGameServer::RegisterPlayer(CPlayer *pPlayer, DWORD guid)
{
	//
	// 1. �������Ƿ���ע��
	//
	GUIDMAP::const_iterator itPlayer = m_guidmap.find(guid);
	if (itPlayer != m_guidmap.end()) return FALSE;

	//
	// 2. ע�����
	//
	pPlayer->guid = guid;
	m_guidmap[pPlayer->guid] = pPlayer->id;

	return TRUE;
}

//
// ע�����
//
BOOL CGameServer::UnRegisterPlayer(CPlayer *pPlayer)
{
	//
	// 1. �������Ƿ���ע��
	//
	GUIDMAP::const_iterator itPlayer = m_guidmap.find(pPlayer->guid);
	if (itPlayer == m_guidmap.end()) return FALSE;

	//
	// 2. ע�����
	//
	pPlayer->guid = 0xffffffff;
	m_guidmap.erase(itPlayer);

	return TRUE;
}

//
// ����ָ�����
//
void CGameServer::SendToPlayer(CPlayer *pPlayer, BYTE *pBuffer, DWORD size)
{
	if (pPlayer && pBuffer && size > 0) {
		pPlayer->sendBuffer.Lock();
		pPlayer->sendBuffer.PushData(pBuffer, size);
		pPlayer->sendBuffer.Unlock();

		m_dwSendDataSize += size;
	}
}

//
// �����������
//
void CGameServer::SendToPlayerAll(CGame *pGame, CPlayer *pIgnore, BYTE *pBuffer, DWORD size, DWORD dwFilter)
{
	if (pGame) {
		if (CPlayer *pPlayer = pGame->pActivePlayer) {
			do {
				if (pPlayer != pIgnore) {
					if (pPlayer->GetFlags() & dwFilter) {
						SendToPlayer(pPlayer, pBuffer, size);
					}
				}
			} while (pPlayer->pNextPlayer);
		}
	}
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
	if (CGameServer *pServer = (CGameServer *)lpParam) {
		while (WAIT_OBJECT_0 != WaitForSingleObject(pServer->m_hShutdownEvent, 0)) {
			Sleep(1000);
		}
	}

	return 0L;
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
				EnterCriticalSection(&pServer->m_sectionIOContext);
				{
					pServer->OnUpdateRecv(dwDeltaTime);

					// ��Ϸ����20FPS
					if (dwGameDeltaTime > 50) {
						pServer->OnUpdateGame(dwGameDeltaTime / 1000.0f);
						dwGameDeltaTime = 0;
					}

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
