#pragma once

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <atlstr.h>

#include "platform.h"
#include "IPolicyConfig.h"

#include <iostream>
#include <sstream>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

char setDefaultDevice(IMMDeviceEnumerator* enumerator, EDataFlow dataFlowFilter, DWORD stateMaskFilter, LPWSTR setDevice);

int getAudioEndpointCount(IMMDeviceEnumerator* enumerator);

LPWSTR getAudioEndpointID(IMMDeviceEnumerator* enumerator, int num);

LPWSTR tryGetDefaultAudioEndpoint(IMMDeviceEnumerator* enumerator, EDataFlow dataFlow, ERole role);

string getAudioEndpointName(IMMDeviceEnumerator* enumerator, int num);