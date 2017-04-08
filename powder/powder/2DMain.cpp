
#include "Powder/appli.h"
#include "MDWorld.h"
#include "DEWorld.h"
#include "Powder\ApplicationProperty.h"

#define DIMENTION 2
#define TOTALFRAME 0
//#define NOCONSOLE

#ifdef NOCONSOLE
#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif _MSC_VER
#endif NOCONSOLE

int main(int argc, char *argv[]){
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ApplicationProperty::Init(NULL,NULL,NULL,NULL,"",0);
	Appli *appli = new Appli;

	int alg=ApplicationProperty::ReadSetupInt("Application","Algorithm",0);
	int WINDOW_SIZE_X=ApplicationProperty::ReadSetupInt("Application","WindowSizeX",640);
	int WINDOW_SIZE_Y=ApplicationProperty::ReadSetupInt("Application","WindowSizeY",480);
	int FULLSCREEN=ApplicationProperty::ReadSetupInt("Application","FullScreen",0);
	World *world;
	int fps;
	if(alg==0){
		world = new DistinctElement;
		fps=ApplicationProperty::ReadSetupInt("DE_Property","FPS",60);
	}else{
		world = new MolecularDynamics;
		fps=ApplicationProperty::ReadSetupInt("MD_Property","FPS",60);
	}
	Listener* listen = new Listener(world);

	appli->SetWindowSize(WINDOW_SIZE_X,WINDOW_SIZE_Y);
	appli->SetFps(fps);
	appli->SetFullscreen(FULLSCREEN!=0);
	appli->SetTotalFrame(TOTALFRAME);
	appli->Init(&argc,argv,listen,DIMENTION);

	appli->Run();
	appli->Close();
	delete appli;
	return 0;
}