#include "iocp.h"


//////////////////////////////////////////////////////////////////////////
// IO������
//========================================================================
CIOContext::CIOContext(void)
	: dwHeartTime(0x00000000)

	, id(0xffffffff)
	, guid(0xffffffff)

	, bInUsed(FALSE)
	, acceptSocket(INVALID_SOCKET)

	, recvBuffer(RECV_BUFFER_SIZE)
	, sendBuffer(SEND_BUFFER_SIZE)

	, pNext(NULL)
	, pNextActive(NULL)
	, pPrevActive(NULL)
{
	ClearBuffer();
}

CIOContext::~CIOContext(void)
{
	OnDisconnect();
}

//
// ����SOCKET
//
void CIOContext::Accept(SOCKET sock)
{
	SOCKADDR_IN addr;
	int size = sizeof(addr);
	memset(&addr, 0, sizeof(addr));

	acceptSocket = sock;
	getpeername(acceptSocket, (SOCKADDR *)&addr, &size);
	strcpy(ip, inet_ntoa(addr.sin_addr));

	int on = 1;
	int off = 0;
	setsockopt(acceptSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
}

//
// ��ջ���
//
void CIOContext::ClearBuffer(void)
{
	memset(ip, 0, sizeof(ip));

	memset(&wsaRecvBuffer, 0, sizeof(wsaRecvBuffer));
	memset(&wsaSendBuffer, 0, sizeof(wsaSendBuffer));

	wsaRecvBuffer.pIOContext = this;
	wsaRecvBuffer.operationType = NONE_POSTED << 16;

	wsaSendBuffer.pIOContext = this;
	wsaSendBuffer.operationType = NONE_POSTED << 16;

	recvBuffer.ClearBuffer();
	sendBuffer.ClearBuffer();
}

//
// ��ж�
//
BOOL CIOContext::IsAlive(void)
{
	return acceptSocket != INVALID_SOCKET ? TRUE : FALSE;
}

//
// ����
//
BOOL CIOContext::Send(BYTE *pBuffer, DWORD size, DWORD dwType)
{
	//
	// 1. �������ݴ�С���
	//
	if (size == 0 || size > sizeof(wsaSendBuffer.dataBuffer)) {
		return FALSE;
	}

	//
	// 2. Ͷ�ݷ��Ͳ���
	//
	DWORD dwBytes = 0;
	DWORD dwFlags = 0;

	memcpy(wsaSendBuffer.dataBuffer, pBuffer, size);

	wsaSendBuffer.dwRequestSize = size;
	wsaSendBuffer.dwCompleteSize = 0;

	wsaSendBuffer.wsaBuffer.len = size;
	wsaSendBuffer.wsaBuffer.buf = (char *)wsaSendBuffer.dataBuffer;

	wsaSendBuffer.operationType = SEND_POSTED << 16 | dwType;

	WSASend(acceptSocket, &wsaSendBuffer.wsaBuffer, 1, &dwBytes, dwFlags, &wsaSendBuffer.overlapped, NULL);

	return TRUE;
}

//
// ����
//
BOOL CIOContext::Recv(DWORD size, DWORD dwType)
{
	//
	// 1. �������ݴ�С���
	//
	if (size == 0 || size > sizeof(wsaRecvBuffer.dataBuffer)) {
		return FALSE;
	}

	//
	// 2. Ͷ�ݽ��ղ���
	//
	DWORD dwBytes = 0;
	DWORD dwFlags = 0;

	wsaRecvBuffer.dwRequestSize = size;
	wsaRecvBuffer.dwCompleteSize = 0;

	wsaRecvBuffer.wsaBuffer.len = size;
	wsaRecvBuffer.wsaBuffer.buf = (char *)wsaRecvBuffer.dataBuffer;

	wsaRecvBuffer.operationType = RECV_POSTED << 16 | dwType;

	WSARecv(acceptSocket, &wsaRecvBuffer.wsaBuffer, 1, &dwBytes, &dwFlags, &wsaRecvBuffer.overlapped, NULL);

	return TRUE;
}

//
// ��ɻص�����
//
void CIOContext::OnComplete(WSA_BUFFER *pIOBuffer, DWORD dwTransferred)
{
	DWORD dwBytes = 0;
	DWORD dwFlags = 0;

	switch (pIOBuffer->operationType >> 16) {
	case RECV_POSTED:
		recvBuffer.Lock();
		{
			pIOBuffer->dwCompleteSize += dwTransferred;

			if (pIOBuffer->dwCompleteSize < pIOBuffer->dwRequestSize) {
				pIOBuffer->wsaBuffer.len = pIOBuffer->dwRequestSize - pIOBuffer->dwCompleteSize;
				pIOBuffer->wsaBuffer.buf = (char *)&pIOBuffer->dataBuffer[pIOBuffer->dwCompleteSize];
				WSARecv(acceptSocket, &pIOBuffer->wsaBuffer, 1, &dwBytes, &dwFlags, &pIOBuffer->overlapped, NULL);
			}
			else {
				OnRecvNext(pIOBuffer->dataBuffer, pIOBuffer->dwCompleteSize, pIOBuffer->operationType&0x0000ffff);
			}
		}
		recvBuffer.Unlock();

		break;
	case SEND_POSTED:
		sendBuffer.Lock();
		{
			pIOBuffer->dwCompleteSize += dwTransferred;

			if (pIOBuffer->dwCompleteSize < pIOBuffer->dwRequestSize) {
				pIOBuffer->wsaBuffer.len = pIOBuffer->dwRequestSize - pIOBuffer->dwCompleteSize;
				pIOBuffer->wsaBuffer.buf = (char *)&pIOBuffer->dataBuffer[pIOBuffer->dwCompleteSize];
				WSASend(acceptSocket, &pIOBuffer->wsaBuffer, 1, &dwBytes, dwFlags, &pIOBuffer->overlapped, NULL);
			}
			else {
				OnSendNext(pIOBuffer->dataBuffer, pIOBuffer->dwCompleteSize, pIOBuffer->operationType&0x0000ffff);
			}
		}
		sendBuffer.Unlock();

		break;
	}
}

//
// ����SOCKET�ص�����
//
void CIOContext::OnAccept(void)
{
	Recv(2, RECV_LEN);
}

//
// �Ͽ��ص�����
//
void CIOContext::OnDisconnect(void)
{
	if (acceptSocket != INVALID_SOCKET) {
		shutdown(acceptSocket, SD_BOTH);
		closesocket(acceptSocket);
		acceptSocket = INVALID_SOCKET;
	}
}

//
// ���ջص�����
//
void CIOContext::OnRecvNext(BYTE *pBuffer, DWORD size, DWORD dwType)
{
	switch (dwType) {
	case RECV_LEN:
		recvBuffer.PushData(pBuffer, size);
		Recv(*(WORD *)pBuffer, RECV_DATA);
		break;

	case RECV_DATA:
		recvBuffer.PushData(pBuffer, size);
		Recv(2, RECV_LEN);
		break;
	}
}

//
// ���ͻص�����
//
void CIOContext::OnSendNext(BYTE *pBuffer, DWORD size, DWORD dwType)
{
	DWORD dataSize;
	BYTE dataBuffer[PACK_BUFFER_SIZE];

	dataSize = sendBuffer.PopData(dataBuffer, sizeof(dataBuffer));
	Send(dataBuffer, dataSize);
}


//////////////////////////////////////////////////////////////////////////
// IOCP Server
//========================================================================
CIOCPServer::CIOCPServer(void)
	: m_listenSocket(INVALID_SOCKET)

	, m_hIOCP(NULL)
	, m_hListenThread(NULL)
	, m_hShutdownEvent(NULL)

	, m_curContexts(0)
	, m_maxContexts(0)
	, m_contexts(NULL)
	, m_pFreeContext(NULL)
	, m_pActiveContext(NULL)
{
	memset(m_hWorkThreads, 0, sizeof(m_hWorkThreads));

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	InitializeCriticalSectionAndSpinCount(&m_sectionIOContext, 4000);
}

CIOCPServer::~CIOCPServer(void)
{
	WSACleanup();
	DeleteCriticalSection(&m_sectionIOContext);
}

//
// ����IO������
//
BOOL CIOCPServer::AllocIOContexts(int maxContexts)
{
	//
	// 1. ����IO�����Ĵ洢
	//
	m_curContexts = 0;
	m_maxContexts = maxContexts;
	m_contexts = new CIOContext* [m_maxContexts];

	//
	// 2. ��������IO����������
	//
	for (int indexContext = 0; indexContext < m_maxContexts; indexContext++) {
		m_contexts[indexContext] = new CIOContext;
		m_contexts[indexContext]->id = indexContext;
	}

	for (int indexContext = 0; indexContext < m_maxContexts - 1; indexContext++) {
		m_contexts[indexContext]->pNext = m_contexts[indexContext + 1];
		m_contexts[indexContext + 1]->pNext = NULL;
	}

	m_pFreeContext = m_contexts[0];

	return TRUE;
}

//
// �ͷ�IO������
//
void CIOCPServer::FreeIOContexts(void)
{
	if (m_contexts) {
		for (int indexContext = 0; indexContext < m_maxContexts; indexContext++) {
			delete m_contexts[indexContext];
		}

		delete[] m_contexts;

		m_contexts = NULL;
		m_pFreeContext = NULL;
	}

	m_curContexts = 0;
	m_maxContexts = 0;
}

//
// ����IOCP
//
BOOL CIOCPServer::CreateIOCP(void)
{
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	return m_hIOCP != NULL ? TRUE : FALSE;
}

//
// ����IOCP
//
void CIOCPServer::DestroyIOCP(void)
{
	if (m_hIOCP) {
		CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
	}
}

//
// ���������߳�
//
BOOL CIOCPServer::CreateWorkThreads(void)
{
	//
	// 1. �����ر��¼�
	//
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hShutdownEvent == NULL) return FALSE;

	//
	// 2. ���������߳�
	//
	int numThreads = GetProcessors() * 2 + 1;

	for (int indexThread = 0; indexThread < min(numThreads, MAX_THREAD_COUNT); indexThread++) {
		m_hWorkThreads[indexThread] = CreateThread(0, 0, WorkThread, (LPVOID)this, 0, NULL);
		if (m_hWorkThreads[indexThread] == NULL) return FALSE;
	}

	//
	// 3. ���������߳�
	//
	m_hListenThread = CreateThread(0, 0, ListenThread, (LPVOID)this, 0, NULL);
	if (m_hListenThread == NULL) return FALSE;

	return TRUE;
}

