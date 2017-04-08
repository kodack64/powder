#include "listener.h"
#include "ApplicationProperty.h"
World* Listener::myworld = 0;
int Listener::fps = 60;
int Listener::winx = 480;
int Listener::winy = 480;
int Listener::dim = 3;
bool Listener::full = false;
MouseState Listener::m;
Camera Listener::cam;
int Listener::fr=0;
int Listener::lastTime=0;
int Listener::tframe=0;
int Listener::cframe=0;
bool Listener::f_calldisp=true;
int Listener::graphicCallCount=0;
int Listener::graphicCallInterval=0;

Listener::Listener(World* w){
	myworld=w;
};
Listener::~Listener(){
	SAFE_DELETE(myworld);
};

void Listener::Init(int s_fps,int s_winx,int s_winy,bool s_full,int sdim,int t){
	dim=sdim;
	myworld->SetDimention(dim);
	fps = s_fps;
	winx = s_winx;
	winy = s_winy;
	full = s_full;
	tframe=t;
	pdInit();
	graphicCallInterval=ApplicationProperty::ReadSetupInt("Graphic","CallInterval",60);
	if(myworld!=0){
		myworld->Init();
	}
	lastTime=glutGet(GLUT_ELAPSED_TIME);
}
void __cdecl Listener::Timer(int val){
	if(myworld!=0){

		myworld->WriteData();

		if(myworld->MainLoop()==-1){
			printf("End\n");
			_getch();
			glutLeaveMainLoop();
		}

		myworld->IncrementFrameCount();
		graphicCallCount++;
		if(graphicCallCount>graphicCallInterval && f_calldisp){
			graphicCallCount=0;
			glutPostRedisplay();
		}

		int elp =glutGet(GLUT_ELAPSED_TIME)-lastTime;
		if(elp>=1000){
			char title[256];
			sprintf_s(title,256,"FPS:%d/%d",fr,fps);
			if(!f_calldisp){
				sprintf_s(title,256,"%s No Graphic",title);
			}
			glutSetWindowTitle(title);
			fr=0;
			lastTime=glutGet(GLUT_ELAPSED_TIME);
			elp=0;
		}
		fr++;
		cframe++;
		if(fps>0){
			int diff=1000*fr/fps-elp;
			if(diff>0)glutTimerFunc(diff,Timer,0);
			else glutTimerFunc(0,Timer,0);
		}else{
			glutTimerFunc(0,Timer,0);
		}
		if(tframe!=0 && cframe>tframe){
			glutLeaveMainLoop();
		}
	}
};

void __cdecl Listener::Disp(void){
	pdClearScreen();
	glLoadIdentity();
	gluLookAt(cam.eye.x, cam.eye.y, cam.eye.z , cam.o.x, cam.o.y, cam.o.z, 0.0, 1.0, 0.0);
	if(myworld!=0){
		myworld->DrawAllBody();
		myworld->Draw();
	}
	if( (m.mbr||m.mbl) && dim==3){
		pdDrawCircle(pdVertex3d(0,0,0),pdColor(1,1,1),cam.GetDist()/5);
	}
	pdSwapScreen();
};



void __cdecl Listener::Reshape(int w,int h){
	glViewport(0, 0, w, h);
//	if(dim==3){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, (double)w / (double)h, 1.0, 500.0);
		glMatrixMode(GL_MODELVIEW);
//	}else{
//		glLoadIdentity();
//		glOrtho(-w / 200.0, w / 200.0, -h / 200.0, h / 200.0, -1.0, 1.0);
//	}
}

void __cdecl Listener::Mouse(int button,int state,int x,int y){
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)	m.mbl=1;
		else					m.mbl=0;
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)	m.mbm=1;
		else					m.mbm=0;
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)	m.mbr=1;
		else					m.mbr=0;
		break;
	case 0x0003:
		if(state==1){
			pdVertex3d v;
			v = cam.eye - cam.o;
			v*= cam.scale;
			cam.eye = cam.o + v;
		}
		break;
	case 0x0004:
		if(state==1){
			pdVertex3d v;
			v = cam.eye - cam.o;
			v/= cam.scale;
			cam.eye = cam.o + v;
		}
		break;
	default:
		break;
	}
	if(m.mbr || m.mbl || m.mbm){
		pdSwapScreen();
	}
	m.ox=x;
	m.oy=y;
	m.ombl=m.mbl;
	m.ombr=m.mbr;
	m.ombm=m.mbm;
}
void __cdecl Listener::Motion(int x,int y){
	int vx,vy;
	if(dim==3){
		if(m.mbl && m.ombl){
			vx=x-m.ox;
			vy=y-m.oy;
			cam.theta+=vx*cam.speed;
			cam.phi+=vy*cam.speed;
			cam.PolarToVertex();
			glLoadIdentity();
			gluLookAt(cam.eye.x, cam.eye.y, cam.eye.z , cam.o.x, cam.o.y, cam.o.z, 0.0, 1.0, 0.0);
//			printf("%lf %lf %lf %lf %lf\n",cam.eye.x,cam.eye.y,cam.eye.z,cam.theta,cam.phi);
		}
	}
	m.ox=x;
	m.oy=y;
	m.ombl=m.mbl;
	m.ombr=m.mbr;
	m.ombm=m.mbm;
}

void __cdecl Listener::Keyboard(unsigned char key,int x,int y){
	double cms=0.1;
	switch (key) {
	case VK_ESCAPE:
		glutLeaveMainLoop();
		break;
	case VK_RETURN:
		if(full){
			ChangeDisplaySettings(NULL, 0);
			glutPositionWindow(100,100);
			glutReshapeWindow(winx,winy);
		}else{
			glutFullScreen();
		}
		full=!full;
		break;
	case 'w':
		if(dim==2){
			cam.eye.y+=cms;
			cam.o.y+=cms;
			cam.VertexToPolar();
		}
		break;
	case 's':
		if(dim==2){
			cam.eye.y-=cms;
			cam.o.y-=cms;
			cam.VertexToPolar();
		}
		break;
	case 'a':
		if(dim==2){
			cam.eye.x-=cms;
			cam.o.x-=cms;
			cam.VertexToPolar();
		}
		break;
	case 'd':
		if(dim==2){
			cam.eye.x+=cms;
			cam.o.x+=cms;
			cam.VertexToPolar();
		}
		break;
	case 'z':
		f_calldisp=!f_calldisp;
	default:
		break;
	}
}


void __cdecl Listener::KeyboardUp(unsigned char key,int x,int y){
}

