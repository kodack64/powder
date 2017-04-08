
#pragma once

#include "Powder/world.h"

#define MN 10000

class MolecularDynamics:public World{
private:
	int count;
	double t;
	double dt;
	double cutoff;
	double w;
	double h;
	double rr;
	double rn;
	double m;
	double temp;
	double g;
	double freq;
	double depth;
	double pa;
	double pb;
	double famp;
	double wamp;
	double p2;

	double sdir;
	double wdiff;
	double hdiff;
	double ts;

	bool shakeToggle;
	bool f_shake;

	pdVertex2d r[MN];
	pdVertex2d v[MN];
	pdVertex2d f[MN];
	double rad[MN];
	int sr;
public:
	virtual int MainLoop(void);
	virtual int Init();
	virtual int Draw(void);
	virtual double drand();
	virtual int WriteData(){return 0;}

	virtual void CalcForce();
	virtual void CalcForceCol();
	virtual void CalcForceWall();
	virtual pdVertex2d CalcPotential(pdVertex2d r1,pdVertex2d r2,double rad1,double rad2);
	virtual void VerletBf();
	virtual void VerletAf();
	virtual void LoadParam();
	virtual void SetParam();
	virtual void CalcWallMove();
	virtual void ProcKey();
	virtual pdVertex2d CalcFirstVec(double temp);
};
