
#pragma once

#include "Powder/world.h"

#include <cassert>

#define NI 1000		//最大粒子数
#define NJ 10		//最大隣接粒子数
#define NC 20000	//最大セル数
#define NW 3		//境界条件の数

#define AMPL 30

class DistinctElement:public World{
private:
	int count;		//描画回数

	double rmax;	//粒子の最大半径
	double rmin;	//粒子の最小半径
	double t;		//実時間
	double uamp;	//粘性項の拡大
	int topcel;		//天井までの余裕

	/* con */
	double dt;	//離散化時間
	double fri;	//粒子間摩擦係数
	double frw;	//壁-粒子間摩擦係数
	double e;	//粒子の弾性係数
	double ew;	//壁の弾性係数
	double po;	//粒子のポアソン比
	double poww;//壁のポアソン比
	double so;	//縦弾性係数とせん断弾性係数の比
	double g;	//重力加速度
	double pi;	//円周率

	/* wep */
	double rr[NI+1];	//粒子半径
	double wei[NI+1];	//粒子質量
	double pmi[NI+1];	//慣性モーメント

	/* cel */
	int n;			//粒子数
	int idx;		//x方向セル数
	int idy;		//y方向セル数
	int ipx;		//x方向粒子総数
	int ipy;		//y方向粒子総数
	double w;		//容器幅
	double h;		//容器の高さ
	double c;		//セル幅
	int ncl[NC+1];	//セル配列
	int nncl[NI+1];	//粒子を格納しているセル番号

	/* pos */
	double x0[NI+1];	//粒子の中心x座標
	double y0[NI+1];	//粒子の中心z座標
	double qq[NI+1];	//粒子の回転変位

	/* vel */
	double u0[NI+1];	//粒子のx方向速度
	double v0[NI+1];	//粒子のz方向速度
	double f0[NI+1];	//粒子の回転方向速度

	/* for1 */
	double xf[NI+1];	//粒子のx方向合力
	double yf[NI+1];	//粒子のz方向合力
	double mf[NI+1];	//粒子のモーメント

	/* for2 */
	double en[NI+1][NJ+NW+1];	//法線方向接触力
	double es[NI+1][NJ+NW+1];	//せん断方向接触力
	int je[NI+1][NJ+NW+1];	//接触点番号配列

	/* dpm */
	double u[NI+NW+1];	//x方向変位増分
	double v[NI+NW+1];	//z方向変位増分
	double f[NI+NW+1];	//回転変位増分

public:
	virtual int MainLoop(void);
	virtual int Init();
	virtual int Draw(void);

	virtual int ncel();
	virtual int wcont(int i);
	virtual int pcont(int i);
	virtual int nposit();
	virtual int actf(int i,int j,int jk,double as,double ac,double gap);
	virtual int random(int* ii,double* ru);

	virtual int WriteData(){
		unsigned int x;
		if(GetFrameCount()%1000!=0)return 0;
		printf("#frame %ld\n",GetFrameCount());
		for(x=0;x<GetBodyNum();x++){
			pdBody* p = GetBody(x);
			if(p->GetDimention()==2){
				pdParam2d par = GetBody(x)->GetParam2d();
				if(par.dataout){
					printf("%d %lf %lf\n",x,par.r.x,par.r.y);
				}
			}else{
				pdParam3d par = GetBody(x)->GetParam3d();
				if(par.dataout){
					printf("%d %lf %lf %lf\n",x,par.r.x,par.r.y,par.r.z);
				}
			}
		}
		return 0;
	}
};