#include "iocp.h"


//////////////////////////////////////////////////////////////////////////
// ������
//========================================================================
CIOContext::CIOContext(void)
	: dwHeartTime(0x00000000)

	, id(0xffffffff)
	, guid(0xffffffff)

	, bInUsed(FALSE)
	, acceptSocket(INVALID_SOCKET)

	, recvBuffer(RECV_BUFFER_SIZE)
	, sendBuffer(SEND_BUFFER_SIZE)

	, bIsRecvBufferOverflow(FALSE)
	, bIsSendBufferOverflow(FALSE)

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
// ��ж�
//
BOOL CIOContext::IsAlive(void)
{
	return acceptSocket != INVALID_SOCKET ? TRUE : FALSE;
}

//
// ��ջ���
//
void CIOContext::ClearBuffer(void)
{
	memset(ip, 0, sizeof(ip));

	memset(&wsaRecvBuffer, 0, sizeof(wsaRecvBuffer));
	memset(&wsaSendBuffer, 0, sizeof(wsaSendBuffer));

	wsaRecvBuffer.pContext = this;
	wsaSendBuffer.pContext = this;

	bIsRecvBufferOverflow = FALSE;
	bIsSendBufferOverflow = FALSE;

	recvBuffer.ClearBuffer();
	sendBuffer.ClearBuffer();
}

//
// ��������
//
void CIOContext::Send(BYTE *pBuffer, DWORD size)
{
	if (bIsSendBufferOverflow == FALSE) {
		sendBuffer.Lock();
		{
			if (sendBuffer.PushData(pBuffer, size) != size) {
				bIsSendBufferOverflow = TRUE;
			}

			if (bIsSendBufferOverflow == FALSE) {
				if (wsaSendBuffer.dwCompleteSize == wsaSendBuffer.dwRequestSize) {
					OnSendNext();
				}
			}
		}
		sendBuffer.Unlock();
	}
}

//
// ����SOCKET�ص�����
//
void CIOContext::OnAccept(SOCKET sock)
{
	//
	// 1. �����ַ
	//
	SOCKADDR_IN addr;
	int size = sizeof(addr);
	memset(&addr, 0, sizeof(addr));

	acceptSocket = sock;
	getpeername(acceptSocket, (SOCKADDR *)&addr, &size);
	strcpy(ip, inet_ntoa(addr.sin_addr));

	//
	// 2. ����Socket
	//
	int on = 1;
	int off = 0;
	setsockopt(acceptSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));

	//
	// 3. ��������
	//
	WSARecv(2, RECV_LEN);
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
				::WSARecv(acceptSocket, &pIOBuffer->wsaBuffer, 1, &dwBytes, &dwFlags, &pIOBuffer->overlapped, NULL);
			}
			else {
				OnRecvNext(pIOBuffer->dataBuffer, pIOBuffer->dwCompleteSize, pIOBuffer->operationType & 0x0000ffff);
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
				::WSASend(acceptSocket, &pIOBuffer->wsaBuffer, 1, &dwBytes, dwFlags, &pIOBuffer->overlapped, NULL);
			}
			else {
				OnSendNext();
			}
		}
		sendBuffer.Unlock();
		break;
	}
}

//
// ���ջص�����
//
void CIOContext::OnRecvNext(BYTE *pBuffer, DWORD size, DWORD dwType)
{
	if (bIsRecvBufferOverflow == FALSE) {
		if (recvBuffer.PushData(pBuffer, size) != size) {
			bIsRecvBufferOverflow = TRUE;
		}

		if (bIsRecvBufferOverflow == FALSE) {
			switch (dwType) {
			case RECV_LEN:
				WSARecv(*(WORD *)pBuffer, RECV_DATA);
				break;

			case RECV_DATA:
				WSARecv(2, RECV_LEN);
				break;
			}
		}
	}
}

//
// ���ͻص�����
//
void CIOContext::OnSendNext(void)
{
	DWORD size;
	BYTE buffer[PACK_BUFFER_SIZE];

	size = sendBuffer.PopData(buffer, min(sizeof(buffer), sendBuffer.GetActiveBufferSize()));
	WSASend(buffer, size);
}

