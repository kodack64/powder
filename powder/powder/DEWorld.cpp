#include "DEWorld.h"
#include "Powder\ApplicationProperty.h"

#define prop ApplicationProperty

int DistinctElement::Init(){
	pdSetScreenColor(pdColor(0,0,0));

	int ii=584287;		//��������

	//���Ԃ̏�����
	t = 0.0;
	pi=PI;

	w=prop::ReadSetupDouble("DE_Container","Width",5.0e-1);
	h=prop::ReadSetupDouble("DE_Container","Height",5.0e-1);
	topcel=prop::ReadSetupInt("DE_Container","TopMergineNum",10);

	g=prop::ReadSetupDouble("DE_Env","Gravity",9.80665e0);
	dt=prop::ReadSetupDouble("DE_Env","DeltaTime",1e-3);
	e=prop::ReadSetupDouble("DE_Env","ParticleYoung",4.9e5);
	ew=prop::ReadSetupDouble("DE_Env","WallYoung",4.9e5);
	uamp=prop::ReadSetupDouble("DE_Env","Viscosity",0.5);
	po=prop::ReadSetupDouble("DE_Env","ParticlePoissonsRatio",2.3e-1);
	poww=prop::ReadSetupDouble("DE_Env","WallPoissonsRatio",2.5e-1);
	fri=prop::ReadSetupDouble("DE_Env","ParticleFriction",2.5e-1);
	frw=prop::ReadSetupDouble("DE_Env","WallFriction",1.7e-1);
	double merg=prop::ReadSetupDouble("DE_Env","Interval",1e-4);

	double r1=prop::ReadSetupDouble("DE_BigParticle","BigRadius",1e-2);
	double r2=prop::ReadSetupDouble("DE_SmallParticle","SmallRadius",5e-3);

	double de1=prop::ReadSetupDouble("DE_BigParticle","Density",2.48e3);
	double de2=prop::ReadSetupDouble("DE_SmallParticle","Density",2.48e3);

	so=1.0/2.0/(1.0+po);	//�c�e���W���Ƃ���f�e���W���̔�

	//���a�̍ő�A�ŏ��l���w��
	rmax=max(r1,r2);
	rmin=min(r1,r2);
	double rn=rmax+merg;	//���q�̔z�u�Ԋu/2

	ipx=(int)(w/(2.0*rn));	//�������a�������z�u�ł�x�����̗��q��
	ipy=(int)(h/(2.0*rn));	//���������a�ŏ����z�u�ł�y�����̗��q��

	//���q��������
	n=0;

	double dx;	//���̕ǂ���ŏ��̗��q�܂ł̋���
	double ip;	//����y���W�̒i�ł�x�����̗��q�̐�

	for(int i=1;i<=ipy;i++){

		//��i�Ƌ����i�Ŋ��ݍ����`�ɂȂ�悤�ɐ��l��ݒ肷��
		//�܂�Aipx��ipx-1�̒i�����݂ɂ���悤�ɂ���i���̌���ip�j
		//���킹�č��̕ǂ��痱�q�܂ł̃}�[�W�������߂���
		//�����i
		if(i%2==0){
			dx=2.0*rn;
			ip=ipx-1;
		}
		//��i
		else{
			dx=rn;
			ip=ipx;
		}

		for(int j=1;j<=ip;j++){

			double ru;
			random(&ii,&ru);	//����ii�ɑ΂��Đ��`�����@��[0,1]�̗������擾����

			//������0.2�����ŗ��q�𐶐����Ȃ�(20%�ŋ�)
			if(ru<2e-1)continue;

			//���q����+1
			n++;

			//n�̒l���ŏ��Ɏw�肵�����q�̔z��̍ő吔�萔NI�����傫��
			//���q���������邩���q�̍ő�������Ȃ�����
			if(n>NI){
				printf("number of particles is more than %d\n",NI);
				assert(n<=NI);
			}

			// ���a * x�����̗��q���Ԗ� + ���}�[�W��
			// ���a * y�����̗��q���Ԗ� + ���܂łɃ}�[�W�������i������Ԃł̏��Ƃ̐ڐG�������j
			x0[n]=2.0*rn*(j-1)+dx;
			y0[n]=2.0*rn*(i-1)+rn;

			//�����̍X�V
			random(&ii,&ru);

			//0.2�`0.5(30%)�Ŕ��ar1
			if(ru<5e-1){
				rr[n]=r1;
				wei[n] = 4.0/3.0*pi*pow(rr[n],3) * de1 ;	// ���̐�*���x
				pmi[n] = 8.0/15.0*de1*pi*pow(rr[n],5);		// I = 2/5 M r^2 ����A2/5 * (4/3*pi*r^3) * de * r^2
			}

			//0.5�`1.0(50%)�Ŕ��ar2
			else{
				rr[n]=r2;
				wei[n] = 4.0/3.0*pi*pow(rr[n],3) * de2 ;	// ���̐�*���x
				pmi[n] = 8.0/15.0*de2*pi*pow(rr[n],5);		// I = 2/5 M r^2 ����A2/5 * (4/3*pi*r^3) * de * r^2
			}

			//�����Ō��肵�����a�A���W��p���ė��q�𐶐�
			//���ۂ̃X�P�[�����s�N�Z���ɂ���Ə���������̂ŁAx,y,r��K�X�g�傷��
			pdParam2d p;
			pdBody* body;
			p.c=pdColor(1,1,1);						//�F�̎w��
			p.r=pdVertex2d(x0[n]*AMPL,y0[n]*AMPL);	//���W�̎w��
			p.v=pdVertex2d(0.0,0.0);				//�������x�͑S��0
			body=CreateCircle2d(rr[n]*AMPL);		//���a�̎w��Ɨ��q����
			body->SetParam(p);						//�p�����[�^�[�X�V
		}
	}

	printf("number of particles:%d\n",n);

	//�ߐڗ��q�𔻒肷�邽�߂̃Z�����̐ݒ�
	//(�ŏ����a*2/(��2)�ȉ��ɂ��邱�ƂŁA�P�Z���ɓ��闱�q��1��0�ɂł���)
	c=rmin*1.35;

	//(�e�핝/�Z����)�ɉE�[�̊���Z�̐؂�̂ĕ����l������+1
	//�����͔�ђ��˂��l�������]�T��t��������
	idx=(int)(w/c)+1;
	idy=(int)(h/c)+1+topcel;

	//�Z�������ŏ��Ɋm�ۂ����Z������������
	if(idx*idy>NC){
		printf("ncl is over!! %d\n",idx*idy);
		assert(false);
	}

	/* initialize */
	//�z���0������
	for(int i=1;i<=n;i++){
		for(int j=1;j<=NJ+NW;j++){
			en[i][j]=0.0;		//i���猩��j�ւ̖@�������ڐG��
			es[i][j]=0.0;		//i���猩��j�ւ̂���f�����ڐG��
		}
		for(int ij=1;ij<=NJ+NW;ij++){
			je[i][ij]=0;		//i����݂�ij�ւ̐ڐG���q���X�g(0�ԗ��q�͑��݂��Ȃ��_�~�[)
		}

		/* pos */
		qq[i]=0;	//���q�̉�]�ψ�

		/* vel */
		u0[i]=0;	//���q��x�������x
		v0[i]=0;	//���q��z�������x
		f0[i]=0;	//���q�̉�]�������x

		/* for1 */
		xf[i]=0;	//���q��x��������
		yf[i]=0;	//���q��z��������
		mf[i]=0;	//���q�̃��[�����g
	}
	for(int i=1;i<=n+NW;i++){
		/* dpm */
		u[i]=0;	//x�����ψʑ���
		v[i]=0;	//z�����ψʑ���
		f[i]=0;	//��]�ψʑ���
	}

	count=0;
	return 0;
}

