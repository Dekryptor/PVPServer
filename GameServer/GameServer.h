#pragma once


#include <map>
#include <vector>
#include <string>
#include "IOCP.h"


//
// ���
//
class CGame;
class CGameServer;
class CPlayer : public CIOContext
{
	// ����/��������
public:
	CPlayer(void);
	virtual ~CPlayer(void);


	// ����
public:
	virtual BOOL IsLogin(void) const;                                                              // ��½״̬
	virtual BOOL IsReady(void) const;                                                              // ��׼��״̬
	virtual BOOL IsWaiting(void) const;                                                            // �ȴ���״̬
	virtual BOOL IsGaming(void) const;                                                             // ��Ϸ��״̬


	// ����
public:
	DWORD dwFlags;                                                                                 // ��ʶ

public:
	CGame *pGame;                                                                                  // ��Ϸ
	CGameServer *pServer;                                                                          // ������

public:
	CPlayer *pNextPlayer;                                                                          // ��һ�����
	CPlayer *pPrevPlayer;                                                                          // ǰһ�����
};

//
// ��Ϸ
//
class CGame
{
	// ����/��������
public:
	CGame(CGameServer *s);
	virtual ~CGame(void);


	// ����
public:
	virtual int AddPlayer(CPlayer *pPlayer, const char *password, BOOL bCreater);                  // ������
	virtual int DelPlayer(CPlayer *pPlayer);                                                       // ɾ�����

	virtual void Clear(void);                                                                      // ������

	virtual BOOL IsFull(void);                                                                     // �������ж�
	virtual BOOL IsEmpty(void);                                                                    // ������ж�

public:
	virtual void Update(float deltaTime);                                                          // ����


	// ����
public:
	CGameServer *pServer;                                                                          // ������ָ��
	CPlayer *pActivePlayer;                                                                        // ���ͷָ��
};

//
// ������
//
class CGameServer : public CIOCPServer
{
	// ���ݽṹ
public:
	typedef std::map<DWORD, int> GUIDMAP;                                                          // GUIDӳ���


	// ����/��������
public:
	CGameServer(void);
	virtual ~CGameServer(void);


	// ����
public:
	virtual BOOL Start(const char *ip, int port, int maxGames, int maxPlayers, int timeOut, const char *rootip, int rootport); // ����������
	virtual void Stop(void);                                                                       // ֹͣ������

protected:
	virtual BOOL AllocPlayers(int maxPlayers);                                                     // �������
	virtual BOOL AllocGames(int maxGames);                                                         // ������Ϸ
	virtual BOOL CreateReportThread(void);                                                         // �����㱨�߳�
	virtual BOOL CreateUpdateThread(void);                                                         // ���������߳�

	virtual void FreePlayers(void);                                                                // �ͷ����
	virtual void FreeGames(void);                                                                  // �ͷ���Ϸ
	virtual void DestroyReportThread(void);                                                        // ���ٻ㱨�߳�
	virtual void DestroyUpdateThread(void);                                                        // ���ٸ����߳�

protected:
	virtual CGame* GetNextGame(void);                                                              // �����Ϸ
	virtual void ReleaseGame(CGame *pGame);                                                        // �ͷ���Ϸ

protected:
	virtual CPlayer* QueryPlayer(DWORD guid);                                                      // ��ѯ���
	virtual BOOL RegisterPlayer(CPlayer *pPlayer, DWORD guid);                                     // ע�����
	virtual BOOL UnRegisterPlayer(CPlayer *pPlayer);                                               // ע�����

protected:
	virtual void SendToPlayer(CPlayer *pPlayer, BYTE *pBuffer, DWORD size);                        // ����ָ�����
	virtual void SendToPlayerAll(CGame *pGame, CPlayer *pPlayerIgnore, BYTE *pBuffer, DWORD size, DWORD dwFilter = 0xffffffff); // �����������

protected:
	virtual void OnConnect(CIOContext *pIOContext, SOCKET acceptSocket);                           // �ͻ������ӻص�
	virtual void OnDisconnect(CIOContext *pIOContext);                                             // �ͻ��˶����ص�

	virtual void OnUpdateRecv(DWORD dwDeltaTime);                                                  // ���½�����Ϣ
	virtual void OnUpdateSend(void);                                                               // ���·�����Ϣ
	virtual void OnUpdateGame(float deltaTime);                                                    // ������Ϸ

	virtual void OnHeart(CPlayer *pPlayer);                                                        // ����
	virtual void OnHeartReset(CPlayer *pPlayer);                                                   // ��������
	virtual void OnLogin(CPlayer *pPlayer);                                                        // ��½
	virtual void OnFlags(CPlayer *pPlayer);                                                        // ���ñ�ʶ
	virtual void OnCreateGame(CPlayer *pPlayer);                                                   // ������Ϸ
	virtual void OnDestroyGame(CPlayer *pPlayer);                                                  // ������Ϸ
	virtual void OnEnterGame(CPlayer *pPlayer);                                                    // ������Ϸ
	virtual void OnExitGame(CPlayer *pPlayer);                                                     // �˳���Ϸ
	virtual void OnSendToPlayer(CPlayer *pPlayer, WORD packSize);                                  // ����ָ�����
	virtual void OnSendToPlayerAll(CPlayer *pPlayer, WORD packSize);                               // �����������
	virtual void OnSendToPlayerFilterAll(CPlayer *pPlayer, WORD packSize);                         // �����������(������)

protected:
	virtual void Monitor(void);                                                                    // ���
	virtual void Report(BYTE *buffer, CCacheBuffer *pCacheBuffer);                                 // ����

protected:
	static DWORD WINAPI ReportThread(LPVOID lpParam);                                              // �㱨�߳�
	static DWORD WINAPI UpdateThread(LPVOID lpParam);                                              // �����߳�


	// ����
protected:
	GUIDMAP guidmap;                                                                               // �߼�ID������IDӳ���

protected:
	int m_maxGames;                                                                                // ��Ϸ��
	CGame **m_games;                                                                               // ��Ϸ����

	int m_timeOut;                                                                                 // ������ʱ

	HANDLE m_hReportThread;                                                                        // �㱨�߳̾��
	HANDLE m_hUpdateThread;                                                                        // �����߳̾��

protected:
	int m_nRootServerPort;                                                                         // ��ڷ������˿�
	char m_szRootServerIP[16];                                                                     // ��ڷ�����IP

protected:
	DWORD m_dwUpdateCount;                                                                         // ���´���
	DWORD m_dwUpdateTime;                                                                          // ƽ������ʱ��
	DWORD m_dwUpdateTimeTotal;                                                                     // ������ʱ��
	DWORD m_dwRuntimeTotal;                                                                        // ������ʱ��

	DWORD m_dwRecvDataSize;                                                                        // ����������
	DWORD m_dwSendDataSize;                                                                        // ����������
};

extern void WriteLog(const char *szFmt, ...);                                                      // �����־