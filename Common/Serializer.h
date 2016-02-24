#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.h"
#include "CacheBuffer.h"


class CSerializer
{
	typedef enum { SERIALIZER_NONE = -1, SERIALIZER_WRITTING, SERIALIZER_READING } STATE;


	// ����/��������
public:
	CSerializer(void);
	virtual ~CSerializer(void);


	// ����
public:
	void BeginWriting(CCacheBuffer *pBuffer, BOOL bLock = FALSE);                                  // ��ʼд��
	void EndWriting(BOOL bUnlock = FALSE);                                                         // ����д��
	void Write(unsigned char *pData, int size);                                                    // д��

	void BeginReading(CCacheBuffer *pBuffer, BOOL bLock = FALSE);                                  // ��ʼ��ȡ
	void EndReading(BOOL bUnlock = FALSE);                                                         // ������ȡ
	void Read(unsigned char *pData, int size);                                                     // ��ȡ

	BOOL IsWritting(void) const;                                                                   // ���д��״̬
	BOOL IsReading(void) const;                                                                    // ��ö�ȡ״̬

public:
	template <typename T>
	CSerializer& operator << (const T& x)                                                          // д��
	{
		if (m_pWriteBuffer) {
			m_pWriteBuffer->PushData((unsigned char *)&x, sizeof(x));
		}

		return *this;
	}

	CSerializer& operator << (const char *x)                                                       // д���ַ���
	{
		if (m_pWriteBuffer) {
			int len = (int)strlen(x);

			m_pWriteBuffer->PushData((unsigned char *)&len, sizeof(len));
			m_pWriteBuffer->PushData((unsigned char *)x, sizeof(*x) * len);
		}

		return *this;
	}

public:
	template <typename T>
	CSerializer& operator >> (T& x)                                                                // ��ȡ
	{
		if (m_pReadBuffer) {
			m_pReadBuffer->PopData((unsigned char *)&x, sizeof(x));
		}

		return *this;
	}

	CSerializer& operator >> (char *x)                                                             // ��ȡ�ַ���
	{
		if (m_pReadBuffer) {
			int len;

			m_pReadBuffer->PopData((unsigned char *)&len, sizeof(len));
			m_pReadBuffer->PopData((unsigned char *)x, sizeof(*x) * len);
		}

		return *this;
	}


	// ����
protected:
	STATE m_state;                                                                                 // ״̬
	CCacheBuffer *m_pReadBuffer;                                                                   // ������
	CCacheBuffer *m_pWriteBuffer;                                                                  // д����
};
