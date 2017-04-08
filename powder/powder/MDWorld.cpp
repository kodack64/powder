

#include "MDWorld.h"
#include "powder\ApplicationProperty.h"

#define SCALING 0

#define TOPWALL 0
#define UNDWALL 1
#define LEFWALL 1
#define RIGWALL 1

#define HEILOOP 0
#define WIDLOOP 0

#define prop ApplicationProperty

int MolecularDynamics::Init(){
	pdSetScreenColor(pdColor(0,0,0));

	t=0;
	ts=0;
	sr=52150;
	shakeToggle=false;
	f_shake=false;

	int saveRes=prop::ReadSetupInt("MD_Property","SaveResults",1);

	dt=prop::ReadSetupDouble("MD_Env","DeltaTime",1.0e-3);
	temp=prop::ReadSetupDouble("MD_Env","Temperature",1.0e-5);
	g=prop::ReadSetupDouble("MD_Env","Gravity",9.8e0);
	pa=prop::ReadSetupDouble("MD_Env","r^-12",2.0e0);
	pb=prop::ReadSetupDouble("MD_Env","r^-6",1.0e0);
	famp=prop::ReadSetupDouble("MD_Env","Par-Par-Amplify",24.0e-3);
	wamp=prop::ReadSetupDouble("MD_Env","Par-Wall-Amplify",24.0e-3);
	cutoff=prop::ReadSetupDouble("MD_Env","CutoffDist/BigParRad",1.0e0);
	p2=prop::ReadSetupDouble("MD_Env","Viscosity",1.0e1);

	w=prop::ReadSetupDouble("MD_Container","width",1.0e1);
	h=prop::ReadSetupDouble("MD_Container","height",1.0e1);
	freq=prop::ReadSetupDouble("MD_Container","ShakeFrequency",1.0e0);
	depth=prop::ReadSetupDouble("MD_Container","ShakeDepth",1.0e-2);
	sdir=prop::ReadSetupDouble("MD_Container","ShakeDirection",PI/2);
	rn=prop::ReadSetupDouble("MD_Container","Interval",1.1e0);

	double m1,m2,r1,r2;
	m1=prop::ReadSetupDouble("MD_BigParticle","Density",1.0e0);
	r1=prop::ReadSetupDouble("MD_BigParticle","Radius",1.0e0);
	m2=prop::ReadSetupDouble("MD_SmallParticle","Density",1.0e0);
	r2=prop::ReadSetupDouble("MD_SmallParticle","Radius",5.0e-1);
	double cr1,cr2,cg1,cg2,cb1,cb2;
	cr1=prop::ReadSetupDouble("MD_BigParticle","ColorRed",1.0e0);
	cg1=prop::ReadSetupDouble("MD_BigParticle","ColorGreen",0.0e0);
	cb1=prop::ReadSetupDouble("MD_BigParticle","ColorBlue",1.0e0);
	cr2=prop::ReadSetupDouble("MD_SmallParticle","ColorRed",1.0);
	cg2=prop::ReadSetupDouble("MD_SmallParticle","ColorGreen",1.0);
	cb2=prop::ReadSetupDouble("MD_SmallParticle","ColorBlue",1.0);

	rr=pow(pa/pb,1.0/6)/2-0.35;	//粒子の半径

	pdBody* body;

	pdParam2d sParam;
	pdParam2d bParam;

	bParam.c=pdColor(cr1,cg1,cb1);
	bParam.dataout=(saveRes!=0);
	bParam.mass=m1*pow(r1,3);

	sParam.c=pdColor(cr2,cg2,cb2);
	sParam.dataout=(saveRes!=0);
	sParam.mass=m2*pow(r2,3);

	int x,y;
	for(y=0;(rn+r2)*2+y*(r2*2+rn)<h;y++){
		for(x=0;(rn+r2)*2+x*(r2*2+rn)<w;x++){
			sParam.r=pdVertex2d(
				-w/2+rn+r2				+(r2*2+rn)*x
				,-h/2+2*r1+rn+r2		+(r2*2+rn)*y);
			sParam.v=CalcFirstVec(temp);
			body=CreateCircle2d(r2);
			body->SetParam(sParam);
		}
	}

	bParam.r=pdVertex2d(0,-h/2+rn+r1);
	bParam.v=CalcFirstVec(temp);

	body=CreateCircle2d(r1);
	body->SetParam(bParam);

	count=0;
	wdiff=0.0;
	hdiff=0.0;
	return 0;
}

pdVertex2d MolecularDynamics::CalcFirstVec(double a){
	pdVertex2d vt;
	if(temp>0.0){
		for(;;){
			vt=pdVertex2d(
			-2.0*temp*log(drand())*cos(PI*2*drand())
			,-2.0*temp*log(drand())*cos(PI*2*drand())
			);
			if(vt.length()<2*temp*3.5)break;
		}
	}else{
		vt=pdVertex2d(0.0f,0.0f);
	}
	return vt;
}

