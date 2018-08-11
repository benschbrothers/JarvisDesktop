#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma once

#include <string>
#include <iostream>
#include <winsock2.h>
#include <windows.h>


using namespace std;

string HTTPReq(const char* verb, const char* hostname, int port, const char* resource, const char* opt_urlencoded);