int DistinctElement::MainLoop(void){

	int i;

	//���o���Ԃ��������߂�
	t=t+dt;

	//�Z���̏�����
	ncel();

	for(i=1;i<=n;i++){
		//���ꂼ��̗��q�Ɋւ��Ċe�X�̗͂�x,y,���[�����g�̐�����������
		xf[i]=0.0;
		yf[i]=0.0;
		mf[i]=0.0;

		//���E�����̌v�Z
		wcont(i);

		//���q�ԗ͂̌v�Z
		pcont(i);
	}
	//�S�Ă̗͂̌v�Z���I������炻�ꂼ��̗͂��瑬�x�A�ʒu���X�V
	//�ω����Ȃ���ΏI��
	if(nposit()==1)return -1;

	//World�ɑ��݂��闱�q�̈ʒu�����X�V
	for(i=0;(unsigned)i<GetBodyNum();i++){
		pdBody *pd = GetBody(i);
		pdParam2d p= pd->GetParam2d();

		//�V�~�����[�V�������Ă�����W�n��[0-w,0-h]�Ȃ̂ŁA[-w/2 - w/2 , -h/2 - h/2]�ɃV�t�g
		p.r.x=x0[i+1]-w/2;
		p.r.y=y0[i+1]-h/2;

		//mks�P�ʌn����s�N�Z���T�C�Y�Ɋg��
		p.r*=AMPL;

		pd->SetParam(p);
	}
	return 0;
}

