
#include "Powder/appli.h"
#include "2DSampleWorld.h"

#define WINDOW_SIZE_X 640
#define WINDOW_SIZE_Y 480
#define FPS 60
#define FULLSCREEN 0
#define DIMENTION 2
#define TOTALFRAME FPS*10
//#define NOCONSOLE

#ifdef NOCONSOLE
#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif _MSC_VER
#endif NOCONSOLE

int main(int argc, char *argv[]){
#ifdef _DEBUG
#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif _MSC_VER
#endif _DEBUG

	Appli *appli = new Appli;

	World *world = new SampleWorld;
	Listener* listen = new Listener(world);

	appli->SetWindowSize(WINDOW_SIZE_X,WINDOW_SIZE_Y);
	appli->SetFps(FPS);
	appli->SetFullscreen(FULLSCREEN);
	appli->SetTotalFrame(TOTALFRAME);
	appli->Init(&argc,argv,listen,DIMENTION);

	appli->Run();
	appli->Close();
	delete appli;
	return 0;
}