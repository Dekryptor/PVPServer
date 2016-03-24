#pragma once


#include <map>
#include <vector>
#include <string>
#include "IOCP.h"
#include "Serializer.h"
#include "ProtoGateClient.pb.h"
#include "ProtoGateServer.pb.h"
#include "ProtoGameServer.pb.h"


//
// ������
//
class CGateServer : public CIOCPServer
{
	// ���ݽṹ
public:
	typedef struct {
		char ip[260];                                                                              // ��Ϸ������IP
		int port;                                                                                  // ��Ϸ�������˿�
		int maxGames;                                                                              // ��Ϸ�����������Ϸ��
		int curGames;                                                                              // ��Ϸ��������ǰ��Ϸ��
	} GameServerStatus;

	typedef std::map<CIOContext*, GameServerStatus> GameServerMap;                                 // ��Ϸ����������
	typedef std::map<DWORD, CIOContext*> GUIDMAP;                                                  // GUIDӳ���


	// ����/��������
public:
	CGateServer(void);
	virtual ~CGateServer(void);


	// ����
public:
	virtual BOOL Start(const char *ip, int port, int maxContexts, int timeOut);                    // ����������
	virtual void Stop(void);                                                                       // ֹͣ������

protected:
	virtual BOOL CreateUpdateThread(void);                                                         // ���������߳�
	virtual void DestroyUpdateThread(void);                                                        // ���ٸ����߳�

protected:
	virtual BOOL Login(CIOContext *pContext, DWORD guid);                                          // ��½
	virtual BOOL Logout(CIOContext *pContext);                                                     // ע��
	virtual CIOContext* QueryContext(DWORD guid);                                                  // ��ѯ

protected:
	virtual void SendTo(CIOContext *pContext, BYTE *pBuffer, size_t size);                         // ����ָ���ͻ���

protected:
	virtual void OnDisconnect(CIOContext *pContext);                                               // �ͻ��˶����ص�

	virtual void OnUpdateSend(void);                                                               // ���·�����Ϣ
	virtual void OnUpdateRecv(DWORD dwDeltaTime);                                                  // ���½�����Ϣ

	virtual void OnHeartReset(CIOContext *pContext);                                               // ��������
	virtual void OnHeart(CIOContext *pContext, WORD size);                                         // ����
	virtual void OnLogin(CIOContext *pContext, WORD size);                                         // ��½
	virtual void OnListGameServer(CIOContext *pContext, WORD size);                                // �����Ϸ�������б�
	virtual void OnGameServerStatus(CIOContext *pContext, WORD size);                              // �����Ϸ������״̬

protected:
	virtual void Monitor(void);                                                                    // ���

protected:
	static DWORD WINAPI UpdateThread(LPVOID lpParam);                                              // �����߳�


	// ����
protected:
	int m_timeOut;                                                                                 // ������ʱ
	HANDLE m_hUpdateThread;                                                                        // �����߳̾��

	GUIDMAP m_guidmap;                                                                             // GUIDӳ���
	GameServerMap m_gameServerMap;                                                                 // ��Ϸ����������

protected:
	DWORD m_dwUpdateCount;                                                                         // ���´���
	DWORD m_dwUpdateTime;                                                                          // ƽ������ʱ��
	DWORD m_dwUpdateTimeTotal;                                                                     // ������ʱ��
	DWORD m_dwRuntimeTotal;                                                                        // ������ʱ��

	DWORD m_dwRecvDataSize;                                                                        // ����������
	DWORD m_dwSendDataSize;                                                                        // ����������
};

extern void WriteLog(const char *szFmt, ...);                                                      // �����־
