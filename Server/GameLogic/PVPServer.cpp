#include "PVPServer.h"


CPVPServer::CPVPServer(void)
{

}

CPVPServer::~CPVPServer(void)
{

}

//
// ������Ϸ
//
BOOL CPVPServer::AllocGames(int maxGames)
{
	//
	// 1. ������Ϸ�洢
	//
	m_curGames = 0;
	m_maxGames = maxGames;
	m_games = new CGame*[m_maxGames];

	//
	// 2. ������Ϸ����
	//
	for (int indexGame = 0; indexGame < m_maxGames; indexGame++) {
		m_games[indexGame] = new CGameLogic(this);
		m_games[indexGame]->id = indexGame;
	}

	for (int indexGame = 0; indexGame < m_maxGames - 1; indexGame++) {
		m_games[indexGame]->pNext = m_games[indexGame + 1];
		m_games[indexGame + 1]->pNext = NULL;
	}

	m_pFreeGame = m_games[0];
	m_pActiveGame = NULL;

	return TRUE;
}

//
// �������
//
BOOL CPVPServer::AllocPlayers(int maxPlayers)
{
	//
	// 1. ���������Ĵ洢
	//
	m_curContexts = 0;
	m_maxContexts = maxPlayers;
	m_contexts = new CIOContext*[m_maxContexts];

	//
	// 2. �����������
	//
	for (int indexContext = 0; indexContext < m_maxContexts; indexContext++) {
		m_contexts[indexContext] = new CPlayerLogic(this);
		m_contexts[indexContext]->id = indexContext;
	}

	for (int indexContext = 0; indexContext < m_maxContexts - 1; indexContext++) {
		m_contexts[indexContext]->pNext = m_contexts[indexContext + 1];
		m_contexts[indexContext + 1]->pNext = NULL;
	}

	m_pFreeContext = m_contexts[0];
	m_pActiveContext = NULL;

	return TRUE;
}

//
// ������Ϸ�߼�
//
void CPVPServer::OnUpdateGameLogic(float deltaTime)
{

}
