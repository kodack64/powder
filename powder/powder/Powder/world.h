#ifndef _WORLD_H_
#define _WORLD_H_

#include "pdbase.h"

class World{
private:
	int dim;
	long framecount;
	vector <pdBody*> myBody;
public:
	World(){
		dim=0;
		framecount=0;
	}
	virtual ~World(){
		for(unsigned int x=0;x<myBody.size();x++){
			SAFE_DELETE(myBody[x]);
		}
	}
	virtual int MainLoop(void)=0;
	virtual int Init()=0;
	virtual int Draw(void)=0;
	virtual int DrawAllBody(){
		for(unsigned int x=0;x<myBody.size();x++){
			myBody[x]->Draw();
		}
		return 0;
	}

	virtual int GetDimention(){return dim;}
	virtual void SetDimention(int d){dim = d;}

	virtual pdBody* AddBody(pdShape* shape){
		pdBody* newbody = new pdBody(shape);
		myBody.push_back(newbody);
		return newbody;
	}
	virtual pdBody* CreateCircle2d(double r){
		pdCircle2d* shape= new pdCircle2d;
		shape->Init(r);
		return AddBody(shape);
	}
	virtual pdBody* CreateCircle3d(double r){
		pdCircle3d* shape= new pdCircle3d;
		shape->Init(r);
		return AddBody(shape);
	}
	virtual int DestroyBody(unsigned int num){
		if(num<myBody.size() && 0<=num){
			SAFE_DELETE(myBody[num]);
			myBody.erase(myBody.begin()+num);
			return num;
		}else{
			return -1;
		}
	}
	virtual void IncrementFrameCount(){
		framecount++;
	};
	virtual long GetFrameCount(){return framecount;}
	virtual pdBody* GetBody(unsigned int num){
		if(num<myBody.size() && 0<=num){
			return myBody[num];
		}else{
			return 0;
		}
	}
	virtual unsigned int GetBodyNum(){
		return myBody.size();
	}
	virtual int WriteData()=0;
};

#endif _WORLD_H_