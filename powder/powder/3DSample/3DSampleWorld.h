#ifndef _MY_WORLD_H_
#define _MY_WORLD_H_

#include "Powder/world.h"

class SampleWorld:public World{
private:
	int count;
public:
	virtual int MainLoop(void);
	virtual int Init();
	virtual int Draw(void);
};


#endif _MY_WORLD_H_