#pragma once

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

void initAudioController();

char setDefaultAudioEndpoint(LPWSTR setDevice);

int getAudioEndpointCount();

LPWSTR getAudioEndpointID(int num);

LPWSTR getDefaultAudioEndpoint();

string getAudioEndpointName(int num);