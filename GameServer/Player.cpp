#include "GameServer.h"


CPlayer::CPlayer(void)
{

}

CPlayer::~CPlayer(void)
{

}

//
// ��½״̬
//
BOOL CPlayer::IsLogin(void) const
{
	return TRUE;
}

//
// ��׼��״̬
//
BOOL CPlayer::IsReady(void) const
{
	return TRUE;
}

//
// �ȴ���״̬
//
BOOL CPlayer::IsWaiting(void) const
{
	return TRUE;
}

//
// ��Ϸ��״̬
//
BOOL CPlayer::IsGaming(void) const
{
	return TRUE;
}
