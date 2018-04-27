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
	//������ ���ÿ� �����ϰ� �����ϵ��� ������.
	http_POST (char *url,char *IP, unsigned short ServerPORT)
	{
		strcpy_s (_ServerIP, IP);
		strcpy_s (_URL, url);
		_ServerPORT = ServerPORT;
		if ( Connection (IP, ServerPORT) == false )
		{
			wprintf (L"Connection ����\n");
			return;
		}
		


	}

	//�ı���.
	~http_POST ()
	{
		closesocket (sock);
	}

	BOOL Connection (char *IP, unsigned short ServerPORT)
	{
		WSADATA wsa;
		struct timeval tv;

		//TCP ���� ����.
		if ( WSAStartup (MAKEWORD (2, 2), &wsa) != 0 )
		{
			wprintf (L"���� �ʱ�ȭ ����\n");
			return false;
		}

		//socket()
		sock = socket (AF_INET, SOCK_STREAM, 0);
		if ( sock == INVALID_SOCKET )
		{
			wprintf (L"���� ����\n");
			return false;
		}
		SOCKADDR_IN serveraddr;

		ZeroMemory (&serveraddr, sizeof (serveraddr));
		inet_pton (AF_INET, IP, &serveraddr.sin_addr);
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons (ServerPORT);

		//SEND, RECV Ÿ�Ӿƿ� ����.
		tv.tv_sec = RECV_SEND_Sec;
		tv.tv_usec = RECV_SEND_usec;

		setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, ( char * )&tv, sizeof (struct timeval));
		setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, ( char * )&tv, sizeof (struct timeval));

		//Connection
		int retval = connect (sock, ( SOCKADDR * )&serveraddr, sizeof (serveraddr));
		if ( retval == SOCKET_ERROR )
		{
			wprintf (L"connect ����\n");
			return false;
		}
		return true;
	}

	//�Ϸ��ڵ� �� recv�� body
	int POST (char *BODY,char *retBody = NULL)
	{
		ContentLength = strlen (BODY);

		//HTTP �������� ������ ����.

		memset (Header, 0, BUFFSIZE);
		sprintf_s<sizeof (Header)> (Header, "POST %s HTTP/1.1\r\nUser-Agent: XXXXXXX\r\nHost: %s:%d\r\nConnection: Close\r\nContent-Length: %d\r\n\r\n%s", _URL, _ServerIP, _ServerPORT, ContentLength, BODY);

		int retval;
		//Send
		retval = send (sock, ( char * )Header, sizeof (Header), 0);
		if ( retval == SOCKET_ERROR )
		{
			wprintf (L"\nsend ����\n");
			return -1;
		}

		//recv
		char recvBuffer[BUFFSIZE];
		char *pCompileCode;
		retval = recv (sock, recvBuffer, sizeof (recvBuffer), 0);
		if ( retval == SOCKET_ERROR )
		{
			wprintf (L"\nrecv ����\n");
			return -1;
		}

		//http �Ϸ� �ڵ� ã��.
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