//
// ����
//
BOOL CIOContext::WSARecv(DWORD size, DWORD dwType)
{
	//
	// 1. �������ݴ�С���
	//
	if (size == 0) {
		return FALSE;
	}

	if (size > sizeof(wsaRecvBuffer.dataBuffer)) {
		bIsRecvBufferOverflow = TRUE;
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

	::WSARecv(acceptSocket, &wsaRecvBuffer.wsaBuffer, 1, &dwBytes, &dwFlags, &wsaRecvBuffer.overlapped, NULL);

	return TRUE;
}

//
// ����
//
BOOL CIOContext::WSASend(BYTE *pBuffer, DWORD size, DWORD dwType)
{
	//
	// 1. �������ݴ�С���
	//
	if (size == 0) {
		return FALSE;
	}

	if (size > sizeof(wsaSendBuffer.dataBuffer)) {
		bIsSendBufferOverflow = TRUE;
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

	::WSASend(acceptSocket, &wsaSendBuffer.wsaBuffer, 1, &dwBytes, dwFlags, &wsaSendBuffer.overlapped, NULL);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// IOCP Server
//========================================================================
CIOCPServer::CIOCPServer(void)
	: m_timeOut(5)
	, m_listenSocket(INVALID_SOCKET)

	, m_hIOCP(NULL)
	, m_hListenThread(NULL)
	, m_hShutdownEvent(NULL)

	, m_curContexts(0)
	, m_maxContexts(0)
	, m_contexts(NULL)
	, m_pFreeContext(NULL)
	, m_pActiveContext(NULL)
{
	m_port = 0;
	memset(m_ip, 0, sizeof(m_ip));
	memset(m_hTransferThreads, 0, sizeof(m_hTransferThreads));

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	InitializeCriticalSectionAndSpinCount(&m_sectionContext, 4000);
}

CIOCPServer::~CIOCPServer(void)
{
	WSACleanup();
	DeleteCriticalSection(&m_sectionContext);
}

//
// ����
//
BOOL CIOCPServer::Start(const char *ip, int port, int maxContexts, int timeOut)
{
	m_timeOut = max(5, timeOut);

	if (AllocContexts(maxContexts) == FALSE) return FALSE;
	if (Listen(ip, port) == FALSE) return FALSE;
	if (CreateIOCP() == FALSE) return FALSE;
	if (CreateShutdownEvent() == FALSE) return FALSE;
	if (CreateListenThread() == FALSE) return FALSE;
	if (CreateTransferThreads() == FALSE) return FALSE;

	return TRUE;
}

//
// ֹͣ
//
void CIOCPServer::Stop(void)
{
	SetEvent(m_hShutdownEvent);

	Disconnect();
	DestroyIOCP();
	DestroyListenThread();
	DestroyTransferThreads();
	DestroyShutdownEvent();
	FreeContexts();
}

//
// ����������
//
BOOL CIOCPServer::AllocContexts(int maxContexts)
{
	//
	// 1. ���������Ĵ洢
	//
	m_curContexts = 0;
	m_maxContexts = maxContexts;
	m_contexts = new CIOContext*[m_maxContexts];

	//
	// 2. ������������������
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
	m_pActiveContext = NULL;

	return TRUE;
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
// ���������߳�
//
BOOL CIOCPServer::CreateListenThread(void)
{
	m_hListenThread = CreateThread(0, 0, ListenThread, (LPVOID)this, 0, NULL);
	return m_hListenThread != NULL ? TRUE : FALSE;
}

//
// ���������߳�
//
BOOL CIOCPServer::CreateTransferThreads(void)
{
	int numThreads = GetProcessors() * 2 + 1;

	for (int indexThread = 0; indexThread < min(numThreads, MAX_THREAD_COUNT); indexThread++) {
		m_hTransferThreads[indexThread] = CreateThread(0, 0, TransferThread, (LPVOID)this, 0, NULL);
		if (m_hTransferThreads[indexThread] == NULL) return FALSE;
	}

	return TRUE;
}

//
// �����ر��¼�
//
BOOL CIOCPServer::CreateShutdownEvent(void)
{
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	return m_hShutdownEvent != NULL ? TRUE : FALSE;
}

//
// �ͷ�������
//
void CIOCPServer::FreeContexts(void)
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
void CIOCPServer::DestroyListenThread(void)
{
	if (m_hListenThread) {
		WaitForSingleObject(m_hListenThread, INFINITE);
		CloseHandle(m_hListenThread);
		m_hListenThread = NULL;
	}
}

//
// ���ٴ����߳�
//
void CIOCPServer::DestroyTransferThreads(void)
{
	int numThreads = GetProcessors() * 2 + 1;

	PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);
	WaitForMultipleObjects(min(numThreads, MAX_THREAD_COUNT), m_hTransferThreads, TRUE, INFINITE);

	for (int indexThread = 0; indexThread < min(numThreads, MAX_THREAD_COUNT); indexThread++) {
		if (m_hTransferThreads[indexThread]) {
			CloseHandle(m_hTransferThreads[indexThread]);
			m_hTransferThreads[indexThread] = NULL;
		}
	}
}

//
// ���ٹر��¼�
//
void CIOCPServer::DestroyShutdownEvent(void)
{
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
	if (WSAGetLastError() != NO_ERROR) return FALSE;

	listen(m_listenSocket, SOMAXCONN);
	if (WSAGetLastError() != NO_ERROR) return FALSE;

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
// ��ÿ���������
//
CIOContext* CIOCPServer::GetNextContext(BOOL bLock)
{
	CIOContext *pContext = NULL;

	if (bLock) EnterCriticalSection(&m_sectionContext);
	{
		if (m_pFreeContext) {
			//
			// 1. ���������
			//
			pContext = m_pFreeContext;

			pContext->bInUsed = TRUE;
			pContext->pPrevActive = NULL;
			pContext->pNextActive = m_pActiveContext;

			if (m_pActiveContext) {
				m_pActiveContext->pPrevActive = pContext;
			}

			m_pActiveContext = pContext;

			//
			// 2. ������������
			//
			m_pFreeContext = m_pFreeContext->pNext;

			//
			// 3. ��¼��������
			//
			m_curContexts++;
		}
	}
	if (bLock) LeaveCriticalSection(&m_sectionContext);

	return pContext;
}

//
// �ͷ�������
//
void CIOCPServer::ReleaseContext(CIOContext *pContext, BOOL bLock)
{
	if (bLock) EnterCriticalSection(&m_sectionContext);
	{
		if (pContext->bInUsed) {
			pContext->bInUsed = FALSE;

			//
			// 1. �Ͽ�����
			//
			OnDisconnect(pContext);

			//
			// 2. ������������
			//
			pContext->pNext = m_pFreeContext;
			m_pFreeContext = pContext;

			//
			// 3. ���������
			//
			if (pContext->pPrevActive) {
				pContext->pPrevActive->pNextActive = pContext->pNextActive;
			}

			if (pContext->pNextActive) {
				pContext->pNextActive->pPrevActive = pContext->pPrevActive;
			}

			if (m_pActiveContext == pContext) {
				m_pActiveContext = pContext->pNextActive;
			}

			pContext->pNextActive = NULL;
			pContext->pPrevActive = NULL;

			//
			// 3. ��¼��������
			//
			m_curContexts--;
		}
	}
	if (bLock) LeaveCriticalSection(&m_sectionContext);
}

//
// ����
//
void CIOCPServer::OnConnect(CIOContext *pContext, SOCKET acceptSocket)
{
	pContext->dwHeartTime = 0;
	pContext->ClearBuffer();
	pContext->OnAccept(acceptSocket);
}

//
// ����
//
void CIOCPServer::OnDisconnect(CIOContext *pContext)
{
	pContext->OnDisconnect();
}

//
// �����߳�
//
DWORD WINAPI CIOCPServer::ListenThread(LPVOID lpParam)
{
	if (CIOCPServer *pIOCPServer = (CIOCPServer *)lpParam) {
		while (WAIT_OBJECT_0 != WaitForSingleObject(pIOCPServer->m_hShutdownEvent, 0)) {
			SOCKET acceptSocket = WSAAccept(pIOCPServer->m_listenSocket, NULL, NULL, NULL, 0);
			if (acceptSocket == INVALID_SOCKET) break;

			if (CIOContext *pContext = pIOCPServer->GetNextContext(TRUE)) {
				CreateIoCompletionPort((HANDLE)acceptSocket, pIOCPServer->m_hIOCP, (ULONG_PTR)acceptSocket, 0);
				pIOCPServer->OnConnect(pContext, acceptSocket);
			}
			else {
				shutdown(acceptSocket, SD_BOTH);
				closesocket(acceptSocket);
			}
		}
	}

	return 0L;
}

//
// �����߳�
//
DWORD WINAPI CIOCPServer::TransferThread(LPVOID lpParam)
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
			if (pOverlapped == NULL) continue;

			//
			// 2. ���������
			//
			pIOBuffer = CONTAINING_RECORD(pOverlapped, CIOContext::WSA_BUFFER, overlapped);

			//
			// 3. ��ɲ���
			//
			if (rcode && dwTransferred) {
				pIOBuffer->pContext->OnComplete(pIOBuffer, dwTransferred);
				continue;
			}

			//
			// 4. �ͻ��˶���, ����������
			//
			pIOCPServer->ReleaseContext(pIOBuffer->pContext, TRUE);
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

	return (UINT)(((double)count.QuadPart / freq.QuadPart) * 1000000);
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
