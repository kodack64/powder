#ifndef __APP_H_
#define __APP_H_

#include "listener.h"

#define DEF_FPS 60
#define DEF_WINX 480
#define DEF_WINY 480

class Appli{
private:
	Listener *pl;
	int fps;
	int winx;
	int winy;
	bool full;
	int dim;
	int tframe;
public:
	Appli();
	virtual ~Appli();
	virtual int Init(int* argc,char** argv,Listener*,int dim);
	virtual int Run();
	virtual int Close();
	virtual void SetWindowSize(int x,int y){
		winx=x;winy=y;
	}
	virtual void SetFps(int f){
		fps=f;
	}
	virtual void SetFullscreen(bool b){
		full = b;
	}
	virtual void SetTotalFrame(int t){
		tframe =t;
	};
};

#endif __APP_H_