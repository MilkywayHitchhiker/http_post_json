#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <Strsafe.h>
#include <string.h>

#include <Winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#define RECV_SEND_Sec 3
#define RECV_SEND_usec 0
#define BUFFSIZE 1024

struct http_POST
{
private :
	SOCKET sock;
	char _ServerIP[19];
	unsigned short _ServerPORT;
	int ContentLength;
	char Header[512];
	char _URL[BUFFSIZE];
	char BODY[BUFFSIZE];
	char Fullhttp[BUFFSIZE * 2];

public :
	//생성과 동시에 연결하고 전송하도록 구성됨.
	http_POST (char *url,char *IP, unsigned short ServerPORT)
	{
		strcpy_s (_ServerIP, IP);
		strcpy_s (_URL, url);
		_ServerPORT = ServerPORT;
		if ( Connection (IP, ServerPORT) == false )
		{
			wprintf (L"Connection 실패\n");
			return;
		}
		


	}

	//파괴자.
	~http_POST ()
	{
		closesocket (sock);
	}

	BOOL Connection (char *IP, unsigned short ServerPORT)
	{
		WSADATA wsa;
		struct timeval tv;

		//TCP 소켓 생성.
		if ( WSAStartup (MAKEWORD (2, 2), &wsa) != 0 )
		{
			wprintf (L"윈속 초기화 오류\n");
			return false;
		}

		//socket()
		sock = socket (AF_INET, SOCK_STREAM, 0);
		if ( sock == INVALID_SOCKET )
		{
			wprintf (L"소켓 오류\n");
			return false;
		}
		SOCKADDR_IN serveraddr;

		ZeroMemory (&serveraddr, sizeof (serveraddr));
		inet_pton (AF_INET, IP, &serveraddr.sin_addr);
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons (ServerPORT);

		//SEND, RECV 타임아웃 설정.
		tv.tv_sec = RECV_SEND_Sec;
		tv.tv_usec = RECV_SEND_usec;

		setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, ( char * )&tv, sizeof (struct timeval));
		setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, ( char * )&tv, sizeof (struct timeval));

		//Connection
		int retval = connect (sock, ( SOCKADDR * )&serveraddr, sizeof (serveraddr));
		if ( retval == SOCKET_ERROR )
		{
			wprintf (L"connect 오류\n");
			return false;
		}
		return true;
	}

	//완료코드 및 recv된 body
	int POST (char *BODY,char *retBody = NULL)
	{
		ContentLength = strlen (BODY);

		//HTTP 프로토콜 데이터 생성.

		memset (Header, 0, BUFFSIZE);
		sprintf_s<sizeof (Header)> (Header, "POST %s HTTP/1.1\r\nUser-Agent: XXXXXXX\r\nHost: %s:%d\r\nConnection: Close\r\nContent-Length: %d\r\n\r\n%s", _URL, _ServerIP, _ServerPORT, ContentLength, BODY);

		int retval;
		//Send
		retval = send (sock, ( char * )Header, sizeof (Header), 0);
		if ( retval == SOCKET_ERROR )
		{
			wprintf (L"\nsend 오류\n");
			return -1;
		}

		//recv
		char recvBuffer[BUFFSIZE];
		char *pCompileCode;
		retval = recv (sock, recvBuffer, sizeof (recvBuffer), 0);
		if ( retval == SOCKET_ERROR )
		{
			wprintf (L"\nrecv 오류\n");
			return -1;
		}

		//http 완료 코드 찾기.
		for ( pCompileCode = recvBuffer; *pCompileCode != 0x20; ++pCompileCode );
		char *Context = NULL;
		char *token = strtok_s (pCompileCode, " ", &Context);
		if ( strcmp (token, "200") != 0 )
		{
			return atoi (token);
		}

		return atoi (token);
	}

};