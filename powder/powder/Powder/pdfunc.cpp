
#include "pdbase.h"
#include "ApplicationProperty.h"



pdVertex3d pdVecCross (pdVertex3d p , pdVertex3d q){
	pdVertex3d r;
	r.x = p.y*q.z-p.z*q.y;
	r.y = p.z*q.x-p.x*q.z;
	r.z = p.x*q.y-p.y*q.x;
	return r;
};
pdVertex2d pdVecCross (pdVertex2d p , pdVertex2d q){
	pdVertex2d r;
	r.x = p.x*q.y-p.y*q.x;
	r.y = p.y*q.x-p.x*q.y;
	return r;
};
pdVertex3d pdVecDot (pdVertex3d p , pdVertex3d q){
	pdVertex3d r;
	r.x=p.x*q.x;
	r.y=p.y*q.y;
	r.z=p.z*q.z;
	return r;
};
pdVertex2d pdVecDot (pdVertex2d p , pdVertex2d q){
	pdVertex2d r;
	r.x = p.x*q.x;
	r.y = p.y*q.y;
	return r;
};

int pdInit(){
	pdBallDivide=ApplicationProperty::ReadSetupInt("Graphic","BallDivide",8);
	pdSphereDivideX=ApplicationProperty::ReadSetupInt("Graphic","SphereDivideX",8);
	pdSphereDivideY=ApplicationProperty::ReadSetupInt("Graphic","SphereDivideY",8);
	return 0;
}

int pdClearScreen(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	return 0;
}

int pdSetScreenColor(pdColor c){
	glClearColor((GLclampf)c.r,(GLclampf)c.g,(GLclampf)c.b, (GLclampf)c.a);
	return 0;
}

int pdDrawBody(pdBody*){
	return 0;
}

int pdSwapScreen(){
	glDisable(GL_BLEND);
	glutSwapBuffers();
	return 0;
}

int pdDrawLine(pdVertex2d* v,pdColor* c,int num){
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glBegin(GL_LINE_STRIP);
	for(int n=0;n<num;n++){
		glColor4d(c[n].r,c[n].g,c[n].b,c[n].a);
		glVertex2d(v[n].x,v[n].y);
	}
	glEnd();
//	glEnable(GL_LIGHTING);
	glPopMatrix();
	return 0;
}

int pdDrawLine(pdVertex3d* v,pdColor* c,int num){
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glBegin(GL_LINE_STRIP);
	for(int n=0;n<num;n++){
		glColor4d(c[n].r,c[n].g,c[n].b,c[n].a);
		glVertex3d(v[n].x,v[n].y,v[n].z);
	}
	glEnd();
//	glEnable(GL_LIGHTING);
	glPopMatrix();
	return 0;
}

int pdDrawLine(vector<pdVertex2d> v,vector <pdColor> c){
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glBegin(GL_LINE_STRIP);
	for(unsigned int n=0;n<v.size();n++){
		if(n<c.size())glColor4d(c[n].r,c[n].g,c[n].b,c[n].a);
		else glColor4d(1,1,1,1);
		glVertex2d(v[n].x,v[n].y);
	}
	glEnd();
//	glEnable(GL_LIGHTING);
	glPopMatrix();
	return 0;
}
int pdDrawLine(vector<pdVertex3d> v,vector<pdColor> c){
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glBegin(GL_LINE_STRIP);
	for(unsigned int n=0;n<v.size();n++){
		if(n<c.size())glColor4d(c[n].r,c[n].g,c[n].b,c[n].a);
		else glColor4d(1,1,1,1);
		glVertex3d(v[n].x,v[n].y,v[n].z);
	}
	glEnd();
//	glEnable(GL_LIGHTING);
	glPopMatrix();
	return 0;
}

int pdDrawPolygon(pdVertex2d* v,pdColor* c,int num){
	glPushMatrix();
	glBegin(GL_POLYGON);
	glNormal3d(0,0,1);
	for(int n=0;n<num;n++){
		glColor4d(c[n].r,c[n].g,c[n].b,c[n].a);
		glVertex2d(v[n].x,v[n].y);
	}
	glEnd();
	glPopMatrix();
	return 0;
}

