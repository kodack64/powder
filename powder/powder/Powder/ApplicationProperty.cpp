
#include "ApplicationProperty.h"

#define DEF_SETUP_NAME "setup.ini"

string ApplicationProperty::myDirectory="";
string ApplicationProperty::mySetupPath="";
int* ApplicationProperty::argc=NULL;
char** ApplicationProperty::argv=NULL;
HINSTANCE ApplicationProperty::hInstance;
HINSTANCE ApplicationProperty::hPrevInstance;
LPSTR ApplicationProperty::lpCmdLine;
HWND ApplicationProperty::hWnd;
int ApplicationProperty::nCmdShow;

int ApplicationProperty::Init(int* _argc,char** _argv,HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow){
	argc=_argc;
	argv=_argv;
	hInstance=_hInstance;
	hPrevInstance=_hPrevInstance;
	lpCmdLine=_lpCmdLine;
	nCmdShow=_nCmdShow;
	char buf[MAX_PATH],drv[MAX_PATH],dir[MAX_PATH],fn[MAX_PATH],ext[MAX_PATH];
	int ret=GetModuleFileName(NULL,buf,sizeof(buf));
	if(ret==0){
		GetCurrentDirectory(MAX_PATH,buf);
	}else{
		_splitpath_s(buf,drv,sizeof(drv),dir,sizeof(dir),fn,sizeof(fn),ext,sizeof(ext));
		sprintf_s(buf,MAX_PATH,"%s%s",drv,dir);
		SetCurrentDirectory(buf);
	}
	myDirectory=buf;
	mySetupPath=myDirectory+DEF_SETUP_NAME;
	return 0;
}

string ApplicationProperty::GetDirectory(){
	return myDirectory;
}

int ApplicationProperty::ReadSetupInt(string section,string key,int def){
	if(mySetupPath=="")return 0;
	int rtn=GetPrivateProfileInt(section.c_str(),key.c_str(),def,mySetupPath.c_str());
	if(rtn==def){
		char buf[MAX_PATH];
		sprintf_s(buf,MAX_PATH,"%d",def);
		WritePrivateProfileString(section.c_str(),key.c_str(),buf,mySetupPath.c_str());
	}
	return rtn;
}

double ApplicationProperty::ReadSetupDouble(string section,string key,double def){
	if(mySetupPath=="")return 0;
	double rtn;
	char buf[MAX_PATH];
	char defstr[MAX_PATH];
	sprintf_s(defstr,MAX_PATH,"%le",def);
	GetPrivateProfileString(section.c_str(),key.c_str(),defstr,buf,MAX_PATH,mySetupPath.c_str());
	if(strcmp(buf,defstr)==0){
		WritePrivateProfileString(section.c_str(),key.c_str(),buf,mySetupPath.c_str());
	}
	sscanf_s(buf,"%le",&rtn);
	return rtn;
}

string ApplicationProperty::ReadSetupString(string section,string key,string def){
	if(mySetupPath=="")return "";
	string rtn;
	char buf[MAX_PATH];
	GetPrivateProfileString(section.c_str(),key.c_str(),def.c_str(),buf,MAX_PATH,mySetupPath.c_str());
	if(strcmp(buf,def.c_str())==0){
		WritePrivateProfileString(section.c_str(),key.c_str(),buf,mySetupPath.c_str());
	}
	rtn=buf;
	return rtn;
}

void ApplicationProperty::WriteSetupInt(string section,string key,int num){
	if(mySetupPath=="")return;
	char buf[MAX_PATH];
	sprintf_s(buf,MAX_PATH,"%d",num);
	WritePrivateProfileString(section.c_str(),key.c_str(),buf,mySetupPath.c_str());
}

void ApplicationProperty::WriteSetupDouble(string section,string key,double num){
	if(mySetupPath=="")return;
	char buf[MAX_PATH];
	sprintf_s(buf,MAX_PATH,"%le",num);
	WritePrivateProfileString(section.c_str(),key.c_str(),buf,mySetupPath.c_str());
}

void ApplicationProperty::WriteSetupString(string section,string key,string str){
	if(mySetupPath=="")return;
	WritePrivateProfileString(section.c_str(),key.c_str(),str.c_str(),mySetupPath.c_str());
}
