#ifndef __LISTEN_H_
#define __LISTEN_H_

#include "world.h"

class MouseState{
public:
	MouseState(){
		mbr=mbl=mbm=ombr=ombl=ombm=ox=oy=0;
	}
	virtual ~MouseState(){};
	int mbr;
	int mbl;
	int mbm;
	int ombr;
	int ombl;
	int ombm;
	int ox;
	int oy;
};

class Camera{
public:
	double theta;
	double phi;
	double speed;
	double scale;
	pdVertex3d o;
	pdVertex3d eye;
	Camera(){
		theta=0;
		phi=0;
		eye.z=20.0;
		speed=0.01;
		scale=1.1;
	}
	virtual double GetDist(){
		return sqrt(pow(eye.x-o.x,2)+pow(eye.y-o.y,2)+pow(eye.z-o.z,2));
	}
	virtual void PolarToVertex(){
		double r = GetDist();
		eye.x=o.x+r*sin(theta)*cos(phi);
		eye.y=o.y+r*sin(theta)*sin(phi);
		eye.z=o.z+r*cos(theta);
//		eye.x=o.x+r*cos(phi)*cos(theta);
//		eye.y=o.y+r*sin(phi);
//		eye.z=o.z+r*cos(phi)*sin(theta);
//		eye.x=o.x+r*sin(phi)*cos(theta);
//		eye.y=o.y+r*sin(phi)*sin(theta);
//		eye.z=o.z+r*cos(phi);
	}
	virtual void VertexToPolar(){
		double r = GetDist();
		theta = acos((eye.z-o.z)/r);
		phi = acos((eye.x-o.x)/(r*sin(theta)));
	}
};

class Listener{
private:
	static World *myworld;
	static int fps;
	static int winx;
	static int winy;
	static bool full;
	static int dim;
	static MouseState m;
	static Camera cam;
	static int fr;
	static int lastTime;
	static int tframe;
	static int cframe;
	static bool f_calldisp;
	static int graphicCallCount;
	static int graphicCallInterval;
public:
	Listener(World* w);
	virtual ~Listener();

	void Init(int fps,int winx,int winy,bool full,int dim,int tframe);
	static void __cdecl Timer(int val);
	static void __cdecl Disp(void);
	static void __cdecl Reshape(int w,int h);
	static void __cdecl Mouse(int b,int st,int x,int y);
	static void __cdecl Motion(int x,int y);
	static void __cdecl Keyboard(unsigned char key,int x,int y);
	static void __cdecl KeyboardUp(unsigned char key,int x,int y);
};

#endif __LISTEN_H_