#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <atlstr.h>

#include "audioController.h"

#include "platform.h"
#include "IPolicyConfig.h"

#include <iostream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define REFTIMES_PER_MS 100000

void HTTPReq(const char* verb,	const char* hostname,	int port,	const char* resource,	const char* opt_urlencoded,	string& response)
{
	WSADATA wsaData;
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

} // HTTPReq


int main(int args, char* argv[])
{
	HRESULT hr;
	IMMDeviceEnumerator *enumerator;
	LPWSTR startDevice;
	int numDevices;
	int currentDevice;
	string audioNames[15];
	LPWSTR audioIDs[15];

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr) { printHResult("CoInitializeEx failed", hr); return 1; }

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);

	if (hr) { printHResult("CoCreateInstance failed", hr); return 1; }

	printf("--------------------------------------\n");
	printf("GetDefaultAudioEndpoint:\n");
	printf("--------------------------------------\n");

	startDevice = tryGetDefaultAudioEndpoint(enumerator, eRender, eConsole);

	printf("--------------------------------------\n");
	printf("EnumAudioEndpoints:\n");
	printf("--------------------------------------\n");

	numDevices = getAudioEndpointCount(enumerator);

	for (int i = 0; i < numDevices; i++) {
		audioIDs[i] = getAudioEndpointID(enumerator, i);
		audioNames[i] = getAudioEndpointName(enumerator, i);
		cout << "AudioName: " << audioNames[i] << endl;

		if (wcscmp(audioIDs[i], startDevice) == 0)
			currentDevice = i;
	}

	cout << "current Device: " << audioNames[currentDevice] << " number " << currentDevice+1 << " of " << numDevices << endl;

	cin >> currentDevice;

	while (true) {
		string response;
		HTTPReq("GET", "taskmanager.zapto.org", 80, "/", NULL, response);

		char endch = response.back();
		int input = endch - '0';
		cout << "has to be " << audioNames[input] << endl;

		cout << "Current Device: " << audioNames[currentDevice] << endl;

		if (input != currentDevice) {
			cout << "NOT SAME CHANGING";
			setDefaultDevice(enumerator, eRender, DEVICE_STATE_ACTIVE, audioIDs[input]);
			currentDevice = input;
		}
		else {
			cout << "SAME";
		}

		Sleep(500);

	}

	return 0;
}