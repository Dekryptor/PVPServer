#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#include "pthread.h"
#include "semaphore.h"
#include "Common.h"
#include "CacheBuffer.h"


class CIOContext
{
	friend class CIOCPServer;


	// ���ݽṹ
public:
	enum {
		RECV_LEN = 0,                                                                              // ���ճ���
		RECV_DATA                                                                                  // ��������
	};

	typedef enum {
		NONE_POSTED = 0,                                                                           // ��Ͷ�ݲ���
		SEND_POSTED,                                                                               // Ͷ�ݷ��Ͳ���
		RECV_POSTED,                                                                               // Ͷ�ݽ��ղ���
	} OPERATION_TYPE;

	typedef struct {
		OVERLAPPED overlapped;                                                                     // �ص��ṹ

		WSABUF wsaBuffer;                                                                          // WSA����
		BYTE dataBuffer[PACK_BUFFER_SIZE];                                                         // WSAָ������ݻ���

		DWORD dwRequestSize;                                                                       // �������ݴ�С
		DWORD dwCompleteSize;                                                                      // ������ݴ�С

		DWORD operationType;                                                                       // ��������
		CIOContext *pContext;                                                                      // ������
	} WSA_BUFFER;


	// ����/��������
public:
	CIOContext(void);
	virtual ~CIOContext(void);


	// ����
public:
	virtual BOOL IsAlive(void);                                                                    // ��ж�
	virtual void ClearBuffer(void);                                                                // ��ջ���
	virtual void Send(BYTE *pBuffer, DWORD size);                                                  // ��������
	virtual BOOL Check(DWORD dwTimeOut);                                                           // ���

public:
	virtual void OnAccept(SOCKET sock);                                                            // ����SOCKET�ص�����
	virtual void OnDisconnect(void);                                                               // �Ͽ��ص�����
	virtual void OnComplete(WSA_BUFFER *pIOBuffer, DWORD dwTransferred);                           // ��ɻص�����

protected:
	virtual void OnRecvNext(BYTE *pBuffer, DWORD size, DWORD dwType);                              // ���ջص�����
	virtual void OnSendNext(void);                                                                 // ���ͻص�����

protected:
	virtual BOOL WSARecv(DWORD size, DWORD dwType = 0);                                            // ����
	virtual BOOL WSASend(BYTE *pBuffer, DWORD size, DWORD dwType = 0);                             // ����


	// ����
public:
	char ip[256];                                                                                  // �ͻ���ip��ַ
	DWORD dwHeartTime;                                                                             // ����ʱ��

public:
	DWORD id;                                                                                      // id
	DWORD guid;                                                                                    // guid

	BOOL bInUsed;                                                                                  // ʹ����

	CCacheBuffer recvBuffer;                                                                       // ���ջ���
	CCacheBuffer sendBuffer;                                                                       // ���ͻ���

protected:
	SOCKET acceptSocket;                                                                           // SOCKET

	WSA_BUFFER wsaRecvBuffer;                                                                      // ���ջ���
	WSA_BUFFER wsaSendBuffer;                                                                      // ���ͻ���

protected:
	BOOL bIsRecvBufferOverflow;                                                                    // ���ջ������
	BOOL bIsSendBufferOverflow;                                                                    // ���ͻ������

public:
	CIOContext *pNext;                                                                             // ��һ��������
	CIOContext *pNextActive;                                                                       // ��һ��������
	CIOContext *pPrevActive;                                                                       // ǰһ��������
};


class CIOCPServer
{
	static const int MAX_THREAD_COUNT = 128;


	// ����/��������
public:
	CIOCPServer(void);
	virtual ~CIOCPServer(void);


	// ����
public:
	virtual BOOL Start(const char *ip, int port, int maxContexts, int timeOut);                    // ����������
	virtual void Stop(void);                                                                       // ֹͣ������

protected:
	virtual BOOL AllocContexts(int maxContexts);                                                   // ����������
	virtual BOOL CreateIOCP(void);                                                                 // ������ɶ˿�
	virtual BOOL CreateListenThread(void);                                                         // ���������߳�
	virtual BOOL CreateTransferThreads(void);                                                      // ���������߳�
	virtual BOOL CreateShutdownEvent(void);                                                        // �����ر��¼�

	virtual void FreeContexts(void);                                                               // �ͷ�������
	virtual void DestroyIOCP(void);                                                                // ������ɶ˿�
	virtual void DestroyListenThread(void);                                                        // ���������߳�
	virtual void DestroyTransferThreads(void);                                                     // ���ٴ����߳�
	virtual void DestroyShutdownEvent(void);                                                       // ���ٹر��¼�

	virtual BOOL Listen(const char *ip, int port);                                                 // ����
	virtual void Disconnect(void);                                                                 // �Ͽ�����

protected:
	virtual CIOContext* GetNextContext(BOOL bLock);                                                // ��ÿ���������
	virtual void ReleaseContext(CIOContext *pContext, BOOL bLock);                                 // �ͷ�������

protected:
	virtual void OnConnect(CIOContext *pContext, SOCKET acceptSocket);                             // �ͻ������ӻص�
	virtual void OnDisconnect(CIOContext *pContext);                                               // �ͻ��˶����ص�

	virtual void OnUpdateSend(void);                                                               // ���·���

protected:
	static DWORD WINAPI ListenThread(LPVOID lpParam);                                              // �����߳�
	static DWORD WINAPI TransferThread(LPVOID lpParam);                                            // �����߳�


	// ����
protected:
	char m_ip[256];                                                                                // IP
	int m_port;                                                                                    // �˿�
	int m_timeOut;                                                                                 // ������ʱ

protected:
	SOCKET m_listenSocket;                                                                         // ����SOCKET

	HANDLE m_hIOCP;                                                                                // ��ɶ˿ھ��
	HANDLE m_hListenThread;                                                                        // �����߳̾��
	HANDLE m_hTransferThreads[MAX_THREAD_COUNT];                                                   // �����߳̾��

	HANDLE m_hShutdownEvent;                                                                       // �˳��¼�
	CRITICAL_SECTION m_sectionContext;                                                             // �������ٽ���

	int m_curContexts;                                                                             // ��ǰ��������
	int m_maxContexts;                                                                             // �����������
	CIOContext **m_contexts;                                                                       // �����ļ���
	CIOContext *m_pFreeContext;                                                                    // ����������
	CIOContext *m_pActiveContext;                                                                  // �������
};

extern int GetProcessors(void);                                                                    // ��ô�������
extern UINT tick(void);                                                                            // ʼ�յδ�

extern int SendData(int s, char *buff, int n);                                                     // ��������
extern int RecvData(int s, char *buff, int n);                                                     // ��������

extern void WriteLog(const char *szFmt, ...);                                                      // �����־