//
// ���ٹ����߳�
//
void CIOCPServer::DestroyWorkThreads(void)
{
	//
	// 1. ���ùر��¼�
	//
	if (m_hShutdownEvent) {
		SetEvent(m_hShutdownEvent);
	}

	//
	// 2. �رռ����߳�
	//
	if (m_hListenThread) {
		WaitForSingleObject(m_hListenThread, INFINITE);
		CloseHandle(m_hListenThread);
		m_hListenThread = NULL;
	}

	//
	// 3. �رչ����߳̾��
	//
	int numThreads = GetProcessors() * 2 + 1;

	PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);
	WaitForMultipleObjects(min(numThreads, MAX_THREAD_COUNT), m_hWorkThreads, TRUE, INFINITE);

	for (int indexThread = 0; indexThread < min(numThreads, MAX_THREAD_COUNT); indexThread++) {
		if (m_hWorkThreads[indexThread]) {
			CloseHandle(m_hWorkThreads[indexThread]);
			m_hWorkThreads[indexThread] = NULL;
		}
	}

	//
	// 4. �ͷŹر��¼�
	//
	if (m_hShutdownEvent) {
		CloseHandle(m_hShutdownEvent);
		m_hShutdownEvent = NULL;
	}
}

//
// ����
//
BOOL CIOCPServer::Listen(const char *ip, int port)
{
	//
	// 1. ����SOCKET
	//
	SOCKADDR_IN serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ip);
	serverAddress.sin_port = htons(port);

	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET) return FALSE;

	//
	// 2. �󶨲���ʼ����
	//
	bind(m_listenSocket, (PSOCKADDR)&serverAddress, sizeof(serverAddress));
	listen(m_listenSocket, SOMAXCONN);

	//
	// 3. �����ַ
	//
	m_port = port;
	strcpy(m_ip, ip);

	return TRUE;
}

