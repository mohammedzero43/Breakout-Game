/* Author : Mohammed Eltawil // mohammedzero43@gmail.com
 * OS:  Linux 4.15.17-300.fc28.x86_64
 * Compiler : gcc-g++ version 7.3.1 20180303 
*/
#define M_PI       3.14159265358979323846   // pi
//BREAKOUT GAME////
//#include <ctime> //JUST TO COUNT FRAMERATE
#include <iostream>
#include <math.h>
//#include <GL/Gl.h>		//uncomment in windows
//#include <windows.h>		//uncomment in windows
#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "vector.h"
#include <string>

#define windowsizex 640 //screen width
#define windowsizey 480 //screen height
#define multiplier 7	//Lower this number if your Graphics card is GTX1080 $$$
static int RIGHT=0;	// RightKeyStat
static int LEFT=0;	// LeftKeyStat
static int RESET=0; // R_KeyStat

using namespace std;
bool FindIntersection(Point A, Point B, Point C, Point D, Point &Intersection) // Oh looks familiar ;P 
{
	Vector b = B - A;
	Vector d = D - C;
	Vector c = C - A;

	Vector bPerp = perp(b);
	Vector dPerp = perp(d);

	double t = (c % dPerp) / (b % dPerp);
	double u = -(c % bPerp) / (d % bPerp);

	Intersection = A + t * b;

	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
		return true;
	else
		return false;
}
//Block :30x30px + 5px margin ,Colors : RedNeon,GreenNeon,Cyan,YellowNeon,MagentaNeon
//Particle : Undefined Yet
//Ball : Radius 12px (RainbowRoll)
//Player : 228x18px (White)
class Obj{
	public:
	int type;								//0:NULL 1:player 2:ball 3:particle 4:block
	float color[3]; 						//RGB
	float x,y; 								//Object's Position 2D
	float sizex,sizey; 						//Object's Dimensions 2D
	int dirx=1;								//BALL object direction on X
	int diry=1;								//BALL object direction on Y
	//constructor ...
	Obj(int _type, float _color[3], float _x,float _y,float _sizex,float _sizey)
	{ type = _type; color[0]=_color[0]; color[1]=_color[1]; color[2]=_color[2];x=_x;y=_y;sizex=_sizex;sizey=_sizey;}
	void animate(){
		if (type==2){						//ball animation
			//Movement
			x+=dirx*0.7 * multiplier;		//Move to -X+ by multiplier factor
			y+=diry*0.7 * multiplier;		//Move to -Y+ by multiplier factor
		}
		
	}
};
class Game{
	public:
	static int combo; 
	static int combo_high; 
	static int power; //5: expand //10 Fireball 
	static int power_duration; //expand 3, firewall 1
	static int scene; //0 loading, 1 main, 2 game, 3 score
	static int score; 
	static Obj *objects[]; //list of objects in the game 
	static void display(){
			//cout<<time(0)<<endl; // to count how many calls per second
			static int combo_high=0;  
			static int power=0;
			static int power_duration=0;
			static int combo=0;
			static int scene=0;
			static int score=0;
			static Obj *objects[100]{0};
			glClear(GL_COLOR_BUFFER_BIT); //clearing screen
			glPointSize(5.0);
			glColor3f(0.0, 0.0, 0.0);
			
			//Loading phase
			if (scene==0){				
				score=0;				//reseting values 
				combo=0;
				power=0;
				power_duration=0; 
				//initializing objects
				float *color;			
				color=new float[3]{1,1,1};
				objects[0]= new Obj(1,color,320,24,180,18);//player
				delete color;
				color = new float[3]{1,0,1};
				objects[1]= new Obj(2,color,300,70,24,24);//ball
				delete color;
				color = new float[3]{0,0,1};
				for (int tmpy=0;tmpy<5;tmpy++){		
					for(int tmpx=0;tmpx<18;tmpx++){
						objects[2+tmpx+(tmpy*18)]= new Obj(4,color,22+(tmpx*35) , 420-(tmpy*35),30,30);//blocks
					}
				}
				delete color;//memory garbage
				scene=1; // go to game phase
			}
			//game_phase
			if (scene==1){
				
				//Get ball index from the object list (object type:2)
				int ballindex;
				for (int o=0;o<100;o++){
					if (objects[o]!=0 || objects[o]!=NULL)
						if (objects[o]->type == 2)
							{ballindex = o;
							break;}
				}
				
				//bounce at walls
				if ((objects[ballindex]->x + objects[ballindex]->sizex/2) >= 640)
					objects[ballindex]->dirx = -1;
				if ((objects[ballindex]->x - objects[ballindex]->sizex/2) <= 1)
					objects[ballindex]->dirx = 1;
				if ((objects[ballindex]->y + objects[ballindex]->sizey/2) >= 450)
					objects[ballindex]->diry = -1;
				
				//losing (hitting the bottom of the screen)
				if ((objects[ballindex]->y - objects[ballindex]->sizey/2) <= 0)
					{objects[ballindex]->y = -100;
					scene=2;}//jump to end game phase
				//winning (hitting score of 900)
				if (score ==900){
					objects[ballindex]->y = -100;
					scene=2;}//jump to end game phase
				
				//Ball Collision Detection
				Point circle[10];//hitbox/circle points
				int i=0;
				for (float t=0;t<=1.1;t+=0.1){	//generate hit-box/circle 
					float tx=objects[ballindex]->x+cos(2*M_PI*t)*objects[ballindex]->sizex/2;
					float ty=objects[ballindex]->y+sin(2*M_PI*t)*objects[ballindex]->sizex/2;
					circle[i].x=tx; //store the points in the hit-box/circle array
					circle[i].y=ty;
					i++;
				}
				for (int o=0;o<100;o++){//for all objects in the scene
					if (objects[o]!=0 || objects[o]!=NULL){//check if object is still alive
						if (o!=ballindex){//avoid collision with self (in case of multiballs) <-- DLC Leaks
								//edge1 (LEFT)
								if (objects[o]!=0 || objects[o]!=NULL) //check if object is not destroyed
								for (int i=0;i<8;i++){ // for each side in the hitbox/circle
									bool isIntersection;//finding intersection
									Point Line1[2];
									Point Line2[2];
									Point I;
									Line1[0].x=objects[o]->x - objects[o]->sizex/2;
									Line1[0].y=objects[o]->y - objects[o]->sizey/2;
									Line1[1].x=objects[o]->x - objects[o]->sizex/2;
									Line1[1].y=objects[o]->y + objects[o]->sizey/2;
									Line2[0].x=circle[i].x;
									Line2[0].y=circle[i].y;
									Line2[1].x=circle[i+1].x;
									Line2[1].y=circle[i+1].y;
									isIntersection = FindIntersection(Line1[0], Line1[1], Line2[0], Line2[1], I); 
									if (isIntersection){
									//calculate mirror
										if (objects[o]->type != 1){// hitting non-player object
										if (power!=2)//if not fireball
											objects[ballindex]->dirx = -1; //bounce off
										objects[o]=0;	//destroy the object
										score += 10;	//increase score
										combo++;		//incrase the combo
										}
										else { // hitting player object
										objects[ballindex]->dirx = -1; //bounce off
										combo=0; // reset the combo
										if (power_duration > 0) power_duration--; //reduce the powerup duration
										}
										goto BREAK; // break the loop to avoid null pointer errors
										break; // just in case the compiler is C++9 not C++11
									}
								}
								//edge2 (UP)	//same stuff as above
								if (objects[o]!=0 || objects[o]!=NULL)
								for (int i=0;i<8;i++){
									bool isIntersection;
									Point Line1[2];
									Point Line2[2];
									Point I;
									Line1[0].x=objects[o]->x - objects[o]->sizex/2;
									Line1[0].y=objects[o]->y + objects[o]->sizey/2;
									Line1[1].x=objects[o]->x + objects[o]->sizex/2;
									Line1[1].y=objects[o]->y + objects[o]->sizey/2;
									Line2[0].x=circle[i].x;
									Line2[0].y=circle[i].y;
									Line2[1].x=circle[i+1].x;
									Line2[1].y=circle[i+1].y;
									isIntersection = FindIntersection(Line1[0], Line1[1], Line2[0], Line2[1], I);
									if (isIntersection){
									//calculate mirror
										if (objects[o]->type != 1){
										if (power!=2)
										objects[ballindex]->diry = 1;
										objects[o]=0;
										score += 10;
										combo++;
										}
										else {
										//allowing the player to control the ball (hitting the paddle at the last 15% will re-stear the ball to that direction )
										if (objects[ballindex]->x > objects[0]->x + objects[0]->sizex*0.35) {objects[ballindex]->dirx =1;}
										if (objects[ballindex]->x < objects[0]->x - objects[0]->sizex*0.35) {objects[ballindex]->dirx =-1;}
										objects[ballindex]->diry = 1;
										combo=0;
										if (power_duration > 0) power_duration--;
										}
										goto BREAK;
										break;
									}
								}
								//edge3 (RIGHT)
								if (objects[o]!=0 || objects[o]!=NULL)
								for (int i=0;i<8;i++){
									bool isIntersection;
									Point Line1[2];
									Point Line2[2];
									Point I;
									Line1[0].x=objects[o]->x + objects[o]->sizex/2;
									Line1[0].y=objects[o]->y + objects[o]->sizey/2;
									Line1[1].x=objects[o]->x + objects[o]->sizex/2;
									Line1[1].y=objects[o]->y - objects[o]->sizey/2;
									Line2[0].x=circle[i].x;
									Line2[0].y=circle[i].y;
									Line2[1].x=circle[i+1].x;
									Line2[1].y=circle[i+1].y;
									isIntersection = FindIntersection(Line1[0], Line1[1], Line2[0], Line2[1], I);
									if (isIntersection){
									//calculate mirror
										if (objects[o]->type != 1){
										if (power!=2)
										objects[ballindex]->dirx = 1;
										objects[o]=0;
										score += 10;
										combo++;
										}
										else {
										objects[ballindex]->dirx = 1;
										combo=0;
										if (power_duration > 0) power_duration--;
										}		
										goto BREAK;			
										break;
									}
								}
								//edge4 (DOWN)
								if (objects[o]!=0 || objects[o]!=NULL)
								for (int i=0;i<8;i++){
									bool isIntersection;
									Point Line1[2];
									Point Line2[2];
									Point I;
									Line1[0].x=objects[o]->x + objects[o]->sizex/2;
									Line1[0].y=objects[o]->y - objects[o]->sizey/2;
									Line1[1].x=objects[o]->x - objects[o]->sizex/2;
									Line1[1].y=objects[o]->y - objects[o]->sizey/2;
									Line2[0].x=circle[i].x;
									Line2[0].y=circle[i].y;
									Line2[1].x=circle[i+1].x;
									Line2[1].y=circle[i+1].y;
									isIntersection = FindIntersection(Line1[0], Line1[1], Line2[0], Line2[1], I);
									if (isIntersection){
									//calculate mirror
										if (objects[o]->type != 1){
										if (power!=2)
										objects[ballindex]->diry = -1;
										objects[o]=0;
										score += 10;
										combo++;}
										else {

										objects[ballindex]->diry = -1;
										combo=0;
										if (power_duration > 0) power_duration--;
										}
										goto BREAK;
										break;
									}
								}
							
						}
					}
				}
				BREAK:; 
				//power-ups
				if (combo>combo_high) combo_high=combo; // set highest-combo count
				if (power!=2){ //to keep the ball away from melting
					if (combo >=5){power=2; power_duration=1;objects[ballindex]->diry=1;} //blacksmith job (FireBall Activated)
					else if (combo>=4) {power =1;power_duration=3;}} //Expanding the paddle size activation 
				if (power_duration==0) power=0;	//restore normal ball status
				if (power == 1){
					objects[0]->sizex=228; //resize to 228px
				}else{
					objects[0]->sizex=180; //resize to 180px
				}
				if (power==2){//fireball red color
					objects[ballindex]->color[0]=1.0;	
					objects[ballindex]->color[1]=0.0;
					objects[ballindex]->color[2]=0.0;
				}else{//normal ball color
					objects[ballindex]->color[0]=0;
					objects[ballindex]->color[1]=0.8;
					objects[ballindex]->color[2]=0;	
				}
				//controls
				if (RIGHT){//if Right[D] is pressed
					objects[0]->x+=1*multiplier; // move on X+ by multiplier factor
				}
				if (LEFT){//if Left[A] is pressed
					objects[0]->x-=1*multiplier; // move on X- by multiplier factor
				}
				if (objects[0]->x+objects[0]->sizex/2 > 640) // block player from passing screen right boundries
					objects[0]->x= 640-objects[0]->sizex/2;
				if (objects[0]->x-objects[0]->sizex/2 < 0) // block player from passing screen left boundries
					objects[0]->x= objects[0]->sizex/2;
				//drawing
				string text; 
				text= "Score : " + to_string(score);//setting score text to the upper left corner
 				drawtext(text,10,460);
				text= "Combo : " + to_string(combo);//setting combo text to the upper right corner
				drawtext(text,545,460);
				
				for (int tmp=0;tmp<100;tmp++){//for all objects in the scene, start drawing 
						if (objects[tmp] != 0){
							if (objects[tmp]->type == 1){
								drawplayer(objects[tmp]->x,objects[tmp]->y,objects[tmp]->sizex,objects[tmp]->sizey,objects[tmp]->color);
							}
							if (objects[tmp]->type == 2){
								drawball(objects[tmp]->x,objects[tmp]->y,(objects[tmp]->sizex)/2,objects[tmp]->color);
								objects[tmp]->animate();
							}
							if (objects[tmp]->type == 4){
								drawblock(objects[tmp]->x,objects[tmp]->y,objects[tmp]->sizex,objects[tmp]->color);
							}
						}
						
				}
			}
			// GAME OVER
			if (scene==2){
				if (RESET==1)//if reset button is pressed
					{scene=0;}//go to loading screen
				//drawing
				string text;
				text= "Score : " + to_string(score); //score text LCorner
				drawtext(text,10,460);
				text= "Highest Combo : " + to_string(combo_high); //High combo text RCorner
				drawtext(text,470,460);
				
				if (score != 900) // if you lose 
				{text= "You scored " + to_string(score) +" press R to restart";
				drawtext(text,150,150);}
				else// if you win
				{text= "GG_WP You Scored " + to_string(score) +" press R to restart";
				drawtext(text,120,150);
				text= "But I suggest playing a real game (Try RainbowSix Siege ;D)";
				drawtext(text,50,130);}
				
				for (int tmp=0;tmp<100;tmp++){ // drawing all objects
						if (objects[tmp] != 0){
							if (objects[tmp]->type == 1){
								drawplayer(objects[tmp]->x,objects[tmp]->y,objects[tmp]->sizex,objects[tmp]->sizey,objects[tmp]->color);
							}
							if (objects[tmp]->type == 2){
								drawball(objects[tmp]->x,objects[tmp]->y,(objects[tmp]->sizex)/2,objects[tmp]->color);
								objects[tmp]->animate();
							}
							if (objects[tmp]->type == 4){
								drawblock(objects[tmp]->x,objects[tmp]->y,objects[tmp]->sizex,objects[tmp]->color);
							}
						}	
				}
			}
			
			glutPostRedisplay(); //Just to make sure it's drawn
			glutSwapBuffers(); //double buffer
			//glFlush(); //single buffer
}
	static void keyboard(unsigned char key, int x, int y){
			
			if (key==27){//esc key pressed
				exit(0);}
				
			if (key=='A' || key=='a')//A key pressed
				LEFT=1;
		
			if (key=='D' || key=='d')//D key pressed
				RIGHT=1;
				
			if (key=='R' || key=='r')//R key pressed
				RESET=1;
	
			glutPostRedisplay();
	}
	static void keyboard_up(unsigned char key, int x, int y){
		
		if (key=='A' || key=='a')//A key released
			LEFT=0;
	
		if (key=='D' || key=='d')//D key released
			RIGHT=0;
		
		if (key=='R' || key=='r')//R key released
				RESET=0;
	}
	Game(int argc, char** argv){
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
		//glutGameModeString( "640x480:32@60" );
		//glutEnterGameMode(); // FULL SCREEN
		glutInitWindowSize(windowsizex,windowsizey); // WINDOW MODE
		glutInitWindowPosition(0,0);
		glutCreateWindow("Breakout Game");
		glClearColor(0.1, 0.1, 0.1 , 0.0);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, windowsizex, 0.0, windowsizey);
		glViewport(0,0,windowsizex,windowsizey);
		
