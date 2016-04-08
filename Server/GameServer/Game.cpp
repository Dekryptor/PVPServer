#include "GameServer.h"


CGame::CGame(CGameServer *s)
	: id(0xffffffff)

	, pServer(s)
	, pActivePlayer(NULL)

	, m_mode(-1)
	, m_mapid(-1)
	, m_curPlayers(0)
	, m_maxPlayers(0)
	, m_evaluation(0)
	
	, m_dwHostGUID(0xffffffff)

	, pNext(NULL)
	, pNextActive(NULL)
	, pPrevActive(NULL)
{
	memset(m_szPassword, 0, sizeof(m_szPassword));
}

CGame::~CGame(void)
{

}

//
// ������Ϸ
//
void CGame::SetGame(const char *password, int mode, int mapid, int maxPlayers, int evaluation)
{
	m_dwHostGUID = 0xffffffff;

	m_mode = mode;
	m_mapid = mapid;
	m_maxPlayers = maxPlayers;
	m_evaluation = evaluation;
	strcpy(m_szPassword, password);
}

//
// �����Ϸ�Ƿ�˽��
//
BOOL CGame::IsPrivate(void) const
{
	return strlen(m_szPassword) > 0 ? TRUE : FALSE;
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
int CGame::GetCurPlayers(void) const
{
	return m_curPlayers;
}

//
// �����������
//
int CGame::GetMaxPlayers(void) const
{
	return m_maxPlayers;
}

//
// �����ϷȨ��
//
int CGame::GetEvaluation(void) const
{
	return m_evaluation;
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
		return ProtoGameServer::ERROR_CODE::ERR_PLAYER_INVALID;
	}

	if (pPlayer->pGame != NULL) {
		return ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_INGAME;
	}

	if (IsFull() == TRUE) {
		return ProtoGameServer::ERROR_CODE::ERR_GAME_FULL;
	}

	if (IsEmpty() == FALSE && bCreater == TRUE) {
		return ProtoGameServer::ERROR_CODE::ERR_GAME_USING;
	}

	if (IsEmpty() == TRUE && bCreater == FALSE) {
		return ProtoGameServer::ERROR_CODE::ERR_GAME_EMPTY;
	}

	if (password == NULL ||
		strncmp(password, m_szPassword, sizeof(m_szPassword))) {
		return ProtoGameServer::ERROR_CODE::ERR_GAME_PASSWORD;
	}

	//
	// 2. ������
	//
	pPlayer->pGame = this;

	pPlayer->pPrevPlayer = NULL;
	pPlayer->pNextPlayer = pActivePlayer;

	if (pActivePlayer) {
		pActivePlayer->pPrevPlayer = pPlayer;
	}

	pActivePlayer = pPlayer;

	//
	// 3. �������״̬
	//
	pPlayer->EnableFlag(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_WAITING);

	//
	// 4. ��¼���
	//
	m_curPlayers++;

	//
	// 5. ��������
	//
	if (bCreater) {
		m_dwHostGUID = pPlayer->guid;
	}

	return ProtoGameServer::ERROR_CODE::ERR_NONE;
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
		return ProtoGameServer::ERROR_CODE::ERR_PLAYER_INVALID;
	}

	if (pPlayer->pGame != this) {
		return ProtoGameServer::ERROR_CODE::ERR_PLAYER_FLAGS_NOT_INGAME;
	}

	//
	// 2. ɾ�����
	//
	pPlayer->pGame = NULL;

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
	pPlayer->SetFlags(ProtoGameServer::FLAGS_CODE::PLAYER_FLAGS_LOGIN);

	//
	// 4. ��¼���
	//
	m_curPlayers--;

	//
	// 5. ��������
	//
	if (m_dwHostGUID == pPlayer->guid && pActivePlayer) {
		m_dwHostGUID = pActivePlayer->guid;
	}

	return ProtoGameServer::ERROR_CODE::ERR_NONE;
}

//
// ������
//
void CGame::Clear(void)
{
	while (pActivePlayer) {
		DelPlayer(pActivePlayer);
	}

	memset(m_szPassword, 0, sizeof(m_szPassword));
	m_mode = -1;
	m_mapid = -1;
	m_curPlayers = 0;
	m_maxPlayers = 0;
	m_evaluation = 0;
	m_dwHostGUID = 0xffffffff;
}

//
// �������ж�
//
BOOL CGame::IsFull(void) const
{
	return m_curPlayers == m_maxPlayers ? TRUE : FALSE;
}

//
// ������ж�
//
BOOL CGame::IsEmpty(void) const
{
	return m_curPlayers == 0 ? TRUE : FALSE;
}

//
// ����������GUID
//
DWORD CGame::GetHostGUID(void) const
{
	return m_dwHostGUID;
}

//
// ����
//
void CGame::Update(float deltaTime)
{

}