int DistinctElement::Draw(void){

	//���E������`��
	pdVertex2d a[4];
	pdColor c[4];
	double wf=w/2*AMPL;
	double hf=h/2*AMPL;
	a[0]=pdVertex2d(-wf,hf);
	a[1]=pdVertex2d(-wf,-hf);
	a[2]=pdVertex2d(wf,-hf);
	a[3]=pdVertex2d(wf,hf);
	c[0]=c[1]=c[2]=c[3]=pdColor(1,1,1);
	pdDrawLine(a,c,4);
	

	//�`��񐔂̃t���[���ƁA�����Ԃ�\��
	count++;
	char str[256];
	sprintf_s(str,"Draw Frame %d",count);
	pdDrawString(str,pdVertex2d(0,0),pdColor(1,1,1));
	sprintf_s(str,"Time %6lf",t);
	pdDrawString(str,pdVertex2d(0,20),pdColor(1,1,1));

	return 0;
}

//�Z���̏�����
int DistinctElement::ncel(){
	int ib;

	//�S�ẴZ���ɂ͂����Ă��闱�q�ԍ���0(�_�~�[)��
	for(ib=1;ib<=idx*idy;ib++){
		ncl[ib]=0;
	}

	for(int i=1;i<=n;i++){
		//�Z���ԍ���
		// (y���W/�Z����)*(�i�������x�����Z��)+(x���W/�Z����)+1 (0�Ԗڂ̃Z���̓_�~�[)
		ib=(int)(y0[i]/c)*idx+(int)(x0[i]/c)+1;

		//ib�Ԗڂ̃Z���ɂ�i�Ԗڂ̗��q
		ncl[ib]=i;

		//i�Ԗڂ̗��q�̏�����ib�Ԗڂ̃Z��
		nncl[i]=ib;
	}
	return 0;
}


//���E��������󂯂�͂̌v�Z
int DistinctElement::wcont(int i){

	//i�Ԗڂ̗��q��n-1�t���[���ł̈ʒu�Ɣ��a���擾
	double xi=x0[i];
	double yi=y0[i];
	double ri=rr[i];

	//���E�����̂��ꂼ��Ɉȉ��̔ԍ������蓖�Ăė͂̌v�Z�̂Ƃ��ɗ��q�Ɠ���Ɉ�����悤�ɂ���

	//���̕�
	//�ڐG���q���X�g�̔ԍ�NJ+1
	//���q�ԍ���n+1
	int jk=NJ+1;
	int j=n+1;
	if(xi<ri){
		//���q���猩���Ǖ����̒P�ʃx�N�g��
		double as=0.0;
		double ac=-1.0;

		//���q�̒��S�ƕǂ̋���
		double gap=abs(xi);

		//�ڐG���X�g�ɒǉ�
		je[i][jk]=n+1;

		//�Փ˂�����̗��q�̔ԍ��Ai�̐ڐG���X�g��j�̔ԍ��Ai����݂�j�̒P�ʃx�N�g���Ai��j�̋�������
		//i�̗��q��j����󂯂��(en,es)=(�@�������̗�,����f�����̗�)���Z�o���X�V����
		actf(i,j,jk,as,ac,gap);
	}else{
		//�ǂɐڐG���Ă��Ȃ��ꍇ�͖@�����������񂾂�������͂�0
		en[i][jk]=0.0;
		es[i][jk]=0.0;
		//�ڐG���X�g��0�i�_�~�[���q�j��
		je[i][jk]=0;
	}

	//���̕�
	//�ڐG���q���X�g�̔ԍ�NJ+2
	//���q�ԍ���n+2
	jk=NJ+2;
	j=n+2;
	if(yi<ri){
		double as=-1.0;
		double ac=0.0;

		double gap=abs(yi);
		je[i][jk]=n+2;
		actf(i,j,jk,as,ac,gap);
	}else{
		en[i][jk]=0.0;
		es[i][jk]=0.0;
		je[i][jk]=0;
	}

	//�E�̕�
	//�ڐG���q���X�g�̔ԍ�NJ+3
	//���q�ԍ���n+3
	jk=NJ+3;
	j=n+3;
	if(xi+ri>w){
		double as=0.0;
		double ac=1.0;
		double gap=abs(xi-w);
		je[i][jk]=n+3;
		actf(i,j,jk,as,ac,gap);
	}else{
		en[i][jk]=0.0;
		es[i][jk]=0.0;
		je[i][jk]=0;
	}

	return 0;
}

