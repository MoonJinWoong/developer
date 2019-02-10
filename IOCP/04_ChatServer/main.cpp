#include "ConnectionManager.h"


void Init()
{
	sLogConfig LogConfig;
	strncpy(LogConfig.s_szLogFileName, "ChatServer", MAX_FILENAME_LENGTH);
	LogConfig.s_nLogInfoTypes[STORAGE_OUTPUTWND] = LOG_ALL;
	LogConfig.s_nLogInfoTypes[STORAGE_WINDOW] = LOG_ALL;
	LogConfig.s_nLogInfoTypes[STORAGE_FILE] = LOG_ERROR_ALL;
	INIT_LOG(LogConfig);

	INITCONFIG InitConfig;
	InitConfig.nServerPort = 32452;
	InitConfig.nRecvBufCnt = 10;
	InitConfig.nRecvBufSize = 1024;
	InitConfig.nProcessPacketCnt = 1000;
	InitConfig.nSendBufCnt = 10;
	InitConfig.nSendBufSize = 1024;
	InitConfig.nWorkerThreadCnt = 2;
	InitConfig.nProcessThreadCnt = 1;

	
	//if (!IocpServer()->ServerStart(InitConfig))
		//cout << "eee" << endl;

	//IOCPServer *a = new IOCPServer();
	
	//IocpServer* iocp = new IocpServer();
	//IOCPServer io = new IOCPServer();


	// m_pIocpServer->ServferStart

	//IocpServer()->ServerStart(InitConfig);
	 
	//IOCPServer io = new IOCPServer();
	 
	//IOCPServer tmp;


    iocpserver()->ServerStart(InitConfig);
	//cout << "ret : " << ret << endl;
	//IOCPServer * server;
	//server->ServerStart(InitConfig);
	//IOCPServer::ServerStart();

	g_ConnectionManager()->CreateConnection(InitConfig, 10);
	LOG(LOG_INFO_LOW, "���� ����..");
}

void End()
{
	CLOSE_LOG();
	Singleton::releaseAll();
}

int main()
{
	Init();

	std::cout << "Ű�� ������ ����..." << std::endl;
	getchar();

	End();
	
	return 0;
}