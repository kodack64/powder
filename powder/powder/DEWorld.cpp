#include "DEWorld.h"
#include "Powder\ApplicationProperty.h"

#define prop ApplicationProperty

int DistinctElement::Init(){
	pdSetScreenColor(pdColor(0,0,0));

	int ii=584287;		//初期乱数

	//時間の初期化
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

	so=1.0/2.0/(1.0+po);	//縦弾性係数とせん断弾性係数の比

	//半径の最大、最小値を指定
	rmax=max(r1,r2);
	rmin=min(r1,r2);
	double rn=rmax+merg;	//粒子の配置間隔/2

	ipx=(int)(w/(2.0*rn));	//幅÷直径が初期配置でのx方向の粒子数
	ipy=(int)(h/(2.0*rn));	//高さ÷直径で初期配置でのy方向の粒子数

	//粒子数初期化
	n=0;

	double dx;	//左の壁から最初の粒子までの距離
	double ip;	//そのy座標の段でのx方向の粒子の数

	for(int i=1;i<=ipy;i++){

		//奇数段と偶数段で噛み合う形になるように数値を設定する
		//つまり、ipx個とipx-1個の段が交互にくるようにする（この個数がip）
		//あわせて左の壁から粒子までのマージンも調節する
		//偶数段
		if(i%2==0){
			dx=2.0*rn;
			ip=ipx-1;
		}
		//奇数段
		else{
			dx=rn;
			ip=ipx;
		}

		for(int j=1;j<=ip;j++){

			double ru;
			random(&ii,&ru);	//整数iiに対して線形合同法で[0,1]の乱数を取得する

			//乱数が0.2未満で粒子を生成しない(20%で空欄)
			if(ru<2e-1)continue;

			//粒子数を+1
			n++;

			//nの値が最初に指定した粒子の配列の最大数定数NIよりも大きい
			//粒子が多すぎるか粒子の最大個数が少なすぎる
			if(n>NI){
				printf("number of particles is more than %d\n",NI);
				assert(n<=NI);
			}

			// 直径 * x方向の粒子数番目 + 左マージン
			// 直径 * y方向の粒子数番目 + 床までにマージンを作る（初期状態での床との接触を避ける）
			x0[n]=2.0*rn*(j-1)+dx;
			y0[n]=2.0*rn*(i-1)+rn;

			//乱数の更新
			random(&ii,&ru);

			//0.2～0.5(30%)で半径r1
			if(ru<5e-1){
				rr[n]=r1;
				wei[n] = 4.0/3.0*pi*pow(rr[n],3) * de1 ;	// 球体積*密度
				pmi[n] = 8.0/15.0*de1*pi*pow(rr[n],5);		// I = 2/5 M r^2 から、2/5 * (4/3*pi*r^3) * de * r^2
			}

			//0.5～1.0(50%)で半径r2
			else{
				rr[n]=r2;
				wei[n] = 4.0/3.0*pi*pow(rr[n],3) * de2 ;	// 球体積*密度
				pmi[n] = 8.0/15.0*de2*pi*pow(rr[n],5);		// I = 2/5 M r^2 から、2/5 * (4/3*pi*r^3) * de * r^2
			}

			//乱数で決定した半径、座標を用いて粒子を生成
			//実際のスケールをピクセルにすると小さすぎるので、x,y,rを適宜拡大する
			pdParam2d p;
			pdBody* body;
			p.c=pdColor(1,1,1);						//色の指定
			p.r=pdVertex2d(x0[n]*AMPL,y0[n]*AMPL);	//座標の指定
			p.v=pdVertex2d(0.0,0.0);				//初期速度は全て0
			body=CreateCircle2d(rr[n]*AMPL);		//半径の指定と粒子生成
			body->SetParam(p);						//パラメーター更新
		}
	}

	printf("number of particles:%d\n",n);

	//近接粒子を判定するためのセル幅の設定
	//(最小半径*2/(√2)以下にすることで、１セルに入る粒子は1個か0個にできる)
	c=rmin*1.35;

	//(容器幅/セル幅)に右端の割り算の切り捨て分を考慮して+1
	//高さは飛び跳ねを考慮した余裕を付け加える
	idx=(int)(w/c)+1;
	idy=(int)(h/c)+1+topcel;

	//セル数が最初に確保したセル数よりも多い
	if(idx*idy>NC){
		printf("ncl is over!! %d\n",idx*idy);
		assert(false);
	}

	/* initialize */
	//配列の0初期化
	for(int i=1;i<=n;i++){
		for(int j=1;j<=NJ+NW;j++){
			en[i][j]=0.0;		//iから見たjへの法線方向接触力
			es[i][j]=0.0;		//iから見たjへのせん断方向接触力
		}
		for(int ij=1;ij<=NJ+NW;ij++){
			je[i][ij]=0;		//iからみたijへの接触粒子リスト(0番粒子は存在しないダミー)
		}

		/* pos */
		qq[i]=0;	//粒子の回転変位

		/* vel */
		u0[i]=0;	//粒子のx方向速度
		v0[i]=0;	//粒子のz方向速度
		f0[i]=0;	//粒子の回転方向速度

		/* for1 */
		xf[i]=0;	//粒子のx方向合力
		yf[i]=0;	//粒子のz方向合力
		mf[i]=0;	//粒子のモーメント
	}
	for(int i=1;i<=n+NW;i++){
		/* dpm */
		u[i]=0;	//x方向変位増分
		v[i]=0;	//z方向変位増分
		f[i]=0;	//回転変位増分
	}

	count=0;
	return 0;
}