//
// �Ͽ�����
//
void CIOCPServer::Disconnect(void)
{
	if (m_listenSocket != INVALID_SOCKET) {
		shutdown(m_listenSocket, SD_BOTH);
		closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
	}
}

//
// ��ÿ���IOContext
//
CIOContext* CIOCPServer::GetIOContext(BOOL bLock /*= TRUE*/)
{
	CIOContext *pIOContext = NULL;

	if (bLock) EnterCriticalSection(&m_sectionIOContext);
	{
		if (m_pFreeContext) {
			//
			// 1. ���������
			//
			pIOContext = m_pFreeContext;

			pIOContext->bInUsed = TRUE;
			pIOContext->pPrevActive = NULL;
			pIOContext->pNextActive = m_pActiveContext;

			if (m_pActiveContext) {
				m_pActiveContext->pPrevActive = pIOContext;
			}

			m_pActiveContext = pIOContext;

			//
			// 2. ������������
			//
			m_pFreeContext = m_pFreeContext->pNext;

			//
			// 3. ��¼IO��������
			//
			m_curContexts++;
		}
	}
	if (bLock) LeaveCriticalSection(&m_sectionIOContext);

	return pIOContext;
}

CIOContext* CIOCPServer::GetIOContextByIndex(int index, BOOL bLock /*= TRUE*/)
{
	CIOContext *pIOContext = NULL;

	if (bLock) EnterCriticalSection(&m_sectionIOContext);
	{
		pIOContext = m_contexts[index];
	}
	if (bLock) LeaveCriticalSection(&m_sectionIOContext);

	return pIOContext;
}

