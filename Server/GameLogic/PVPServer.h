#pragma once


#include "GameServer.h"


//
// ���
//
class _ServerExport CPVPServer;
class _ServerExport CGameLogic;
class _ServerExport CPlayerLogic : public CPlayer
{
	// ����/��������
public:
	CPlayerLogic(CPVPServer *s);
	virtual ~CPlayerLogic(void);


	// ����
public:


	// ����
protected:
};

//
// ��Ϸ
//
class _ServerExport CGameLogic : public CGame
{
	// ����/��������
public:
	CGameLogic(CPVPServer *s);
	virtual ~CGameLogic(void);


	// ����
public:
	virtual void Update(float deltaTime);                                                          // ����


	// ����
protected:
};

//
// ������
//
class _ServerExport CPVPServer : public CGameServer
{
	// ����/��������
public:
	CPVPServer(void);
	virtual ~CPVPServer(void);


	// ����
protected:
	virtual BOOL AllocGames(int maxGames);                                                         // ������Ϸ
	virtual BOOL AllocPlayers(int maxPlayers);                                                     // �������

protected:
	virtual void OnUpdateGameMessage(CPlayer *pPlayer, WORD size, WORD msg);                       // ������Ϸ��Ϣ
	virtual void OnUpdateGameLogic(float deltaTime);                                               // ������Ϸ�߼�


	// ����
protected:
};
