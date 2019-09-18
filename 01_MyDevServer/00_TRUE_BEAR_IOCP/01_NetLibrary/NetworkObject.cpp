#include "stdafx.h"
#ifdef _WIN32
#include <rpc.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include "NetworkObject.h"
#include "Endpoint.h"
#include "SocketInit.h"
#include "Exception.h"


using namespace std;

std::string GetLastErrorAsString();

// ������ �����ϴ� ������.
NetworkObject::NetworkObject()
{
	g_socketInit.Touch();
	m_fd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&m_readOverlappedStruct, sizeof(m_readOverlappedStruct));
	ZeroMemory(&m_writeOverlappedStruct, sizeof(m_writeOverlappedStruct));

}


// �ܺ� ���� �ڵ��� �޴� ������.
NetworkObject::NetworkObject(SOCKET fd)
{
	g_socketInit.Touch();
	m_fd = fd;
	ZeroMemory(&m_readOverlappedStruct, sizeof(m_readOverlappedStruct));
	ZeroMemory(&m_writeOverlappedStruct, sizeof(m_writeOverlappedStruct));
}

NetworkObject::~NetworkObject()
{
	Close();
}

void NetworkObject::BindAndListen(const Endpoint& endpoint)
{
	if (bind(m_fd, (sockaddr*)&endpoint.m_ipv4Endpoint, sizeof(endpoint.m_ipv4Endpoint)) < 0)
	{
		stringstream ss;
		ss << "bind failed:" << GetLastErrorAsString();
		throw Exception(ss.str().c_str());
	}

	if (listen(m_fd, SOMAXCONN) == SOCKET_ERROR)
	{
		stringstream ss;
		ss << "listen failed:" << GetLastErrorAsString();
		throw Exception(ss.str().c_str());
	}

}



// TODO : �̰� �񵿱�� �ٲ����... ��������
int NetworkObject::Send(const char* data, int length)
{
	//memcpy_s(sendContext->mBuffer, BUFSIZE, buf, len);

	ZeroMemory(&m_writeOverlappedStruct, sizeof(OVERLAPPED));

	WSABUF b;
	b.buf = (char*)data;
	b.len = MaxSendLength;

	m_writeFlag = 0;
	DWORD sendByte = 0;

	return WSASend(
		m_fd,
		&b,
		1,
		&b.len,
		m_writeFlag,
		&m_writeOverlappedStruct,
		NULL
	);


//	return ::send(m_fd, data, length, 0);
}

void NetworkObject::Close()
{
	closesocket(m_fd);
}


#ifdef _WIN32

// acceptCandidateSocket���� �̹� ������� ���� �ڵ��� ����, accept�� �ǰ� ���� �� ���� �ڵ��� TCP ���� ��ü�� �����մϴ�.
bool NetworkObject::AcceptOverlapped(NetworkObject& acceptCandidateSocket, string& errorText)
{
	if (AcceptEx == NULL)
	{
		DWORD bytes;
		// AcceptEx�� ��Ÿ �����Լ��� �޸� ���� ȣ���ϴ� ���� �ƴϰ�,
		// �Լ� �����͸� ���� ������ ���� ȣ���� �� �ִ�. �װ��� ���⼭ �Ѵ�.
		WSAIoctl(m_fd,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&UUID(WSAID_ACCEPTEX),
			sizeof(UUID),
			&AcceptEx,
			sizeof(AcceptEx),
			&bytes,
			NULL,
			NULL);

		if (AcceptEx == NULL)
		{
			throw Exception("Getting AcceptEx ptr failed.");
		}
	}


	// ���⿡�� accept�� ������ �����ּҿ� ����Ʈ�ּҰ� ä�����ϴٸ� �� �������� ���ڵ鿡�� �������� ������ ����Ƿ� �׳� �����ϴ�.
	char ignored[200];
	DWORD ignored2 = 0;

	bool ret = AcceptEx(m_fd,
		acceptCandidateSocket.m_fd,
		&ignored,
		0,
		50,
		50,
		&ignored2,
		&m_readOverlappedStruct
	) == TRUE;
	
	return ret;
}


// AcceptEx�� I/O �ϷḦ �ϴ��� ���� TCP ���� �ޱ� ó���� �� ���� ���� �ƴϴ�.
// �� �Լ��� ȣ�����־�߸� �Ϸᰡ �ȴ�.
int NetworkObject::FinishAcceptEx(NetworkObject& listenSocket)
{
	sockaddr_in ignore1;
	sockaddr_in ignore3;
	INT ignore2,ignore4;

	char ignore[1000];
	GetAcceptExSockaddrs(ignore,
		0,
		50,
		50,
		(sockaddr**)&ignore1,
		&ignore2,
		(sockaddr**)&ignore3,
		&ignore4);

	return setsockopt(m_fd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(char*)&listenSocket.m_fd, sizeof(listenSocket.m_fd));
}

#endif // _WIN32



#ifdef _WIN32

// overlapeed ������ �̴ϴ�. �� ��׶���� ���� ó���� �մϴ�.
// ���ŵǴ� �����ʹ� m_receiveBuffer�� �񵿱�� ä�����ϴ�.
// ���ϰ�: WSARecv�� ���ϰ� �״���Դϴ�.
int NetworkObject::ReceiveOverlapped()
{
	WSABUF b;
	b.buf = m_receiveBuffer;
	b.len = MaxReceiveLength;

	// overlapped I/O�� ����Ǵ� ���� ���� ���� ä�����ϴ�.
	m_readFlags = 0;

	return WSARecv(m_fd, &b, 1, NULL, &m_readFlags, &m_readOverlappedStruct, NULL);
}

#endif

// �ͺ��� �������� ��带 �����մϴ�.
void NetworkObject::SetNonblocking()
{
	u_long val = 1;
#ifdef _WIN32
	int ret = ioctlsocket(m_fd, FIONBIO, &val);
#else
	int ret = ioctl(m_fd, FIONBIO, &val);
#endif
	if (ret != 0)
	{
		stringstream ss;
		ss << "bind failed:" << GetLastErrorAsString();
		throw Exception(ss.str().c_str());
	}
}

std::string GetLastErrorAsString()
{
#ifdef _WIN32
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

#else 
	std::string message = strerror(errno);
#endif
	return message;
}