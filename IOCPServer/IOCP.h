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
		CIOContext *pIOContext;                                                                    // IO������
	} WSA_BUFFER;


	// ����/��������
public:
	CIOContext(void);
	virtual ~CIOContext(void);


	// ����
public:
	virtual void Accept(SOCKET sock);                                                              // ����SOCKET
	virtual void ClearBuffer(void);                                                                // ��ջ���

	virtual BOOL IsAlive(void);                                                                    // ��ж�

	virtual BOOL Send(BYTE *pBuffer, DWORD size, DWORD dwType = 0);                                // ����
	virtual BOOL Recv(DWORD size, DWORD dwType = 0);                                               // ����

	virtual void OnComplete(WSA_BUFFER *pIOBuffer, DWORD dwTransferred);                           // ��ɻص�����
	virtual void OnAccept(void);                                                                   // ����SOCKET�ص�����
	virtual void OnDisconnect(void);                                                               // �Ͽ��ص�����
	virtual void OnRecvNext(BYTE *pBuffer, DWORD size, DWORD dwType);                              // ���ջص�����
	virtual void OnSendNext(BYTE *pBuffer, DWORD size, DWORD dwType);                              // ���ͻص�����


	// ����
public:
	char ip[16];                                                                                   // �ͻ���ip��ַ
	DWORD dwHeartTime;                                                                             // ����ʱ��

public:
	DWORD id;                                                                                      // id
	DWORD guid;                                                                                    // guid

	BOOL bInUsed;                                                                                  // ʹ����
	SOCKET acceptSocket;                                                                           // SOCKET

	WSA_BUFFER wsaRecvBuffer;                                                                      // ���ջ���
	WSA_BUFFER wsaSendBuffer;                                                                      // ���ͻ���

	CCacheBuffer recvBuffer;                                                                       // ���ջ���
	CCacheBuffer sendBuffer;                                                                       // ���ͻ���

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
	virtual BOOL Start(const char *ip, int port, int maxContexts = 1000);                          // ����������
	virtual void Stop(void);                                                                       // ֹͣ������

protected:
	virtual BOOL AllocContexts(int maxContexts);                                                   // ����IO������
	virtual BOOL CreateIOCP(void);                                                                 // ������ɶ˿�
	virtual BOOL CreateListenThread(void);                                                         // ���������߳�
	virtual BOOL CreateTransferThreads(void);                                                      // ���������߳�
	virtual BOOL CreateShutdownEvent(void);                                                        // �����ر��¼�

	virtual void FreeContexts(void);                                                               // �ͷ�IO������
	virtual void DestroyIOCP(void);                                                                // ������ɶ˿�
	virtual void DestroyListenThread(void);                                                        // ���������߳�
	virtual void DestroyTransferThreads(void);                                                     // ���ٴ����߳�
	virtual void DestroyShutdownEvent(void);                                                       // ���ٹر��¼�

	virtual BOOL Listen(const char *ip, int port);                                                 // ����
	virtual void Disconnect(void);                                                                 // �Ͽ�����

protected:
	virtual CIOContext* GetIOContext(BOOL bLock = TRUE);                                           // ���IO������
	virtual CIOContext* GetIOContextByIndex(int index, BOOL bLock = TRUE);                         // ���IO������
	virtual void ReleaseIOContext(CIOContext *pIOContext, BOOL bLock = TRUE);                      // �ͷ�IO������

protected:
	virtual void OnConnect(CIOContext *pIOContext, SOCKET acceptSocket);                           // �ͻ������ӻص�
	virtual void OnDisconnect(CIOContext *pIOContext);                                             // �ͻ��˶����ص�

protected:
	static DWORD WINAPI ListenThread(LPVOID lpParam);                                              // �����߳�
	static DWORD WINAPI TransferThread(LPVOID lpParam);                                            // �����߳�


	// ����
protected:
	int m_port;                                                                                    // �˿�
	char m_ip[16];                                                                                 // IP

protected:
	SOCKET m_listenSocket;                                                                         // ����SOCKET

	HANDLE m_hIOCP;                                                                                // ��ɶ˿ھ��
	HANDLE m_hListenThread;                                                                        // �����߳̾��
	HANDLE m_hTransferThreads[MAX_THREAD_COUNT];                                                   // �����߳̾��

	HANDLE m_hShutdownEvent;                                                                       // �˳��¼�
	CRITICAL_SECTION m_sectionIOContext;                                                           // IO�������ٽ���

	int m_curContexts;                                                                             // ��ǰIO��������
	int m_maxContexts;                                                                             // ���IO��������
	CIOContext **m_contexts;                                                                       // IO�����ļ���
	CIOContext *m_pFreeContext;                                                                    // ����IO������
	CIOContext *m_pActiveContext;                                                                  // �IO������
};

extern int GetProcessors(void);                                                                    // ��ô�������
extern UINT tick(void);                                                                            // ʼ�յδ�

extern int SendData(int s, char *buff, int n);                                                     // ��������
extern int RecvData(int s, char *buff, int n);                                                     // ��������
