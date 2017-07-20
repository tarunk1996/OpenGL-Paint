#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

#define PI 3.14159265
using namespace std;



int ww =800, wh = 600;
int first = 0;
int algo=1,insclip=0;
int xi, yi, xf, yf,rx,ry,rxf,ryf;
int px,py,cx,cy,fx,fy;
enum fig {circle,line};
fig cur=line;
enum option {menu1,menu2};
option op=menu1;
enum cfill{Y,N};
cfill CFill=N,BFill=N;
enum select {YES,NO};
select cstatus,lstatus;
enum Linetype {simple,dotted,dashed};
Linetype lt = simple;
float red=1.0,green=0.0,blue=0.0,pc[3];
int dot=2;
int coun=0;
int dash=7;
int thickness=1;
int scalex=0;
int scaley=0;
void circlebres(int x0,int y0,int r);
int stat=0;
int shapeno=1,sn=0;
int rightclickset=0;
int degree=0;
int currentdeg=0;
int pivotx,pivoty;
int in=0;
int bcount=0;

struct details
{
    int x1,y1,x2,y2;
    float r,g,b;
    int algo;
    int thickness;
    int cdegree=0;
    Linetype l;
};
struct poly
{
    int p[50][2];
    int n=0;
}newp;
struct polyq
{
    int p[50][3];
    int n=0;
}setp;
struct findc
{
    int x1,y1;
    int n;
};

class point {
public:
    float x, y;
    void setxy(float x2, float y2) { x = x2; y = y2; }
    const point & operator=(const point &rPoint) {
         x = rPoint.x;
         y = rPoint.y;
         return *this;
      }

}abc[4];
point POld;

vector <details> shapes;
vector <poly> npoly;
vector <polyq> spoly;
vector <poly> :: iterator pt;
vector <details> :: iterator st;
vector <findc> v;
vector <findc>::iterator it;
//it=v.begin();


void draw();
void getcolor(int x,int y,float c[]);
void bresenhamAlg(int x0, int y0, int x1, int y1);

void drawDot(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x,y);
    glEnd();
    glFlush();
}