int DistinctElement::MainLoop(void){

	int i;

	//感覚時間だけすすめる
	t=t+dt;

	//セルの初期化
	ncel();

	for(i=1;i<=n;i++){
		//それぞれの粒子に関して各々の力のx,y,モーメントの成分を初期化
		xf[i]=0.0;
		yf[i]=0.0;
		mf[i]=0.0;

		//境界条件の計算
		wcont(i);

		//粒子間力の計算
		pcont(i);
	}
	//全ての力の計算が終わったらそれぞれの力から速度、位置を更新
	//変化がなければ終了
	if(nposit()==1)return -1;

	//Worldに存在する粒子の位置情報を更新
	for(i=0;(unsigned)i<GetBodyNum();i++){
		pdBody *pd = GetBody(i);
		pdParam2d p= pd->GetParam2d();

		//シミュレーションしている座標系は[0-w,0-h]なので、[-w/2 - w/2 , -h/2 - h/2]にシフト
		p.r.x=x0[i+1]-w/2;
		p.r.y=y0[i+1]-h/2;

		//mks単位系からピクセルサイズに拡大
		p.r*=AMPL;

		pd->SetParam(p);
	}
	return 0;
}

int DistinctElement::Draw(void){

	//境界条件を描画
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
	

	//描画回数のフレームと、実時間を表示
	count++;
	char str[256];
	sprintf_s(str,"Draw Frame %d",count);
	pdDrawString(str,pdVertex2d(0,0),pdColor(1,1,1));
	sprintf_s(str,"Time %6lf",t);
	pdDrawString(str,pdVertex2d(0,20),pdColor(1,1,1));

	return 0;
}

//セルの初期化
int DistinctElement::ncel(){
	int ib;

	//全てのセルにはいっている粒子番号を0(ダミー)に
	for(ib=1;ib<=idx*idy;ib++){
		ncl[ib]=0;
	}

	for(int i=1;i<=n;i++){
		//セル番号は
		// (y座標/セル幅)*(段あたりのx方向セル)+(x座標/セル幅)+1 (0番目のセルはダミー)
		ib=(int)(y0[i]/c)*idx+(int)(x0[i]/c)+1;

		//ib番目のセルにはi番目の粒子
		ncl[ib]=i;

		//i番目の粒子の所属はib番目のセル
		nncl[i]=ib;
	}
	return 0;
}