//���q�Ԃ̗͂̌v�Z
int DistinctElement::pcont(int i){

	//i�Ԗڂ̗��q��n-1�t���[���ł̈ʒu�Ɣ��a���擾
	double xi=x0[i];
	double yi=y0[i];
	double ri=rr[i];

	//�l�����闱�q�������Ă���Z�����ڐG������Z���͈̔͂��w��
	//���̕ǂ���left,right�A���̏����琔����up,under�Ԗڂ̃Z���Ɉ͂܂�鐳���`�͈͓̔�
	int lup=(int)((yi+2.0*(rmax))/c);
	int lun=(int)((yi-2.0*(rmax))/c);
	int llf=(int)((xi-2.0*(rmax))/c);
	int lrg=(int)((xi+2.0*(rmax))/c);

	//�V�����̃Z���͖����̂Ŗ߂��i���݂���y�����̃Z���͈̔͂�[0 - idy-1]�j
	if(lup>=idy)lup=idy-1;

	//�ǂ��E�̃Z���͖����̂Ŗ߂�(���݂���x�����̃Z���͈̔͂�[0 - idx-1])
	if(lrg>=idx)lrg=idx-1;

	//����艺�̃Z���͖����̂Œ��߂���
	if(lun<0)lun=0;

	//�ǂ�荶�̗��q�͖����̂Œ��߂���
	if(llf<0)llf=0;

	//�l������V��Z�����l�����鏰�Z���̂ق�����ɂ��邩�A���[�Z����荶�ɉE�[�Z��������(�{���͋N���肦�Ȃ�)
	//�܂�A���q�����ɂQ���q���ȏ��������A�V��𗱎q���ȏ�˂��������A�Ȃǂ̌����Ōv�Z�Ɉُ킪�N�����Ă���
	//�ُ�̂̂��������q���R���\�[���Ƀf�o�b�O�o�͂��ďI������
	if(lup<lun || lrg<llf){
		cout << "cel over" << endl;
		cout << "lup=" << lup
			<< " lun=" << lun
			<< " llf=" << llf
			<< " lrg=" << lrg
			<< " c=" << c
			<< " rmax=" << rmax
			<< " xi="<< xi
			<< " yi=" << yi
			<< " idy="<< idy
			<< endl;
//		assert(false);
	}

	//ly,lx�����[�v�ŉ񂵂ĐڐG������S�ẴZ�����l��
	for(int ly=lun;ly<=lup;ly++){
		for(int lx=llf;lx<=lrg;lx++){

			//�l�����Ă���Z���̔ԍ��́ily*�i�������x�����Z�����j+lx+1 (0�ԃZ���̓_�~�[)
			int ib=ly*idx+lx+1;

			//�l������Z���ɓ����Ă��闱�q�̔ԍ����擾����
			//�擾�����ԍ��̗��q���l������i�Ԗڗ��q�̒��S����A���a(rmax*2)�ȓ��ɒ��S�����݂���A�܂�Փ˂��Ă��邩������Ȃ����q
			int j=ncl[ib];

			//�擾�����ԍ���0�i�_�~�[�܂�Z�����󔒁j�̂Ƃ��͎���
			if(j<=0) continue;

			//�擾�����ԍ����������g�̂Ƃ��͎���
			if(j==i) continue;

			//��̓�̏����𖞂����Ȃ��Ƃ��A���̃Z���ɂ͍l������ׂ����q�������Ă���

			//�ڐG���X�g�ԍ����_�~�[�ɍX�V
			int jk=0;

			//�ڐG���X�g[0]�̓_�~�[
			//�O��̐ڐG���X�g[1-NJ]�ɂ��łɍl�����闱�q�����ɖ��������ׂ�
			//�ڐG���X�g[NJ+1 - NJ+NW]�͕Ǘp�Ɋ��蓖�Ă��Ă���
			for(int jj=1;jj<=NJ;jj++){
				if(je[i][jj]==j){
					//�O��ڐG���Ă����̂ŁA���̐ڐG���X�g�̔ԍ����擾����
					jk=jj;
					break;
				}
			}

			//jk��0�̂܂܂ł���A�܂�O��ڐG���Ă��Ȃ������ꍇ
			if(jk==0){
				//���x�͐ڐG���X�g�Ƀ_�~�[���q�܂�󔒂����������ׂāA
				//����ΐڐG���X�g���X�V���A���̐ڐG���X�g�̔ԍ����擾����
				for(int jj=1;jj<=NJ;jj++){
					if(je[i][jj]==0){
						jk=jj;
						je[i][jj]=j;
						break;
					}
				}
			}

			//�ˑR�Ƃ���jk��0�Ƃ������Ƃ́A�ڐG���X�g�ɋ󂫂��������z�肵�Ă����ȏ�̗��q�ƈ�x�ɐڐG���Ă��܂���
			//�Ƃ������ƂȂ̂ŏI������
			if(jk==0){
				cout << "junction over" << endl;
				assert(false);
			}

			//�ΏۂƂȂ闱�q��x,y,r���擾����
			double xj=x0[j];
			double yj=y0[j];
			double rj=rr[j];

			//�ΏۂƂȂ闱�q�Ƃ̒��S�̊Ԋu���v�Z����
			double gap=hypot(xi-xj,yi-yj);

			//�i���S�̊Ԋu<��̔��a�̘a�j�̂Ƃ��A�ڐG���Ă���
			if(gap<abs(ri+rj)){
				// i > j�̂Ƃ��������l������
				// ���̏������ɂ͕K��(i,j)��(j,i)�̓�̑g�ݍ��킹�œ�����B
				// ���̗������v�Z����̂͌v�Z�ʂ̖��ʂȂ̂ŕЕ������Ōv�Z���A����ɂ��t�����̗͂�������
				if(i>j){
					//i�Ԗڂ̗��q���猩��j�Ԗڂ̗��q�̕����̒P�ʃx�N�g�����w�肷��
					double ac=(xj-xi)/gap;
					double as=(yj-yi)/gap;

					//j�Ԗڂ̗��q�̐ڐG���X�g�ł�i�̔ԍ����擾����
					int j0=0;
					for(int jj=1;jj<=NJ;jj++){
						if(je[j][jj]==i){
							j0=jj;
							break;
						}
					}

					//���̂Ƃ��Aj0=0�Ƃ͐�΂Ȃ�Ȃ��A�Ȃ��Ȃ�Ai�͏������ق�����for�����񂵂Ă���̂ŁA
					//��̗��q�ԍ���a<b�ƂȂ���ۂɐڐG���Ă��闱�qa,b���l�����Ƃ��A
					//gap<abs(ri+rj)�𖞂������̑g�ݍ��킹�͕K��(i,j)=(a,b)��(b,a)�̏��Ԃł���
					//�܂�i>j�𖞂������ɂ͊���(a,b)�̑g�ݍ��킹�ŐڐG���X�g���X�V��������̓I�Ȍv�Z�͂��Ă��Ȃ��Ƃ��Ȃ̂ŁA
					//(b,a)�̂Ƃ��Aa�̐ڐG���X�g�ɂ͂��Ȃ炸���qb�����݂���
					//�܂肱�ꂪ���݂��Ȃ��Ƃ������Ƃ͂Ȃ�炩�ُ̈킪�N���Ă���̂ŏI������
					if(j0==0){
						cout << "junction over 2" << endl;
						assert(false);
					}

					//i,j,jk,as,ac,gap����en[i][jk]��es[i][jk]���X�V
					actf(i,j,jk,as,ac,gap);

					//�����͂�j,j0�ɑ債�Ă��X�V����
					en[j][j0]=en[i][jk];
					es[j][j0]=es[i][jk];
				}
			}
			//�ߐڃZ���ɂ͂������A�ڐG�͂��Ă��Ȃ������ꍇ
			else{
				//�ڐG���X�gjk�Ԗڂ̗��q�Ƃ̖@���A����f�����̐ڐG�͓͂��R0�ł���
				en[i][jk]=0.0;
				en[i][jk]=0.0;

				//����ɐڐG���X�g�����������
				je[i][jk]=0;
			}
		}
	}
	return 0;
}

