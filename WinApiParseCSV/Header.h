#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#define PATH L"Users.csv"
struct User
{
    WCHAR f[30];
    WCHAR i[30];
    WCHAR o[30];
    DWORD vozr;
};
typedef struct User U;