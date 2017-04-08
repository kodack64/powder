#include "3DSampleWorld.h"

int SampleWorld::Init(){
	pdSetScreenColor(pdColor(0,0,0));

	pdBody* body;
	pdParam3d p;

	p.c=pdColor(0,1,0);
	p.mass=1;
	p.r=pdVertex3d(0,0,0);
	p.v=pdVertex3d(0.02,0.03,0.01);
	p.dataout=true;
	body=CreateCircle3d(0.1);
	body->SetParam(p);

	p.c=pdColor(1,0,1);
	p.mass=1;
	p.r=pdVertex3d(0,0,0);
	p.v=pdVertex3d(0.05,0.01,0.03);
	p.dataout=true;
	body=CreateCircle3d(0.1);
	body->SetParam(p);

	count=0;
	return 0;
}

int SampleWorld::MainLoop(void){
	unsigned int x;
	for(x=0;x<GetBodyNum();x++){
		pdBody *pd = GetBody(x);
		pdParam3d p= pd->GetParam3d();

		p.r+=p.v;
		if(p.r.x<-1 || p.r.x>1)	p.v.x=-p.v.x;
		if(p.r.y<-1 || p.r.y>1)	p.v.y=-p.v.y;
		if(p.r.z<-1 || p.r.z>1)	p.v.z=-p.v.z;
		pd->SetParam(p);
	}
	return 0;
}

int SampleWorld::Draw(void){

	pdVertex3d a[5];
	pdColor c[5];
	a[0]=a[4]=pdVertex3d(-1,-1,-1);
	a[1]=pdVertex3d(-1,-1,1);
	a[2]=pdVertex3d(-1,1,1);
	a[3]=pdVertex3d(-1,1,-1);
	c[0]=c[1]=c[2]=c[3]=c[4]=pdColor(1,1,1);
	pdDrawLine(a,c,5);

	a[0]=a[4]=pdVertex3d(1,-1,-1);
	a[1]=pdVertex3d(1,-1,1);
	a[2]=pdVertex3d(1,1,1);
	a[3]=pdVertex3d(1,1,-1);
	c[0]=c[1]=c[2]=c[3]=c[4]=pdColor(1,1,1);
	pdDrawLine(a,c,5);

	a[0]=pdVertex3d(1,-1,-1);
	a[1]=pdVertex3d(-1,-1,-1);
	pdDrawLine(a,c,2);
	a[0]=pdVertex3d(1,1,-1);
	a[1]=pdVertex3d(-1,1,-1);
	pdDrawLine(a,c,2);
	a[0]=pdVertex3d(1,1,1);
	a[1]=pdVertex3d(-1,1,1);
	pdDrawLine(a,c,2);
	a[0]=pdVertex3d(1,-1,1);
	a[1]=pdVertex3d(-1,-1,1);
	pdDrawLine(a,c,2);

	count++;
	char str[256];
	sprintf_s(str,"Main Frame %d",GetFrameCount());
	pdDrawString(str,pdVertex2d(0,0),pdColor(1,1,1));
	sprintf_s(str,"Draw Frame %d",count);
	pdDrawString(str,pdVertex2d(0,20),pdColor(1,1,1));

	return 0;
}