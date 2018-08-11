#include "restController.h"

string HTTPReq(const char* verb, const char* hostname, int port, const char* resource, const char* opt_urlencoded)
{
	WSADATA wsaData;
	string response;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup failed.\n";
		exit(1);
	}

	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct hostent *host;
	host = gethostbyname(hostname);

	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(port);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	cout << "Connecting...\n";

	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0)
	{
		cout << "Could not connect";
		exit(1);
	}
	cout << "Connected.\n";

	// Build request
	string req = verb; // GET | POST
	req.append(" ");

	// Note, on GET, 'resource' must contain the encoded parameters, if any:
	req.append(resource);
	req.append(" HTTP/1.1\r\n");

	req.append("Host: ");
	req.append(hostname);
	req.append(":");
	req.append(to_string(port));
	req.append("\r\n");

	if (strcmp(verb, "POST") == 0)
	{
		req.append("Cache-Control: no-cache\r\n");
		req.append("Content-length: ");
		req.append(to_string(strlen(opt_urlencoded)));
		req.append("\r\n");
		req.append("Content-Type: application/x-www-form-urlencoded\r\n\r\n");

		// User is required to handle URI encoding for this value
		req.append(opt_urlencoded);

	}
	else // default, GET
	{
		req.append("Cache-Control: no-cache\r\n");
		req.append("Connection: close\r\n\r\n");
	}

	cout << "=============================== request"
		<< endl
		<< req
		<< endl
		<< "=============================== "
		<< endl;

	send(Socket, req.c_str(), req.size(), 0);

	char buffer[1024 * 10];
	int nlen;

	while ((nlen = recv(Socket, buffer, 1024 * 10, 0)) > 0)
	{
		response.append(buffer, 0, nlen);
	}
	closesocket(Socket);
	WSACleanup();

	return response;

} // HTTPReq
