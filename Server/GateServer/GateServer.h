#pragma once


#include <map>
#include <vector>
#include <string>
#include "IOCP.h"
#include "Serializer.h"


//
// ������
//
class CGateServer : public CIOCPServer
{
	// ���ݽṹ
public:


	// ����/��������
public:
	CGateServer(void);
	virtual ~CGateServer(void);


	// ����
public:


	// ����
protected:
};

extern void WriteLog(const char *szFmt, ...);                                                      // �����־
