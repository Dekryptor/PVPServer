#include "GameServer.h"


CGame::CGame(CGameServer *s)
{

}

CGame::~CGame(void)
{

}

//
// ������
//
int CGame::AddPlayer(CPlayer *pPlayer, const char *password, BOOL bCreater)
{
	return 0;
}

//
// ɾ�����
//
int CGame::DelPlayer(CPlayer *pPlayer)
{
	return 0;
}

//
// ������
//
void CGame::Clear(void)
{

}

//
// �������ж�
//
BOOL CGame::IsFull(void)
{
	return TRUE;
}

//
// ������ж�
//
BOOL CGame::IsEmpty(void)
{
	return TRUE;
}

//
// ����
//
void CGame::Update(float deltaTime)
{

}
