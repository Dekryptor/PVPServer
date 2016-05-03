#include "CacheBuffer.h"


CCacheBuffer::CCacheBuffer(void)
	: m_totalBufferSize(0)
	, m_activeBufferSize(0)

	, m_bAllocBuffer(FALSE)
	, m_pBuffer(NULL)
	, m_pPushPointer(NULL)
	, m_pPopPointer(NULL)
{
	pthread_mutex_init(&m_mutex, NULL);
}

CCacheBuffer::CCacheBuffer(size_t size)
	: m_totalBufferSize(0)
	, m_activeBufferSize(0)

	, m_bAllocBuffer(FALSE)
	, m_pBuffer(NULL)
	, m_pPushPointer(NULL)
	, m_pPopPointer(NULL)
{
	//
	// 1. ��ʼ��������
	//
	pthread_mutex_init(&m_mutex, NULL);

	//
	// 2. �������ݻ���
	//
	AllocBuffer(size, NULL);
}

CCacheBuffer::CCacheBuffer(size_t size, unsigned char *pBuffer)
	: m_totalBufferSize(0)
	, m_activeBufferSize(0)

	, m_bAllocBuffer(FALSE)
	, m_pBuffer(NULL)
	, m_pPushPointer(NULL)
	, m_pPopPointer(NULL)
{
	//
	// 1. ��ʼ��������
	//
	pthread_mutex_init(&m_mutex, NULL);

	//
	// 2. �������ݻ���
	//
	AllocBuffer(size, pBuffer);
}

CCacheBuffer::~CCacheBuffer(void)
{
	//
	// 1. �ͷŻ���
	//
	if (m_bAllocBuffer && m_pBuffer) {
		free(m_pBuffer);
	}

	//
	// 2. ���ٻ�����
	//
	pthread_mutex_destroy(&m_mutex);
}

//
// ���仺��
//
void CCacheBuffer::AllocBuffer(size_t size, unsigned char *pBuffer)
{
	//
	// 1. �ͷŻ���
	//
	if (m_pBuffer && m_bAllocBuffer) {
		free(m_pBuffer);

		m_pBuffer = NULL;
		m_bAllocBuffer = FALSE;
	}

	//
	// 2. ���仺��
	//
	m_totalBufferSize = size;

	m_bAllocBuffer = pBuffer == NULL ? TRUE : FALSE;
	m_pBuffer = pBuffer == NULL ? (unsigned char *)malloc(size) : pBuffer;
	m_pPopPointer = m_pBuffer;
	m_pPushPointer = m_pBuffer;

	memset(m_pBuffer, 0xc, m_totalBufferSize);
}

//
// ����
//
void CCacheBuffer::Lock(void)
{
	pthread_mutex_lock(&m_mutex);
}

//
// ����
//
void CCacheBuffer::Unlock(void)
{
	pthread_mutex_unlock(&m_mutex);
}

//
// ��û����ܳߴ�
//
size_t CCacheBuffer::GetTotalBufferSize(void)
{
	return m_totalBufferSize;
}

//
// ��û����ߴ�
//
size_t CCacheBuffer::GetActiveBufferSize(void)
{
	return m_activeBufferSize;
}

//
// ��ÿ��û���ߴ�
//
size_t CCacheBuffer::GetFreeBufferSize(void)
{
	return m_totalBufferSize - m_activeBufferSize;
}

//
// ��ջ���
//
void CCacheBuffer::ClearBuffer(void)
{
	if (m_pBuffer) {
		m_activeBufferSize = 0;

		m_pPopPointer = m_pBuffer;
		m_pPushPointer = m_pBuffer;

		memset(m_pBuffer, 0xc, m_totalBufferSize);
	}
}

//
// ѹ������
//
int CCacheBuffer::PushData(unsigned char *pData, size_t size)
{
	//
	// 1. ������
	//
	if (pData == NULL || m_pBuffer == NULL) {
		return -1;
	}

	if (size == 0 || size > GetFreeBufferSize()) {
		return 0;
	}

	//
	// 2. ѹ������
	//
	size_t sizeParts[2];

	sizeParts[0] = min(m_totalBufferSize + (size_t)(m_pBuffer - m_pPushPointer), size);
	sizeParts[1] = size - sizeParts[0];

	_PushData(pData, sizeParts[0]);
	_PushData(pData + sizeParts[0], sizeParts[1]);

	return (int)size;
}

void CCacheBuffer::_PushData(unsigned char *pData, size_t size)
{
	memcpy(m_pPushPointer, pData, size);

	if (m_pPushPointer + size < m_pBuffer + m_totalBufferSize) {
		m_pPushPointer += size;
	}
	else {
		m_pPushPointer = m_pBuffer;
	}

	m_activeBufferSize += size;
}

//
// ��������
//
int CCacheBuffer::PopData(unsigned char *pData, size_t size)
{
	//
	// 1. ������
	//
	if (pData == NULL || m_pBuffer == NULL) {
		return -1;
	}

	if (size == 0 || size > GetActiveBufferSize()) {
		return 0;
	}

	//
	// 2. ��������
	//
	size_t sizeParts[2];

	sizeParts[0] = min(m_totalBufferSize + (size_t)(m_pBuffer - m_pPopPointer), size);
	sizeParts[1] = size - sizeParts[0];

	_PopData(pData, sizeParts[0]);
	_PopData(pData + sizeParts[0], sizeParts[1]);

	return (int)size;
}

//
// �������
//
int CCacheBuffer::GetData(unsigned char *pData, size_t size)
{
	//
	// 1. ������
	//
	if (pData == NULL || m_pBuffer == NULL) {
		return -1;
	}

	if (size == 0 || size > GetActiveBufferSize()) {
		return 0;
	}

	//
	// 2. �������
	//
	size_t sizeParts[2];

	sizeParts[0] = min(m_totalBufferSize + (size_t)(m_pBuffer - m_pPopPointer), size);
	sizeParts[1] = size - sizeParts[0];

	size_t activeBufferSize = m_activeBufferSize;
	unsigned char *pPopPointer = m_pPopPointer;
	{
		_PopData(pData, sizeParts[0]);
		_PopData(pData + sizeParts[0], sizeParts[1]);
	}
	m_activeBufferSize = activeBufferSize;
	m_pPopPointer = pPopPointer;

	return (int)size;
}

void CCacheBuffer::_PopData(unsigned char *pData, size_t size)
{
	memcpy(pData, m_pPopPointer, size);

	if (m_pPopPointer + size < m_pBuffer + m_totalBufferSize) {
		m_pPopPointer += size;
	}
	else {
		m_pPopPointer = m_pBuffer;
	}

	m_activeBufferSize -= size;
}
