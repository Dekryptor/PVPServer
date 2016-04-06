#pragma once


#include <stdio.h>
#include <stdlib.h>

#include "pthread.h"
#include "semaphore.h"
#include "Common.h"


#ifdef __cplusplus
extern "C" {
#endif


	typedef struct {
		BOOL flag;                                                                                 // ������־
		pthread_cond_t cond;                                                                       // ������
		pthread_mutex_t mutex;                                                                     // ������
	} event_t;


	_ServerExport void event_init(event_t *event);                                                 // ��ʼ��
	_ServerExport void event_destroy(event_t *event);                                              // ����
	_ServerExport void event_set(event_t *event);                                                  // ����
	_ServerExport void event_reset(event_t *event);                                                // ����
	_ServerExport void event_wait(event_t *event);                                                 // �ȴ�
	_ServerExport void event_timedwait(event_t *event, int sec, int nsec);                         // �ȴ�


#ifdef __cplusplus
}
#endif