int pdDrawPolygon(pdVertex3d* v,pdColor* c,int num){
	glPushMatrix();
	glBegin(GL_POLYGON);
	for(int n=0;n<num;n++){
		glColor4d(c[n].r,c[n].g,c[n].b,c[n].a);
		glVertex3d(v[n].x,v[n].y,v[n].z);
	}
	glEnd();
	glPopMatrix();
	return 0;
}
int pdDrawPolygon(vector<pdVertex2d> v,vector<pdColor> c){
	glPushMatrix();
	glBegin(GL_POLYGON);
	glNormal3d(0,0,1);
	for(unsigned int n=0;n<v.size();n++){
		if(n<c.size())glColor4d(c[n].r,c[n].g,c[n].b,c[n].a);
		else glColor4d(1,1,1,1);
		glVertex2d(v[n].x,v[n].y);
	}
	glEnd();
	glPopMatrix();
	return 0;
}
int pdDrawPolygon(vector<pdVertex3d> v,vector<pdColor> c){
	glPushMatrix();
	glBegin(GL_POLYGON);
	for(unsigned int n=0;n<v.size();n++){
		if(n<c.size())glColor4d(c[n].r,c[n].g,c[n].b,c[n].a);
		else glColor4d(1,1,1,1);
		glVertex3d(v[n].x,v[n].y,v[n].z);
	}
	glEnd();
	glPopMatrix();
	return 0;
}

int pdDrawCircle(pdVertex2d v,pdColor c,double r){
	double theta,d;
	d=PI/pdBallDivide;
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glBegin(GL_LINE_STRIP);
	for (theta=0;theta<2*PI+d;theta+=d)
	{
		glColor4d(c.r,c.g,c.b,c.a);
		glVertex2d( v.x+r*cos(theta) , v.y+r*sin(theta) );
    }
	glEnd();						 
//	glEnable(GL_LIGHTING);
	glPopMatrix();
	return 0;
}
int pdDrawCircle(pdVertex3d v,pdColor c,double r){
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor4d(c.r,c.g,c.b,c.a);
	glTranslated(v.x,v.y,v.z);

	glutWireSphere(r,pdSphereDivideX,pdSphereDivideY);
//	glEnable(GL_LIGHTING);
	glPopMatrix();
	return 0;
}
int pdDrawBall(pdVertex2d v,pdColor c,double r){
	double theta,d;
	d=PI/pdBallDivide;
	glPushMatrix();
	glBegin(GL_POLYGON);
	glNormal3d(0,0,1);
	for (theta=0;theta<2*PI;theta+=d)
	{
		glColor4d(c.r,c.g,c.b,c.a);
		glVertex2d( v.x+r*cos(theta) , v.y+r*sin(theta) );
    }
	glEnd();
	glPopMatrix();
	return 0;
}
int pdDrawBall(pdVertex3d v,pdColor c,double r){
	glPushMatrix();
	glColor4d(c.r,c.g,c.b,c.a);
	glTranslated(v.x,v.y,v.z);
	glutSolidSphere(r,pdSphereDivideX,pdSphereDivideY);
	glPopMatrix();
	return 0;
}
int pdDrawString(const char *str,pdVertex2d p,pdColor c){
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	gluOrtho2D(0,glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
	glPushAttrib(GL_CURRENT_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glColor4d(c.r,c.g,c.b,c.a);
	glRasterPos2d(p.x, p.y); //¶‰º‚ªŒ´“_
	while(*str){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str);
		str++;
	}

	glPopAttrib();			
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//‚à‚Æ‚Ìó‘Ô‚É‚à‚Ç‚é
	return 0;
};
int pdDrawString(const char *str,pdVertex3d p,pdColor c){
	pdDrawString(str,pdVertex2d(p.x,p.y),c);
	return 0;
};
int pdDrawString(string str,pdVertex2d p,pdColor c){
	pdDrawString(str.c_str(),p,c);
	return 0;
};
int pdDrawString(string str,pdVertex3d p,pdColor c){
	pdDrawString(str.c_str(),pdVertex2d(p.x,p.y),c);
	return 0;
};

