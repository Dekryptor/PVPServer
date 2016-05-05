#pragma once


#include "GameServer.h"


//
// ���
//
class _ServerExport CPVPGameServer;
class _ServerExport CPVPGame;
class _ServerExport CPVPPlayer : public CPlayer
{
	// ����/��������
public:
	CPVPPlayer(CPVPGameServer *s);
	virtual ~CPVPPlayer(void);


	// ����
public:


	// ����
protected:
};

//
// ��Ϸ
//
class _ServerExport CPVPGame : public CGame
{
	// ����/��������
public:
	CPVPGame(CPVPGameServer *s);
	virtual ~CPVPGame(void);


	// ����
public:
	virtual void Update(float deltaTime);                                                          // ����


	// ����
protected:
};

//
// ������
//
class _ServerExport CPVPGameServer : public CGameServer
{
	// ����/��������
public:
	CPVPGameServer(void);
	virtual ~CPVPGameServer(void);


	// ����
protected:
	virtual BOOL AllocGames(int maxGames);                                                         // ������Ϸ
	virtual BOOL AllocPlayers(int maxPlayers);                                                     // �������


	// ����
protected:
};
