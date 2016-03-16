#pragma once


#include <map>
#include <vector>
#include <string>
#include "IOCP.h"
#include "Serializer.h"
#include "Flags.pb.h"
#include "Error.pb.h"
#include "Client.pb.h"
#include "Server.pb.h"
#include "Version.pb.h"


//
// ���
//
class CGame;
class CGameServer;
class CPlayer : public CIOContext
{
	// ����/��������
public:
	CPlayer(CGameServer *s);
	virtual ~CPlayer(void);


	// ����
public:
	virtual VOID SetFlags(DWORD dwFlags);                                                          // ���ñ�ʶ
	virtual VOID ClearFlags(void);                                                                 // ��ձ�ʶ

	virtual VOID EnableFlag(DWORD dwFlag);                                                         // ʹ�ñ�ʶ
	virtual VOID DisableFlag(DWORD dwFlag);                                                        // ���ñ�ʶ

	virtual DWORD GetFlags(void) const;                                                            // ��ñ�ʶ

	virtual BOOL IsLogin(void) const;                                                              // ��½״̬
	virtual BOOL IsReady(void) const;                                                              // ��׼��״̬
	virtual BOOL IsWaiting(void) const;                                                            // �ȴ���״̬
	virtual BOOL IsGaming(void) const;                                                             // ��Ϸ��״̬


	// ����
private:
	DWORD m_dwFlags;                                                                               // ��ʶ

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
	virtual void SetGame(const char *password, int mode, int mapid, int maxPlayers);               // ������Ϸ

	virtual int GetMode(void) const;                                                               // �����Ϸģʽ
	virtual int GetMapID(void) const;                                                              // �����Ϸ��ͼ
	virtual int GetPlayers(void) const;                                                            // ��õ�ǰ�����
	virtual int GetMaxPlayers(void) const;                                                         // �����������

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
	DWORD id;                                                                                      // id

private:
	CHAR m_szPassword[16];                                                                         // ��Ϸ����

	int m_mode;                                                                                    // ��Ϸģʽ
	int m_mapid;                                                                                   // ��Ϸ��ͼ
	int m_maxPlayers;                                                                              // ��������
	int m_numPlayers;                                                                              // ��ǰ�����

public:
	CGameServer *pServer;                                                                          // ������ָ��
	CPlayer *pActivePlayer;                                                                        // ���ͷָ��

public:
	CGame *pNext;                                                                                  // ��һ����Ϸ
	CGame *pNextActive;                                                                            // ��һ����Ϸ
	CGame *pPrevActive;                                                                            // ǰһ����Ϸ
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
	virtual BOOL AllocGames(int maxGames);                                                         // ������Ϸ
	virtual BOOL AllocPlayers(int maxPlayers);                                                     // �������
	virtual BOOL CreateReportThread(void);                                                         // �����㱨�߳�
	virtual BOOL CreateUpdateThread(void);                                                         // ���������߳�

	virtual void FreeGames(void);                                                                  // �ͷ���Ϸ
	virtual void FreePlayers(void);                                                                // �ͷ����
	virtual void DestroyReportThread(void);                                                        // ���ٻ㱨�߳�
	virtual void DestroyUpdateThread(void);                                                        // ���ٸ����߳�

protected:
	virtual CGame* GetNextGame(void);                                                              // �����Ϸ
	virtual void ReleaseGame(CGame *pGame);                                                        // �ͷ���Ϸ

protected:
	virtual BOOL Login(CPlayer *pPlayer, DWORD guid);                                              // ��ҵ�½
	virtual BOOL Logout(CPlayer *pPlayer);                                                         // ���ע��
	virtual CPlayer* QueryPlayer(DWORD guid);                                                      // ��ѯ���

protected:
	virtual void SendToPlayer(CPlayer *pPlayer, BYTE *pBuffer, size_t size);                       // ����ָ�����
	virtual void SendToPlayerAll(CGame *pGame, CPlayer *pIgnore, BYTE *pBuffer, size_t size, DWORD dwFilter = 0xffffffff); // �����������

protected:
	virtual void OnConnect(CIOContext *pIOContext, SOCKET acceptSocket);                           // �ͻ������ӻص�
	virtual void OnDisconnect(CIOContext *pIOContext);                                             // �ͻ��˶����ص�

	virtual void OnUpdateSend(void);                                                               // ���·�����Ϣ
	virtual void OnUpdateRecv(DWORD dwDeltaTime);                                                  // ���½�����Ϣ
	virtual void OnUpdateGameMessage(CPlayer *pPlayer, WORD msg);                                  // ������Ϸ��Ϣ
	virtual void OnUpdateGameLogic(float deltaTime);                                               // ������Ϸ�߼�

	virtual void OnHeartReset(CPlayer *pPlayer);                                                   // ��������
	virtual void OnHeart(CPlayer *pPlayer, WORD size);                                             // ����
	virtual void OnFlags(CPlayer *pPlayer, WORD size);                                             // ��ʶ
	virtual void OnLogin(CPlayer *pPlayer, WORD size);                                             // ��½
	virtual void OnCreateGame(CPlayer *pPlayer, WORD size);                                        // ������Ϸ
	virtual void OnDestroyGame(CPlayer *pPlayer, WORD size);                                       // ������Ϸ
	virtual void OnEnterGame(CPlayer *pPlayer, WORD size);                                         // ������Ϸ
	virtual void OnExitGame(CPlayer *pPlayer, WORD size);                                          // �˳���Ϸ
	virtual void OnSendToPlayer(CPlayer *pPlayer, WORD size);                                      // ����ָ�����
	virtual void OnSendToPlayerAll(CPlayer *pPlayer, WORD size);                                   // �����������

protected:
	virtual void Monitor(void);                                                                    // ���
	virtual void Report(BYTE *buffer, CCacheBuffer *pCacheBuffer);                                 // ����

protected:
	static DWORD WINAPI ReportThread(LPVOID lpParam);                                              // �㱨�߳�
	static DWORD WINAPI UpdateThread(LPVOID lpParam);                                              // �����߳�


	// ����
protected:
	GUIDMAP m_guidmap;                                                                             // �߼�ID������IDӳ���

protected:
	int m_curGames;                                                                                // ��ǰ��Ϸ��
	int m_maxGames;                                                                                // �����Ϸ��
	CGame **m_games;                                                                               // ��Ϸ����
	CGame *m_pFreeGame;                                                                            // ������Ϸ
	CGame *m_pActiveGame;                                                                          // ���Ϸ

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
