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
	if (AllocPlayers(maxPlayers) == FALSE) return FALSE;
	if (Listen(ip, port) == FALSE) return FALSE;
	if (CreateIOCP() == FALSE) return FALSE;
	if (CreateWorkThreads() == FALSE) return FALSE;
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

	Disconnect();
	DestroyIOCP();
	DestroyWorkThreads();
	DestroyReportThread();
	DestroyUpdateThread();
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
		CloseHandle(m_hUpdateThread);
		m_hUpdateThread = NULL;
	}
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