//n�t���[���ł�xf,yf,mf���X�V���ꂽ��Ԃ���
//���x�A�ʒu�̕ψʁA�ʒu�����ꂼ��X�V����
int DistinctElement::nposit(){
	double sum=0.0;
	for(int i=1;i<=n;i++){

		//���ʂƂ��ĎZ�o���ꂽ�͂ɏd�͂��l�����đ��x���X�V
		v0[i]=v0[i]+(yf[i]-wei[i]*g)/wei[i]*dt;
		u0[i]=u0[i]+xf[i]/wei[i]*dt;
		f0[i]=f0[i]+mf[i]/pmi[i]*dt;

		//���x�ƁA�O��̕ψʂ���A���̃t���[���ł̕ψʂ��X�V
		//�ψ� = ( v0(n-1) + v0(n) ) / 2 * dt
		// v0(n-1)*dt = �O��̕ψ� �Ƒ������Ă���
		v[i]=(v0[i]*dt+v[i])/2.0;
		u[i]=(u0[i]*dt+u[i])/2.0;
		f[i]=(f0[i]*dt+f[i])/2.0;

		//�ψʂ����Z���Č��݂̏�Ԃ��X�V
		y0[i]=y0[i]+v[i];
		x0[i]=x0[i]+u[i];
		qq[i]=qq[i]+f[i];

		//��~�����̂��߂Ɉʒu�̕ψʂ̘a���Ƃ�
		sum+=hypot(u[i],v[i]);
	}

	//�ψʂ̂P���q������̕��ς��Z�o����
	//�ꗱ�q�̕��ϕψʂ��w��l�ȉ��ɂȂ�����
	//�ω����Ȃ��Ȃ����Ƃ��ďI��
	if(sum/n < (dt*dt*g/2)*0.9){
//		return 1;
	}else{
		return 0;
	}
	return 0;
}

