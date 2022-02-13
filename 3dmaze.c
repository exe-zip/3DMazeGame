#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <X11/Xatom.h>
#include "Flib/FillMask.h"
#include "Flib/FFont.h"


#define BORDER 2
#define WIDTH 1000
#define HEIGHT 600

typedef struct{
	float x;
	float y;
}point;

typedef struct{
	float a;
	float b;
	float c;
}abc;

typedef struct{
	int type;
	int pass;
}road;

typedef struct{
	int x;
	int y;
}ppos;

point LU={0,0};
point RU={HEIGHT,0};
point LD={0,HEIGHT};
point RD={HEIGHT,HEIGHT};
point UM={HEIGHT/2,0};
point DM={HEIGHT/2,HEIGHT};
FFont font;
float distance=120;

long colors[8]={0x696969,0x808080,0xa9a9a9,0xc0c0c0,0xd3d3d3,0xdcdcdc,0xf5f5f5,0xffffff};

abc GetLine(point p1,point p2);
point GetCross(abc l1,abc l2);
point GetCrossPoint(point p1,point p2,point p3,point p4);
float GetY(abc f,float x);
float GetX(abc f,float y);
point SetDeltaL(point click,float dis);
point SetDeltaR(point click,float dis);
void DrawFloor(Display *dpy,Window w,GC gc,point click,point rd,point ld,float dis,int num,XPoint *xp);
void DrawWall(Display *dpy,Window w,GC gc,point click,point rd,point ld,float dis,int num,XPoint *xp,road *map,ppos pos,int direction,int map_len);
void Move(point deltaL,point deltaR,int mode);
void DrawGrid(Display *dpy,Window w,GC  gc,int map_len);
void DrawMap(Display *dpy,Window w,GC  gc,road *map,int map_len);
void DrawPlayer(Display *dpy,Window w,GC  gc,XPoint *xp,ppos pos,float map_len,int direction);
int WritePass(road *map,ppos pos,int num,int direction,int map_len);
void DrawXRec(Display *dpy,Window w,GC gc,float d,float u);
int CheckLeftWall(road *map,ppos pos,int direction,int map_len);
int CheckRightWall(road *map,ppos pos,int direction,int map_len);
int CheckFrontWall(road *map,ppos pos,int direction,int map_len,int num);
int ChackMove(road *map,ppos pos,int direction,int map_len);
void ResetPass(road *map,int map_len);