//境界条件から受ける力の計算
int DistinctElement::wcont(int i){

	//i番目の粒子のn-1フレームでの位置と半径を取得
	double xi=x0[i];
	double yi=y0[i];
	double ri=rr[i];

	//境界条件のそれぞれに以下の番号を割り当てて力の計算のときに粒子と同列に扱えるようにする

	//左の壁
	//接触粒子リストの番号NJ+1
	//粒子番号はn+1
	int jk=NJ+1;
	int j=n+1;
	if(xi<ri){
		//粒子から見た壁方向の単位ベクトル
		double as=0.0;
		double ac=-1.0;

		//粒子の中心と壁の距離
		double gap=abs(xi);

		//接触リストに追加
		je[i][jk]=n+1;

		//衝突した二つの粒子の番号、iの接触リストのjの番号、iからみたjの単位ベクトル、iとjの距離から
		//iの粒子がjから受ける力(en,es)=(法線方向の力,せん断方向の力)を算出し更新する
		actf(i,j,jk,as,ac,gap);
	}else{
		//壁に接触していない場合は法線方向もせんだん方向も力は0
		en[i][jk]=0.0;
		es[i][jk]=0.0;
		//接触リストも0（ダミー粒子）に
		je[i][jk]=0;
	}

	//下の壁
	//接触粒子リストの番号NJ+2
	//粒子番号はn+2
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

	//右の壁
	//接触粒子リストの番号NJ+3
	//粒子番号はn+3
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

//粒子間の力の計算
int DistinctElement::pcont(int i){

	//i番目の粒子のn-1フレームでの位置と半径を取得
	double xi=x0[i];
	double yi=y0[i];
	double ri=rr[i];

	//考慮する粒子が属しているセルが接触しうるセルの範囲を指定
	//左の壁からleft,right、下の床から数えてup,under番目のセルに囲まれる正方形の範囲内
	int lup=(int)((yi+2.0*(rmax))/c);
	int lun=(int)((yi-2.0*(rmax))/c);
	int llf=(int)((xi-2.0*(rmax))/c);
	int lrg=(int)((xi+2.0*(rmax))/c);

	//天井より上のセルは無いので戻す（存在するy方向のセルの範囲は[0 - idy-1]）
	if(lup>=idy)lup=idy-1;

	//壁より右のセルは無いので戻す(存在するx方向のセルの範囲は[0 - idx-1])
	if(lrg>=idx)lrg=idx-1;

	//床より下のセルは無いので調節する
	if(lun<0)lun=0;

	//壁より左の粒子は無いので調節する
	if(llf<0)llf=0;

	//考慮する天井セルより考慮する床セルのほうが上にあるか、左端セルより左に右端セルがある(本来は起こりえない)
	//つまり、粒子が床に２粒子分以上潜ったり、天井を粒子分以上突き抜けた、などの原因で計算に異常が起こっている
	//異常ののあった粒子をコンソールにデバッグ出力して終了する
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

	//ly,lxをループで回して接触しうる全てのセルを考慮
	for(int ly=lun;ly<=lup;ly++){
		for(int lx=llf;lx<=lrg;lx++){

			//考慮しているセルの番号は（ly*段あたりのx方向セル数）+lx+1 (0番セルはダミー)
			int ib=ly*idx+lx+1;

			//考慮するセルに入っている粒子の番号を取得する
			//取得した番号の粒子が考慮するi番目粒子の中心から、半径(rmax*2)以内に中心が存在する、つまり衝突しているかもしれない粒子
			int j=ncl[ib];

			//取得した番号が0（ダミーつまりセルが空白）のときは次へ
			if(j<=0) continue;

			//取得した番号が自分自身のときは次へ
			if(j==i) continue;

			//上の二つの条件を満たさないとき、そのセルには考慮するべき粒子が入っている

			//接触リスト番号をダミーに更新
			int jk=0;

			//接触リスト[0]はダミー
			//前回の接触リスト[1-NJ]にすでに考慮する粒子が既に無いか調べる
			//接触リスト[NJ+1 - NJ+NW]は壁用に割り当てられている
			for(int jj=1;jj<=NJ;jj++){
				if(je[i][jj]==j){
					//前回接触していたので、その接触リストの番号を取得する
					jk=jj;
					break;
				}
			}

			//jkが0のままである、つまり前回接触していなかった場合
			if(jk==0){
				//今度は接触リストにダミー粒子つまり空白が無いか調べて、
				//あれば接触リストを更新し、その接触リストの番号を取得する
				for(int jj=1;jj<=NJ;jj++){
					if(je[i][jj]==0){
						jk=jj;
						je[i][jj]=j;
						break;
					}
				}
			}

			//依然としてjkが0ということは、接触リストに空きが無い＝想定していた以上の粒子と一度に接触してしまった
			//ということなので終了する
			if(jk==0){
				cout << "junction over" << endl;
				assert(false);
			}

			//対象となる粒子のx,y,rを取得する
			double xj=x0[j];
			double yj=y0[j];
			double rj=rr[j];

			//対象となる粒子との中心の間隔を計算する
			double gap=hypot(xi-xj,yi-yj);

			//（中心の間隔<二つの半径の和）のとき、接触している
			if(gap<abs(ri+rj)){
				// i > jのときだけを考慮する
				// この条件文には必ず(i,j)と(j,i)の二つの組み合わせで二回入る。
				// この両方を計算するのは計算量の無駄なので片方だけで計算し、相手にも逆向きの力を代入する
				if(i>j){
					//i番目の粒子から見たj番目の粒子の方向の単位ベクトルを指定する
					double ac=(xj-xi)/gap;
					double as=(yj-yi)/gap;

					//j番目の粒子の接触リストでのiの番号を取得する
					int j0=0;
					for(int jj=1;jj<=NJ;jj++){
						if(je[j][jj]==i){
							j0=jj;
							break;
						}
					}

					//このとき、j0=0とは絶対ならない、なぜなら、iは小さいほうからfor文を回しているので、
					//二つの粒子番号でa<bとなる実際に接触している粒子a,bを考えたとき、
					//gap<abs(ri+rj)を満たす時の組み合わせは必ず(i,j)=(a,b)→(b,a)の順番である
					//つまりi>jを満たす時には既に(a,b)の組み合わせで接触リストを更新したが具体的な計算はしていないときなので、
					//(b,a)のとき、aの接触リストにはかならず粒子bが存在する
					//つまりこれが存在しないということはなんらかの異常が起きているので終了する
					if(j0==0){
						cout << "junction over 2" << endl;
						assert(false);
					}

					//i,j,jk,as,ac,gapからen[i][jk]とes[i][jk]を更新
					actf(i,j,jk,as,ac,gap);

					//同じ力をj,j0に大しても更新する
					en[j][j0]=en[i][jk];
					es[j][j0]=es[i][jk];
				}
			}
			//近接セルにはいたが、接触はしていなかった場合
			else{
				//接触リストjk番目の粒子との法線、せん断方向の接触力は当然0であり
				en[i][jk]=0.0;
				en[i][jk]=0.0;

				//さらに接触リストから消去する
				je[i][jk]=0;
			}
		}
	}
	return 0;
}

//nフレームでのxf,yf,mfが更新された状態から
//速度、位置の変位、位置をそれぞれ更新する
int DistinctElement::nposit(){
	double sum=0.0;
	for(int i=1;i<=n;i++){

		//結果として算出された力に重力を考慮して速度を更新
		v0[i]=v0[i]+(yf[i]-wei[i]*g)/wei[i]*dt;
		u0[i]=u0[i]+xf[i]/wei[i]*dt;
		f0[i]=f0[i]+mf[i]/pmi[i]*dt;

		//速度と、前回の変位から、このフレームでの変位を更新
		//変位 = ( v0(n-1) + v0(n) ) / 2 * dt
		// v0(n-1)*dt = 前回の変位 と多分している
		v[i]=(v0[i]*dt+v[i])/2.0;
		u[i]=(u0[i]*dt+u[i])/2.0;
		f[i]=(f0[i]*dt+f[i])/2.0;

		//変位を加算して現在の状態を更新
		y0[i]=y0[i]+v[i];
		x0[i]=x0[i]+u[i];
		qq[i]=qq[i]+f[i];

		//停止条件のために位置の変位の和をとる
		sum+=hypot(u[i],v[i]);
	}

	//変位の１粒子あたりの平均を算出する
	//一粒子の平均変位が指定値以下になったら
	//変化がなくなったとして終了
	if(sum/n < (dt*dt*g/2)*0.9){
//		return 1;
	}else{
		return 0;
	}
	return 0;
}

// i = 注目している粒子番号
// j = 衝突している対象の粒子番号（もしくは壁）
// jk = iの接触リストでのjの番号
// as = iから見たj方向のsin
// ac = iから見たj方向のcos
// gap = 粒子i,jの中心間距離
// を引数として受け取り、
// ↓
// 運動方程式の項の係数(knn,kss,vnn,vss)を求めて、
// en[i][jk] = iの粒子が接触リストのjk番目の粒子から受ける法線方向の弾性項
// es[i][jk] = iの粒子が接触リストのjk番目の粒子から受けるせん断方向の弾性項
// dn = 粘性項の値
// ds = 粘性項の値
// を算出する。
// 弾性項の値はn-1フレームでの値を使うので配列として保存してある
// 弾性項 = 垂直抗力 = 圧縮力 = 接触力 として多分扱われている
// ↓
// xf[i] = i粒子が受けるx方向の力
// yf[i] = i粒子が受けるy方向の力
// mf[i] = i粒子が受ける回転モーメント
// を算出し、さらにjが壁で無ければ、
// xf[j] = j粒子が受けるx方向の力
// yf[j] = j粒子が受けるy方向の力
// mf[j] = j粒子が受ける回転モーメント
// を更新する。（ひとつのi,jの組み合わせに対して一回しかこの関数は呼ばれない）
int DistinctElement::actf(int i,int j,int jk,double as,double ac,double gap){
	//粒子の半径を取得
	double ri=rr[i];

	double rj;
	double dis;
	double wei3;

	//jの粒子番号がn以下のときはjは粒子なので、半径を取得し、
	//中心をつなぐ線分のうち二つの粒子が属している領域での長さを計算し
	//計算する際に用いる平均質量を計算する
	if((j-n)<=0){
		rj=rr[j];
		dis=ri+rj-(gap);
		wei3=2.0*wei[i]*wei[j]/(wei[i]+wei[j]);
	}

	//jが壁のときは、半径は0で、重さも0とする。
	//平均質量はi粒子の質量を用いる
	else{
		rj=0.0;
		dis=ri-(gap);
		wei3=wei[i];
	}

	//n-1フレームでの弾性力から圧縮力を求める
	//en[i][jk]が0のとき（つまり初回）では、圧縮力は1.0
	double enn=en[i][jk];
	if(enn==0.0)enn=1.0;

	//前回の圧縮力が0未満であることは無いので、もしそうなったら異常として終了する
	if(enn<0.0){
		cout << " enn < 0 " <<endl;
		assert(false);
	}

	double b1;
	double knn;	//法線方向の弾性力の項の係数
	double kss;	//せん断方向の弾性力の項の係数
	double vnn;	//法線方向の粘性抵抗力の項の係数
	double vss;	//せん断方向の粘性抵抗力の項の係数
	//jが粒子の場合の弾性項、粘性項の定数を求める
	if((j-n)<=0){	
		b1=pow( (3.0/2.0 * (1.0-po*po)/e * ri*rj/(ri+rj) * enn ) , 1.0/3.0 );
		knn=2.0/3.0*b1*e/(1.0-po*po);	//ヘルツの弾性接触理論から求める
		kss=knn*so;						//せん断方向の係数は定数費になるので、設定した変数を掛ける
		vnn=2.0*sqrt(wei3*knn);			//臨界減衰の条件からこのように求められる
		vss=vnn*sqrt(so);				//上と同じ
	}
	//jが壁の場合も同様に求める
	else{
		b1=pow( ( ( 3.0/4.0 * ri * ( (1.0-po*po)/e + (1.0-poww*poww)/ew ) ) *enn ) , 1.0/3.0 );
		knn= 4.0/3.0 * b1 * e*ew/((1.0-po*po)*ew+(1.0-poww*poww)*e) ;
		kss=knn*so;
		vnn=2.0*sqrt(wei3*knn);
		vss=vnn*sqrt(so);
	}

	//粘性項の補正
	vnn*=uamp;
	vss*=uamp;

	//差分法の安定条件を満たさないとき、終了する
	double ddt=(1e-1)*sqrt(wei3/knn);
	//ddt=2*sqrt(wei3/knn); と理論式ではなっているが、おそらく一度安定条件を満たさないと、
	//その後も発散的に安定条件を破るようになるので緩めに設定されている
	if(ddt<dt){
//		printf("over !! ddt=%6lf %6d %6d %6d %6lf %6lf\n",ddt,i,j,jk,knn,wei[i]);
//		assert(false);
	}

	//n-1フレームでの法線方向の変位量とせん断方向の変位量を計算する
	double un=(u[i]-u[j])*(ac)+(v[i]-v[j])*(as);
	double us=-(u[i]-u[j])*(as)+(v[i]-v[j])*(ac)+(ri*f[i]+rj*f[j]);

	//法線方向の接触力が0のとき（接触が初回のとき）
	if(en[i][jk]==0.0){
		//これから計算するような弾性力や粘性抵抗は
		//重なり始めてから発生するので、
		//n-1フレームから重なり始めるまでの変位は計算から除外するべきである
		//そのための補正を行う

		//せん断方向の変位を(法線方向の重なった距離 / dtで法線方向に移動した距離)倍にすることで、
		//dtでのせん断方向の変位を、重なり始めてから現地店までのせん断方向の変位にする
		if(un!=0.0){
			us=us*dis/un;
		}

		//法線方向の変位は重なった分の距離とする
		un=dis;

	}

	//弾性力を更新する
	en[i][jk]=knn*un+en[i][jk];
	es[i][jk]=es[i][jk]+kss*us;

	//法線、せん断方向の粘性抵抗力を計算する
	double dn=vnn*un/dt;
	double ds=vss*us/dt;

	//法線方向の圧縮が0未満のとき（つまり接触していない時は）
	//力を0にして接触リストからも削除して接触力を0にして関数から出る
	//おそらくn-1フレームで接触していてnフレームで接触しなくなるとき、
	//弾性項が二つを近づける向きになるがこれは実際にはないことに起因する
	if(en[i][jk]<0.0){
		en[i][jk]=0.0;
		es[i][jk]=0.0;
		je[i][jk]=0;
		return 0;
	}

	//物質がなんであるかを場合わけして摩擦係数を求める
	double frc;
	//粒子-粒子間の接触の場合は粒子-粒子間摩擦係数を選ぶ
	if((j-n)<=0){
		frc=fri;
	}
	//粒子-壁間の接触では粒子-壁間摩擦係数を選ぶ
	else{
		frc=frw;
	}

	//クーロンの摩擦法則から、せん断方向の力が大きくなり
	//（垂直抗力*摩擦係数）以上になると、かかる摩擦の力は（垂直抗力*摩擦係数）になる
	if((abs(es[i][jk])-frc*en[i][jk])>0){
		//es[i][jk]の値は符号を保ったままfrc*en[i][jk]になる
		if(es[i][jk]>=0){
			es[i][jk]=frc*abs(en[i][jk]);
		}else{
			es[i][jk]=frc*(-abs(en[i][jk]));
		}
		//このとき、粘性抵抗力は0となる
		ds=0.0;
	}

	//よって、最終的に求められる接線方向、せん断方向の、
	//弾性力と粘性抵抗力を合わせた力が求まる
	double hn=en[i][jk]+dn;
	double hs=es[i][jk]+ds;

	//最終的な法線、せん断方向の力に単位ベクトルのcos,sinを掛けて、x,y方向の力にして、更新する
	//全粒子分を加算していくので、代入ではなく和をとる。
	xf[i]+=-hn*(ac)+hs*(as);
	yf[i]+=-hn*(as)-hs*(ac);
	//回転モーメントは半径の長さにせん断方向の力を掛けたものである
	mf[i]+=-ri*hs;

	//接触しているjが壁ではなく粒子なら、反対向きの力も更新する
	if(jk<=NJ){
		xf[j]+=hn*(ac)-hs*(as);
		yf[j]+=hn*(as)+hs*(ac);
		mf[j]+=-rj*hs;
	}
	return 0;
}

//入力された値iiに対して線形合同法で[0,1]の乱数を返す
int DistinctElement::random(int* ii,double* ru){
	(*ii)=(*ii)*65539;
	if((*ii)<0)(*ii)=((*ii)+2147483647)+1;
	(*ru)=(*ii)*0.4656613e-9;
	return 0;
}