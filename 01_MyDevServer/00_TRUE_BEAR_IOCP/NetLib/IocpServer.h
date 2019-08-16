#pragma once

#include "PreComplie.h"

#define	MAX_BUFFER		1024
#define SERVER_PORT		8000



struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;
};


class IocpServer :public NetBase
{
public:
	IocpServer();
	~IocpServer();

	// 소켓 등록 및 서버 정보 설정
	bool InitAndCreateSocket();
	bool Run();



	// 작업 스레드 생성
	//bool CreateWorkerThread();


	// 작업 스레드
	void AcceptThread();
	void WorkerThread();

	void CloseServer();


private:
	stSOCKETINFO*   m_pSocketInfo;		// 소켓 정보
	SOCKET			m_listenSocket;		// 서버 리슨 소켓
	HANDLE			m_hIOCP;			// IOCP 객체 핸들
	bool			m_bAccept;			// 요청 동작 플래그
	bool			m_bWorkerThread;	// 작업 스레드 동작 플래그
	HANDLE* m_pWorkerHandle;	// 작업 스레드 핸들

private:
};
