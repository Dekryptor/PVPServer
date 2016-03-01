#include "GameServer.h"


CGame::CGame(CGameServer *s)
	: pServer(s)

	, m_mode(-1)
	, m_mapid(-1)
	, m_maxPlayers(0)
	, m_numPlayers(0)

	, pActivePlayer(NULL)
{
	memset(m_szPassword, 0, sizeof(m_szPassword));
}

CGame::~CGame(void)
{

}

//
// ������Ϸ����
//
void CGame::SetPassword(const char *password)
{
	if (password && strlen(password) < sizeof(m_szPassword)) {
		strcpy(m_szPassword, password);
	}
	else {
		memset(m_szPassword, 0, sizeof(m_szPassword));
	}
}

//
// ������Ϸģʽ
//
void CGame::SetMode(int mode)
{
	m_mode = mode;
}

//
// ������Ϸ��ͼ
//
void CGame::SetMapID(int mapid)
{
	m_mapid = mapid;
}

//
// ������������
//
void CGame::SetMaxPlayers(int maxPlayers)
{
	m_maxPlayers = maxPlayers;
}

//
// �����Ϸģʽ
//
int CGame::GetMode(void) const
{
	return m_mode;
}

//
// �����Ϸ��ͼ
//
int CGame::GetMapID(void) const
{
	return m_mapid;
}

//
// ��õ�ǰ�����
//
int CGame::GetPlayers(void) const
{
	return m_numPlayers;
}

//
// �����������
//
int CGame::GetMaxPlayers(void) const
{
	return m_maxPlayers;
}

//
// ������
//
int CGame::AddPlayer(CPlayer *pPlayer, const char *password, BOOL bCreater)
{
	//
	// 1. ������ȫ���
	//
	if (pPlayer == NULL) {
		return ERR_PLAYER_INVALID;
	}

	if (pPlayer->IsWaiting() == TRUE) {
		return ERR_PLAYER_IN_GAME;
	}

	if (IsFull() == TRUE) {
		return ERR_GAME_FULL;
	}

	if (IsEmpty() == FALSE && bCreater == TRUE) {
		return ERR_GAME_USING;
	}

	if (IsEmpty() == TRUE && bCreater == FALSE) {
		return ERR_GAME_EMPTY;
	}

	if (password == NULL ||
		strncmp(password, m_szPassword, sizeof(m_szPassword))) {
		return ERR_GAME_PASSWORD;
	}

	//
	// 2. ������
	//
	pPlayer->pGame = this;
	pPlayer->pServer = pServer;

	pPlayer->pPrevPlayer = NULL;
	pPlayer->pNextPlayer = pActivePlayer;

	if (pActivePlayer) {
		pActivePlayer->pPrevActive = pPlayer;
	}

	pActivePlayer = pPlayer;

	//
	// 3. �������״̬
	//
	pPlayer->EnableFlag(PLAYER_FLAGS_WAITING);

	//
	// 4. ��¼���
	//
	m_numPlayers++;

	return ERR_NONE;
}

//
// ɾ�����
//
int CGame::DelPlayer(CPlayer *pPlayer)
{
	//
	// 1. ������ȫ���
	//
	if (pPlayer == NULL) {
		return ERR_PLAYER_INVALID;
	}

	if (pPlayer->pGame != this) {
		return ERR_PLAYER_OUT_GAME;
	}

	//
	// 2. ɾ�����
	//
	if (pPlayer->pPrevPlayer) {
		pPlayer->pPrevPlayer->pNextPlayer = pPlayer->pNextPlayer;
	}

	if (pPlayer->pNextPlayer) {
		pPlayer->pNextPlayer->pPrevPlayer = pPlayer->pPrevPlayer;
	}

	if (pActivePlayer == pPlayer) {
		pActivePlayer = pPlayer->pNextPlayer;
	}

	pPlayer->pPrevPlayer = NULL;
	pPlayer->pNextPlayer = NULL;

	//
	// 3. �������״̬
	//
	pPlayer->DisableFlag(PLAYER_FLAGS_WAITING);

	//
	// 4. ��¼���
	//
	m_numPlayers--;

	return ERR_NONE;
}

//
// ������
//
void CGame::Clear(void)
{
	while (pActivePlayer) {
		DelPlayer(pActivePlayer);
	}
}

//
// �������ж�
//
BOOL CGame::IsFull(void)
{
	return m_numPlayers == m_maxPlayers ? TRUE : FALSE;
}

//
// ������ж�
//
BOOL CGame::IsEmpty(void)
{
	return m_numPlayers == 0 ? TRUE : FALSE;
}

//
// ����
//
void CGame::Update(float deltaTime)
{

}