//
// �ͷ�IOContext
//
void CIOCPServer::ReleaseIOContext(CIOContext *pIOContext, BOOL bLock /*= TRUE*/)
{
	if (bLock) EnterCriticalSection(&m_sectionIOContext);
	{
		if (pIOContext->bInUsed) {
			pIOContext->bInUsed = FALSE;

			//
			// 1. �Ͽ�����
			//
			OnDisconnect(pIOContext);

			//
			// 2. ������������
			//
			pIOContext->pNext = m_pFreeContext;
			m_pFreeContext = pIOContext;

			//
			// 3. ���������
			//
			if (pIOContext->pNextActive) {
				pIOContext->pNextActive->pPrevActive = pIOContext->pPrevActive;
			}

			if (pIOContext->pPrevActive) {
				pIOContext->pPrevActive->pNextActive = pIOContext->pNextActive;
			}

			if (m_pActiveContext == pIOContext) {
				m_pActiveContext =  pIOContext->pNextActive;
			}

			pIOContext->pNextActive = NULL;
			pIOContext->pPrevActive = NULL;

			//
			// 3. ��¼IO��������
			//
			m_curContexts--;
		}
	}
	if (bLock) LeaveCriticalSection(&m_sectionIOContext);
}

//
// ����
//
BOOL CIOCPServer::Start(const char *ip, int port, int maxContexts)
{
	if (!Listen(ip, port)) return FALSE;
	if (!AllocIOContexts(maxContexts)) return FALSE;
	if (!CreateIOCP()) return FALSE;
	if (!CreateWorkThreads()) return FALSE;

	return TRUE;
}

//
// ֹͣ
//
void CIOCPServer::Stop(void)
{
	Disconnect();
	DestroyWorkThreads();
	DestroyIOCP();
	FreeIOContexts();
}

