
#pragma once

#include "Powder/world.h"

#include <cassert>

#define NI 1000		//�ő嗱�q��
#define NJ 10		//�ő�אڗ��q��
#define NC 20000	//�ő�Z����
#define NW 3		//���E�����̐�

#define AMPL 30

class DistinctElement:public World{
private:
	int count;		//�`���

	double rmax;	//���q�̍ő唼�a
	double rmin;	//���q�̍ŏ����a
	double t;		//������
	double uamp;	//�S�����̊g��
	int topcel;		//�V��܂ł̗]�T

	/* con */
	double dt;	//���U������
	double fri;	//���q�Ԗ��C�W��
	double frw;	//��-���q�Ԗ��C�W��
	double e;	//���q�̒e���W��
	double ew;	//�ǂ̒e���W��
	double po;	//���q�̃|�A�\����
	double poww;//�ǂ̃|�A�\����
	double so;	//�c�e���W���Ƃ���f�e���W���̔�
	double g;	//�d�͉����x
	double pi;	//�~����

	/* wep */
	double rr[NI+1];	//���q���a
	double wei[NI+1];	//���q����
	double pmi[NI+1];	//�������[�����g

	/* cel */
	int n;			//���q��
	int idx;		//x�����Z����
	int idy;		//y�����Z����
	int ipx;		//x�������q����
	int ipy;		//y�������q����
	double w;		//�e�핝
	double h;		//�e��̍���
	double c;		//�Z����
	int ncl[NC+1];	//�Z���z��
	int nncl[NI+1];	//���q���i�[���Ă���Z���ԍ�

	/* pos */
	double x0[NI+1];	//���q�̒��Sx���W
	double y0[NI+1];	//���q�̒��Sz���W
	double qq[NI+1];	//���q�̉�]�ψ�

	/* vel */
	double u0[NI+1];	//���q��x�������x
	double v0[NI+1];	//���q��z�������x
	double f0[NI+1];	//���q�̉�]�������x

	/* for1 */
	double xf[NI+1];	//���q��x��������
	double yf[NI+1];	//���q��z��������
	double mf[NI+1];	//���q�̃��[�����g

	/* for2 */
	double en[NI+1][NJ+NW+1];	//�@�������ڐG��
	double es[NI+1][NJ+NW+1];	//����f�����ڐG��
	int je[NI+1][NJ+NW+1];	//�ڐG�_�ԍ��z��

	/* dpm */
	double u[NI+NW+1];	//x�����ψʑ���
	double v[NI+NW+1];	//z�����ψʑ���
	double f[NI+NW+1];	//��]�ψʑ���

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