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
class _ServerExport CGateServer : public CIOCPServer
{
	// ���ݽṹ
public:
	typedef struct {
		CIOContext *pContext;                                                                      // ���������
		float timeout;                                                                             // ���ƥ�䳬ʱ
		int minEvaluation;                                                                         // ��Сƥ�䷶Χ
		int maxEvaluation;                                                                         // ���ƥ�䷶Χ
	} PlayerStatus;

	typedef struct {
		BOOL bEmpty;                                                                               // �շ���
		int id;                                                                                    // ��ϷID
		int mode;                                                                                  // ��Ϸģʽ
		int mapid;                                                                                 // ��Ϸ��ͼ
		int evaluation;                                                                            // ��Ϸ����
	} GameStatus;

	typedef struct {
		char ip[260];                                                                              // ��Ϸ������IP
		int port;                                                                                  // ��Ϸ�������˿�
		int curGames;                                                                              // ��Ϸ��������ǰ��Ϸ��
		int maxGames;                                                                              // ��Ϸ�����������Ϸ��
		std::vector<GameStatus> games;                                                             // ��Ϸ����(�ɼ���)
	} GameServerStatus;

	typedef std::map<DWORD, CIOContext*> PlayerMap;                                                // ��Ҽ���
	typedef std::map<CIOContext*, GameServerStatus> GameServerMap;                                 // ��Ϸ����������
	typedef std::map<int, std::map<DWORD, PlayerStatus>> PlayerEvaluationMap;                      // ��ƥ����Ҽ���


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

	virtual void ClearGameServer(CIOContext *pContext);                                            // ������Ϸ������

protected:
	virtual void SendTo(CIOContext *pContext, BYTE *pBuffer, size_t size);                         // ����ָ���ͻ���

protected:
	virtual void OnConnect(CIOContext *pContext, SOCKET acceptSocket);                             // �ͻ������ӻص�
	virtual void OnDisconnect(CIOContext *pContext);                                               // �ͻ��˶����ص�

	virtual void OnUpdateRecv(DWORD dwDeltaTime);                                                  // ���½�����Ϣ
	virtual void OnUpdateMatch(DWORD dwDeltaTime);                                                 // ����ƥ��

	virtual void OnHeartReset(CIOContext *pContext);                                               // ��������
	virtual void OnHeart(CIOContext *pContext, WORD size);                                         // ����
	virtual void OnLogin(CIOContext *pContext, WORD size);                                         // ��½
	virtual void OnMatch(CIOContext *pContext, WORD size);                                         // ƥ��
	virtual void OnCancelMatch(CIOContext *pContext, WORD size);                                   // ȡ��ƥ��
	virtual void OnListGameServer(CIOContext *pContext, WORD size);                                // ��Ϸ�������б�
	virtual void OnSendToPlayer(CIOContext *pContext, WORD size);                                  // ����ָ�����
	virtual void OnGameServerStatus(CIOContext *pContext, WORD size);                              // ��Ϸ������״̬

protected:
	virtual void Monitor(void);                                                                    // ���

protected:
	static DWORD WINAPI UpdateThread(LPVOID lpParam);                                              // �����߳�


	// ����
protected:
	HANDLE m_hUpdateThread;                                                                        // �����߳̾��

	PlayerMap m_players;                                                                           // ��Ҽ���
	GameServerMap m_servers;                                                                       // ��Ϸ����������
	PlayerEvaluationMap m_evaluations;                                                             // ��ƥ����Ҽ���

protected:
	DWORD m_dwUpdateCount;                                                                         // ���´���
	DWORD m_dwUpdateTime;                                                                          // ƽ������ʱ��
	DWORD m_dwUpdateTimeTotal;                                                                     // ������ʱ��
	DWORD m_dwRuntimeTotal;                                                                        // ������ʱ��

	DWORD m_dwRecvDataSize;                                                                        // ����������
	DWORD m_dwSendDataSize;                                                                        // ����������
};
