//#pragma once
//
//
//
//
//#pragma pack(push,1)
//struct PacketHead
//{
//	short TotalSize;
//	short Id;
//};
//
//struct SendPacketInfo
//{
//	int Index = 0;
//	short PacketId = 0;
//	short PacketBodySize = 0;
//	char* pRefData = 0;
//};
//
//
//
//
////- 로그인 요청
//const int MAX_USER_ID_SIZE = 16;
//const int MAX_USER_PASSWORD_SIZE = 16;
//
//// 클라가 서버한테 보내는 패킷
//struct Packet_Login_CtoS
//{
//	char szID[MAX_USER_ID_SIZE + 1] = { 0, };
//	char szPW[MAX_USER_PASSWORD_SIZE + 1] = { 0, };
//};
//
//// 서버가 클라한테 보내는 패킷
//struct Packet_Login_ServerToClient
//{
//
//};
//
//const int PACKET_HEADER_SIZE = sizeof(PacketHead);
//
//
//struct PktNtfSysCloseSession : PacketHead
//{
//	int SockFD;
//};
//#pragma pack(pop)
