#pragma once


enum {
	GAMESERVER_MSG_BASE = 3000,
	GAMESERVER_MSG_C2S_BASE = GAMESERVER_MSG_BASE + 100,
	GAMESERVER_MSG_S2C_BASE = GAMESERVER_MSG_BASE + 200,

	GAMESERVER_MSG_C2S_HEART = GAMESERVER_MSG_C2S_BASE + 0,                                        // ����
	GAMESERVER_MSG_C2S_LOGIN = GAMESERVER_MSG_C2S_BASE + 1,                                        // ��¼
	GAMESERVER_MSG_C2S_FLAGS = GAMESERVER_MSG_C2S_BASE + 2,                                        // ���ñ�ʶ
	GAMESERVER_MSG_C2S_CREATE_GAME = GAMESERVER_MSG_C2S_BASE + 3,                                  // ������Ϸ
	GAMESERVER_MSG_C2S_DESTROY_GAME = GAMESERVER_MSG_C2S_BASE + 4,                                 // ������Ϸ
	GAMESERVER_MSG_C2S_ENTER_GAME = GAMESERVER_MSG_C2S_BASE + 5,                                   // ������Ϸ
	GAMESERVER_MSG_C2S_EXIT_GAME = GAMESERVER_MSG_C2S_BASE + 6,                                    // �˳���Ϸ
	GAMESERVER_MSG_C2S_MODIFY_GAME_PASSWORD = GAMESERVER_MSG_C2S_BASE + 7,                         // �޸���Ϸ����
	GAMESERVER_MSG_C2S_SENDTO_PLAYER = GAMESERVER_MSG_C2S_BASE + 8,                                // ����ָ�����
	GAMESERVER_MSG_C2S_SENDTO_PLAYER_ALL = GAMESERVER_MSG_C2S_BASE + 9,                            // �����������
	GAMESERVER_MSG_C2S_COUNT,

	GAMESERVER_MSG_S2C_HEART = GAMESERVER_MSG_S2C_BASE + 1,                                        // ����
	GAMESERVER_MSG_S2C_LOGIN = GAMESERVER_MSG_S2C_BASE + 2,                                        // ����
	GAMESERVER_MSG_S2C_FLAGS = GAMESERVER_MSG_S2C_BASE + 3,                                        // ���ñ�ʶ
	GAMESERVER_MSG_S2C_CREATE_GAME = GAMESERVER_MSG_S2C_BASE + 4,                                  // ������Ϸ
	GAMESERVER_MSG_S2C_DESTROY_GAME = GAMESERVER_MSG_S2C_BASE + 5,                                 // ������Ϸ
	GAMESERVER_MSG_S2C_ENTER_GAME = GAMESERVER_MSG_S2C_BASE + 6,                                   // ������Ϸ
	GAMESERVER_MSG_S2C_EXIT_GAME = GAMESERVER_MSG_S2C_BASE + 7,                                    // �˳���Ϸ
	GAMESERVER_MSG_S2C_MODIFY_GAME_PASSWORD = GAMESERVER_MSG_S2C_BASE + 8,                         // �޸���Ϸ����
	GAMESERVER_MSG_S2C_SENDTO_PLAYER = GAMESERVER_MSG_S2C_BASE + 9,                                // ת��
	GAMESERVER_MSG_S2C_COUNT,
};
