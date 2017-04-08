#include "pdbase.h"

int pdCircle2d::Init(double sr){
	r=sr;
	return 0;
}
int pdCircle2d::Draw(pdParam2d p){
	pdDrawBall(p.r,p.c,r);
	return 0;
}

int pdCircle3d::Init(double sr){
	r=sr;
	return 0;
}
int pdCircle3d::Draw(pdParam3d p){
	pdDrawBall(p.r,p.c,r);
	return 0;
}