//
// ����
//
void CIOCPServer::OnConnect(CIOContext *pIOContext, SOCKET acceptSocket)
{
	pIOContext->dwHeartTime = 0;
	pIOContext->ClearBuffer();
	pIOContext->Accept(acceptSocket);
	pIOContext->OnAccept();
}

//
// ����
//
void CIOCPServer::OnDisconnect(CIOContext *pIOContext)
{
	pIOContext->OnDisconnect();
}

//
// �����߳�
//
DWORD WINAPI CIOCPServer::ListenThread(LPVOID lpParam)
{
	if (CIOCPServer *pIOCPServer = (CIOCPServer *)lpParam) {
		while (WAIT_OBJECT_0 != WaitForSingleObject(pIOCPServer->m_hShutdownEvent, 0)) {
			SOCKET acceptSocket = WSAAccept(pIOCPServer->m_listenSocket, NULL, NULL, NULL, 0);

			if (acceptSocket != INVALID_SOCKET) {
				if (CIOContext *pIOContext = pIOCPServer->GetIOContext()) {
					CreateIoCompletionPort((HANDLE)acceptSocket, pIOCPServer->m_hIOCP, (ULONG_PTR)acceptSocket, 0);
					pIOCPServer->OnConnect(pIOContext, acceptSocket);
				}
				else {
					shutdown(acceptSocket, SD_BOTH);
					closesocket(acceptSocket);
				}
			}
		}
	}

	return 0L;
}

//
// IOCP�߳�
//
DWORD WINAPI CIOCPServer::WorkThread(LPVOID lpParam)
{
	if (CIOCPServer *pIOCPServer = (CIOCPServer *)lpParam) {
		while (WAIT_OBJECT_0 != WaitForSingleObject(pIOCPServer->m_hShutdownEvent, 0)) {
			BOOL rcode = FALSE;
			DWORD dwTransferred = 0;
			SOCKET acceptSocket = INVALID_SOCKET;
			OVERLAPPED *pOverlapped = NULL;
			CIOContext::WSA_BUFFER *pIOBuffer = NULL;

			//
			// 1. ��ѯ��ɶ˿�
			//
			rcode = GetQueuedCompletionStatus(pIOCPServer->m_hIOCP, &dwTransferred, (PULONG_PTR)&acceptSocket, (LPOVERLAPPED *)&pOverlapped, INFINITE);
			if (!pOverlapped) continue;

			//
			// 2. ���IO������
			//
			pIOBuffer = CONTAINING_RECORD(pOverlapped, CIOContext::WSA_BUFFER, overlapped);

			//
			// 3. ���IO����
			//
			if (rcode && dwTransferred) {
				pIOBuffer->pIOContext->OnComplete(pIOBuffer, dwTransferred);
				continue;
			}

			//
			// 4. �ͻ��˶���, ����IO������
			//
			pIOCPServer->ReleaseIOContext(pIOBuffer->pIOContext);
		}
	}

	return 0L;
}


//////////////////////////////////////////////////////////////////////////
// ���ܺ���
//========================================================================
//
// ��ô�������
//
int GetProcessors(void)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

//
// ʼ�յδ�
//
UINT tick(void)
{
	LARGE_INTEGER freq;
	LARGE_INTEGER count;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&count);

	return (UINT)(((double)count.QuadPart/freq.QuadPart)*1000000);
}

//
// ��������
//
int SendData(int s, char *buff, int n)
{
	int bw = 0;
	int bcount = 0;

	while (bcount < n) {
		if ((bw = send(s, buff, n - bcount, 0)) > 0) {
			bcount += bw;
			buff += bw;
		}
		else {
			return SOCKET_ERROR;
		}
	}

	return bcount;
}

//
// ��������
//
int RecvData(int s, char *buff, int n)
{
	int br = 0;
	int bcount = 0;

	while (bcount < n) {
		if ((br = recv(s, buff, n - bcount, 0)) > 0) {
			bcount += br;
			buff += br;
		}
		else {
			return SOCKET_ERROR;
		}
	}

	return bcount;
}
