#pragma once

#include <Windows.h>
#include <string>
#include <cmath>
using namespace std;

class ApplicationProperty{
private:
	static string myDirectory;
	static string mySetupPath;
public:
	static HINSTANCE hInstance;
	static HINSTANCE hPrevInstance;
	static LPSTR lpCmdLine;
	static HWND hWnd;
	static int nCmdShow;
	static int* argc;
	static char** argv;
	static int Init(int* argc,char** argv,HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	static string GetDirectory();
	static int ReadSetupInt(string section,string key,int def);
	static double ReadSetupDouble(string section,string key,double def);
	static string ReadSetupString(string section,string key,string def);
	static void WriteSetupInt(string section,string key,int num);
	static void WriteSetupDouble(string section,string key,double num);
	static void WriteSetupString(string section,string key,string str);
};