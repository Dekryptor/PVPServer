#pragma once


#include <map>
#include <vector>
#include <string>
#include "IOCP.h"
#include "Serializer.h"
#include "ProtoGameClient.pb.h"
#include "ProtoGameServer.pb.h"


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
	virtual void SetGame(const char *password, int mode, int mapid, int maxPlayers, float weight); // ������Ϸ

	virtual BOOL IsPrivate(void) const;                                                            // �����Ϸ�Ƿ�˽��
	virtual int GetMode(void) const;                                                               // �����Ϸģʽ
	virtual int GetMapID(void) const;                                                              // �����Ϸ��ͼ

	virtual int GetCurPlayers(void) const;                                                         // ��õ�ǰ�����
	virtual int GetMaxPlayers(void) const;                                                         // �����������

	virtual float GetWeight(void) const;                                                           // �����ϷȨ��

public:
	virtual int AddPlayer(CPlayer *pPlayer, const char *password, BOOL bCreater);                  // ������
	virtual int DelPlayer(CPlayer *pPlayer);                                                       // ɾ�����

	virtual void Clear(void);                                                                      // ������

	virtual BOOL IsFull(void) const;                                                               // �������ж�
	virtual BOOL IsEmpty(void) const;                                                              // ������ж�

	virtual DWORD GetHostGUID(void) const;                                                         // ����������GUID

public:
	virtual void Update(float deltaTime);                                                          // ����


	// ����
public:
	DWORD id;                                                                                      // id

private:
	CHAR m_szPassword[16];                                                                         // ��Ϸ����

	int m_mode;                                                                                    // ��Ϸģʽ
	int m_mapid;                                                                                   // ��Ϸ��ͼ

	int m_curPlayers;                                                                              // ��ǰ�����
	int m_maxPlayers;                                                                              // ��������

	float m_weight;                                                                                // ��ϷȨ��

private:
	DWORD m_dwHostGUID;                                                                            // �������GUID

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
	typedef std::map<DWORD, CPlayer*> GUIDMAP;                                                     // GUIDӳ���


	// ����/��������
public:
	CGameServer(void);
	virtual ~CGameServer(void);


	// ����
public:
	virtual BOOL Start(const char *ip, int port, int maxGames, int maxPlayers, int timeOut, const char *gateip, int gateport); // ����������
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
	virtual void Host(CPlayer *pPlayer);                                                           // ����
	virtual void Host(CGame *pGame);                                                               // ����

protected:
	virtual void OnConnect(CIOContext *pContext, SOCKET acceptSocket);                             // �ͻ������ӻص�
	virtual void OnDisconnect(CIOContext *pContext);                                               // �ͻ��˶����ص�

	virtual void OnUpdateRecv(DWORD dwDeltaTime);                                                  // ���½�����Ϣ
	virtual void OnUpdateGameMessage(CPlayer *pPlayer, WORD msg);                                  // ������Ϸ��Ϣ
	virtual void OnUpdateGameLogic(float deltaTime);                                               // ������Ϸ�߼�

	virtual void OnHeartReset(CPlayer *pPlayer);                                                   // ��������
	virtual void OnHeart(CPlayer *pPlayer, WORD size);                                             // ����
	virtual void OnFlags(CPlayer *pPlayer, WORD size);                                             // ��ʶ
	virtual void OnLogin(CPlayer *pPlayer, WORD size);                                             // ��½
	virtual void OnListGame(CPlayer *pPlayer, WORD size);                                          // �����Ϸ�б�
	virtual void OnCreateGame(CPlayer *pPlayer, WORD size);                                        // ������Ϸ
	virtual void OnDestroyGame(CPlayer *pPlayer, WORD size);                                       // ������Ϸ
	virtual void OnEnterGame(CPlayer *pPlayer, WORD size);                                         // ������Ϸ
	virtual void OnExitGame(CPlayer *pPlayer, WORD size);                                          // �˳���Ϸ
	virtual void OnSendToPlayer(CPlayer *pPlayer, WORD size);                                      // ����ָ�����
	virtual void OnSendToPlayerAll(CPlayer *pPlayer, WORD size);                                   // �����������

protected:
	virtual void Monitor(void);                                                                    // ���

protected:
	static DWORD WINAPI ReportThread(LPVOID lpParam);                                              // �㱨�߳�
	static DWORD WINAPI UpdateThread(LPVOID lpParam);                                              // �����߳�


	// ����
protected:
	GUIDMAP m_guidmap;                                                                             // GUIDӳ���

protected:
	int m_curGames;                                                                                // ��ǰ��Ϸ��
	int m_maxGames;                                                                                // �����Ϸ��
	CGame **m_games;                                                                               // ��Ϸ����
	CGame *m_pFreeGame;                                                                            // ������Ϸ
	CGame *m_pActiveGame;                                                                          // ���Ϸ

	HANDLE m_hReportThread;                                                                        // �㱨�߳̾��
	HANDLE m_hUpdateThread;                                                                        // �����߳̾��

protected:
	int m_nGateServerPort;                                                                         // ���ط������˿�
	char m_szGateServerIP[256];                                                                    // ���ط�����IP

protected:
	DWORD m_dwUpdateCount;                                                                         // ���´���
	DWORD m_dwUpdateTime;                                                                          // ƽ������ʱ��
	DWORD m_dwUpdateTimeTotal;                                                                     // ������ʱ��
	DWORD m_dwRuntimeTotal;                                                                        // ������ʱ��

	DWORD m_dwRecvDataSize;                                                                        // ����������
	DWORD m_dwSendDataSize;                                                                        // ����������
};
