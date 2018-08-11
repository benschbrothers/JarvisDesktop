#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//---------- API-Includes ---------
#include <windows.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>

using namespace std;

//---------- Klassen ----------
void console(int y, int color, string write);
void clearConsole(int line);