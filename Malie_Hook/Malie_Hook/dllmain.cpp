// dllmain.cpp : 定义 DLL 应用程序的入口点。
//For その花が咲いたら、また僕は君に出逢う（Need unpack TheEnigmaProtector. The Version is 5.6）
//目前还会出现大量重复的东西，而且无法替换，哪天有时间了再来看看是怎么回事。
#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include "detours.h"
#pragma comment(lib, "detours.lib")
using namespace std;
ofstream TXT("Text.txt");


char* wtocGBK(LPCTSTR str)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(936, NULL, str, -1, NULL, 0, NULL, FALSE); //计算长度
	char *out = new char[dwMinSize];
	WideCharToMultiByte(936, NULL, str, -1, out, dwMinSize, NULL, FALSE);//转换
	return out;
}

LPWSTR ctowUTF(char* str)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0); //计算长度
	LPWSTR out = new wchar_t[dwMinSize];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, out, dwMinSize);//转换
	return out;
}

char* wtocUTF(LPCTSTR str)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_UTF8, NULL, str, -1, NULL, 0, NULL, FALSE); //计算长度
	char *out = new char[dwMinSize];
	WideCharToMultiByte(CP_UTF8, NULL, str, -1, out, dwMinSize, NULL, FALSE);//转换
	return out;
}

LPWSTR ctowGBK(char* str)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(936, 0, str, -1, NULL, 0); //计算长度
	LPWSTR out = new wchar_t[dwMinSize];
	MultiByteToWideChar(936, 0, str, -1, out, dwMinSize);//转换
	return out;
}

static void make_console() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	cout << "Console\n" << endl;
}

char* (*OldStr)(int a1, int a2, int a3, int a4);
char* __cdecl  MyStr(int a1, int a2, int a3, int a4)
{
	_asm
	{
		pushad
	}
	
	LPCTSTR ret;
	int index;
	ret = (LPCTSTR)a2;
	index = *(int*)ret;
	cout <<  index << "|" << wtocUTF(ret) << endl;
	TXT <<  index << "|" << wtocUTF(ret) << endl;
	_asm
	{
		popad
	}
	return OldStr(a1, a2, a3,a4);
}
void Hook()
{
	DWORD BaseAddr = (DWORD)GetModuleHandle(NULL);
	cout << "BaseAddress:0x" << hex << BaseAddr << endl;
	TXT << "BaseAddress:0x" << hex << BaseAddr << endl;
	*(DWORD*)&OldStr = BaseAddr + 0x262fc0;
	cout << "HookAddress:0x" << hex << (BaseAddr + 0x262fc0) <<"\n"<< endl;
	TXT << "HookAddress:0x" << hex << (BaseAddr + 0x262fc0) << endl;
	DetourTransactionBegin();
	DetourAttach((void**)&OldStr, MyStr);
	if(DetourTransactionCommit()!=NOERROR){ MessageBox(NULL, L"ERROR", L"Crescendo", MB_OK); }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		make_console();
		Hook();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void) {
	return;
}
