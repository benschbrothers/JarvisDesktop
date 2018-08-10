
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>

#include <stdio.h>
#include <Mmdeviceapi.h>
#include <Audioclient.h>

#include "platform.h"

#include "IPolicyConfig.h"

#include <iostream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")


using namespace std;

#define REFTIMES_PER_MS 100000


void HTTPReq(
	const char* verb,
	const char* hostname,
	int port,
	const char* resource,
	const char* opt_urlencoded,
	string& response)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup failed.\n";
		exit(1);
	}

	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct hostent *host;
	//host = gethostbyname(hostname);
	host = gethostbyname("taskmanager.zapto.org");

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

  // 0 = success
char testAudioDevice(IMMDevice* device)
{
	HRESULT hr;

	IAudioClient* client;
	hr = device->Activate(__uuidof(IAudioClient),
		CLSCTX_ALL,
		NULL,
		(void**)&client);
	if (hr) { printHResult("IMMDevice.Activate failed", hr); return 1; }

	WAVEFORMATEX* waveFormat;
	hr = client->GetMixFormat(&waveFormat);
	if (hr) { printHResult("IAudioClient.GetMixFormat failed", hr); return 1; }

	printWaveFormat(waveFormat);

	hr = client->Initialize(AUDCLNT_SHAREMODE_SHARED,
		0,
		REFTIMES_PER_MS * 100,
		0,
		waveFormat,
		NULL);
	if (hr) { printHResult("IAudioClient.Initialize failed", hr); return 1; }

	//
	// TODO: write code to render audio to the device
	//
	{
		//if(
	}

	return 0;
}

const char* debug_GetDeviceStateString(DWORD state)
{
	switch (state) {
	case DEVICE_STATE_ACTIVE: return "ACTIVE";
	case DEVICE_STATE_DISABLED: return "DISABLED";
	case DEVICE_STATE_NOTPRESENT: return "NOTPRESENT";
	case DEVICE_STATE_UNPLUGGED: return "UNPLUGGED";
	default: return "DEVICE_STATE_UNKNOWN";
	}
}
// returns 0 on success
char printMMDevice(IMMDevice* device)
{
	HRESULT hr;

	{
		LPWSTR id;
		hr = device->GetId(&id);
		if (hr) { printHResult("IMMDevice.GetId failed", hr); return 1; }
		wprintf(L"    device->GetID    = '%s'\n", id);
	}
	{
		DWORD state;
		hr = device->GetState(&state);
		if (hr) { printHResult("IMMDevice.GetState failed", hr); return 1; }
		printf("    device->GetState = %s\n", debug_GetDeviceStateString(state));
	}
	return 0;
}

// returns 0 on success
char setDefaultDevice(IMMDeviceEnumerator* enumerator,
	EDataFlow dataFlowFilter,
	DWORD stateMaskFilter, int nummer)
{
	HRESULT hr;

	IMMDeviceCollection* devices;

	hr = enumerator->EnumAudioEndpoints(dataFlowFilter, stateMaskFilter, &devices);
	if (hr) { printHResult("EnumAudioEndpoints failed", hr); return 1; }

	UINT deviceCount;
	hr = devices->GetCount(&deviceCount);
	if (hr) { printHResult("IMMDeviceCollection->GetCount failed", hr); return 1; }

	printf("Found %d devices\n", deviceCount);

	for (UINT i = 0; i < deviceCount; i++) {
		IMMDevice* device;
		hr = devices->Item(i, &device);
		if (hr) { printHResult("IMMDeviceCollection->Item failed", hr); return 1; }

		printf("  Device %d:\n", i);
		//printMMDevice(device);

		//testAudioDevice(device);

		if (i == nummer) {
			LPWSTR devID;
			hr = device->GetId(&devID);
			IPolicyConfigVista *pPolicyConfig;
			HRESULT hrn = CoCreateInstance(
				__uuidof(CPolicyConfigVistaClient),
				NULL,
				CLSCTX_ALL,
				__uuidof(IPolicyConfigVista),
				(LPVOID *)&pPolicyConfig
			);

			if (SUCCEEDED(hrn))
			{
				hrn = pPolicyConfig->SetDefaultEndpoint(devID, eConsole);
				pPolicyConfig->Release();
			}

		}
		device->Release();
	}
	devices->Release();

	return 0;
}

