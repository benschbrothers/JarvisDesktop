#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#include "audioController.h"
#include "restController.h"
#include "console.h"

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

	int conInt = 0;

	initAudioController();

	console(0, 9, "--------------------------------------\n");
	console(1, 10, "Get default audio Endpoint\n");
	console(3, 9, "--------------------------------------\n");

	startDevice = getDefaultAudioEndpoint();

	console(4,9,"--------------------------------------\n");
	console(5,10,"Enum audio Endpoints \n");

	numDevices = getAudioEndpointCount();
	string conOut = "Found " + to_string(numDevices) + " devices";
	console(6, 10, conOut);

	conInt = 7;

	for (int i = 0; i < numDevices; i++) {
		audioIDs[i] = getAudioEndpointID( i);
		audioNames[i] = getAudioEndpointName( i);
		//cout << "AudioName: " << audioNames[i] << endl;
		string conOut = to_string(i) + " Device: " + audioNames[i];
		console(conInt, 10, conOut);
		conInt++;

		if (wcscmp(audioIDs[i], startDevice) == 0)
			currentDevice = i;
	}
	console(conInt, 9, "--------------------------------------\n");
	conInt++;

	console(conInt, 10, "current Device: " + audioNames[currentDevice] + " number " + to_string(currentDevice+1) + " of " + to_string(numDevices));

	while (true) {
		string response;
		response = HTTPReq("GET", "taskmanager.zapto.org", 80, "/", NULL);
		console(conInt + 1, 10, "HTTP GET Information");
		char endch = response.back();
		int input = endch - '0';
		input--;
		console(conInt + 2, 10,  "Set Device: "+ audioNames[input]);
		console(conInt + 3, 10,	 "Current Device: " + audioNames[currentDevice]);

		if (input != currentDevice) {
			console(conInt + 4, 12, "NOT SAME CHANGING");
			setDefaultAudioEndpoint(audioIDs[input]);
			currentDevice = input;
			clearConsole(conInt + 1);
			clearConsole(conInt + 2);
			clearConsole(conInt + 3);
			clearConsole(conInt + 4);
		}
		else {
			console(conInt + 4, 10, "SAME ");
		}

		Sleep(500);
	}

	return 0;
}