#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pthread.h"
#include "semaphore.h"
#include "Common.h"


class CCacheBuffer
{
	// ����/��������
public:
	CCacheBuffer(size_t size);
	CCacheBuffer(size_t size, unsigned char *pBuffer);
	virtual ~CCacheBuffer(void);


	// ����
public:
	size_t GetTotalBufferSize(void);                                                               // ��û����ܳߴ�
	size_t GetActiveBufferSize(void);                                                              // ��û����ߴ�
	size_t GetFreeBufferSize(void);                                                                // ��ÿ��û���ߴ�

public:
	void ClearBuffer(void);                                                                        // ��ջ���

	void Lock(void);                                                                               // ����
	void Unlock(void);                                                                             // ����

	unsigned char* GetPushPointer(void);                                                           // ���ѹ��ָ��
	unsigned char* GetPopPointer(void);                                                            // ��õ���ָ��

	int PushData(unsigned char *pData, size_t size);                                               // ѹ������
	int PopData(unsigned char *pData, size_t size);                                                // ��������

protected:
	void _PushData(unsigned char *pData, size_t size);                                             // ѹ������
	void _PopData(unsigned char *pData, size_t size);                                              // ��������


	// ����
protected:
	size_t m_totalBufferSize;                                                                      // �����ܳߴ�
	size_t m_activeBufferSize;                                                                     // �����ߴ�

	int m_bAllocBuffer;                                                                            // ���仺��
	unsigned char *m_pBuffer;                                                                      // ����ָ��
	unsigned char *m_pPushPointer;                                                                 // ѹ��ָ��
	unsigned char *m_pPopPointer;                                                                  // ����ָ��

protected:
	pthread_mutex_t m_mutex;                                                                       // ������
};