// return 0 on success
LPWSTR tryGetDefaultAudioEndpoint(IMMDeviceEnumerator* enumerator, EDataFlow dataFlow, ERole role)
{
	HRESULT hr;

	IMMDevice* device;

	printf("tryGetDefaultAudioEndpoint(%d,%d)\n", dataFlow, role);

	hr = enumerator->GetDefaultAudioEndpoint(dataFlow,
		role,
		&device);
	if (hr) {
		printHResult("GetDefaultAudioEndpoint failed", hr);
		//return 1; 
	}

	printf("  DefaultDevice:\n");

	LPWSTR id;
	hr = device->GetId(&id);

	return id;
}
// returns 0 on success
int tryEnumAudioEndpoints(IMMDeviceEnumerator* enumerator,
	EDataFlow dataFlowFilter,
	DWORD stateMaskFilter,
	LPWSTR outDef)
{
	HRESULT hr;

	IMMDeviceCollection* devices;

	hr = enumerator->EnumAudioEndpoints(dataFlowFilter, stateMaskFilter, &devices);
	if (hr) {
		printHResult("EnumAudioEndpoints failed", hr);
		//return 1; 
	}

	UINT deviceCount;
	hr = devices->GetCount(&deviceCount);
	if (hr) {
		printHResult("IMMDeviceCollection->GetCount failed", hr);
		//return 1; 
	}

	printf("Found %d devices\n", deviceCount);

	int result = 0;

	for (UINT i = 0; i < deviceCount; i++) {
		IMMDevice* device;
		hr = devices->Item(i, &device);
		if (hr) { printHResult("IMMDeviceCollection->Item failed", hr); return 1; }

		printf("  Device %d:\n", i);
		//printMMDevice(device);
		LPWSTR id;
		hr = device->GetId(&id);

		wprintf(L"    device->GetID    = '%s'\n", id);
		wprintf(L"    device->GetID    = '%s'\n", outDef);

		if (wcscmp(id, outDef) == 0) {
			cout << " FOUND: " << i;
			result = i;
		}

		//testAudioDevice(device);
	}

	return result;
}
int main(int args, char* argv[])
{
	HRESULT hr;
	const char* error;
	IMMDeviceEnumerator *enumerator;

	LPWSTR defOut;

	int currentDevice = 0;

	// Initializes the COM library.  Required before calling any Co methods.
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr) { printHResult("CoInitializeEx failed", hr); return 1; }

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
		NULL,
		CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&enumerator);
	if (hr) { printHResult("CoCreateInstance failed", hr); return 1; }

	printf("--------------------------------------\n");
	printf("GetDefaultAudioEndpoint:\n");
	printf("--------------------------------------\n");
	defOut = tryGetDefaultAudioEndpoint(enumerator, eRender, eConsole);


	printf("--------------------------------------\n");
	printf("EnumAudioEndpoints:\n");
	printf("--------------------------------------\n");

	int aRes = 0;
	//aRes = tryEnumAudioEndpoints(enumerator, eAll, DEVICE_STATEMASK_ALL);
	//aRes = tryEnumAudioEndpoints(enumerator, eRender, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED);
	currentDevice = tryEnumAudioEndpoints(enumerator, eRender, DEVICE_STATE_ACTIVE, defOut);
	cout << "current Device: " << currentDevice << endl;
	cin >> aRes;

	int choice;
	while (true) {
		string response;
		HTTPReq("GET", "taskmanager.zapto.org", 80, "/", NULL, response);

		char endch = response.back();
		int input = endch - '0';
		cout << input;

		cout << "Current Device: " << currentDevice << "\n";

		if (input != currentDevice) {
			cout << "NOT SAME CHANGING";
			//setDefaultDevice(enumerator, eRender, DEVICE_STATE_ACTIVE, input);
			currentDevice = input;
		}
		else {
			cout << "SAME SAME";
		}

		//setDefaultDevice(enumerator, eRender, DEVICE_STATE_ACTIVE, choice);

		Sleep(500);
		//cin >> choice;

	}


	return 0;
}