pdVertex2d MolecularDynamics::CalcPotential(pdVertex2d r1,pdVertex2d r2,double rad1,double rad2){
	pdVertex2d rij;
	rij = r1-r2;

	double dis = rij.length();

	if(dis>(rad1+rad2)*cutoff)return pdVertex2d(0,0);

	double ff=rad1+rad2-rr*2;
	return rij/dis*( pa*pow(dis-ff,-12) - pb*pow(dis-ff,-6) );
}

void MolecularDynamics::CalcForceCol(){
	int i,j;
	int n=GetBodyNum();
	//レナードジョーンズ分子のポテンシャルに従って分子間力からfを計算
	for(i=0;i<n;i++){
		for(j=i+1;j<n;j++){
			pdVertex2d ff=CalcPotential(r[i],r[j],rad[i],rad[j])*famp;
			f[i]+=ff;
			f[j]-=ff;
		}
	}
}
void MolecularDynamics::CalcForceWall(){
	pdVertex2d rjt;
	int i,j;
	int n=GetBodyNum();
	//境界条件から受けるfを計算
	for(i=0;i<n;i++){
		for(j=0;j<4;j++){
			if(j==0 && UNDWALL){
				rjt=pdVertex2d(r[i].x,-h/2-rr+hdiff);
			}
			if(j==1 && TOPWALL){
				rjt=pdVertex2d(r[i].x,h/2+rr+hdiff);
			}
			if(j==2 && LEFWALL){
				rjt=pdVertex2d(-w/2-rr+wdiff,r[i].y);
			}
			if(j==3 && RIGWALL){
				rjt=pdVertex2d(w/2+rr+wdiff,r[i].y);
			}

			f[i]+=CalcPotential(r[i],rjt,rad[i],rr)*wamp;

		}
	}
}

void MolecularDynamics::CalcForce(){
	int i;
	int n=GetBodyNum();

	//初期化
	for(i=0;i<n;i++){
		f[i]=pdVertex2d(0,0);
	}

	//衝突を考慮
	CalcForceCol();

	//境界条件を考慮
	CalcForceWall();

	//重力を考慮
	for(i=0;i<n;i++){
		f[i].y-=g;
	}

	for(i=0;i<n;i++){
		f[i]/=m;
	}
}
void MolecularDynamics::VerletBf(){
	for(unsigned int i=0;i<GetBodyNum();i++){
		pdVertex2d ri;
		r[i]+=v[i]*dt+f[i]*dt*dt/2.0;
		v[i]+=f[i]*dt/2.0;
	}
}
void MolecularDynamics::VerletAf(){
	int i;
	int n=GetBodyNum();

	//差分法でfからvを更新
	for(i=0;i<n;i++){
		v[i]+=f[i]*dt/2.0;
		v[i]*=(1.0-p2*dt);
	}
}

void MolecularDynamics::LoadParam(){
	//bodyから値をとって配列に代入
	unsigned int i,n;
	n=GetBodyNum();
	for(i=0;i<n;i++){
		r[i]=GetBody(i)->GetParam2d().r;
		v[i]=GetBody(i)->GetParam2d().v;
		rad[i]=((pdCircle2d*)(GetBody(i)->GetShape()))->GetR();
	}
}
void MolecularDynamics::SetParam(){
	//bodyの値を更新
	unsigned int i,n;
	n=GetBodyNum();
	for(i=0;i<n;i++){
		pdParam2d par=GetBody(i)->GetParam2d();
		par.r=r[i];
		par.v=v[i];
		GetBody(i)->SetParam(par);
	}
}
void MolecularDynamics::CalcWallMove(){
	wdiff=sin(2*PI*freq*ts)*depth*cos(sdir);
	hdiff=sin(2*PI*freq*ts)*depth*sin(sdir);
}

int MolecularDynamics::MainLoop(void){
	ProcKey();
	t+=dt;
	if(shakeToggle){
		ts+=dt;
	}

	LoadParam();

	CalcWallMove();

	VerletBf();

	CalcForce();

	VerletAf();

	SetParam();
	return 0;
}

int MolecularDynamics::Draw(void){

	pdVertex2d a[5];
	pdColor c[5];
	a[0]=a[4]=pdVertex2d(	-w/2+wdiff	,-h/2+hdiff);
	a[1]=pdVertex2d(		w/2+wdiff	,-h/2+hdiff);
	a[2]=pdVertex2d(		w/2+wdiff	,h/2+hdiff);
	a[3]=pdVertex2d(		-w/2+wdiff	,h/2+hdiff);
	c[0]=c[1]=c[2]=c[3]=c[4]=pdColor(1,1,1);
	pdDrawLine(a,c,5);

	count++;
	char str[256];
	sprintf_s(str,"Time %.4lf",t);
	pdDrawString(str,pdVertex2d(0,0),pdColor(1,1,1));
	return 0;
}

double MolecularDynamics::drand(){
	sr=sr*65539;
	if(sr<0)sr=(sr+2147483647)+1;
	return sr*0.4656613e-9;
}

void MolecularDynamics::ProcKey(){
	bool shakeKey=(GetKeyState(VK_SPACE)&0x8000)!=0;
	if(!f_shake && shakeKey)shakeToggle=!shakeToggle;
	f_shake=shakeKey;
}