int main(int argc, char  **argv){

	FLoadFont(&font,"fonts/CoolBitFont.dat");
	FSetLayout(LAYOUT_CENTER);

  Display *dpy;
  Window w;
  Window root;
  int  screen;
  unsigned long black, white;
  GC  gc;
  XTextProperty w_name;
  XPoint xp[4];
  XEvent e;

	Pixmap pix;

  setlocale(LC_ALL,"");
  dpy = XOpenDisplay("");

  root = DefaultRootWindow(dpy);
  screen = DefaultScreen(dpy);
  white = WhitePixel(dpy, screen);
  black = BlackPixel(dpy, screen);
  
	w_name.value="Choose a level";
  w_name.encoding=XA_STRING;
  w_name.format=8;
  w_name.nitems=strlen("Choose a level");
  
  w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HEIGHT, BORDER, black, white);
  gc = XCreateGC(dpy, w, 0, NULL);
  pix= XCreatePixmap(dpy,w,WIDTH,HEIGHT,24);
  
  XSetWMProperties(dpy,w,&w_name,NULL,argv,argc,NULL,NULL,NULL);
  XSelectInput(dpy,w,ButtonPressMask|ButtonReleaseMask|KeyPressMask);
  XMapWindow(dpy, w);

	point click={301,300};
	
	int num=3;
	int nownum=3;
	point crossL,crossR,crossM,crossNM,crossNX;
	point deltaR=SetDeltaR(click,distance);
	point deltaL=SetDeltaL(click,distance);
	int deltime=0;
	int movemode=0;
	int gamemode=0;
	int level=1;
	int show_map=1;
	ppos pos={1,1};
	int direction=1;
	int map_len=10;
	road map1[10][10]=
	{
	{{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0}},
	{{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{2,0},{1,0}},
	{{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0}}
	};
	
	road map2[20][20]=
	{
	{{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0}},
	{{1,0},{1,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0}},
	{{1,0},{1,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0}},
	{{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{2,0},{1,0}},
	{{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0}}
	};
	
	road map3[30][30]=
	{
	{{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{1,0},{0,0},{1,0}},
	{{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{0,0},{1,0}},
	{{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0}},
	{{1,0},{1,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{0,0},{0,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{1,0},{0,0},{0,0},{0,0},{1,0},{1,0},{1,0},{0,0},{0,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0}},
	{{1,0},{0,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{0,0},{1,0},{1,0},{0,0},{0,0},{0,0},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{2,0},{1,0}},
	{{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0}}
	};
  while(1){
  	while(XEventsQueued(dpy, QueuedAlready) != 0){
    	XNextEvent(dpy,&e);
    	switch(e.type){
    		case ButtonPress:
    			if(gamemode==0){
    				if(e.xbutton.x<WIDTH/3){
    					level=1;
    					map_len=10;
    					w_name.value="Lv1";
  						w_name.nitems=strlen("Lv1");
    				}
    				else if(e.xbutton.x>WIDTH/3 && e.xbutton.x<(WIDTH/3)*2){
    					level=2;
    					map_len=20;
    					w_name.value="Lv2";
  						w_name.nitems=strlen("Lv2");
    				}
    				else if(e.xbutton.x>(WIDTH/3)*2){
    					level=3;
    					map_len=30;
    					w_name.value="Lv3";
  						w_name.nitems=strlen("Lv3");
    				}
    				XSetWMProperties(dpy,w,&w_name,NULL,argv,argc,NULL,NULL,NULL);
    				gamemode=1;
    			}
    			break;
    		case ButtonRelease:
    			break;
    		case KeyPress:
    			if(e.xkey.keycode==9){//esc
    				return 0;
    			}
    			else if(e.xkey.keycode==111){//up
    				if(num!=7){
    					num++;
    				}
    			}
    			else if(e.xkey.keycode==116){//down
    				if(num!=1){
    					num-=1;
    				}
    			}
    			else if(e.xkey.keycode==114){//right
    				if(deltime==0){
    					if(direction!=3){
    						direction++;
    					}
    					else{
    						direction=0;
    					}
    				}
    			}
    			else if(e.xkey.keycode==113){//left
    				if(deltime==0){
    					if(direction!=0){
    						direction-=1;
    					}
    					else{
    						direction=3;
    					}
    				}
    			}
    			else if(e.xkey.keycode==65){//space
    				if(gamemode==1){
    					switch(level){
    					case 1:
    						if(ChackMove(*map1,pos,direction,map_len)==0){
    							movemode=1;
    						}
    						else if(ChackMove(*map1,pos,direction,map_len)==2){
    							gamemode=2;
    							w_name.value="GOAL!!";
  								w_name.nitems=strlen("GOAL!!");
  								XSetWMProperties(dpy,w,&w_name,NULL,argv,argc,NULL,NULL,NULL);
    						}
    						break;
    					case 2:
    						if(ChackMove(*map2,pos,direction,map_len)==0){
    							movemode=1;
    						}
    						else if(ChackMove(*map2,pos,direction,map_len)==2){
    							gamemode=2;
    							w_name.value="GOAL!!";
  								w_name.nitems=strlen("GOAL!!");
  								XSetWMProperties(dpy,w,&w_name,NULL,argv,argc,NULL,NULL,NULL);
    						}
    						break;
    					case 3:
    						if(ChackMove(*map3,pos,direction,map_len)==0){
    							movemode=1;
    						}
    						else if(ChackMove(*map3,pos,direction,map_len)==2){
    							gamemode=2;
    							w_name.value="GOAL!!";
  								w_name.nitems=strlen("GOAL!!");
  								XSetWMProperties(dpy,w,&w_name,NULL,argv,argc,NULL,NULL,NULL);
    						}
    						break;
    				}
    				}
    			}
    			else if(e.xkey.keycode==24){//Q
    				gamemode=0;
    				pos.x=1;
    				pos.y=1;
    				direction=1;
    				ResetPass(*map1,10);
    				ResetPass(*map2,20);
    				ResetPass(*map3,30);
    				w_name.value="Choose a level";
  					w_name.nitems=strlen("Choose a level");
  					XSetWMProperties(dpy,w,&w_name,NULL,argv,argc,NULL,NULL,NULL);
    			}
    			else if(e.xkey.keycode==27){//R
    				pos.x=1;
    				pos.y=1;
    			}
    			else if(e.xkey.keycode==58){//M
    				if(show_map){
    					show_map=0;
    				}
    				else{
    					show_map=1;
    				}
    			}
    			else{
    				printf("%d\n",e.xkey.keycode);
    			}
    			break;
			}
    }
    
    if(movemode){
    	Move(deltaL,deltaR,1);
    	deltime++;
    	if(deltime==20){
    		switch (direction){
    			case 0:
    				pos.y-=1;	
    				break;
    			case 1:
    				pos.x++;
    				break;
    			case 2:
    				pos.y++;	
    				break;
    			case 3:
    				pos.x-=1;
    				break;
    		}
    		movemode=0;
    		deltime=0;
    	}
    }
    else{
    	Move(deltaL,deltaR,0);
    }
    
    XSetForeground(dpy,gc,black);
    XFillRectangle(dpy,pix,gc,0,0,WIDTH,HEIGHT);
    XSetForeground(dpy,gc,white);
    
    switch(level){
    	case 1:
    		DrawFloor(dpy,pix,gc,click,RD,LD,HEIGHT-distance,nownum,xp);
		    DrawWall(dpy,pix,gc,click,RD,LD,HEIGHT-distance,nownum,xp,*map1,pos,direction,map_len);
    
  		  XSetForeground(dpy,gc,black);
    		XFillRectangle(dpy,pix,gc,HEIGHT,0,WIDTH-HEIGHT,HEIGHT);
    
    		XSetForeground(dpy,gc,white);
    		nownum=WritePass(*map1,pos,num,direction,map_len);
    		if(show_map){
    			DrawMap(dpy,pix,gc,*map1,map_len);
    			DrawPlayer(dpy,pix,gc,xp,pos,map_len,direction);
    			DrawGrid(dpy,pix,gc,map_len);
    		}
    		break;
    	case 2:
    		DrawFloor(dpy,pix,gc,click,RD,LD,HEIGHT-distance,nownum,xp);
		    DrawWall(dpy,pix,gc,click,RD,LD,HEIGHT-distance,nownum,xp,*map2,pos,direction,map_len);
    
  		  XSetForeground(dpy,gc,black);
    		XFillRectangle(dpy,pix,gc,HEIGHT,0,WIDTH-HEIGHT,HEIGHT);
    
    		XSetForeground(dpy,gc,white);
    		nownum=WritePass(*map2,pos,num,direction,map_len);
    		if(show_map){
    			DrawMap(dpy,pix,gc,*map2,map_len);
    			DrawPlayer(dpy,pix,gc,xp,pos,map_len,direction);
    			DrawGrid(dpy,pix,gc,map_len);
    		}
    		break;
    	case 3:
    		DrawFloor(dpy,pix,gc,click,RD,LD,HEIGHT-distance,nownum,xp);
		    DrawWall(dpy,pix,gc,click,RD,LD,HEIGHT-distance,nownum,xp,*map3,pos,direction,map_len);
    
  		  XSetForeground(dpy,gc,black);
    		XFillRectangle(dpy,pix,gc,HEIGHT,0,WIDTH-HEIGHT,HEIGHT);
    
    		XSetForeground(dpy,gc,white);
    		nownum=WritePass(*map3,pos,num,direction,map_len);
    		if(show_map){
    			DrawMap(dpy,pix,gc,*map3,map_len);
    			DrawPlayer(dpy,pix,gc,xp,pos,map_len,direction);
    			DrawGrid(dpy,pix,gc,map_len);
    		}
    		break;
    }
    
    switch(direction){
    	case 0:
    		FDrawString(dpy, pix, gc,font,"Direction N",800,10,6,0);
    		break;
    	case 1:
    		FDrawString(dpy, pix, gc,font,"Direction E",800,10,6,0);
    		break;
    	case 2:
    		FDrawString(dpy, pix, gc,font,"Direction S",800,10,6,0);
    		break;
    	case 3:
    		FDrawString(dpy, pix, gc,font,"Direction W",800,10,6,0);
    		break;
    }
    switch(num){
    	case 1:
    		FDrawString(dpy, pix, gc,font,"View distance 1",800,80,6,0);
    		break;
    	case 2:
    		FDrawString(dpy, pix, gc,font,"View distance 2",800,80,6,0);
    		break;
    	case 3:
    		FDrawString(dpy, pix, gc,font,"View distance 3",800,80,6,0);
    		break;
    	case 4:
    		FDrawString(dpy, pix, gc,font,"View distance 4",800,80,6,0);
    		break;
    	case 5:
    		FDrawString(dpy, pix, gc,font,"View distance 5",800,80,6,0);
    		break;
    	case 6:
    		FDrawString(dpy, pix, gc,font,"View distance 6",800,80,6,0);
    		break;
    	case 7:
    		FDrawString(dpy, pix, gc,font,"View distance 7",800,80,6,0);
    		break;
    }
    if(show_map){
    	FDrawString(dpy, pix, gc,font,"Map On",800,510,6,0);
    }
    else{
    	FDrawString(dpy, pix, gc,font,"Map Off",800,510,6,0);
    	FDrawString(dpy, pix, gc,font,"?",800,200,30,0);
    }
    
    if(gamemode==0){
    	XSetForeground(dpy,gc,white);
    	XFillRectangle(dpy,pix,gc,0,0,WIDTH,HEIGHT);
    	XSetForeground(dpy,gc,black);
    	XDrawLine(dpy,pix,gc,WIDTH/3,0,WIDTH/3,HEIGHT);
    	XDrawLine(dpy,pix,gc,(WIDTH/3)*2,0,(WIDTH/3)*2,HEIGHT);
    	FDrawString(dpy, pix, gc,font,"Lv1",166,HEIGHT/3,15,0);
    	FDrawString(dpy, pix, gc,font,"Lv2",500,HEIGHT/3,15,0);
    	FDrawString(dpy, pix, gc,font,"Lv3",834,HEIGHT/3,15,0);
    	FDrawString(dpy, pix, gc,font,"10x10",166,(HEIGHT/3)*2,7,0);
    	FDrawString(dpy, pix, gc,font,"20x20",500,(HEIGHT/3)*2,7,0);
    	FDrawString(dpy, pix, gc,font,"30x30",834,(HEIGHT/3)*2,7,0);
    }
    else if(gamemode==2){
    	XSetForeground(dpy,gc,white);
    	XFillRectangle(dpy,pix,gc,0,0,WIDTH,HEIGHT);
    	XSetForeground(dpy,gc,black);
    	FDrawString(dpy, pix, gc,font,"GOAL!!!",WIDTH/2,HEIGHT/2-130,30,0);
    	FDrawString(dpy, pix, gc,font,"Press 'Q' to Restart",WIDTH/2,HEIGHT/2+150,10,0);
    }
    
    XCopyArea(dpy,pix,w,gc,0,0,WIDTH,HEIGHT,0,0);
    
    XFlush(dpy);
		usleep(1000 * 10);
  }
}










abc GetLine(point p1,point p2){
	abc _return;
	_return.a=(p2.y-p1.y)/(p2.x-p1.x);
	_return.b=-1;
	_return.c=((p2.y-p1.y)/(p2.x-p1.x))*(-1)*p1.x+p1.y;
	return _return;
}

point GetCross(abc l1,abc l2){
	point _return;
	_return.x=(l1.b*l2.c-l2.b*l1.c)/(l1.a*l2.b-l2.a*l1.b);
	_return.y=(l2.a*l1.c-l1.a*l2.c)/(l1.a*l2.b-l2.a*l1.b);
	return _return;
}

point GetCrossPoint(point p1,point p2,point p3,point p4){
	return GetCross(GetLine(p1,p2),GetLine(p3,p4));
}

float GetY(abc f,float x){
	return (-1*f.a*x-f.c)/f.b;
}

float GetX(abc f,float y){
	return (-1*f.b*y-f.c)/f.a;
}

point SetDeltaL(point click,float dis){
	point _return;
	_return.x=GetX(GetLine(LD,click),dis);
	_return.y=HEIGHT-dis;
	return _return;
}

point SetDeltaR(point click,float dis){
	point _return;
	_return.x=RD.x-GetX(GetLine(RD,click),dis);
	_return.y=HEIGHT-dis;
	return _return;
}

void DrawFloor(Display *dpy,Window w,GC gc,point click,point rd,point ld,float dis,int num,XPoint *xp){
	if(num!=0){
    XSetForeground(dpy,gc,0xFFFFFF);
		point l={0,dis};
		point r={WIDTH,dis};
		point inl=GetCrossPoint(click,ld,l,r);
		point inr=GetCrossPoint(click,rd,l,r);
		point h=GetCrossPoint(rd,GetCrossPoint(click,GetCrossPoint(ld,GetCrossPoint(click,rd,l,r),rd,GetCrossPoint(click,ld,l,r)),l,r),click,ld);	
		
		XSetForeground(dpy,gc,colors[num-1]);
		DrawXRec(dpy,w,gc,ld.y,inl.y);
		
		XSetForeground(dpy,gc,0xFFFFFF);
		point nl=GetCrossPoint(click,ld,l,r);
		point nr=GetCrossPoint(click,rd,l,r);
		num-=1;
		DrawFloor(dpy,w,gc,click,nr,nl,h.y,num,xp);
	}
}

void DrawWall(Display *dpy,Window w,GC gc,point click,point rd,point ld,float dis,int num,XPoint *xp,road *map,ppos pos,int direction,int map_len){
	if(num!=0){
    XSetForeground(dpy,gc,0xFFFFFF);
		point l={0,dis};
		point r={WIDTH,dis};
		point inl=GetCrossPoint(click,ld,l,r);
		point inr=GetCrossPoint(click,rd,l,r);
		point h=GetCrossPoint(rd,GetCrossPoint(click,GetCrossPoint(ld,GetCrossPoint(click,rd,l,r),rd,GetCrossPoint(click,ld,l,r)),l,r),click,ld);	

		XSetForeground(dpy,gc,0xffffff);
		switch(CheckLeftWall(map,pos,direction,map_len)){
			case 0:
				xp[0].x = ld.x; xp[0].y = GetY(GetLine(LU,click),inl.x);
				xp[1].x = inl.x; xp[1].y = GetY(GetLine(LU,click),inl.x);
				xp[2].x = inl.x; xp[2].y = inl.y;
				xp[3].x = ld.x; xp[3].y = inl.y;
				break;
			case 1:
				xp[0].x = ld.x; xp[0].y = GetY(GetLine(LU,click),ld.x);
				xp[1].x = inl.x; xp[1].y = GetY(GetLine(LU,click),inl.x);
				xp[2].x = inl.x; xp[2].y = inl.y;
				xp[3].x = ld.x; xp[3].y = ld.y;
				break;
			case 2:
				XSetForeground(dpy,gc,0x4169e1);
				xp[0].x = ld.x; xp[0].y = GetY(GetLine(LU,click),ld.x);
				xp[1].x = inl.x; xp[1].y = GetY(GetLine(LU,click),inl.x);
				xp[2].x = inl.x; xp[2].y = inl.y;
				xp[3].x = ld.x; xp[3].y = ld.y;
				break;
		}
		XFillPolygon(dpy, w, gc, xp, 4, Complex, CoordModeOrigin);
		
		XSetForeground(dpy,gc,0xffffff);
		switch(CheckRightWall(map,pos,direction,map_len)){
			case 0:
				xp[0].x = rd.x; xp[0].y = GetY(GetLine(RU,click),inr.x);
				xp[1].x = inr.x; xp[1].y = GetY(GetLine(RU,click),inr.x);
				xp[2].x = inr.x; xp[2].y = inr.y;
				xp[3].x = rd.x; xp[3].y = inr.y;
				break;
			case 1:
				xp[0].x = rd.x; xp[0].y = GetY(GetLine(RU,click),rd.x);
				xp[1].x = inr.x; xp[1].y = GetY(GetLine(RU,click),inr.x);
				xp[2].x = inr.x; xp[2].y = inr.y;
				xp[3].x = rd.x; xp[3].y = rd.y;
				break;
			case 2:
				XSetForeground(dpy,gc,0x4169e1);
				xp[0].x = rd.x; xp[0].y = GetY(GetLine(RU,click),rd.x);
				xp[1].x = inr.x; xp[1].y = GetY(GetLine(RU,click),inr.x);
				xp[2].x = inr.x; xp[2].y = inr.y;
				xp[3].x = rd.x; xp[3].y = rd.y;
				break;
		}
		XFillPolygon(dpy, w, gc, xp, 4, Complex, CoordModeOrigin);
		
		XSetForeground(dpy,gc,0x000000);
		XDrawLine(dpy, w, gc, inl.x-1, inl.y, inl.x-1, GetY(GetLine(LU,click),inl.x));
		XDrawLine(dpy, w, gc, inr.x, inr.y, inr.x, GetY(GetLine(RU,click),inr.x));
		point nl=GetCrossPoint(click,ld,l,r);
		point nr=GetCrossPoint(click,rd,l,r);
		num-=1;
		
		ppos newpos;
		switch (direction){
			case 0:
				newpos.x=pos.x;
				newpos.y=pos.y-1;
				break;
			case 1:
				newpos.x=pos.x+1;
				newpos.y=pos.y;
				break;
			case 2:
				newpos.x=pos.x;
				newpos.y=pos.y+1;
				break;
			case 3:
				newpos.x=pos.x-1;
				newpos.y=pos.y;
				break;
		}
		
		DrawWall(dpy,w,gc,click,nr,nl,h.y,num,xp,map,newpos,direction,map_len);
	}
	else{
		switch(CheckFrontWall(map,pos,direction,map_len,num)){
			case 0:
				XSetForeground(dpy,gc,0x000000);
				break;
			case 1:
				XSetForeground(dpy,gc,0xFFFFFF);
				break;
			case 2:
				XSetForeground(dpy,gc,0x4169e1);
				break;
		}
		xp[0].x = ld.x; xp[0].y = GetY(GetLine(LU,click),ld.x);
		xp[1].x = rd.x; xp[1].y = GetY(GetLine(RU,click),rd.x);
		xp[2].x = rd.x; xp[2].y = rd.y;
		xp[3].x = ld.x; xp[3].y = ld.y;
		XFillPolygon(dpy, w, gc, xp, 4, Complex, CoordModeOrigin);
	}
}

void Move(point deltaL,point deltaR,int mode){
	if(mode){
		LD.x-=deltaL.x/20;
		LD.y+=deltaL.y/20;
		RD.x+=deltaR.x/20;
		RD.y+=deltaL.y/20;
		distance-=6;
	}
	else{
		LD.x=0;
		LD.y=HEIGHT;
		RD.x=HEIGHT;
		RD.y=HEIGHT;
		distance=120;
	}
}

void DrawGrid(Display *dpy,Window w,GC  gc,int map_len){
	XSetForeground(dpy, gc, 0xFFFFFF);
	for(int i=0;i<2;i++){
		for(int j=0;j<map_len+1;j++){
			if(i){
				XDrawLine(dpy, w, gc, 650, 150+j*(300/map_len+1), 650+map_len*(300/map_len+1), 150+j*(300/map_len+1));
			}
			else{
				XDrawLine(dpy, w, gc, 650+j*(300/map_len+1), 150, 650+j*(300/map_len+1), 150+map_len*(300/map_len+1));
			}			
		}
	}
}

void DrawMap(Display *dpy,Window w,GC  gc,road *map,int map_len){
	for(int i=0;i<map_len;i++){
		for(int j=0;j<map_len;j++){
			if(map[i*map_len+j].pass){
				switch(map[i*map_len+j].type){
				case 0:
					break;
				case 1:
					XSetForeground(dpy, gc, 0xffffff);
					XFillRectangle(dpy,w,gc,650+i*(300/map_len+1),150+j*(300/map_len+1),300/map_len+1,300/map_len+1);
					break;
				case 2:
					XSetForeground(dpy, gc, 0x4169e1);
					XFillRectangle(dpy,w,gc,650+i*(300/map_len+1),150+j*(300/map_len+1),300/map_len+1,300/map_len+1);
					break;
				}
			}
			else{
				XSetForeground(dpy, gc, 0x808080);
				XFillRectangle(dpy,w,gc,650+i*(300/map_len+1),150+j*(300/map_len+1),300/map_len+1,300/map_len+1);
			}
		}
	}
}

void DrawPlayer(Display *dpy,Window w,GC  gc,XPoint *xp,ppos pos,float map_len,int direction){
	switch (direction){
		case 0:
			xp[0].x = 650+pos.x*(300/map_len+1)+(300/map_len+1)/2; xp[0].y = 150+pos.y*(300/map_len+1);
			xp[1].x = 650+pos.x*(300/map_len+1); xp[1].y = 150+pos.y*(300/map_len+1)+300/map_len+1;
			xp[2].x = 650+pos.x*(300/map_len+1)+300/map_len+1; xp[2].y = 150+pos.y*(300/map_len+1)+300/map_len+1;
			break;
		case 1:
			xp[0].x = 650+pos.x*(300/map_len+1); xp[0].y = 150+pos.y*(300/map_len+1);
			xp[1].x = 650+pos.x*(300/map_len+1); xp[1].y = 150+pos.y*(300/map_len+1)+300/map_len+1;
			xp[2].x = 650+pos.x*(300/map_len+1)+300/map_len+1; xp[2].y = 150+pos.y*(300/map_len+1)+(300/map_len+1)/2;
			break;
		case 2:
			xp[0].x = 650+pos.x*(300/map_len+1)+(300/map_len+1)/2; xp[0].y = 150+pos.y*(300/map_len+1)+300/map_len+1;
			xp[1].x = 650+pos.x*(300/map_len+1); xp[1].y = 150+pos.y*(300/map_len+1);
			xp[2].x = 650+pos.x*(300/map_len+1)+300/map_len+1; xp[2].y = 150+pos.y*(300/map_len+1);
			break;
		case 3:
			xp[0].x = 650+pos.x*(300/map_len+1)+300/map_len+1; xp[0].y = 150+pos.y*(300/map_len+1);
			xp[1].x = 650+pos.x*(300/map_len+1)+300/map_len+1; xp[1].y = 150+pos.y*(300/map_len+1)+300/map_len+1;
			xp[2].x = 650+pos.x*(300/map_len+1); xp[2].y = 150+pos.y*(300/map_len+1)+(300/map_len+1)/2;
			break;
	}
	XSetForeground(dpy, gc, 0xff0000);
	XFillPolygon(dpy, w, gc, xp, 3, Complex, CoordModeOrigin);
}

int WritePass(road *map,ppos pos,int num,int direction,int map_len){
	ppos nowpos=pos;
	int onroad=1;
	int count=0;
	map[nowpos.x*map_len+nowpos.y].pass=1;
	switch (direction){
		case 0:
			while(onroad&&count<num+1){
				map[nowpos.x*map_len+nowpos.y-1].pass=1;
				map[(nowpos.x+1)*map_len+nowpos.y].pass=1;
				map[(nowpos.x-1)*map_len+nowpos.y].pass=1;
				if(map[nowpos.x*map_len+nowpos.y-1].type){
					onroad=0;
					return count;	
				}
				else{
					nowpos.y-=1;
				}
				count++;			
			}
			break;
		case 1:
			while(onroad&&count<num+1){
				map[(nowpos.x+1)*map_len+nowpos.y].pass=1;
				map[nowpos.x*map_len+nowpos.y+1].pass=1;
				map[nowpos.x*map_len+nowpos.y-1].pass=1;
				if(map[(nowpos.x+1)*map_len+nowpos.y].type){
					onroad=0;
					return count;	
				}
				else{
					nowpos.x++;
				}
				count++;			
			}
			break;
		case 2:
			while(onroad&&count<num+1){
				map[nowpos.x*map_len+nowpos.y+1].pass=1;
				map[(nowpos.x+1)*map_len+nowpos.y].pass=1;
				map[(nowpos.x-1)*map_len+nowpos.y].pass=1;
				if(map[nowpos.x*map_len+nowpos.y+1].type){
					onroad=0;
					return count;	
				}
				else{
					nowpos.y++;
				}
				count++;			
			}
			break;
		case 3:
			while(onroad&&count<num+1){
				map[(nowpos.x-1)*map_len+nowpos.y].pass=1;
				map[nowpos.x*map_len+nowpos.y+1].pass=1;
				map[nowpos.x*map_len+nowpos.y-1].pass=1;
				if(map[(nowpos.x-1)*map_len+nowpos.y].type){
					onroad=0;
					return count;	
				}
				else{
					nowpos.x-=1;
				}
				count++;			
			}
			break;
	}
}

void DrawXRec(Display *dpy,Window w,GC gc,float d,float u){
	XFillRectangle(dpy,w,gc,0,u,HEIGHT,d-u+1);
}

int CheckLeftWall(road *map,ppos pos,int direction,int map_len){
	switch (direction){
	case 0:
		return map[(pos.x-1)*map_len+pos.y-1].type;
		break;
	case 1:
		return map[(pos.x+1)*map_len+pos.y-1].type;
		break;
	case 2:
		return map[(pos.x+1)*map_len+pos.y+1].type;
		break;
	case 3:
		return map[(pos.x-1)*map_len+pos.y+1].type;
		break;
	}
}

int CheckRightWall(road *map,ppos pos,int direction,int map_len){
	switch (direction){
	case 0:
		return map[(pos.x+1)*map_len+pos.y-1].type;
		break;
	case 1:
		return map[(pos.x+1)*map_len+pos.y+1].type;
		break;
	case 2:
		return map[(pos.x-1)*map_len+pos.y+1].type;
		break;
	case 3:
		return map[(pos.x-1)*map_len+pos.y-1].type;
		break;
	}
}

int CheckFrontWall(road *map,ppos pos,int direction,int map_len,int num){
	switch (direction){
	case 0:
		return map[pos.x*map_len+pos.y-1-num].type;
		break;
	case 1:
		return map[(pos.x+1+num)*map_len+pos.y].type;
		break;
	case 2:
		return map[pos.x*map_len+pos.y+1+num].type;
		break;
	case 3:
		return map[(pos.x-1-num)*map_len+pos.y].type;
		break;
	}
}

int ChackMove(road *map,ppos pos,int direction,int map_len){
	switch (direction){
	case 0:
		return map[pos.x*map_len+pos.y-1].type;
		break;
	case 1:
		return map[(pos.x+1)*map_len+pos.y].type;
		break;
	case 2:
		return map[pos.x*map_len+pos.y+1].type;
		break;
	case 3:
		return map[(pos.x-1)*map_len+pos.y].type;
		break;
	}
}

void ResetPass(road *map,int map_len){
	for(int i=0;i<map_len;i++){
		for(int j=0;j<map_len;j++){
			map[i*map_len+j].pass=0;
		}
	}
}
