#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#include "audioController.h"
#include "restController.h"

#include <iostream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define REFTIMES_PER_MS 100000

int main(int args, char* argv[])
{
	HRESULT hr;
	LPWSTR startDevice;
	int numDevices;
	int currentDevice;
	string audioNames[15];
	LPWSTR audioIDs[15];

	initAudioController();

	printf("--------------------------------------\n");
	printf("GetDefaultAudioEndpoint:\n");
	printf("--------------------------------------\n");

	startDevice = getDefaultAudioEndpoint();

	printf("--------------------------------------\n");
	printf("EnumAudioEndpoints:\n");
	printf("--------------------------------------\n");

	numDevices = getAudioEndpointCount();

	for (int i = 0; i < numDevices; i++) {
		audioIDs[i] = getAudioEndpointID( i);
		audioNames[i] = getAudioEndpointName( i);
		cout << "AudioName: " << audioNames[i] << endl;

		if (wcscmp(audioIDs[i], startDevice) == 0)
			currentDevice = i;
	}

	cout << "current Device: " << audioNames[currentDevice] << " number " << currentDevice+1 << " of " << numDevices << endl;

	while (true) {
		string response;
		response = HTTPReq("GET", "taskmanager.zapto.org", 80, "/", NULL);
		char endch = response.back();
		int input = endch - '0';
		input--;
		cout << "has to be " << audioNames[input] << endl;

		cout << "Current Device: " << audioNames[currentDevice] << endl;

		if (input != currentDevice) {
			cout << "NOT SAME CHANGING";
			setDefaultAudioEndpoint(audioIDs[input]);
			currentDevice = input;
		}
		else {
			cout << "SAME";
		}

		Sleep(500);
	}

	return 0;
}