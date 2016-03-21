#include "GameServer.h"


CPlayer::CPlayer(CGameServer *s)
	: pServer(s)
	, pGame(NULL)

	, m_dwFlags(0x00000000)

	, pNextPlayer(NULL)
	, pPrevPlayer(NULL)
{

}

CPlayer::~CPlayer(void)
{

}

//
// ���ñ�ʶ
//
VOID CPlayer::SetFlags(DWORD dwFlags)
{
	m_dwFlags = dwFlags;
}

//
// ��ձ�ʶ
//
VOID CPlayer::ClearFlags(void)
{
	m_dwFlags = 0x00000000;
}

//
// ʹ�ñ�ʶ
//
VOID CPlayer::EnableFlag(DWORD dwFlag)
{
	SET_ENABLE(m_dwFlags, dwFlag);
}

//
// ���ñ�ʶ
//
VOID CPlayer::DisableFlag(DWORD dwFlag)
{
	SET_DISABLE(m_dwFlags, dwFlag);
}

//
// ��ñ�ʶ
//
DWORD CPlayer::GetFlags(void) const
{
	return m_dwFlags;
}

//
// ��½״̬
//
BOOL CPlayer::IsLogin(void) const
{
	return IS_ENABLE(m_dwFlags, ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_LOGIN);
}

//
// ��׼��״̬
//
BOOL CPlayer::IsReady(void) const
{
	return IS_ENABLE(m_dwFlags, ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_READY);
}

//
// �ȴ���״̬
//
BOOL CPlayer::IsWaiting(void) const
{
	return IS_ENABLE(m_dwFlags, ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_WAITING);
}

//
// ��Ϸ��״̬
//
BOOL CPlayer::IsGaming(void) const
{
	return IS_ENABLE(m_dwFlags, ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_GAMING);
}
