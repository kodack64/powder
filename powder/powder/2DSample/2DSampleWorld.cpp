#include "2DSampleWorld.h"

int SampleWorld::Init(){
	pdSetScreenColor(pdColor(0,0,0));

	pdParam2d p;
	pdBody* body;

	p.c=pdColor(0,1,0);
	p.mass=1;
	p.r=pdVertex2d(0,0);
	p.v=pdVertex2d(0.05,0.1);
	p.dataout=true;

	body=CreateCircle2d(0.1);
	body->SetParam(p);

	p.c=pdColor(1,0,1);
	p.r=pdVertex2d(0,0);
	p.v=pdVertex2d(0.02,0.01);
	p.dataout=true;

	body=CreateCircle2d(0.1);
	body->SetParam(p);

	count=0;
	return 0;
}

int SampleWorld::MainLoop(void){
	unsigned int x;
	for(x=0;x<GetBodyNum();x++){
		pdBody *pd = GetBody(x);
		pdParam2d p= pd->GetParam2d();

		p.r+=p.v;
		if(p.r.x<-1 || p.r.x>1){
			p.v.x=-p.v.x;
		}
		if(p.r.y<-1 || p.r.y>1){
			p.v.y=-p.v.y;
		}
		pd->SetParam(p);
	}
	return 0;
}

int SampleWorld::Draw(void){

	pdVertex2d a[5];
	pdColor c[5];
	a[0]=a[4]=pdVertex2d(-1,-1);
	a[1]=pdVertex2d(1,-1);
	a[2]=pdVertex2d(1,1);
	a[3]=pdVertex2d(-1,1);
	c[0]=c[1]=c[2]=c[3]=c[4]=pdColor(1,1,1);
	pdDrawPolygon(a,c,5);

	count++;
	char str[256];
	sprintf_s(str,"Main Frame %d",GetFrameCount());
	pdDrawString(str,pdVertex2d(0,0),pdColor(1,1,1));
	sprintf_s(str,"Draw Frame %d",count);
	pdDrawString(str,pdVertex2d(0,20),pdColor(1,1,1));

	return 0;
}