void drawLine(point p1, point p2) {
    glBegin(GL_LINES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
    glFlush();
}

// Calculate the next bezier point.
//Point drawBezier(Point A, Point B, Point C, Point D, double t) {
//    Point P;
//    P.x = pow((1 - t), 3) * A.x + 3 * t * pow((1 -t), 2) * B.x + 3 * (1-t) * pow(t, 2)* C.x + pow (t, 3)* D.x;
//    P.y = pow((1 - t), 3) * A.y + 3 * t * pow((1 -t), 2) * B.y + 3 * (1-t) * pow(t, 2)* C.y + pow (t, 3)* D.y;
//    return P;
//}

int factorial(int n)
{
    if (n<=1)
        return(1);
    else
        n=n*factorial(n-1);
    return n;
}
float binomial_coff(float n,float k)
{
    float ans;
    ans = factorial(n) / (factorial(k)*factorial(n-k));
    return ans;
}

point drawBezier(point PT[], double t) {
    point P;
    P.x = pow((1 - t), 3) * PT[0].x + 3 * t * pow((1 -t), 2) * PT[1].x + 3 * (1-t) * pow(t, 2)* PT[2].x + pow (t, 3)* PT[3].x;
    P.y = pow((1 - t), 3) * PT[0].y + 3 * t * pow((1 -t), 2) * PT[1].y + 3 * (1-t) * pow(t, 2)* PT[2].y + pow (t, 3)* PT[3].y;

    return P;
}
point drawBezierGeneralized(point PT[], double t,int clicks) {
     point P;
    P.x = 0; P.y = 0;
    for (int i = 0; i<clicks; i++)
    {
        P.x = P.x + binomial_coff((float)(clicks - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (clicks - 1 - i)) * PT[i].x;
        P.y = P.y + binomial_coff((float)(clicks - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (clicks - 1 - i)) * PT[i].y;
    }
    //cout<<P.x<<endl<<P.y;
    //cout<<endl<<endl;
    return P;
}

void change(int &x,int &y)
{
    float TX,TY;
    x-=pivotx;
    y-=pivoty;
    float th=currentdeg*PI/180.0;
    TX=x*cos(th)-y*sin(th);
    TY=x*sin(th)+y*cos(th);
    x=TX;
    y=TY;
    x+=pivotx;
    y+=pivoty;
}
void checkpix(int x,int y)
{
    float ai[3];
    getcolor(x,wh-y,ai);
    //if(x==setp.p[0][0]&&y)
    //if(!(ai[0]==pc[0]&&ai[1]==pc[1]&&ai[2]==pc[2])&&!(ai[0]==0.0&&ai[1]==0.0&&ai[2]==0.0)&&!(ai[0]==red&&ai[1]==green&&ai[2]==blue))
    if(!(ai[0]==pc[0]&&ai[1]==pc[1]&&ai[2]==pc[2])&&!(ai[0]==0&&ai[1]==0&&ai[2]==0))
    {

        if((setp.p[setp.n-1][0]+3>=x&&setp.p[setp.n-1][0]-3<=x)&&setp.p[setp.n-1][1]-3<=y&&setp.p[setp.n-1][1]+3>=y)
        {
            cout<<"working"<<endl;
            return;
        }
        if(ai[0]==1&&ai[1]==0)
        {
            cout<<"Found";
            cout<<x<<" "<<y<<endl;
            setp.p[setp.n][0]=x;
            setp.p[setp.n][1]=y;
            setp.p[setp.n][2]=1;
            setp.n++;
            cout<<ai[0]<<" "<<ai[1]<<" "<<ai[2]<<" "<<setp.p[setp.n-1][2]<<endl;
            in=1;
        }
        else if(ai[0]==1&&ai[1]==1&&pc[0]==1&&pc[1]==0)
        {

            cout<<"Found";
            cout<<x<<" "<<y<<endl;
            setp.p[setp.n][0]=x;
            setp.p[setp.n][1]=y;
            setp.p[setp.n][2]=2;
            setp.n++;
            cout<<ai[0]<<" "<<ai[1]<<" "<<ai[2]<<" "<<setp.p[setp.n-1][2]<<endl;
        }
        //cout<<ai[0]<<" "<<ai[1]<<" "<<ai[2];



    }

    if(!(ai[0]==0&&ai[1]==0&&ai[2]==0))
    {
        pc[0]=ai[0];
        pc[1]=ai[1];
        pc[2]=ai[2];
    }




    return;
}
void putPixel(int x, int y)
{
    coun++;
    if(insclip==1)
    {
        checkpix( x, y);
        //return ;
    }
    if(currentdeg!=0&&algo!=5)
    {
        change(x,y);
    }

	if(lt==dotted)
    {
        if(coun%dot==0)
        {
            glColor3f(red, green, blue);
            glBegin(GL_POINTS);
            glVertex2i(x, y);
            glEnd();
            //cout<<"a";
        }
    }
    if(lt==dashed)
    {
        if(coun%dash!=0&&coun%dash!=1&&coun%dash!=2)
        {
            glColor3f(red, green, blue);
            glBegin(GL_POINTS);
            glVertex2i(x, y);
            glEnd();
            //cout<<"a";
        }
    }
    if(lt==simple)
    {
        glColor3f(red, green, blue);
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();

    }
    //glFlush();

}
float c[3],p[3];
void scanpoly()
{
    poly spl;
    int xmax,xmin;
    int ct,y,eds=0;

    spl=npoly[npoly.size()-1];
    xmax=0;
    xmin=1000;
    int arry[100],co=0;


    for(int i=0;i<spl.n;i++)
    {
        if(spl.p[i][0]>xmax)
        {
            xmax=spl.p[i][0];
        }
        if(spl.p[i][0]<xmin)
        {
            xmin=spl.p[i][0];
        }

    }
    //cout<<"Working";

    spl.p[spl.n][0]=spl.p[0][0];
    spl.p[spl.n][1]=spl.p[0][1];
    cout<<xmin<<" "<<xmax;
    for(int i=xmin+1;i<xmax-1;i++)
    {
        co=0;
        for(int j=0;j<spl.n;j++)
        {
            //cout<<"Line"<<endl;
            if(i==spl.p[j][0]||i==spl.p[j+1][0])
            {
                if(eds==0)
                    i-=1;
                eds=1;

            }
            y=(float(spl.p[j+1][1]-spl.p[j][1])/float(spl.p[j+1][0]-spl.p[j][0]))*float(i-spl.p[j][0])+spl.p[j][1];
            if(spl.p[j][0]>spl.p[j+1][0])
            {
                if(i>spl.p[j+1][0]&&i<spl.p[j][0])
                {
                    arry[co]=y;
                    co++;
                }
            }
            else
            {
                if(i<spl.p[j+1][0]&&i>spl.p[j][0])
                {
                    arry[co]=y;
                    co++;
                }
            }
        }
        if(eds==1)
        {
            eds=0;
            i+=1;
        }

        sort(arry,arry+co);
        for(int k=0;k<co;k+=2)
        {
            cout<<"Line"<<endl;
            glColor3f(0.0,1.0,0.0);
            bresenhamAlg(i,arry[k],i,arry[k+1]);
//            glBegin(GL_LINES);
//            glVertex2f(i,arry[k+1]);
//            glVertex2f(i,arry[k]);
//            glEnd();
            glFlush();
        }

    }


}

void scanpoly2(poly abc)
{
    poly spl;
    cout<<"Poly color";
    int xmax,xmin;
    int ct,y,eds=0;

    spl=abc;
    xmax=0;
    xmin=1000;
    int arry[100],co=0;


    for(int i=0;i<spl.n;i++)
    {
        if(spl.p[i][0]>xmax)
        {
            xmax=spl.p[i][0];
        }
        if(spl.p[i][0]<xmin)
        {
            xmin=spl.p[i][0];
        }

    }
    //cout<<"Working";

    spl.p[spl.n][0]=spl.p[0][0];
    spl.p[spl.n][1]=spl.p[0][1];
    cout<<xmin<<" "<<xmax;
    for(int i=xmin;i<xmax;i++)
    {
        co=0;
        for(int j=0;j<spl.n;j++)
        {
            if(i==spl.p[j][0]||i==spl.p[j+1][0])
            {
                eds=1;
                i-=1;
            }
            y=(float(spl.p[j+1][1]-spl.p[j][1])/float(spl.p[j+1][0]-spl.p[j][0]))*float(i-spl.p[j][0])+spl.p[j][1]+1;
            if(spl.p[j][0]>spl.p[j+1][0])
            {
                if(i>spl.p[j+1][0]&&i<spl.p[j][0])
                {
                    arry[co]=y;
                    co++;
                }
            }
            else
            {
                if(i<spl.p[j+1][0]&&i>spl.p[j][0])
                {
                    arry[co]=y;
                    co++;
                }
            }
        }
        if(eds==1)
        {
            eds=0;
            i+=1;
        }

        sort(arry,arry+co);
        for(int k=0;k<co;k+=2)
        {
            //cout<<"Line";
            glColor3f(red,green,blue);
            //bresenhamAlg(i,arry[k],i,arry[k+1]);
            glBegin(GL_LINES);
            glVertex2f(i,arry[k+1]);
            glVertex2f(i,arry[k]);
            glEnd();
            //glFlush();
        }

    }


}
void axis()
{
    //std::cout<<"HI";

    glBegin(GL_LINES);
    glColor3b(0.0,0.0,0.0);
    glVertex2i(ww/2,0);
    glVertex2i(ww/2,wh);
    glVertex2i(ww,wh/2);
    glVertex2i(0,wh/2);

    glEnd();


}
void drawStrokeText(char*string,int x,int y,int z)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x, y+8,z);
	  glScalef(0.09f,0.08f,z);
	  for (c=string; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
	  }
	  glPopMatrix();
}
void circlemenu()
{
    //cout<<"Hello";
    //glClear(GL_COLOR_BUFFER_BIT);
    if(op==menu2)
    {
        glBegin(GL_QUADS); //For Clearing All
        glColor3f(1,1, 1);
        glVertex2i(0,0);
        glVertex2i(ww/2-10,0);
        glVertex2i(ww/2-10,25);
        glVertex2i(0,25);
        glEnd();

        glBegin(GL_QUADS); //For Bresenham Button
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(0,0);
        glVertex2i(80,0);
        glVertex2i(80,25);
        glVertex2i(0,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Mid-Point Button
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(100,0);
        glVertex2i(200,0);
        glVertex2i(200,25);
        glVertex2i(100,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Back Button
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(ww/2+20,0);
        glVertex2i(ww/2+100,0);
        glVertex2i(ww/2+100,25);
        glVertex2i(ww/2+20,25);
        glEnd();
        glColor3f(0,0, 1);

        drawStrokeText("BACK",ww/2+25,0,0);
        drawStrokeText("Bresenham",5,0,0);
        drawStrokeText("Mid-Point",105,0,0);
    }
    if(op==menu1)
    {
        glBegin(GL_QUADS); //For Clearing All
        glColor3f(1,1, 1);
        glVertex2i(0,0);
        glVertex2i(ww/2-10,0);
        glVertex2i(ww/2-10,25);
        glVertex2i(0,25);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(1,1, 1);
        glVertex2i(ww/2+20,0);
        glVertex2i(ww/2+100,0);
        glVertex2i(ww/2+100,25);
        glVertex2i(ww/2+20,25);
        glEnd();

        glBegin(GL_QUADS); //For Circle Drawing
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(0,0);
        glVertex2i(80,0);
        glVertex2i(80,25);
        glVertex2i(0,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Line Drawing
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(100,0);
        glVertex2i(180,0);
        glVertex2i(180,25);
        glVertex2i(100,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Ellipse Drawing
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(200,0);
        glVertex2i(280,0);
        glVertex2i(280,25);
        glVertex2i(200,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Polygon Drawing
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(300,0);
        glVertex2i(380,0);
        glVertex2i(380,25);
        glVertex2i(300,25);
        glEnd();
        glColor3f(0,0, 1);

        drawStrokeText("Circle",5,0,0);
        drawStrokeText("Line",127,0,0);
        drawStrokeText("Ellipse",220,0,0);
        drawStrokeText("Polygon",320,0,0);
    }

	glFlush();
	//cout<<"hi";
}


void linemenu()
{
    if(op==menu2)
    {
        glBegin(GL_QUADS); //For Clearing All
        glColor3f(1,1, 1);
        glVertex2i(0,0);
        glVertex2i(ww/2-10,0);
        glVertex2i(ww/2-10,25);
        glVertex2i(0,25);
        glEnd();

        glBegin(GL_QUADS); //For Simple DDA Button
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(0,0);
        glVertex2i(80,0);
        glVertex2i(80,25);
        glVertex2i(0,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Symmetric DDA Button
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(100,0);
        glVertex2i(200,0);
        glVertex2i(200,25);
        glVertex2i(100,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Bresenham Button
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(220,0);
        glVertex2i(300,0);
        glVertex2i(300,25);
        glVertex2i(220,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Mid-Point Button
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(320,0);
        glVertex2i(400-15,0);
        glVertex2i(400-15,25);
        glVertex2i(320,25);
        glEnd();
        glColor3f(0,0, 1);

        glBegin(GL_QUADS); //For Back Button
        glColor3f(0.8,0.7, 0.2);
        glVertex2i(ww/2+20,0);
        glVertex2i(ww/2+100,0);
        glVertex2i(ww/2+100,25);
        glVertex2i(ww/2+20,25);
        glEnd();
        glColor3f(0,0, 1);

        drawStrokeText("BACK",ww/2+25,0,0);

        drawStrokeText("Simple DDA",5,0,0);
        drawStrokeText("Symmetric DDA",105,0,0);
        drawStrokeText("Bresenham",225,0,0);
        drawStrokeText("Mid-Point",325,0,0);
    }

	glFlush();

}
void menu()
{
    glBegin(GL_QUADS); //For Clear Button
	glColor3f(0.8,0.7, 0);
	glVertex2i(ww-80,0);
	glVertex2i(ww,0);
	glVertex2i(ww,25);
	glVertex2i(ww-80,25);
	glEnd();
	glColor3f(0,0, 1);



    circlemenu();
    linemenu();

	glBegin(GL_QUADS); //For Simple Lines
	glColor3f(0.8,0.7, 0);
	glVertex2i(0,wh);
	glVertex2i(80,wh);
	glVertex2i(80,wh-25);
	glVertex2i(0,wh-25);
	glEnd();



	glBegin(GL_QUADS); //For Dotted Lines
	glColor3f(0.8,0.7, 0);
	glVertex2i(100,wh);
	glVertex2i(180,wh);
	glVertex2i(180,wh-25);
	glVertex2i(100,wh-25);
	glEnd();

	glBegin(GL_QUADS); //For Dashed Lines
	glColor3f(0.8,0.7, 0);
	glVertex2i(200,wh);
	glVertex2i(280,wh);
	glVertex2i(280,wh-25);
	glVertex2i(200,wh-25);
	glEnd();


	glBegin(GL_QUADS); //For Bezier Curve
	glColor3f(0.8,0.7, 0);
	glVertex2i(300,wh);
	glVertex2i(380,wh);
	glVertex2i(380,wh-25);
	glVertex2i(300,wh-25);
	glEnd();



	glBegin(GL_QUADS); //For Red Color
	glColor3f(1,0, 0);
	glVertex2i(ww,wh);
	glVertex2i(ww-25,wh);
	glVertex2i(ww-25,wh-25);
	glVertex2i(ww,wh-25);
	glEnd();

	glBegin(GL_QUADS); //For Blue Color
	glColor3f(0,0, 1);
	glVertex2i(ww,wh-50);
	glVertex2i(ww-25,wh-50);
	glVertex2i(ww-25,wh-25);
	glVertex2i(ww,wh-25);
	glEnd();

	glBegin(GL_QUADS); //For Green Color
	glColor3f(0,1, 0);
	glVertex2i(ww,wh-50);
	glVertex2i(ww-25,wh-50);
	glVertex2i(ww-25,wh-75);
	glVertex2i(ww,wh-75);
	glEnd();

	glBegin(GL_QUADS); //For Yellow Color
	glColor3f(1,1, 0);
	glVertex2i(ww,wh-100);
	glVertex2i(ww-25,wh-100);
	glVertex2i(ww-25,wh-75);
	glVertex2i(ww,wh-75);
	glEnd();

	glBegin(GL_QUADS); //For Pink Color
	glColor3f(1,0, 1);
	glVertex2i(ww,wh-100);
	glVertex2i(ww-25,wh-100);
	glVertex2i(ww-25,wh-125);
	glVertex2i(ww,wh-125);
	glEnd();



	glBegin(GL_QUADS); //For 1%
	glColor3f(0.8,1, 1);
	glVertex2i(ww,wh-150);
	glVertex2i(ww-25,wh-150);
	glVertex2i(ww-25,wh-175);
	glVertex2i(ww,wh-175);
	glEnd();
	glColor3f(0,0, 1);
	drawStrokeText("1%",ww-20,wh-175,0);

	glBegin(GL_QUADS); //For 5%
	glColor3f(0.8,1, 1);
	glVertex2i(ww,wh-190);
	glVertex2i(ww-25,wh-190);
	glVertex2i(ww-25,wh-215);
	glVertex2i(ww,wh-215);
	glEnd();
	glColor3f(0,0, 1);
	drawStrokeText("5%",ww-20,wh-215,0);

	glBegin(GL_QUADS); //For 20%
	glColor3f(0.8,1, 1);
	glVertex2i(ww,wh-230);
	glVertex2i(ww-25,wh-230);
	glVertex2i(ww-25,wh-255);
	glVertex2i(ww,wh-255);
	glEnd();
	glColor3f(0,0, 1);
	drawStrokeText("20%",ww-23,wh-255,0);


	glBegin(GL_QUADS); //For 10
	glColor3f(0.8,1, 1);
	glVertex2i(ww,wh/2-25);
	glVertex2i(ww-25,wh/2-25);
	glVertex2i(ww-25,wh/2-50);
	glVertex2i(ww,wh/2-50);
	glEnd();
	glColor3f(0,0, 1);
	drawStrokeText("10'",ww-20,wh/2-50,0);

	glBegin(GL_QUADS); //For 30
	glColor3f(0.8,1, 1);
	glVertex2i(ww,wh/2-65);
	glVertex2i(ww-25,wh/2-65);
	glVertex2i(ww-25,wh/2-90);
	glVertex2i(ww,wh/2-90);
	glEnd();
	glColor3f(0,0, 1);
	drawStrokeText("30'",ww-20,wh/2-90,0);

	glBegin(GL_QUADS); //For 45
	glColor3f(0.8,1, 1);
	glVertex2i(ww,wh/2-105);
	glVertex2i(ww-25,wh/2-105);
	glVertex2i(ww-25,wh/2-130);
	glVertex2i(ww,wh/2-130);
	glEnd();
	glColor3f(0,0, 1);
	drawStrokeText("45'",ww-23,wh/2-130,0);


	glBegin(GL_QUADS); //Thickness 1X
	glColor3f(0.8,0.7, 0);
	glVertex2i(ww/2+50,wh);
	glVertex2i(ww/2+25,wh);
	glVertex2i(ww/2+25,wh-25);
	glVertex2i(ww/2+50,wh-25);
	glEnd();
	glColor3f(0,0, 1);

	glBegin(GL_QUADS); //Thickness 3X
	glColor3f(0.8,0.7, 0);
	glVertex2i(ww/2+100,wh);
	glVertex2i(ww/2+75,wh);
	glVertex2i(ww/2+75,wh-25);
	glVertex2i(ww/2+100,wh-25);
	glEnd();
	glColor3f(0,0, 1);

	glBegin(GL_QUADS); //Thickness 5X
	glColor3f(0.8,0.7, 0);
	glVertex2i(ww/2+150,wh);
	glVertex2i(ww/2+125,wh);
	glVertex2i(ww/2+125,wh-25);
	glVertex2i(ww/2+150,wh-25);
	glEnd();
	glColor3f(0,0, 1);

	glBegin(GL_QUADS); //B Fill Color
	glColor3f(0.8,0.7, 0);
	glVertex2i(ww/2+215,wh);
	glVertex2i(ww/2+175,wh);
	glVertex2i(ww/2+175,wh-25);
	glVertex2i(ww/2+215,wh-25);
	glEnd();
	glColor3f(0,0, 1);

	glBegin(GL_QUADS); //F Fill Color
	glColor3f(0.8,0.7, 0);
	glVertex2i(ww/2+275,wh);
	glVertex2i(ww/2+235,wh);
	glVertex2i(ww/2+235,wh-25);
	glVertex2i(ww/2+275,wh-25);
	glEnd();
	glColor3f(0,0, 1);



	//drawStrokeText("Bresenham",225,0,0);
	//drawStrokeText("Mid-Point",325,0,0);

	drawStrokeText("Simple Lines",4,wh-25,0);
	drawStrokeText("Dotted Lines",104,wh-25,0);
	drawStrokeText("Dashed Lines",204,wh-25,0);
	drawStrokeText("Bezier Curve",304,wh-25,0);
	drawStrokeText("Choose Color",ww-105,wh-25,0);

	drawStrokeText("Scaling",ww-55,wh-150,0);
	drawStrokeText("Rotation",ww-55,wh/2-25,0);

	drawStrokeText("1X",ww/2+29,wh-25,0);
	drawStrokeText("3X",ww/2+79,wh-25,0);
	drawStrokeText("5X",ww/2+129,wh-25,0);
	drawStrokeText("F. Fill",ww/2+179,wh-25,0);
	drawStrokeText("B. Fill",ww/2+239,wh-25,0);
	drawStrokeText("CLEAR",ww-55,0,0);
	drawStrokeText("0",ww/2+5,wh/2-20,0);
}
void display()
{
    //std::cout<<"HI";
	glClearColor(1, 1,1, 0);
	glColor3f(0.2, 0.3, 0.3);
	glClear(GL_COLOR_BUFFER_BIT);
	axis();
	menu();
	glFlush();
	//glutDisplayFunc(display);
}
void bresenhamAlgo(int x0, int y0, int x1, int y1)
{
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int x, y;
	if (dx >= dy)
	{
		int d = 2 * dy - dx;
		int ds = 2 * dy;
		int dt = 2 * (dy - dx);
		if (x0 < x1)
		{
			x = x0;
			y = y0;
		}
		else
		{
			x = x1;
			y = y1;
			x1 = x0;
			y1 = y0;
		}
		putPixel(x, y);
		while (x < x1)
		{
			if (d < 0)
				d += ds;
			else {
				if (y < y1) {
					y++;
					d += dt;
				}
				else {
					y--;
					d += dt;
				}
			}
			x++;
			putPixel(x, y);
		}
	}
	else
    {
		int d = 2 * dx - dy;
		int ds = 2 * dx;
		int dt = 2 * (dx - dy);
		if (y0 < y1) {
			x = x0;
			y = y0;
		}
		else {
			x = x1;
			y = y1;
			y1 = y0;
			x1 = x0;
		}
		putPixel(x, y);
		while (y < y1)
		{
			if (d < 0)
				d += ds;
			else {
				if (x > x1) {
					x--;
					d += dt;
				}
				else {
					x++;
					d += dt;
				}
			}
			y++;
			putPixel(x, y);
		}
	}
	glFlush();
}
void simpleDDA(int x0, int y0, int x1, int y1)
    {
        //cout<<"Hi";
        int dx = x1 - x0;
        int dy = y1 - y0;
        int m,i;
        double inc,l;
        putPixel(x0,y0);
        if(abs(dx)>=abs(dy))
        {
            l=y0+0.5;
            inc=dy/(float)dx;
            //cout<<inc;
            for(i=x0;i<=x1;i++)
            {
                l+=inc;
                m=l;
                putPixel(i,m);
            }
        }
        else
        {
            l=x0+0.5;
            inc=dx/(float)dy;
            for(i=y0;i<=y1;i++)
            {
                l+=inc;
                m=l;
                putPixel(m,i);
            }
        }
        glFlush();
    }
void bresenhamAlg(int x1, int y1, int x2, int y2)
{
    float dX,dY,iSteps;
    float xInc,yInc,iCount,x,y;

    dX = x1 - x2;
    dY = y1 - y2;

    if (fabs(dX) > fabs(dY))
    {
        iSteps = fabs(dX);
    }
    else
    {
        iSteps = fabs(dY);
    }

    xInc = dX/iSteps;
    yInc = dY/iSteps;

    x = x1;
    y = y1;
//    circle(x,y,1);

    for (iCount=1; iCount<=iSteps; iCount++)
    {
        putPixel(floor(x),floor(y));
        x -= xInc;
        y -= yInc;
    }
    glFlush();
}

void SymDDA(int x0, int y0, int x1, int y1)

    {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int x,y,i,n;
        double xinc,yinc,k1,k2,ep;
        k1=x0+0.5;
        k2=y0+0.5;
        x=k1;
        y=k2;
        n=1;
        if(abs(dx)>abs(dy))
        {
            while(abs(dx)>pow(2,n))
            {
                n++;
            }
            ep=1/pow(2,n);
            xinc=ep*dx;
            yinc=ep*dy;
            while(x!=x1)
            {
                k1+=xinc;
                k2+=yinc;
                x=k1;
                y=k2;
                putPixel(x,y);
            }
        }
        else
        {
            while(abs(dy)>pow(2,n))
            {
                n++;
            }
            ep=1/pow(2,n);
            xinc=ep*dx;
            yinc=ep*dy;
            while(y!=y1)
            {
                k1+=xinc;
                k2+=yinc;
                x=k1;
                y=k2;
                putPixel(x,y);
            }
        }
        glFlush();
}

void midPoint(int x1, int y1, int x2, int y2)
{
    bool steep = abs(y2 - y1) > abs(x2 - x1);
    if(steep) {
        // swap x1 and y1
        int tmp = x1;
        x1 = y1;
        y1 = tmp;
        // swap x2 and y2
        tmp = x2;
        x2 = y2;
        y2 = tmp;
    }
    if(x1 > x2) {
        // swap x1 and x2
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
        // swap y1 and y2
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    int dx, dy, error, ystep, y,x;

    dx = x2 - x1;
    dy = abs(y2 - y1);
    error = dx / 2;
    y = y1;

    if(y1 < y2)
        ystep = 1;
    else
        ystep = -1;

    for (x = x1; x <= x2; x++) {
        if(steep)
            putPixel(y, x);
        else
            putPixel(x, y);
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
    glFlush();
}
void getcolor(int x,int y,float c[])
{
    unsigned char a[3];
    glReadPixels(x,wh-y,1,1,GL_RGB,GL_UNSIGNED_BYTE,a);
    //float r,g,b;
    c[0] = (float)a[0]/255.0;
    c[1] = (float)a[1]/255.0;
    c[2] = (float)a[2]/255.0;
    //cout<<endl<<c[0]<<c[1]<<c[2];
}

void fillcolor(int x,int y)
{

    float cp[3],cc[3];
    getcolor(x,y,cc);

    putPixel(x,wh-y);
    //cout<<endl<<cc[0]<<cc[1]<<cc[2];
    getcolor(x-1,y,cp);
    //cout<<endl<<cp[0]<<cp[1]<<cp[2];
    if(cp[0]==cc[0]&&cp[1]==cc[1]&&cp[2]==cc[2])
    {
        //putPixel(x-1,y);
        fillcolor(x-1,y);
    }

    getcolor(x+1,y,cp);
    if(cp[0]==cc[0]&&cp[1]==cc[1]&&cp[2]==cc[2])
    {
        //putPixel(x+1,y);
        fillcolor(x+1,y);
    }

    getcolor(x,y-1,cp);
    if(cp[0]==cc[0]&&cp[1]==cc[1]&&cp[2]==cc[2])
    {
        //putPixel(x,y-1);
        fillcolor(x,y-1);
    }

    getcolor(x,y+1,cp);
    if(cp[0]==cc[0]&&cp[1]==cc[1]&&cp[2]==cc[2])
    {
        //putPixel(x,y+1);
        fillcolor(x,y+1);
    }


//    unsigned char a[3];
//    glReadPixels(x,wh-y,1,1,GL_RGB,GL_UNSIGNED_BYTE,a);
//    float r,g,b;
//    r = (float)a[0]/255.0;
//    g = (float)a[1]/255.0;
//    b = (float)a[2]/255.0;
//    cout<<endl<<r<<g<<b;

    //cout<<endl<<a[0]<<a[1]<<a[2];

    glFlush();
}

bool findbc(int x, int y, float a[])
{
    //Check left for a color.
    float mb[3],cc[3],mt[3];
    getcolor(x,y+1,cc);
    int t,b,l,r;
    t=b=y;
    l=r=x;
    //cout<<"'"<<x<<" "<<y<<"'"<<endl;
    while(t>50)
    {
        t--;
        b=y;
        getcolor(x,t,mb);
        //cout<<"'"<<x<<" "<<t<<"'"<<endl;
        if(mb[0]!=cc[0]||mb[1]!=cc[1]||mb[2]!=cc[2])
        {

            while(b<wh-50)
            {
                b++;
                //cout<<"'"<<x<<" "<<b<<"'"<<endl;
                getcolor(x,b,mt);
                if(mb[0]==mt[0]&&mb[1]==mt[1]&&mb[2]==mt[2])
                {
                    a[0]=mb[0];
                    a[1]=mb[1];
                    a[2]=mb[2];
                    return true;
                }
            }
        }


    }
    return false;
}



void fillboundary(int x,int y,float cc[])
{

    float cp[3];
    cout<<x<<" "<<y<<endl;
    putPixel(x,wh-y);
    if(red==cp[0]&&green==cp[1]&&blue==cp[2])
        return;
    //cout<<endl<<cc[0]<<cc[1]<<cc[2];
    getcolor(x-1,y,cp);
    cout<<endl<<cp[0]<<cp[1]<<cp[2];
    if(!(cp[0]==cc[0]&&cp[1]==cc[1]&&cp[2]==cc[2]))
    {
        //putPixel(x-1,y);
        fillboundary(x-1,y,cc);
    }
    getcolor(x+1,y,cp);
    if(!(cp[0]==cc[0]&&cp[1]==cc[1]&&cp[2]==cc[2]))
    {
        //putPixel(x+1,y);
        fillboundary(x+1,y,cc);
    }

    getcolor(x,y-1,cp);
    if(!(cp[0]==cc[0]&&cp[1]==cc[1]&&cp[2]==cc[2]))
    {
        //putPixel(x,y-1);
        fillboundary(x,y-1,cc);
    }

    getcolor(x,y+1,cp);
    if(!(cp[0]==cc[0]&&cp[1]==cc[1]&&cp[2]==cc[2]))
    {
        //putPixel(x,y+1);
        fillboundary(x,y+1,cc);
    }

}


void Bfillcolor(int x,int y)
{

    float cp[3],cc[3];
    int t,b,l,r;
    t=b=y;
    l=r=x;
    if(findbc(x,y,cc))
    {
        cout<<cc[0]<<cc[1]<<cc[2]<<endl;
        getcolor(x,b,cp);
        //cout<<"i";
        while(cp[0]!=cc[0]||cp[1]!=cc[1]||cp[2]!=cc[2])
        {
            //cout<<"j";
            l=r=x;
            getcolor(l,b,cp);
            while(cp[0]!=cc[0]||cp[1]!=cc[1]||cp[2]!=cc[2])
            {
                putPixel(l,wh-b);
                //cout<<"k";
                getcolor(--l,b,cp);
            }
            getcolor(r,b,cp);
            while(cp[0]!=cc[0]||cp[1]!=cc[1]||cp[2]!=cc[2])
            {
                putPixel(r,wh-b);
                getcolor(++r,b,cp);
            }
            x=(l+r)/2;
            getcolor(x,--b,cp);
        }

        getcolor(x,t,cp);
        while(cp[0]!=cc[0]||cp[1]!=cc[1]||cp[2]!=cc[2])
        {
            //cout<<"j";
            l=r=x;
            getcolor(l,t,cp);
            while(cp[0]!=cc[0]||cp[1]!=cc[1]||cp[2]!=cc[2])
            {
                putPixel(l,wh-t);
                //cout<<"k";
                getcolor(--l,t,cp);
            }
            getcolor(r,t,cp);
            while(cp[0]!=cc[0]||cp[1]!=cc[1]||cp[2]!=cc[2])
            {
                putPixel(r,wh-t);
                getcolor(++r,t,cp);
            }
            x=(l+r)/2;
            getcolor(x,++t,cp);
        }

    }
    else
    {
        cout<<endl<<"Not Possible";
    }


}


void circlebres(int x0,int y0,int r)
{
    cout<<"Circle";
    float dk=3-2*r;
    int xk,yk;
    xk=0;
    yk=r;
    while(yk>=xk)
    {
        putPixel(xk+x0,yk+y0);
        putPixel(-xk+x0,yk+y0);
        putPixel(-yk+x0,xk+y0);
        putPixel(yk+x0,xk+y0);


        putPixel(xk+x0,-yk+y0);
        putPixel(-xk+x0,-yk+y0);
        putPixel(-yk+x0,-xk+y0);
        putPixel(yk+x0,-xk+y0);

        putPixel(ww+100,wh+100);

        if(dk>=0)
        {
            dk+= 4*(xk-yk)+10;
            yk--;
        }
        else
        {
            dk+=4*xk+6;
        }
        xk++;


    }
}
void ellipseplot(int x,int y,int xc,int yc)
{
    putPixel(ww+100,wh+100);

    putPixel(x+xc,y+yc);
    putPixel(x+xc,-y+yc);
    putPixel(-x+xc,-y+yc);
    putPixel(-x+xc,y+yc);


    //putPixel(ww+100,wh+100);
    //putPixel(ww+100,wh+100);
}
void ellipsealgo(int rx,int ry,int xc,int yc)
{
    cout<<"Ellipse";
    unsigned int x,y;
    x=0;
    y=ry;
    double inc,dec,p;
    inc=2*pow(ry,2);
    dec=2*pow(rx,2);
    p=pow(ry,2)-pow(rx,2)*ry+0.25*rx*rx;
    ellipseplot(x,y,xc,yc);
    while(inc*x<dec*y)
    {
        //cout<<x<<y<<endl;
        if(p<0)
        {
            x++;
            p=p+inc*x+2*ry*ry+ry*ry;
        }
        else
        {
            x++;
            y--;
            p=p+inc*x+2*ry*ry-dec*y+2*rx*rx+ry*ry;
        }
        ellipseplot(x,y,xc,yc);
    }
    while(y>0)
    {
        //cout<<x<<y<<endl;
        p=ry*ry*(pow(x+0.5,2))+rx*rx*(y-1)*(y-1)-rx*rx*ry*ry;
        if(p>0)
        {
            y--;
            p=p-dec*y+2*rx*rx+ry*ry;
        }
        else
        {
            x++;
            y--;
            p=p+inc*x+2*ry*ry-dec*y+2*rx*rx+ry*ry;
        }
        ellipseplot(x,y,xc,yc);
    }

}
void printchk()
{

    insclip=0;

    if(npoly.size()>1)
    {


        poly cn;
    int i;
    //cout<<npoly.size()<<" ";

    red=1.0;
    blue=1.0;
    green=1.0;

    insclip=0;

    for(int j=0;j<npoly.size()-1;j++)
    {
        cn=npoly[j];
        for( i=0;i<cn.n-1;i++)
        {
            //glPointSize(2.0f);
            bresenhamAlg(cn.p[i][0],cn.p[i][1],cn.p[i+1][0],cn.p[i+1][1]);

        }
        bresenhamAlg(cn.p[i][0],cn.p[i][1],cn.p[0][0],cn.p[0][1]);
        scanpoly2(npoly[j]);
        //glPointSize(1.0f);
    }

    red=1.0;
    blue=0.0;
    green=0.0;

    cn=newp;
    for( i=0;i<cn.n-1;i++)
    {
        //glPointSize(2.0f);
        bresenhamAlg(cn.p[i][0],cn.p[i][1],cn.p[i+1][0],cn.p[i+1][1]);

    }
    bresenhamAlg(cn.p[i][0],cn.p[i][1],cn.p[0][0],cn.p[0][1]);
    scanpoly2(cn);
    glFlush();
    //glPointSize(1.0f);

    red=0.0;
    blue=1.0;
    green=0.0;



    //return;

        insclip=1;
        cn=npoly[0];
        cout<<"Poly 2"<<endl;
        setp.p[0][0]=cn.p[0][0];
        setp.p[0][1]=cn.p[0][1];
        setp.p[0][2]=0;
        setp.n=1;
        for(i=0;i<cn.n-1;i++)
        {
            //glPointSize(2.0f);
            bresenhamAlg(cn.p[i][0],cn.p[i][1],cn.p[i+1][0],cn.p[i+1][1]);
            setp.p[setp.n][0]=cn.p[i+1][0];
            setp.p[setp.n][1]=cn.p[i+1][1];
            setp.p[setp.n][2]=0;
            setp.n++;
        }
        bresenhamAlg(cn.p[i][0],cn.p[i][1],cn.p[0][0],cn.p[0][1]);
        scanpoly2(cn);
        //glPointSize(1.0f);
    }

    red=0.0;
    blue=1.0;
    green=0.0;


//    glBegin(GL_POLYGON);
//    for(int j=npoly.size()-1;j<npoly.size();j++)
//    {
//        cn=npoly[j];
//        for(int i=0;i<cn.n;i++)
//        {
//            glVertex2i(cn.p[i][0],cn.p[i][1]);
//            cout<<cn.p[i][0]<<" "<<cn.p[i][1]<<endl;
//        }
//        cout<<j<<endl<<endl;
//    }
//    glEnd();

    glFlush();
}
void keyboard(unsigned char key, int x, int y)
{
    if(key=='b')

        {
                glColor3f(0.0,0.0,1.0);
                for(int i=0;i<bcount-1;i++)
                    drawLine(abc[i], abc[i+1]);
//                glColor3f(1.0,0.0,1.0);
//                drawLine(abc[0], abc[1]);
//                drawLine(abc[1], abc[2]);
//                drawLine(abc[2], abc[3]);
                glColor3f(0.0,0.0,1.0);

                point p1 = abc[0];
                /* Draw each segment of the curve.Make t increment in smaller amounts for a more detailed curve.*/
                for(double t = 0.0;t <= 1.0; t += 0.02)
                {
                    point p2 = drawBezierGeneralized(abc,t,bcount);
                    cout<<p1.x<<"  ,  "<<p1.y<<endl;
                    cout<<p2.x<<"  ,  "<<p2.y<<endl;
                    cout<<endl;
                    drawLine(p1, p2);
                    p1 = p2;
                }
                glColor3f(0.0,0.0,1.0);

                bcount = 0;


//                for(double t = 0.0;t <= 1.0; t += 0.1)
//                {
//                    Point P = drawBezier(abc[0], abc[1], abc[2], abc[3],  t);
//                    drawLine(POld, P);
//                    POld = P;
//                }
                glFlush();

        }

    if(key=='s')
    {
        insclip=0;
        scanpoly();
        insclip=1;
    }
    if(key=='1')
    {
        cout<<"1";
        int xn,yn;
        cout<<" N:"<<setp.n;
        poly chk;
        chk.n=0;


        polyq cn=spoly[spoly.size()-1];
        cout<<" N2:"<<cn.n;

        glColor3f(0.0,1.0,0.0);
        //glBegin(GL_POLYGON);

        for(int i=0;i<setp.n;i++)
        {
            cout<<"c";
            xn=setp.p[i][0];
            yn=setp.p[i][1];
            if(setp.p[i][2]==1)
            {

                int j;
                for(j=0;j<cn.n;j++)
                {
                    if(cn.p[j][0]>=xn-3&&cn.p[j][0]<=xn+3&&cn.p[j][1]>=yn-3&&cn.p[j][1]<=yn+3)
                    {
                        cout<<"YES";
                        chk.p[chk.n][0]=cn.p[j][0];
                        chk.p[chk.n][1]=cn.p[j][1];
                        chk.n++;
                        //glVertex2i(cn.p[j][0],cn.p[j][1]);
                        break;
                    }

                }
                while(1)
                {
                    cout<<"k";
                    j++;
                    if(j>cn.n-1)
                        j=0;
                    //glVertex2i(cn.p[j][0],cn.p[j][1]);

                    chk.p[chk.n][0]=cn.p[j][0];
                    chk.p[chk.n][1]=cn.p[j][1];
                    chk.n++;

                    if(cn.p[j][2]==1)
                    {
                        xn=cn.p[j][0];
                        yn=cn.p[j][1];
                        while(1)
                        {
                            i++;
                            if(setp.p[i][2]==2)
                            {
                                if(setp.p[i][0]>=xn-3&&setp.p[i][0]<=xn+3&&setp.p[i][1]>=yn-3&&setp.p[i][1]<=yn+3)
                                    goto lb;
                            }

                        }
                        //break;
                    }

                }
            }
            else
            {
                chk.p[chk.n][0]=setp.p[i][0];
                chk.p[chk.n][1]=setp.p[i][1];
                chk.n++;
            }
            lb:
                cout<<" ";

            //glVertex2i(setp.p[i][0],setp.p[i][1]);
        }
        //glEnd();
        cout<<"Result"<<chk.n;
        //scanpoly2(chk);
        glPointSize(3.0f);
        red=0.50;
        blue=0.0;
        green=1.0;
        int i;
        for(i=0;i<chk.n-1;i++)
        {
            bresenhamAlg(chk.p[i][0],chk.p[i][1],chk.p[i+1][0],chk.p[i+1][1]);
        }
        bresenhamAlg(chk.p[i][0],chk.p[i][1],chk.p[0][0],chk.p[0][1]);
        glPointSize(1.0f);
        glFlush();

    }
    if(key=='2')
    {
        cout<<"2";
        int xn,yn,j,k;
        cout<<"N:"<<setp.n;


        polyq cn=spoly[spoly.size()-1];
        cout<<"N2:"<<cn.n;

        glColor3f(0.0,1.0,1.0);

        for(int i=0;i<setp.n;i++)
        {

            glBegin(GL_POLYGON);
            if(setp.p[i][2]==1)
            {
                glVertex2i(setp.p[i][0],setp.p[i][1]);
                while(setp.p[i][2]!=2)
                {
                    cout<<"c";
                    i++;
                    glVertex2i(setp.p[i][0],setp.p[i][1]);

                }


            xn=setp.p[i][0];
            yn=setp.p[i][1];


                for(j=0;j<cn.n;j++)
                {
                    if(cn.p[j][0]>=xn-3&&cn.p[j][0]<=xn+3)
                    {
                        cout<<"YES";
                        glVertex2i(cn.p[j][0],cn.p[j][1]);
                        break;
                    }

                }
                while(cn.p[j][2]!=2)
                {
                    cout<<"k";
                    j++;
                    if(j>cn.n-1)
                        j=0;
                    glVertex2i(cn.p[j][0],cn.p[j][1]);

                }

            xn=cn.p[j][0];
            yn=cn.p[j][1];
            for(k=0;k<cn.n;k++)
                {
                    if(setp.p[k][0]>=xn-3&&setp.p[k][0]<=xn+3)
                    {
                        cout<<"YES";
                       // glVertex2i(cn.p[j][0],cn.p[j][1]);
                       glEnd();
                       glBegin(GL_POLYGON);
                        break;
                    }

                }
            }
        }


            //glVertex2i(setp.p[i][0],setp.p[i][1]);

        glEnd();
        glFlush();
    }
    if(key=='3')
    {
        int i;
        glColor3f(1.0,1.0,0.0);
        cout<<"3";
        glBegin(GL_POLYGON);
        for(i=0;i<setp.n;i++)
        {
            glVertex2i(setp.p[i][0],setp.p[i][1]);
        }
        glEnd();
        setp=spoly[spoly.size()-1];
        glBegin(GL_POLYGON);
        for(i=0;i<setp.n;i++)
        {
            glVertex2i(setp.p[i][0],setp.p[i][1]);
        }
        glEnd();
        glFlush();
    }
    if(key=='c')
    {
        //glPointSize(2.0f);
            getcolor(fx,wh-fy,pc);
            bresenhamAlg(fx,fy,px,py);
        //glPointSize(1.0f);

//            glBegin(GL_LINES);
//                glColor3f(red,green,blue);
//                glVertex2i(px,py);
//                glVertex2i(fx,fy);
//            glEnd();
//            glFlush();

            glColor3f(0.0,1.0,0.0);
            //scanpoly2(newp);
//            glBegin(GL_POLYGON);
//            for(int i=0;i<newp.n;i++)
//                glVertex2d(newp.p[i][0],newp.p[i][1]);
//            glEnd();
            glFlush();

            px=-1;
            py=-1;
            npoly.push_back(newp);
            cout<<"Check";
            if(newp.n!=setp.n)
            {
                spoly.push_back(setp);
                cout<<" N:"<<setp.n<<endl;
                setp.n=0;
            }

            if(npoly.size()==2)
            {
                scanpoly2(npoly[1]);
            }
            //setp.n=0;
            printchk();
            insclip=1;
            newp.n=0;

    }
    if(sn==0)
    {
        return;
    }
    switch(key)
    {


        case '+':
            {
                cout<<"Hello";

            details de = shapes[sn-1];
            xi=de.x1;
            xf=de.x2;
            yi=de.y1;
            yf=de.y2;

            red=1.0;
            blue=1.0;
            green=1.0;

            algo=de.algo;

            currentdeg=de.cdegree;

            draw();

            axis();

            int incx=scalex*(xf-xi)/100;
            int incy=scaley*(yf-yi)/100;

            xi=de.x1;
            xf=de.x2+incx;
            yi=de.y1;
            yf=de.y2+incy;

            red=de.r;
            blue=de.b;
            green=de.g;

            draw();

            first=0;

            shapes[sn-1].x1=xi;
            shapes[sn-1].x2=xf;
            shapes[sn-1].y1=yi;
            shapes[sn-1].y2=yf;

            currentdeg=0;

        // call a function
        break;
            }
        case '-':
            {
                cout<<"Hello";

            details de = shapes[sn-1];
            xi=de.x1;
            xf=de.x2;
            yi=de.y1;
            yf=de.y2;

            red=1.0;
            blue=1.0;
            green=1.0;

            algo=de.algo;

            currentdeg=de.cdegree;

            draw();

            axis();

            int incx=scalex*(xf-xi)/100;
            int incy=scaley*(yf-yi)/100;

            xi=de.x1;
            xf=de.x2-incx;
            yi=de.y1;
            yf=de.y2-incy;

            red=de.r;
            blue=de.b;
            green=de.g;

            draw();

            first=0;

            shapes[sn-1].x1=xi;
            shapes[sn-1].x2=xf;
            shapes[sn-1].y1=yi;
            shapes[sn-1].y2=yf;

            currentdeg=0;

        // call a function
        break;
            }

        case 'q':
            {
                cout<<"q";

            details de = shapes[sn-1];
            xi=de.x1;
            xf=de.x2;
            yi=de.y1;
            yf=de.y2;

            red=1.0;
            blue=1.0;
            green=1.0;
            currentdeg=de.cdegree;

            algo=de.algo;

            draw();

            axis();

            currentdeg=de.cdegree+degree;

            red=de.r;
            blue=de.b;
            green=de.g;

            draw();

            first=0;

            shapes[sn-1].x1=xi;
            shapes[sn-1].x2=xf;
            shapes[sn-1].y1=yi;
            shapes[sn-1].y2=yf;
            shapes[sn-1].cdegree=currentdeg;
            currentdeg=0;

            }

        // call a function
        break;
        case 'w':
            {
                cout<<"q";

            details de = shapes[sn-1];
            xi=de.x1;
            xf=de.x2;
            yi=de.y1;
            yf=de.y2;

            red=1.0;
            blue=1.0;
            green=1.0;
            currentdeg=de.cdegree;

            algo=de.algo;

            draw();

            axis();

            currentdeg=de.cdegree-degree;

            red=de.r;
            blue=de.b;
            green=de.g;

            draw();

            first=0;

            shapes[sn-1].x1=xi;
            shapes[sn-1].x2=xf;
            shapes[sn-1].y1=yi;
            shapes[sn-1].y2=yf;
            shapes[sn-1].cdegree=currentdeg;
            currentdeg=0;
            }
        break;

        default:
            cout<<key<<endl;
        break;
    }
    //glutPostRedisplay();
    /* this redraws the scene without
    waiting for the display callback so that any changes appear
    instantly */
}

details d;
findc f;


void mouse(int btn, int state, int x, int y)
{

   // y=wh-y;
    if (btn == GLUT_RIGHT_BUTTON  && state == GLUT_DOWN)
	{
        rightclickset=0;

	    if(x>ww-25&&y<175&&y>150)
        {
            scaley=1;
        }
        else if(x>ww-25&&y<215&&y>190)
        {
            scaley=5;
        }
        else if(x>ww-25&&y<255&&y>230)
        {
            scaley=20;
            cout<<"RYES";
        }
        else
        {
            rightclickset=1;
            y=wh-y;
            rx=x;
            ry=y;
            findc f=v[0];
            int dis=sqrt(pow(f.x1-x,2)+pow(f.y1-y,2));
            int dmin;
            dmin=dis;
            sn=f.n;
            for(int i=1;i<v.size();i++)
            {
                f=v[i];
                cout<<"dis "<<dis<<endl;
                dis=sqrt(pow(f.x1-x,2)+pow(f.y1-y,2));
                cout<<dis<<endl;
                if(dmin>dis)
                {
                    dmin=dis;
                    sn=f.n;
                }
            }
            cout<<sn<<" Figure selected"<<endl;
            cout<<"X and Y:"<<x<<" "<<ry<<endl;

        }


	}
	if (btn == GLUT_RIGHT_BUTTON  && state == GLUT_UP)
    {
        if(rightclickset)
        {
            y=wh-y;
            details de = shapes[sn-1];
            int changeX,changeY;
            //y=wh-y;
            changeX=x-rx;
            changeY=y-ry;

            cout<<changeX<<" "<<changeY<<endl;

            xi=de.x1;
            xf=de.x2;
            yi=de.y1;
            yf=de.y2;

            red=1.0;
            blue=1.0;
            green=1.0;

            algo=de.algo;

            lt=simple;
            //thickness=3;
            currentdeg=de.cdegree;

            draw();

            axis();

            xi=de.x1+changeX;
            xf=de.x2+changeX;
            yi=de.y1+changeY;
            yf=de.y2+changeY;

            red=de.r;
            blue=de.b;
            green=de.g;

            lt=de.l;



            draw();

            first=0;

            shapes[sn-1].x1=xi;
            shapes[sn-1].x2=xf;
            shapes[sn-1].y1=yi;
            shapes[sn-1].y2=yf;

            cout<<v[sn-1].x1<<" "<<v[sn-1].y1<<endl;

            v[sn-1].x1=(xi+xf)/2;
            v[sn-1].y1=(yi+yf)/2;

            cout<<v[sn-1].x1<<" "<<v[sn-1].y1<<endl;
            currentdeg=0;
        }


        //cout<<de.x1<<" "<<de.x2<<" "<<de.y1<<" "<<de.y2<<endl;
    }
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
	    //fillcolor(x,y);
        //y=wh-y;
        std::cout<<x<<" "<<wh-y<<" "<<algo<<std::endl;
        if(x>ww-80&&y>wh-25)
        {
            op=menu1;
            display();
            BFill=N;
            CFill=N;
            v.clear();
            shapes.clear();
            shapeno=1;
            npoly.clear();
            spoly.clear();
            red=1.0;
            blue=0.0;
            green=0.0;
            insclip=0;
            newp.n=0;
            setp.n=0;
            px=-1;
            py=-1;
        }
        else if(x<80&&y>wh-25)
        {
            if(op==menu1)
            {
                op=menu2;
                cstatus=YES;
                circlemenu();

            }
            else if(op==menu2&&cstatus==YES)
            {
                algo=5;
                //circlebres(ww/2,wh/2,250);
            }
            else
                algo=2;
        }
        else if(x>100&&x<180&&y>wh-25)
        {
            if(op==menu1)
            {
                op=menu2;
                linemenu();
                cstatus=NO;
            }
            else if(op==menu2&&cstatus==YES)
            {
                algo=5;
                //circlebres(ww/2,wh/2,250);
            }
            else
                algo =3;
        }
        else if(x>200&&x<280&&y>wh-25&&op==menu1)
        {
            algo =6;
        }
        else if(x>300&&x<380&&y>wh-25&&op==menu1)
        {
            algo =7;
            px=-1;py=-1;
            cout<<"Hi";
        }
        else if(x>220&&x<300&&y>wh-25)
        {
            algo=1;
        }
        else if(x>320&&x<400&&y>wh-25)
        {
            algo=4;
        }
        else if(x>ww/2+20&&x<ww/2+100&&y>wh-25&&op==menu2)
        {
            op=menu1;
            circlemenu();
            linemenu();
            cstatus=NO;
            lstatus=YES;
            algo=1;
        }
        else if(x<80&&y<25)
        {
            lt=simple;
        }
        else if(x>100&&x<180&&y<25)
        {
            lt=dotted;
            //cout<<"Dotted Lines";
        }
        else if(x>200&&x<280&&y<25)
        {
            lt=dashed;
            //cout<<"Dashed Lines";
        }
        else if(x>300&&x<380&&y<25)
        {
            algo=8;
        }
        else if(x>ww-25&&y<25)
        {
            red=1.0;
            green=0.0;
            blue=0.0;
        }
        else if(x>ww-25&&y<50&&y>25)
        {
            red=0.0;
            green=0.0;
            blue=1.0;
        }
        else if(x>ww-25&&y<75&&y>50)
        {
            red=0.0;
            green=1.0;
            blue=0.0;
        }
        else if(x>ww-25&&y<100&&y>75)
        {
            red=1.0;
            green=1.0;
            blue=0.0;
        }
        else if(x>ww-25&&y<125&&y>100)
        {
            red=1.0;
            green=0.0;
            blue=1.0;
        }
        else if(x>ww-25&&y<175&&y>150)
        {
            scalex=1;
        }
        else if(x>ww-25&&y<215&&y>190)
        {
            scalex=5;
        }
        else if(x>ww-25&&y<255&&y>230)
        {
            scalex=20;
            cout<<"YES";
        }
         else if(x>ww-25&&y>wh/2+25&&y<wh/2+50)
        {
            degree=10;
            cout<<"gdf";
        }
        else if(x>ww-25&&y<wh/2+90&&y>wh/2+65)
        {
            degree=30;
        }
        else if(x>ww-25&&y<wh/2+130&&y>wh/2+105)
        {
            degree=45;
            cout<<"YES";
        }
        else if(x>ww/2+25&&x<ww/2+50&&y<25)
        {
            thickness=1;
        }
        else if(x>ww/2+75&&x<ww/2+100&&y<25)
        {
            thickness=3;
            cout<<" 3 ";
        }
        else if(x>ww/2+125&&x<ww/2+150&&y<25)
        {
            thickness=5;
             cout<<" 5 ";
        }
        else if(x>ww/2+175&&x<ww/2+215&&y<25)
        {
            if(CFill==N)
                CFill=Y;
            else
                CFill=N;
            BFill=N;
        }
        else if(x>ww/2+235&&x<ww/2+275&&y<25)
        {
            if(BFill==N)
                BFill=Y;
            else
                BFill=N;
            CFill=N;
        }
        else if(CFill==Y)
        {
            fillcolor(x,y);
            glFlush();
        }

        else if(BFill==Y)
        {
            //scanpoly();
            Bfillcolor(x,y);
            glFlush();
        }
        else if(algo==8)
        {

            glPointSize(3.0f);
            if(bcount==0)
            {
                abc[0].x=x,abc[0].y=wh-y;
                POld=abc[0];
                bcount++;
                putPixel(x,wh-y);
            }
            else
            {
                abc[bcount].x=x,abc[bcount].y=wh-y;
                bcount++;
                putPixel(x,wh-y);
            }

            glPointSize(1.0f);

            glFlush();
            //glPointSize(1.0f);

        }
        else if(algo==7)
        {
            //newp;
            if(px<0)
            {
                if(npoly.size()>0)
                {
                    red=1.0;
                    blue=0.0;
                    green=0.0;
                    scanpoly2(npoly[0]);
                    glFlush();
                    red=0.0;
                    blue=1.0;
                    green=0.0;
                }
                newp.p[newp.n][0]=fx=px=x;
                newp.p[newp.n][1]=fy=py=wh-y;
                newp.n=1;
                setp.p[0][0]=x;
                setp.p[0][1]=wh-y;
                setp.p[0][2]=0;
                setp.n=1;
                getcolor(x,wh-y,pc);
            }
            else
            {
                cx=x;
                cy=wh-y;
                //glPointSize(2.0f);

                bresenhamAlg(px, py, cx, cy);

                //glPointSize(1.0f);
//                glBegin(GL_LINES);
//                glColor3f(red,green,blue);
//                glVertex2i(px,py);
//                glVertex2i(cx,cy);
//                glEnd();
                glFlush();
                newp.p[newp.n][0]=px=cx;
                newp.p[newp.n][1]=py=cy;
                setp.p[setp.n][0]=cx;
                setp.p[setp.n][1]=cy;
                setp.p[setp.n][2]=0;
                setp.n++;
                newp.n++;
            }
        }
        else

		switch (first)
		{
		case 0:
			xi = x;
			yi = (wh- y);
			first = 1;
			break;
		case 1:
			xf = x;
			yf = (wh- y);



                d.x1=xi;
                d.x2=xf;
                d.y1=yi;
                d.y2=yf;
                d.algo=algo;
                d.l=lt;
                d.thickness=thickness;
                d.r=red;
                d.g=green;
                d.b=blue;

                f.n=shapeno++;
                f.x1=(xi+xf)/2;
                f.y1=(yi+yf)/2;
                cout<<f.x1<<" "<<f.y1<<endl;

                draw();

                v.push_back(f);
                //details <
                shapes.push_back(d);

                cout<<"inserted in V"<<endl;


            first = 0;
		}
	}
	//glFlush();
}

void draw()
{
    pivotx=(xi+xf)/2.0;
    pivoty=(yi+yf)/2.0;

    if(algo==1)
            {

                bresenhamAlg(xi, yi, xf, yf);



                if(thickness>=3)
                {
                    if(abs(xf-xi)>abs(yf-yi))
                    {
                        bresenhamAlg(xi, yi+1, xf, yf+1);
                        bresenhamAlg(xi, yi-1, xf, yf-1);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi, yi+1);
                        glVertex2i(xi, yi-1);
                        glVertex2i(xf, yf+1);
                        glVertex2i(xf, yf-1);
                        glEnd();
                        glFlush();
                    }
                    else
                    {
                        bresenhamAlg(xi+1, yi, xf+1, yf);
                        bresenhamAlg(xi-1, yi, xf-1, yf);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi-1, yi);
                        glVertex2i(xi+1, yi);
                        glVertex2i(xf-1, yf);
                        glVertex2i(xf+1, yf);
                        glEnd();
                        glFlush();

                    }
                }
                if(thickness==5)
                {
                    if(abs(xf-xi)>abs(yf-yi))
                    {
                        bresenhamAlg(xi, yi+2, xf, yf+2);
                        bresenhamAlg(xi, yi-2, xf, yf-2);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi, yi+2);
                        glVertex2i(xi, yi-2);
                        glVertex2i(xf, yf+2);
                        glVertex2i(xf, yf-2);

                        glVertex2i(xi+1, yi+2);
                        glVertex2i(xi-1, yi-2);
                        glVertex2i(xf+1, yf+2);
                        glVertex2i(xf-1, yf-2);
                        glEnd();
                        glFlush();

                    }
                    else
                    {
                        bresenhamAlg(xi+2, yi, xf+2, yf);
                        bresenhamAlg(xi-2, yi, xf-2, yf);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi-2, yi);
                        glVertex2i(xi+2, yi);
                        glVertex2i(xf-2, yf);
                        glVertex2i(xf+2, yf);

                        glVertex2i(xi-2, yi-1);
                        glVertex2i(xi+2, yi+1);
                        glVertex2i(xf-2, yf-1);
                        glVertex2i(xf+2, yf+1);

                        glEnd();
                        glFlush();

                    }
                }
            }
            if(algo==2)
            {
                simpleDDA(xi, yi, xf, yf);
                if(thickness>=3)
                {
                    if(abs(xf-xi)>abs(yf-yi))
                    {
                        simpleDDA(xi, yi+1, xf, yf+1);
                        simpleDDA(xi, yi-1, xf, yf-1);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi, yi+1);
                        glVertex2i(xi, yi-1);
                        glVertex2i(xf, yf+1);
                        glVertex2i(xf, yf-1);
                        glEnd();
                        glFlush();
                    }
                    else
                    {
                        simpleDDA(xi+1, yi, xf+1, yf);
                        simpleDDA(xi-1, yi, xf-1, yf);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi-1, yi);
                        glVertex2i(xi+1, yi);
                        glVertex2i(xf-1, yf);
                        glVertex2i(xf+1, yf);
                        glEnd();
                        glFlush();

                    }
                }
                if(thickness==5)
                {
                    if(abs(xf-xi)>abs(yf-yi))
                    {
                        simpleDDA(xi, yi+2, xf, yf+2);
                        simpleDDA(xi, yi-2, xf, yf-2);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi, yi+2);
                        glVertex2i(xi, yi-2);
                        glVertex2i(xf, yf+2);
                        glVertex2i(xf, yf-2);

                        glVertex2i(xi+1, yi+2);
                        glVertex2i(xi-1, yi-2);
                        glVertex2i(xf+1, yf+2);
                        glVertex2i(xf-1, yf-2);
                        glEnd();
                        glFlush();

                    }
                    else
                    {
                        simpleDDA(xi+2, yi, xf+2, yf);
                        simpleDDA(xi-2, yi, xf-2, yf);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi-2, yi);
                        glVertex2i(xi+2, yi);
                        glVertex2i(xf-2, yf);
                        glVertex2i(xf+2, yf);

                        glVertex2i(xi-2, yi-1);
                        glVertex2i(xi+2, yi+1);
                        glVertex2i(xf-2, yf-1);
                        glVertex2i(xf+2, yf+1);

                        glEnd();
                        glFlush();

                    }
                }
            }
			if(algo==3)
            {
                bresenhamAlg(xi, yi, xf, yf);
                if(thickness>=3)
                {
                    if(abs(xf-xi)>abs(yf-yi))
                    {
                        //symDDA(xi, yi+1, xf, yf+1);
                        //symDDA(xi, yi-1, xf, yf-1);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi, yi+1);
                        glVertex2i(xi, yi-1);
                        glVertex2i(xf, yf+1);
                        glVertex2i(xf, yf-1);
                        glEnd();
                        glFlush();
                    }
                    else
                    {
                        //symDDA(xi+1, yi, xf+1, yf);
                        //symDDA(xi-1, yi, xf-1, yf);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi-1, yi);
                        glVertex2i(xi+1, yi);
                        glVertex2i(xf-1, yf);
                        glVertex2i(xf+1, yf);
                        glEnd();
                        glFlush();

                    }
                }
                if(thickness==5)
                {
                    if(abs(xf-xi)>abs(yf-yi))
                    {
                        //symDDA(xi, yi+2, xf, yf+2);
                        //symDDA(xi, yi-2, xf, yf-2);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi, yi+2);
                        glVertex2i(xi, yi-2);
                        glVertex2i(xf, yf+2);
                        glVertex2i(xf, yf-2);

                        glVertex2i(xi+1, yi+2);
                        glVertex2i(xi-1, yi-2);
                        glVertex2i(xf+1, yf+2);
                        glVertex2i(xf-1, yf-2);
                        glEnd();
                        glFlush();

                    }
                    else
                    {
                        //symDDA(xi+2, yi, xf+2, yf);
                        //symDDA(xi-2, yi, xf-2, yf);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi-2, yi);
                        glVertex2i(xi+2, yi);
                        glVertex2i(xf-2, yf);
                        glVertex2i(xf+2, yf);

                        glVertex2i(xi-2, yi-1);
                        glVertex2i(xi+2, yi+1);
                        glVertex2i(xf-2, yf-1);
                        glVertex2i(xf+2, yf+1);

                        glEnd();
                        glFlush();

                    }
                }
            }
            if(algo==4)
            {
                midPoint(xi, yi, xf, yf);
                if(thickness>=3)
                {
                    if(abs(xf-xi)>abs(yf-yi))
                    {
                        midPoint(xi, yi+1, xf, yf+1);
                        midPoint(xi, yi-1, xf, yf-1);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi, yi+1);
                        glVertex2i(xi, yi-1);
                        glVertex2i(xf, yf+1);
                        glVertex2i(xf, yf-1);
                        glEnd();
                        glFlush();
                    }
                    else
                    {
                        midPoint(xi+1, yi, xf+1, yf);
                        midPoint(xi-1, yi, xf-1, yf);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi-1, yi);
                        glVertex2i(xi+1, yi);
                        glVertex2i(xf-1, yf);
                        glVertex2i(xf+1, yf);
                        glEnd();
                        glFlush();

                    }
                }
                if(thickness==5)
                {
                    if(abs(xf-xi)>abs(yf-yi))
                    {
                        midPoint(xi, yi+2, xf, yf+2);
                        midPoint(xi, yi-2, xf, yf-2);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi, yi+2);
                        glVertex2i(xi, yi-2);
                        glVertex2i(xf, yf+2);
                        glVertex2i(xf, yf-2);

                        glVertex2i(xi+1, yi+2);
                        glVertex2i(xi-1, yi-2);
                        glVertex2i(xf+1, yf+2);
                        glVertex2i(xf-1, yf-2);
                        glEnd();
                        glFlush();

                    }
                    else
                    {
                        midPoint(xi+2, yi, xf+2, yf);
                        midPoint(xi-2, yi, xf-2, yf);

                        glColor3f(1, 1, 1);
                        glBegin(GL_POINTS);
                        glVertex2i(xi-2, yi);
                        glVertex2i(xi+2, yi);
                        glVertex2i(xf-2, yf);
                        glVertex2i(xf+2, yf);

                        glVertex2i(xi-2, yi-1);
                        glVertex2i(xi+2, yi+1);
                        glVertex2i(xf-2, yf-1);
                        glVertex2i(xf+2, yf+1);

                        glEnd();
                        glFlush();

                    }
                }
            }
            if(algo==5)
            {

                int r=sqrt(pow(xf-xi,2)+pow(yf-yi,2));
                f.x1=xi;
                f.y1=yi;

                pivotx=xi;
                pivoty=yi;


                if(thickness==3)
                {
                    glPointSize(3.0f);
                    circlebres(xi,yi,r);
                    glPointSize(1.0f);
                }
                else if(thickness==5)
                {
                    glPointSize(5.0f);
                    circlebres(xi,yi,r);
                    glPointSize(1.0f);
                }
                else
                    circlebres(xi,yi,r);
            }
            if(algo==6)
            {
               // xi=abs(ww/2-xi);
                //yf=abs(wh/2-yf);


                if(thickness==3)
                {
                    glPointSize(3.0f);
                    ellipsealgo(abs(xf-xi)/2,abs(yf-yi)/2,(xf+xi)/2,(yf+yi)/2);
                    glPointSize(1.0f);
                }
                else if(thickness==5)
                {
                    glPointSize(5.0f);
                    ellipsealgo(abs(xf-xi)/2,abs(yf-yi)/2,(xf+xi)/2,(yf+yi)/2);
                    glPointSize(1.0f);
                }
                else
                    ellipsealgo(abs(xf-xi)/2,abs(yf-yi)/2,(xf+xi)/2,(yf+yi)/2);

            }

            glFlush();

			//break;
}

void myinit()
{
    glFlush();
    glutSwapBuffers();
	glViewport(0, 0, ww, wh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)ww, 0.0, (GLdouble)wh);
	glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE| GLUT_RGB);
	glutInitWindowSize(ww, wh);
	glutCreateWindow("Shape's Drawing Algorithm");
	myinit();
	glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
