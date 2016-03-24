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
// ��½
//
BOOL CGateServer::Login(CIOContext *pContext, DWORD guid)
{
	//
	// 1. ������ȫ���
	//
	if (guid == 0xffffffff) {
		return FALSE;
	}

	//
	// 2. ����
	//
	GUIDMAP::const_iterator itContext = m_guidmap.find(guid);
	if (itContext != m_guidmap.end()) return FALSE;

	//
	// 3. ��½
	//
	pContext->guid = guid;
	m_guidmap[guid] = pContext;

	return TRUE;
}

//
// ע��
//
BOOL CGateServer::Logout(CIOContext *pContext)
{
	//
	// 1. ����
	//
	GUIDMAP::const_iterator itContext = m_guidmap.find(pContext->guid);
	if (itContext == m_guidmap.end()) return FALSE;

	//
	// 2. ע��
	//
	pContext->guid = 0xffffffff;
	m_guidmap.erase(itContext);

	return TRUE;
}

//
// ��ѯ
//
CIOContext* CGateServer::QueryContext(DWORD guid)
{
	GUIDMAP::const_iterator itContext = m_guidmap.find(guid);
	return itContext != m_guidmap.end() ? itContext->second : NULL;
}

//
// ������Ϸ������
//
void CGateServer::ClearGameServer(CIOContext *pContext)
{
	GameServerMap::const_iterator itGameServer = m_gameServerMap.find(pContext);
	if (itGameServer != m_gameServerMap.end()) m_gameServerMap.erase(itGameServer);
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