		glutDisplayFunc(display);
		glutKeyboardFunc(keyboard);
		glutKeyboardUpFunc(keyboard_up); 
		glutMainLoop();
	}
	private:
	static void drawball(float x,float y,float r,float color[3]){	//drawing the ball
		glColor3f(color[0], color[1], color[2]); // get object color
		glBegin(GL_POLYGON);
		//int i=0;
		for (float t=0;t<=1.02;t+=0.02){
			float tx=x+cos(2*M_PI*t)*r;
			float ty=y+sin(2*M_PI*t)*r;
			//circle[i].x=tx;
			//circle[i].y=ty;
			glVertex2d(tx,ty);
			//i++;
		}
		glEnd();	
	}
	
	static void drawblock(float x,float y,float s,float color[3]){//drawing blocks
		glColor3f(color[0], color[1], color[2]);
		glBegin(GL_POLYGON);	
		glVertex2d(x-s/2,y+s/2); //pos -(size/2)+
		glVertex2d(x+s/2,y+s/2);
		glVertex2d(x+s/2,y-s/2);
		glVertex2d(x-s/2,y-s/2);
		glEnd();	
	}
	
	static void drawplayer(float x,float y,float sizex,float sizey,float color[3]){ //draw player
		glColor3f(0.95, 0.95, 0.95);
		glBegin(GL_POLYGON);	
		glVertex2d(x-sizex/2,y+sizey/2);
		glVertex2d(x+sizex/2,y+sizey/2);
		glVertex2d(x+sizex/2,y-sizey/2);
		glVertex2d(x-sizex/2,y-sizey/2);
		glEnd();
	}
	static void drawtext(string s,int x,int y){ //Drawing text
		glMatrixMode(GL_MODELVIEW); //switching to ModelView Mode for text drawing
		glPushMatrix();							
		glLoadIdentity();
		glColor3f(0.0, 1.0, 0.0);
		glRasterPos2i(x, y);		//setting the position of the new coordinates (including)
		void * font = GLUT_BITMAP_9_BY_15;	//builtin glut font
		for (string::iterator i = s.begin(); i != s.end(); ++i) //for all the letters in the text 
		{
			char c = *i;
			glutBitmapCharacter(font, c); //draw the bitmap texture of the letter
		}
		glMatrixMode(GL_MODELVIEW); // destroy the coordinates system
		glPopMatrix();				

	}
};


int main(int argc, char** argv){
	Game game(argc,argv);	//The ONLY line you need to destroy the game >:D
	return 0;
}

