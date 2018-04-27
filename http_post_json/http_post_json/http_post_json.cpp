// http_post_json.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "C_http.h"
#include "rapidjson\document.h"

int main (int argc, char *argv[])
{
	char ServerURL[50] = "http://127.0.0.1:9000/auth_reg.php";
	char IP[] = "127.0.0.1";
	unsigned short ServerPORT = 9000;
	char BODY[1024] = "{\"id\":\"aaa\", \"pass\":\"aaa\", \"nickname\":\"aaa\"}";

	char retBody[BUFFSIZE];
	http_POST http (ServerURL, IP, ServerPORT);
	int retval = http.POST (BODY, retBody);
	if ( retval != 200 )
	{
		wprintf (L"code = %d, http post not complete", retval);
	}
	return 0;

}