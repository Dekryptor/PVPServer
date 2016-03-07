#pragma once


enum {
	ERR_NONE = 0,                                                                                  // �޴���

	ERR_VERSION_INVALID,                                                                           // ��Ч�汾

	ERR_PLAYER_BASE = 1000,
	ERR_PLAYER_INVALID,                                                                            // �����Ч
	ERR_PLAYER_INVALID_ID,                                                                         // �����ЧID
	ERR_PLAYER_INVALID_GUID,                                                                       // �����ЧGUID
	ERR_PLAYER_STATE_LOGIN,                                                                        // ����Ѿ���½
	ERR_PLAYER_IN_GAME,                                                                            // ����Ѿ��ڻỰ��
	ERR_PLAYER_OUT_GAME,                                                                           // ���û���ڻỰ��

	ERR_GAME_BASE = 2000,
	ERR_GAME_INVALID,                                                                              // ��Ϸ��Ч
	ERR_GAME_INVALID_ID,                                                                           // ��Ϸ��ЧID
	ERR_GAME_USING,                                                                                // ��Ϸ��
	ERR_GAME_EMPTY,                                                                                // ��Ϸ��
	ERR_GAME_FULL,                                                                                 // ��Ϸ��
	ERR_GAME_PASSWORD,                                                                             // �Ự�������

	ERR_SERVER_BASE = 3000,
	ERR_SERVER_FULL,                                                                               // ��������

	ERR_GAMEPLAY_BASE = 4000,
	ERR_GAMEPLAY_INVALID_MODE,                                                                     // ��Ч��Ϸģʽ
};
