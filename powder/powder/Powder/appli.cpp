#include "appli.h"

int Appli::Init(int* argc,char** argv,Listener* plisten,int sdim){
	//glut initialize
	glutInit(argc,argv);
	glutInitWindowSize(winx,winy);
	glutInitWindowPosition(100,100);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

	glutCreateWindow(argv[0]);

	//event regist
	pl = plisten;
	dim=sdim;
	if(fps>0){
		glutDisplayFunc(plisten->Disp);
		glutMouseFunc(plisten->Mouse);
		glutMotionFunc(plisten->Motion);
	}
	glutReshapeFunc(plisten->Reshape);
	glutTimerFunc(0,(plisten->Timer),0);
	glutKeyboardFunc(plisten->Keyboard);
	glutKeyboardUpFunc(plisten->KeyboardUp);

	//display init
	glClearColor(0.0, 0.0, 0.0, 1.0);
	if(full)glutFullScreen();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	//world init
	pl->Init(fps,winx,winy,full,dim,tframe);

	//other
	srand((unsigned)time(NULL));

	return 0;
}

int Appli::Run(){
	glutMainLoop();
	return 0;
}

int Appli::Close(){
	return 0;
}

Appli::Appli(){
	fps=DEF_FPS;
	winx=DEF_WINX;
	winy=DEF_WINY;
	full=false;
	tframe=0;
}

Appli::~Appli(){
	SAFE_DELETE(pl);
}