// i = ���ڂ��Ă��闱�q�ԍ�
// j = �Փ˂��Ă���Ώۂ̗��q�ԍ��i�������͕ǁj
// jk = i�̐ڐG���X�g�ł�j�̔ԍ�
// as = i���猩��j������sin
// ac = i���猩��j������cos
// gap = ���qi,j�̒��S�ԋ���
// �������Ƃ��Ď󂯎��A
// ��
// �^���������̍��̌W��(knn,kss,vnn,vss)�����߂āA
// en[i][jk] = i�̗��q���ڐG���X�g��jk�Ԗڂ̗��q����󂯂�@�������̒e����
// es[i][jk] = i�̗��q���ڐG���X�g��jk�Ԗڂ̗��q����󂯂邹��f�����̒e����
// dn = �S�����̒l
// ds = �S�����̒l
// ���Z�o����B
// �e�����̒l��n-1�t���[���ł̒l���g���̂Ŕz��Ƃ��ĕۑ����Ă���
// �e���� = �����R�� = ���k�� = �ڐG�� �Ƃ��đ��������Ă���
// ��
// xf[i] = i���q���󂯂�x�����̗�
// yf[i] = i���q���󂯂�y�����̗�
// mf[i] = i���q���󂯂��]���[�����g
// ���Z�o���A�����j���ǂŖ�����΁A
// xf[j] = j���q���󂯂�x�����̗�
// yf[j] = j���q���󂯂�y�����̗�
// mf[j] = j���q���󂯂��]���[�����g
// ���X�V����B�i�ЂƂ�i,j�̑g�ݍ��킹�ɑ΂��Ĉ�񂵂����̊֐��͌Ă΂�Ȃ��j
int DistinctElement::actf(int i,int j,int jk,double as,double ac,double gap){
	//���q�̔��a���擾
	double ri=rr[i];

	double rj;
	double dis;
	double wei3;

	//j�̗��q�ԍ���n�ȉ��̂Ƃ���j�͗��q�Ȃ̂ŁA���a���擾���A
	//���S���Ȃ������̂�����̗��q�������Ă���̈�ł̒������v�Z��
	//�v�Z����ۂɗp���镽�ώ��ʂ��v�Z����
	if((j-n)<=0){
		rj=rr[j];
		dis=ri+rj-(gap);
		wei3=2.0*wei[i]*wei[j]/(wei[i]+wei[j]);
	}

	//j���ǂ̂Ƃ��́A���a��0�ŁA�d����0�Ƃ���B
	//���ώ��ʂ�i���q�̎��ʂ�p����
	else{
		rj=0.0;
		dis=ri-(gap);
		wei3=wei[i];
	}

	//n-1�t���[���ł̒e���͂��爳�k�͂����߂�
	//en[i][jk]��0�̂Ƃ��i�܂菉��j�ł́A���k�͂�1.0
	double enn=en[i][jk];
	if(enn==0.0)enn=1.0;

	//�O��̈��k�͂�0�����ł��邱�Ƃ͖����̂ŁA���������Ȃ�����ُ�Ƃ��ďI������
	if(enn<0.0){
		cout << " enn < 0 " <<endl;
		assert(false);
	}

	double b1;
	double knn;	//�@�������̒e���͂̍��̌W��
	double kss;	//����f�����̒e���͂̍��̌W��
	double vnn;	//�@�������̔S����R�͂̍��̌W��
	double vss;	//����f�����̔S����R�͂̍��̌W��
	//j�����q�̏ꍇ�̒e�����A�S�����̒萔�����߂�
	if((j-n)<=0){	
		b1=pow( (3.0/2.0 * (1.0-po*po)/e * ri*rj/(ri+rj) * enn ) , 1.0/3.0 );
		knn=2.0/3.0*b1*e/(1.0-po*po);	//�w���c�̒e���ڐG���_���狁�߂�
		kss=knn*so;						//����f�����̌W���͒萔��ɂȂ�̂ŁA�ݒ肵���ϐ����|����
		vnn=2.0*sqrt(wei3*knn);			//�ՊE�����̏������炱�̂悤�ɋ��߂���
		vss=vnn*sqrt(so);				//��Ɠ���
	}
	//j���ǂ̏ꍇ�����l�ɋ��߂�
	else{
		b1=pow( ( ( 3.0/4.0 * ri * ( (1.0-po*po)/e + (1.0-poww*poww)/ew ) ) *enn ) , 1.0/3.0 );
		knn= 4.0/3.0 * b1 * e*ew/((1.0-po*po)*ew+(1.0-poww*poww)*e) ;
		kss=knn*so;
		vnn=2.0*sqrt(wei3*knn);
		vss=vnn*sqrt(so);
	}

	//�S�����̕␳
	vnn*=uamp;
	vss*=uamp;

	//�����@�̈�������𖞂����Ȃ��Ƃ��A�I������
	double ddt=(1e-1)*sqrt(wei3/knn);
	//ddt=2*sqrt(wei3/knn); �Ɨ��_���ł͂Ȃ��Ă��邪�A�����炭��x��������𖞂����Ȃ��ƁA
	//���̌�����U�I�Ɉ��������j��悤�ɂȂ�̂Ŋɂ߂ɐݒ肳��Ă���
	if(ddt<dt){
//		printf("over !! ddt=%6lf %6d %6d %6d %6lf %6lf\n",ddt,i,j,jk,knn,wei[i]);
//		assert(false);
	}

	//n-1�t���[���ł̖@�������̕ψʗʂƂ���f�����̕ψʗʂ��v�Z����
	double un=(u[i]-u[j])*(ac)+(v[i]-v[j])*(as);
	double us=-(u[i]-u[j])*(as)+(v[i]-v[j])*(ac)+(ri*f[i]+rj*f[j]);

	//�@�������̐ڐG�͂�0�̂Ƃ��i�ڐG������̂Ƃ��j
	if(en[i][jk]==0.0){
		//���ꂩ��v�Z����悤�Ȓe���͂�S����R��
		//�d�Ȃ�n�߂Ă��甭������̂ŁA
		//n-1�t���[������d�Ȃ�n�߂�܂ł̕ψʂ͌v�Z���珜�O����ׂ��ł���
		//���̂��߂̕␳���s��

		//����f�����̕ψʂ�(�@�������̏d�Ȃ������� / dt�Ŗ@�������Ɉړ���������)�{�ɂ��邱�ƂŁA
		//dt�ł̂���f�����̕ψʂ��A�d�Ȃ�n�߂Ă��猻�n�X�܂ł̂���f�����̕ψʂɂ���
		if(un!=0.0){
			us=us*dis/un;
		}

		//�@�������̕ψʂ͏d�Ȃ������̋����Ƃ���
		un=dis;

	}

	//�e���͂��X�V����
	en[i][jk]=knn*un+en[i][jk];
	es[i][jk]=es[i][jk]+kss*us;

	//�@���A����f�����̔S����R�͂��v�Z����
	double dn=vnn*un/dt;
	double ds=vss*us/dt;

	//�@�������̈��k��0�����̂Ƃ��i�܂�ڐG���Ă��Ȃ����́j
	//�͂�0�ɂ��ĐڐG���X�g������폜���ĐڐG�͂�0�ɂ��Ċ֐�����o��
	//�����炭n-1�t���[���ŐڐG���Ă���n�t���[���ŐڐG���Ȃ��Ȃ�Ƃ��A
	//�e����������߂Â�������ɂȂ邪����͎��ۂɂ͂Ȃ����ƂɋN������
	if(en[i][jk]<0.0){
		en[i][jk]=0.0;
		es[i][jk]=0.0;
		je[i][jk]=0;
		return 0;
	}

	//�������Ȃ�ł��邩���ꍇ�킯���Ė��C�W�������߂�
	double frc;
	//���q-���q�Ԃ̐ڐG�̏ꍇ�͗��q-���q�Ԗ��C�W����I��
	if((j-n)<=0){
		frc=fri;
	}
	//���q-�ǊԂ̐ڐG�ł͗��q-�ǊԖ��C�W����I��
	else{
		frc=frw;
	}

	//�N�[�����̖��C�@������A����f�����̗͂��傫���Ȃ�
	//�i�����R��*���C�W���j�ȏ�ɂȂ�ƁA�����門�C�̗͂́i�����R��*���C�W���j�ɂȂ�
	if((abs(es[i][jk])-frc*en[i][jk])>0){
		//es[i][jk]�̒l�͕�����ۂ����܂�frc*en[i][jk]�ɂȂ�
		if(es[i][jk]>=0){
			es[i][jk]=frc*abs(en[i][jk]);
		}else{
			es[i][jk]=frc*(-abs(en[i][jk]));
		}
		//���̂Ƃ��A�S����R�͂�0�ƂȂ�
		ds=0.0;
	}

	//����āA�ŏI�I�ɋ��߂���ڐ������A����f�����́A
	//�e���͂ƔS����R�͂����킹���͂����܂�
	double hn=en[i][jk]+dn;
	double hs=es[i][jk]+ds;

	//�ŏI�I�Ȗ@���A����f�����̗͂ɒP�ʃx�N�g����cos,sin���|���āAx,y�����̗͂ɂ��āA�X�V����
	//�S���q�������Z���Ă����̂ŁA����ł͂Ȃ��a���Ƃ�B
	xf[i]+=-hn*(ac)+hs*(as);
	yf[i]+=-hn*(as)-hs*(ac);
	//��]���[�����g�͔��a�̒����ɂ���f�����̗͂��|�������̂ł���
	mf[i]+=-ri*hs;

	//�ڐG���Ă���j���ǂł͂Ȃ����q�Ȃ�A���Ό����̗͂��X�V����
	if(jk<=NJ){
		xf[j]+=hn*(ac)-hs*(as);
		yf[j]+=hn*(as)+hs*(ac);
		mf[j]+=-rj*hs;
	}
	return 0;
}

//���͂��ꂽ�lii�ɑ΂��Đ��`�����@��[0,1]�̗�����Ԃ�
int DistinctElement::random(int* ii,double* ru){
	(*ii)=(*ii)*65539;
	if((*ii)<0)(*ii)=((*ii)+2147483647)+1;
	(*ru)=(*ii)*0.4656613e-9;
	return 0;
}