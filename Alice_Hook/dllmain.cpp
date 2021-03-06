﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
//For Evenicle2
#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include "detours.h"
#pragma comment(lib, "detours.lib")
using namespace std;
ofstream TXT("Text.txt");


PVOID g_pOldCreateFontIndirectA = CreateFontIndirectA;
typedef HFONT(WINAPI *PfuncCreateFontIndirectA)(LOGFONTA *lplf);
HFONT WINAPI NewCreateFontIndirectA(LOGFONTA *lplf)
{
	lplf->lfCharSet = GB2312_CHARSET;
	strcpy(lplf->lfFaceName, "黑体");

	return ((PfuncCreateFontIndirectA)g_pOldCreateFontIndirectA)(lplf);
}


LPWSTR ctowJIS(char* str)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(932, 0, str, -1, NULL, 0); //计算长度
	LPWSTR out = new wchar_t[dwMinSize];
	MultiByteToWideChar(932, 0, str, -1, out, dwMinSize);//转换
	return out;
}

char* wtocGBK(LPCTSTR str)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(936, NULL, str, -1, NULL, 0, NULL, FALSE); //计算长度
	char *out = new char[dwMinSize];
	WideCharToMultiByte(936, NULL, str, -1, out, dwMinSize, NULL, FALSE);//转换
	return out;
}

static void make_console() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	cout << "Console\n" << endl;
}

unsigned int(*OldStr)(unsigned int a1, const void *a2, unsigned int a3);
unsigned int MyStr(unsigned int a1, const void *a2, unsigned int a3)//会出现很多重复的东西，这一段或许是循环，可能用于检测
{
	_asm
	{
		pushad
	}
	char* ret;
	int index;
	int rep;
	if (a1 != 0) {
		ret = (char*)OldStr(a1, a2, a3);
		index = *(int*)ret;
		char* Pstr = ret + sizeof(int);
		if ((USHORT)*Pstr > 0x7F) {
			//cout <<  index << "|" << wtocGBK(ctowJIS(Pstr)) << endl;//堆在这上面就炸了，改天还是要想个办法解决掉文本的问题
			TXT << index << "|" << wtocGBK(ctowJIS(Pstr)) << endl;
		}
	}
	_asm
	{
		popad
	}
	return OldStr(a1, a2, a3);
}
void Hook()
{
	DWORD BaseAddr = (DWORD)GetModuleHandle(NULL);
	cout << "BaseAddress:0x" <<hex<< BaseAddr << endl;
	TXT << "BaseAddress:0x" << hex<<BaseAddr << endl;
	*(DWORD*)&OldStr = BaseAddr + 0x351ff0;
	cout << "HookAddress:0x" << hex << (BaseAddr + 0x351ff0) <<"\n"<< endl;
	TXT << "HookAddress:0x" << hex << (BaseAddr + 0x351ff0) << endl;
	DetourTransactionBegin();
	DetourAttach((void**)&OldStr, MyStr);
	DetourAttach(&g_pOldCreateFontIndirectA, NewCreateFontIndirectA);
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


