#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glslprogram.h"

const char* WINDOWTITLE = { "OpenGL / GLUT Sample -- Joe Graphics" };
const char* GLUITITLE = { "User Interface Window" };
const int GLUITRUE = { true };
const int GLUIFALSE = { false };
#define ESCAPE		0x1b
const int INIT_WINDOW_SIZE = { 600 };
const float BOXSIZE = { 2.f };
const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };
const float MINSCALE = { 0.05f };
const int SCROLL_WHEEL_UP = { 3 };
const int SCROLL_WHEEL_DOWN = { 4 };
const float SCROLL_WHEEL_CLICK_FACTOR = { 5. };
const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };
enum Projections
{
	ORTHO,
	PERSP
};
enum ButtonVals
{
	RESET,
	QUIT
};
const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };
const GLfloat AXES_WIDTH = { 3. };
enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};
char* ColorNames[] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};
const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};
// fog parameters:
const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE = { GL_LINEAR };
const GLfloat FOGDENSITY = { 0.30f };
const GLfloat FOGSTART = { 1.5 };
const GLfloat FOGEND = { 4. };
// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
GLuint	BoxList;				// object display list
GLuint  SpiralList;				// object display list for the spiral.
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

bool	Frozen = true;

int		isInside;				// Is inside view?
int		meshType;				// Type of mesh!
unsigned char* EARTHTEX;
int tHeight = 512;
int tWidth = 1024;
float sANIM = 0.0f;
float tANIM = 0.0f;
bool	Distort;		// global -- true means to distort the texture

//static GLuint tex_id;


GLSLProgram* Pattern;
float Time;
#define MS_IN_THE_ANIMATION_CYCLE	10000
bool uFRAG = true;
bool uVERT = true;
bool points = true;
bool lines = true;
float A, B, C, D;


// function prototypes:

void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoViewMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoShadowMenu();
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);
void	MjbSphere(float radius, int slices, int stacks);
int     LoadObjFile(char*);

void			Axes(float);
unsigned char*  BmpToTexture(char*, int*, int*);
void			HsvRgb(float[3], float[3]);
int				ReadInt(FILE*);
short			ReadShort(FILE*);

void			Cross(float[3], float[3], float[3]);
float			Dot(float[3], float[3]);
float			Unit(float[3], float[3]);

char* ReadRestOfLine(FILE*);
void	ReadObjVTN(char*, int*, int*, int*);
float	Unit(float[3]);

GLfloat light0Pos[] = { 0.0, 0.0, 0.0 };
float* Array3(float x, float y, float z) {
	static float array[4];
	array[0] = x;
	array[1] = y;
	array[2] = z;
	array[3] = 1.0;
	return array;
}
float* MulArray3(float fac, float array0[3]) {
	static float array[4];
	array[0] = fac * array0[0];
	array[1] = fac * array0[1];
	array[2] = fac * array0[2];
	array[3] = 1.0;
	return array;
}

void setmat(float r, float g, float b, float shininenss) {
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0.0, 0.0, 0.0));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(0.84f, Array3(1, 1, 1)));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1.0f, Array3(1, 1, 1)));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0.0, 0.0, 0.0));
	glMaterialf(GL_BACK, GL_SHININESS, 2.0f);

	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0.0, 0.0, 0.0));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(0.8f, Array3(1, 1, 1)));
	glMaterialf(GL_FRONT, GL_SHININESS, shininenss);
}
void setmat2(float r, float g, float b, float shininenss) {
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(r, g, b));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(1.0f, Array3(1, 1, 1)));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1.0f, Array3(1, 1, 1)));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0.0, 0.0, 0.0));
	glMaterialf(GL_BACK, GL_SHININESS, 2.0f);

	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(0.8f, Array3(1, 1, 1)));
	glMaterialf(GL_FRONT, GL_SHININESS, shininenss);
}
void setlit(int id, float x, float y, float z, float r, float g, float b) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glDisable(GL_LIGHTING);
	glColor3f(r, g, b);
	glutWireCube(0.25);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	glLightfv(id, GL_POSITION, Array3(x, y, z));
	glLightfv(id, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(id, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(id, GL_SPECULAR, Array3(r, g, b));
	glLightf(id, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(id, GL_LINEAR_ATTENUATION, 0.);
	glLightf(id, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(id);
}

///////////////////////////////////////////////////////////
// Planetary Sim Structs and functions
///////////////////////////////////////////////////////////

#define WORLD_SCALE 4
double SHIP_SPEED = 0.25;
const int numPlanets = 10;
struct Planet* planets[16];
double DT = 0.125;
double dist;
const double G = 9.8f;
const double AU = 1.496e11/9000;

typedef struct m_float4 {
	float x;
	float y;
	float z;
	float w;
} float4;
// The planets (and also the sun)
struct Planet
{
	float4 pos;
	float4 vel;
	// Display list
	GLuint d_list;
	// Physics
	float  mass;
	float  radius;
	bool   use_lighting;
	struct Planet* parent;
};
struct Ship
{
	float4 pos;
	float4 cpos;
	float4 ang;
	float4 vel;
	// Display list
	GLuint d_list;
	// Physics
	float  mass;
	float  radius;
	bool   use_lighting;
	struct Planet* parent;
};
// Lem, the black hole.
struct Planet* Lem;
// Titania 4 System.
struct Planet* Titania4;
struct Planet* Titania4a;
struct Planet* Titania4aC;
struct Planet* Titania4b;
struct Planet* Titania4c;
struct Planet* Titania4cA;
struct Planet* Titania4cB;
struct Planet* Titania4d;
struct Planet* Titania4dA;
struct Planet* Titania4e;
struct Planet* Titania4f;
GLuint ringsList;
GLuint skybox;
// The Pillar of Winter.
struct Ship* PillarOfWinter;
// Textures
static GLuint tex_id[16];
unsigned char* textures[16];
// functions
struct Planet* init_planet(float x, float y, float z, float vx, float vy, float vz, bool lighted, float mass, float radius,struct Planet* parent)
{
	struct Planet* p = (Planet*)malloc(sizeof(struct Planet));
	if(p != NULL)
	{	
		// Assign the values of the planet struct
		p->pos.x = x;
		p->pos.y = y;
		p->pos.z = z;
		p->vel.x = vx * DT;
		p->vel.y = 0.;
		p->vel.z = 0.;
		p->use_lighting = lighted;
		p->mass = mass;
		p->radius = radius;
		p->parent = parent;
	}
	return p;
}
struct Ship* init_ship(float x, float y, float z, float vx, float vy, float vz, bool lighted, float mass, float radius, struct Planet* parent)
{
	struct Ship* sh = (Ship*)malloc(sizeof(struct Ship));
	if (sh != NULL)
	{
		// Assign the values of the planet struct
		sh->pos.x = sh->cpos.x = x;
		sh->pos.y = sh->cpos.y = y;
		sh->pos.z = sh->cpos.z = z;
		sh->ang.x = vx;
		sh->ang.y = vy;
		sh->ang.z = vz;
		sh->vel.x = sh->vel.y = sh->vel.z = 0.0;
		sh->use_lighting = lighted;
		sh->mass = mass;
		sh->radius = radius;
		sh->parent = parent;
	}
	return sh;
}
void grav_planets()
{
	// FIX THIS IT'S BROKEN!!
	// 2 BODY ORBIT GRAVITY JUST WONT WORK FOR WHATEVER REASON!
	int c, d;
	struct Planet *p1, *p2;
	for (c = 0; c < numPlanets; c++)
	{
		p1 = planets[c];
		// Static planet rotation
		p1->vel.z += p1->vel.x * DT;
		if (p1->vel.z > 360.0)
		{
			p1->vel.z -= 360.0;
		}
	}
}
void ship_move()
{
	struct Ship* sh = PillarOfWinter;
	// Advanced Ship Movement! Not really, we can only move on x and z axis right now.
	float angle = sh->ang.y * 0.0174533;
	float4 vec;
	vec.x = sh->vel.x * DT * 0.01;
	vec.z = sh->vel.z * DT * 0.01;
	sh->pos.x += vec.x;
	sh->pos.z += vec.z;
}

///////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////

#define NUMCURVES 8
struct Point
{
	float x0, y0, z0;       // initial coordinates
	float x, y, z;        // animated coordinates
};
struct Curve
{
	float r, g, b;
	Point* points;
	int count;
};
/*
	Curve Curves[NUMCURVES];		// if you are creating a pattern of curves
	struct Point  p0,  p1,  p2,  p3;
	struct Point  p4,  p5,  p6,  p7;
	struct Point  p8,  p9, p10, p11;
	struct Point p12, p13, p14, p15;
	struct Point p16, p17, p18, p19;
	struct Point p20, p21, p22, p23;
*/
void RotateX(Point* p, float deg, float xc, float yc, float zc)
{
	float rad = deg * (M_PI / 180.f);         // radians
	float x = p->x0 - xc;
	float y = p->y0 - yc;
	float z = p->z0 - zc;

	float xp = x;
	float yp = y * cos(rad) - z * sin(rad);
	float zp = y * sin(rad) + z * cos(rad);

	p->x = xp + xc;
	p->y = yp + yc;
	p->z = zp + zc;
}
void RotateY(Point* p, float deg, float xc, float yc, float zc)
{
	float rad = deg * (M_PI / 180.f);         // radians
	float x = p->x0 - xc;
	float y = p->y0 - yc;
	float z = p->z0 - zc;

	float xp = x * cos(rad) + z * sin(rad);
	float yp = y;
	float zp = -x * sin(rad) + z * cos(rad);

	p->x = xp + xc;
	p->y = yp + yc;
	p->z = zp + zc;
}
void RotateZ(Point* p, float deg, float xc, float yc, float zc)
{
	float rad = deg * (M_PI / 180.f);         // radians
	float x = p->x0 - xc;
	float y = p->y0 - yc;
	float z = p->z0 - zc;

	float xp = x * cos(rad) - y * sin(rad);
	float yp = x * sin(rad) + y * cos(rad);
	float zp = z;

	p->x = xp + xc;
	p->y = yp + yc;
	p->z = zp + zc;
}
Point CatmulRom(Point* p0, Point* p1, Point* p2, Point* p3, float t)
{
	float t2 = t * t;
	float t3 = t * t * t;
	if (t > 1 || t < 0) 
	{
		Point pf;
		pf.x0 = 0.0;
		pf.y0 = 0.0;
		pf.z0 = 0.0;
		printf("Curve out of bounds!");
		return pf;
	}
	Point pf;
	pf.x0 = 0.5 * (p0->x * (-t3 + 2 * t2 - t) + p1->x * (3 * t3 - 5 * t2 + 2) + p2->x * (-3 * t3 + 4 * t2 + t) + p3->x * (t3 - t2));
	pf.y0 = 0.5 * (p0->y * (-t3 + 2 * t2 - t) + p1->y * (3 * t3 - 5 * t2 + 2) + p2->y * (-3 * t3 + 4 * t2 + t) + p3->y * (t3 - t2));
	pf.z0 = 0.5 * (p0->z * (-t3 + 2 * t2 - t) + p1->z * (3 * t3 - 5 * t2 + 2) + p2->z * (-3 * t3 + 4 * t2 + t) + p3->z * (t3 - t2));
	
	pf.x = pf.x0;
	pf.y = pf.y0;
	pf.z = pf.z0;
	return pf;
}
void glColorCode(int c)
{
	glColor3f(Colors[c][0], Colors[c][1], Colors[c][2]);
}
void plotPoint(Point p) 
{
	glVertex3f(p.x, p.y, p.z);
}
int
main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	InitGraphics();
	InitLists();
	Reset();
	InitMenus();
	glutSetWindow(MainWindow);
	glutMainLoop();
	return 0;
}
void
Animate()
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:
	int ms = glutGet(GLUT_ELAPSED_TIME);	// milliseconds
	ms %= MS_IN_THE_ANIMATION_CYCLE;
	
	Time = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;

	// [ 0., 1. )
	// force a call to Display( ) next time it is convenient:
	//p1.y += -(Time / 4) * 0.002;
	//p1.x += -(Time / 2) * 0.002;
	if (Time == 0.)
	{
		//p1.y = p1.y0;
		//p1.x = p1.x0;
		//printf("resetti spaghetti!\n");
	}
	grav_planets();
	ship_move();
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
InitLists()
{
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow(MainWindow);
	
	int list_size = 1024 * 1024;
	SpiralList = glGenLists(list_size);
	glNewList(SpiralList, GL_COMPILE);
	MjbSphere(1., 16, 16);
	glEndList();

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();
	
	/////////////////////////////////////////////////////////////////
	// Planets and ship init
	/////////////////////////////////////////////////////////////////
	// Sun
	Titania4 = init_planet(
		0.,
		0.,
		0.,
		0., // orbital period
		0., // rotation speed
		0., // angle around sun: dont touch
		false,
		1.989e30,
		69634000 / AU / 5 * WORLD_SCALE,
		NULL
	);
	// Venus
	Titania4aC = init_planet(
		1.5 * WORLD_SCALE + Titania4->radius,
		0.,
		0.,
		0.09, // orbital period
		0., // rotation speed
		0., // angle around sun: dont touch
		true,
		6.39e23,
		3389500 / AU / 5 * WORLD_SCALE,
		Titania4
	);
	// Mars
	Titania4b = init_planet(
		0.72 * WORLD_SCALE + Titania4->radius,
		0.,
		0.,
		0.16, // orbital period
		0., // rotation speed
		0., // angle around sun: dont touch
		true,
		4.867e24,
		6051800 / AU / 5 * WORLD_SCALE,
		Titania4a
	);

	// Jupiter
	Titania4a = init_planet(
		5.2 * WORLD_SCALE + Titania4->radius,
		0.,
		0.,
		0.008, // orbital period
		0., // rotation speed
		0., // angle around sun: dont touch
		true,
		1.898e27,
		69911000/AU / 5 * WORLD_SCALE,
		Titania4
	);
	// Saturn
	Titania4c = init_planet(
		9.5 * WORLD_SCALE + Titania4->radius,
		0.,
		0.,
		0.005, // orbital period
		0., // rotation speed
		0., // angle around sun: dont touch
		true,
		1.586e21,
		58232000/AU / 5 * WORLD_SCALE,
		Titania4
	);
	// DIONE
	Titania4cA = init_planet(
		0.00252273644 * WORLD_SCALE + Titania4c->radius,
		0.,
		0.,
		0.1 / (2.736915 / 365), // Earth year length / (orbit period in days / 1 earth year)
		0.,
		0.,
		true,
		0.,
		1122800 / AU / 5 * WORLD_SCALE,
		Titania4c
	);
	// RHEA
	Titania4cB = init_planet(
		0.00352349935 * WORLD_SCALE + Titania4c->radius,
		0.,
		0.,
		0.1 / (4.518212 / 365), // Earth year length / (orbit period in days / 1 earth year)
		0.,
		0.,
		true,
		0.,
		763800 / AU / 5 * WORLD_SCALE,
		Titania4c
	);

	// EARTH as NEPTUNE
	Titania4d = init_planet(
		30 * WORLD_SCALE + Titania4->radius,
		0.,
		0.,
		0.0018, // orbital period
		0., // rotation speed
		0., // angle around sun: dont touch
		true,
		1.586e21,
		6371000 / AU / 5 * WORLD_SCALE,
		Titania4
	);
	// THE MOON as EARTH's best friend, that followed it all the way out to where NEPTUNE was!
	Titania4dA = init_planet(
		0.002569 * WORLD_SCALE + Titania4d->radius, // close
		0.,
		0.,
		0.1 / (27.321661 / 365), // Earth year length / (orbit period in days / 1 earth year)
		0., // rotation speed
		0., // angle around sun: dont touch
		true,
		1.586e21,
		1738100 / AU / 5 * WORLD_SCALE,
		Titania4d
	);
	// PLUTO
	Titania4e = init_planet(
		49.305 * WORLD_SCALE + Titania4->radius, // Far!
		0.,
		0.,
		0.001, // orbital period
		0., // rotation speed
		0., // angle around sun: dont touch
		true,
		1.303e22,// Mass is no longer needed
		1188300 / AU / 5 * WORLD_SCALE,
		Titania4
	);


	// Ship starts where earth is.
	PillarOfWinter = init_ship(
		1.1 * WORLD_SCALE + Titania4->radius,
		0.,
		0.,
		0.1,
		0.,
		0.,
		true,
		52000,
		0.0,
		Titania4aC
	);

	/////////////////////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////////////////////
	// SUN
	Titania4->d_list = glGenLists(list_size);
	glNewList(Titania4->d_list, GL_COMPILE);
	setmat2(1., 1., 1., 15.);
	MjbSphere(Titania4->radius, 32, 32);
	glEndList();

	// VENUS
	Titania4b->d_list = glGenLists(list_size);
	glNewList(Titania4b->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	MjbSphere(Titania4b->radius, 32, 32);
	glEndList();

	// MARS
	Titania4aC->d_list = glGenLists(list_size);
	glNewList(Titania4aC->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	MjbSphere(Titania4aC->radius, 32, 32);
	glEndList();

	// JUPITER
	Titania4a->d_list = glGenLists(list_size);
	glNewList(Titania4a->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	MjbSphere(Titania4a->radius, 32, 32);
	glEndList();

	// SATURN
	Titania4c->d_list = glGenLists(list_size);
	glNewList(Titania4c->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	MjbSphere(Titania4c->radius, 32, 32);
	glEndList();
	// SATURN RINGS need to be a seperate list because different texture!
	ringsList = glGenLists(list_size);
	glNewList(ringsList, GL_COMPILE);
	setmat(1., 1., 1., 0.);
	LoadObjFile("obj/rings.obj");
	glEndList();
	// DIONE
	Titania4cA->d_list = glGenLists(list_size);
	glNewList(Titania4cA->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	MjbSphere(Titania4cA->radius, 32, 32);
	glEndList();
	// RHEA
	Titania4cB->d_list = glGenLists(list_size);
	glNewList(Titania4cB->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	MjbSphere(Titania4cB->radius, 32, 32);
	glEndList();

	// EARTH as NEPTUNE
	Titania4d->d_list = glGenLists(list_size);
	glNewList(Titania4d->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	MjbSphere(Titania4d->radius, 32, 32);
	glEndList();

	// MOON!
	Titania4dA->d_list = glGenLists(list_size);
	glNewList(Titania4dA->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	MjbSphere(Titania4dA->radius, 32, 32);
	glEndList();



	// SKYBOX
	skybox = glGenLists(list_size);
	glNewList(skybox,GL_COMPILE);
	setmat2(1., 1., 1., 0.);
	LoadObjFile("obj/box.obj");
	glEndList();

	// SHIP
	PillarOfWinter->d_list = glGenLists(list_size);
	glNewList(PillarOfWinter->d_list, GL_COMPILE);
	setmat(1., 1., 1., 15.);
	LoadObjFile("obj/spcShuttle.obj");
	glEndList();

	// Put planets in the list of planets
	// SUN
	planets[0] = Titania4;
	// VENUS
	planets[1] = Titania4aC;
	// MARS
	planets[2] = Titania4b;
	// JUPITER
	planets[3] = Titania4a;
	// SATURN
	planets[4] = Titania4c;
	// EARTH as NEPTUNE
	planets[5] = Titania4d;
	// THE MOON
	planets[6] = Titania4dA;
	// PLUTO
	planets[7] = Titania4e;
	// DIONE
	planets[8] = Titania4cA;
	// RHEA
	planets[9] = Titania4cB;

	/////////////////////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////////////////////
}


// draw the complete scene:

void
Display()
{
	if (DebugOn != 0)
	{
		fprintf(stderr, "Display\n");
	}
	glutSetWindow(MainWindow);
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
	if (DepthBufferOn == 0)
		glDisable(GL_DEPTH_TEST);
#endif
	glShadeModel(GL_SMOOTH);
	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-3., 3., -3., 3., 0.1, 1000.);
	else
		gluPerspective(90., 1., 0.1, 1000.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	gluLookAt(
		0.,
		0.5,
		1.,
		0.,
		0.5,
		0.,
		0.,
		1.,
		0.
	);
	// Holy crap, this was super finicky to get right.
	/*
		In order to get the ship to be locked at 0,0,0 relative to the eye, but
		still allow the player to look around, we had to do the rotate, then the
		transform of the entire world, as the negative of the ship position, then
		draw the ship, and move it back to where the camera was supposed to be.
		We also had to disable the ability to zoom in and out, as that wrecked havoc
		with the ship positioning, oftentimes resulting in the ship scaling wierdly
		or it's position relative to the world changing with the scaling.
	*/
	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);
	glTranslatef(-PillarOfWinter->pos.x, -PillarOfWinter->pos.y, -PillarOfWinter->pos.z);
	glPushMatrix();
		/*
			Pillar Of Winter
			Your space ship
		 */
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		// Position, rotate, and scale the ship.
		glTranslatef(PillarOfWinter->pos.x, 0., PillarOfWinter->pos.z);
		glRotatef(PillarOfWinter->ang.y, 0., 1., 0.);
		glScalef(0.05, 0.05, 0.05);
		// Get distance from sun.
		float x = (PillarOfWinter->pos.x + Titania4->pos.x) * (PillarOfWinter->pos.x + Titania4->pos.x);
		float y = (PillarOfWinter->pos.y + Titania4->pos.y) * (PillarOfWinter->pos.y + Titania4->pos.y);
		float z = (PillarOfWinter->pos.z + Titania4->pos.z) * (PillarOfWinter->pos.z + Titania4->pos.z);
		dist = sqrt(x + y + z);
		// Player crash into sun.
		if (dist <= Titania4->radius + PillarOfWinter->radius - 0.1)
		{
			printf("\nYOU CRASHED INTO THE SUN!\nWHAT WERE YOU THINKING???\nDO YOU KNOW HOW MUCH THAT SPACE SHUTTLE COST???\nTWO HUNDRED AND NINE BILLION DOLLARS!!!\nAAAHHHHHH!\n");
			DoMainMenu(QUIT);
		}
		// Disabled scaling
		//if (Scale < MINSCALE)
		//	Scale = MINSCALE;
		//glScalef((GLfloat)Scale * 0.05, (GLfloat)Scale * 0.05, (GLfloat)Scale * 0.05);
		// Then bind the textures, and finally call the list for the ship.
		glBindTexture(GL_TEXTURE_2D, tex_id[5]);
		glCallList(PillarOfWinter->d_list);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	//if (Scale < MINSCALE)
	//	Scale = MINSCALE;
	//glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}
	if (AxesOn != 1)
	{
		glColor3fv(&Colors[WhichColor][0]);
		glCallList(AxesList);
	}
	glEnable(GL_NORMALIZE);

	/////////////////////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////////////////////
	// The light is set at whatever position the sun is at.
	setlit(GL_LIGHT0,
		light0Pos[0],
		light0Pos[1],
		light0Pos[2],
		1, 1, 1
	);
	light0Pos[0] = Titania4->pos.x;
	light0Pos[1] = Titania4->pos.y;
	light0Pos[2] = Titania4->pos.z;
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	// The Skybox that I didn't totally take from a mod of KSP.
	glPushMatrix();
	double skyscale = 15.;
	glScalef(skyscale, skyscale, skyscale);
	glBindTexture(GL_TEXTURE_2D, tex_id[15]);
	glCallList(skybox);
	glPopMatrix();
	/*
		Our planetary system
		An analog for our own solar system, if a bit different in some regards.
		The Titania star system is a quaterniary star system, with 3 stars orbiting
		a black hole. The closest star, Titania 2 is nearing the end of it's life as
		it is slowly swallowed up by Titania 1. Titania 3 and 4 orbit much further out,
		however Titania 3 doesn't have any suitable planets, and is not of interest in this
		mission.
	 */
	/*
		Titania 4 SUN
		On of the stars in this quaterniary system. As the furthest out it is
		labeled as 4. It orbits furthest from the Lem Black Hole.
	 */
	glPushMatrix();

		glBindTexture(GL_TEXTURE_2D, tex_id[0]);
		glCallList(Titania4->d_list);

	glPopMatrix();

	/*
		Titania 4a(c) VENUS
		The last of 3 moons left of Titania 4a. The other two crashed
		together, and shattered into the rings of Titania 4a. During this
		event, Titania 4a(C) was forcibly moved elsewhere. It is unknown
		how this happened. A prospect for human colonization.
	*/
	glPushMatrix();

		glRotatef(Titania4aC->vel.z, 0., 1., 0.);
		glTranslatef(Titania4aC->pos.x, 0., 0.);
		glBindTexture(GL_TEXTURE_2D, tex_id[3]);
		glCallList(Titania4aC->d_list);

	glPopMatrix();


	/*
		Titania 4b MARS
		This planet was moved at some point, as indicated by geological
		records, showing that it was at one point much colder than it is now.
		Was formerly covered in huge ice sheets. Is currently geologically active
		and a prospect for human colonization.
	*/
	glPushMatrix();

		glRotatef(Titania4b->vel.z, 0., 1., 0.);
		glTranslatef(Titania4b->pos.x, 0., 0.);
		glBindTexture(GL_TEXTURE_2D, tex_id[2]);
		glCallList(Titania4b->d_list);

	glPopMatrix();
	/*
		Titania 4a JUPITER
		The resident ice giant of the Titania 4 system. Has since lost all of
		its moons. The first two crashed into the planet, and the other was moved.
		It is unknown what exactly caused the first two to crash.
		Its rings are all that remains of Titania 4a(a) and 4a(b).
	*/
	glPushMatrix();

		glRotatef(Titania4a->vel.z, 0., 1., 0.);
		glTranslatef(Titania4a->pos.x, 0., 0.);
		glBindTexture(GL_TEXTURE_2D, tex_id[1]);
		glCallList(Titania4a->d_list);

	glPopMatrix();

	/*
		Titania 4c SATURN
		Another Ice giant, but this one still has moons.
	 */
	glPushMatrix();

		glRotatef(Titania4c->vel.z, 0., 1., 0.);
		glTranslatef(Titania4c->pos.x, 0., 0.);
		glBindTexture(GL_TEXTURE_2D, tex_id[4]);
		glCallList(Titania4c->d_list);
		double scf = 3.;
		glScalef(scf, scf, scf);
		glRotatef(5., 0., 0., 1.);
		glBindTexture(GL_TEXTURE_2D, tex_id[6]);
		glCallList(ringsList);
		/*
			Titania 4c(a) DIONE
		 */
		glPushMatrix();

			glRotatef(Titania4cA->vel.z, 0., 1., 0.);
			glTranslatef(Titania4cA->pos.x, 0., 0.);
			glBindTexture(GL_TEXTURE_2D, tex_id[8]);
			glCallList(Titania4cA->d_list);

		glPopMatrix();
		/*
			Titania 4c(b) RHEA
		 */
		glPushMatrix();

			glRotatef(Titania4cB->vel.z, 0., 1., 0.);
			glTranslatef(Titania4cB->pos.x, 0., 0.);
			glBindTexture(GL_TEXTURE_2D, tex_id[8]);
			glCallList(Titania4cB->d_list);

		glPopMatrix();

	glPopMatrix();
	/*
		Titania 4d EARTH
	 */
	glPushMatrix();
		
		glRotatef(Titania4d->vel.z, 0., 1., 0.);
		glTranslatef(Titania4d->pos.x, 0., 0.);
		glBindTexture(GL_TEXTURE_2D, tex_id[7]);
		glCallList(Titania4d->d_list);
		/*
			Titania 4d(a) MOON
		 */
		glPushMatrix();

			glRotatef(Titania4dA->vel.z, 0., 1., 0.);
			glTranslatef(Titania4dA->pos.x*4, 0., 0.);
			glBindTexture(GL_TEXTURE_2D, tex_id[8]);
			glCallList(Titania4dA->d_list);

		glPopMatrix();

	glPopMatrix();
	/*
		Titania 4e PLUTO SAD! CANNOT DO PLUTO! TIS TOO FAR AWAY, AND OUTSIDE OF SKYBOX!
	 */
	
	// DRAW THE HUD
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	char* shuttleHUD = (char*)malloc(sizeof(char) * 128);
	sprintf(shuttleHUD, "Heading %2.2f Vel(%2.2f,%2.2f)\n",PillarOfWinter->ang.y,PillarOfWinter->vel.x,PillarOfWinter->vel.z);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0., 100., 0., 100.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1., 1., 1.);
	DoRasterString(1., 1., 0., shuttleHUD);
	free(shuttleHUD);
	/////////////////////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////////////////////
	
	#ifdef DEMO_Z_FIGHTING
		if (DepthFightingOn != 0)
		{
			glPushMatrix();
			glRotatef(90., 0., 1., 0.);
			glCallList(BoxList);
			glPopMatrix();
		}
	#endif
	glutSwapBuffers();
	glFlush();
}




// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 32);
	glutInitWindowSize(INIT_WINDOW_SIZE*1.5, INIT_WINDOW_SIZE);
	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);
	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);


	if (Frozen) {
		glutIdleFunc(NULL);
	}
	else {
		glutIdleFunc(Animate);
	}
	
	/////////////////////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////////////////////
	
	int w = 2048;
	int h = 1024;
	
	textures[0] = BmpToTexture("bmp/Sun.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &tex_id[0]);
	glBindTexture(GL_TEXTURE_2D, tex_id[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		w, 
		h, 
		0, 
		GL_RGB, 
		GL_UNSIGNED_BYTE, 
		textures[0]
	);
	textures[1] = BmpToTexture("bmp/Titania 4a.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &tex_id[1]);
	glBindTexture(GL_TEXTURE_2D, tex_id[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		w, 
		h, 
		0, 
		GL_RGB, 
		GL_UNSIGNED_BYTE, 
		textures[1]
	);
	textures[2] = BmpToTexture("bmp/Titania 4aC.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &tex_id[2]);
	glBindTexture(GL_TEXTURE_2D, tex_id[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		w, 
		h, 
		0, 
		GL_RGB, 
		GL_UNSIGNED_BYTE, 
		textures[2]
	);
	textures[3] = BmpToTexture("bmp/Titania 4b.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &tex_id[3]);
	glBindTexture(GL_TEXTURE_2D, tex_id[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		w, 
		h, 
		0, 
		GL_RGB, 
		GL_UNSIGNED_BYTE, 
		textures[3]
	);
	textures[4] = BmpToTexture("bmp/Titania 4c.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &tex_id[4]);
	glBindTexture(GL_TEXTURE_2D, tex_id[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		w, 
		h, 
		0, 
		GL_RGB, 
		GL_UNSIGNED_BYTE, 
		textures[4]
	);
	int sw = 1024;
	int sh = 1024;
	textures[5] = BmpToTexture("bmp/spcShuttle-export.bmp", &sw, &sh);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex_id[5]);
	glBindTexture(GL_TEXTURE_2D, tex_id[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		sw,
		sh,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		textures[5]
	);
	textures[6] = BmpToTexture("bmp/Titania 4c Rings.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex_id[6]);
	glBindTexture(GL_TEXTURE_2D, tex_id[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		w,
		h,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		textures[6]
	);
	textures[7] = BmpToTexture("bmp/Titania 4d.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex_id[7]);
	glBindTexture(GL_TEXTURE_2D, tex_id[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		w,
		h,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		textures[7]
	);
	textures[8] = BmpToTexture("bmp/Titania 4dA.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex_id[8]);
	glBindTexture(GL_TEXTURE_2D, tex_id[8]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		w,
		h,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		textures[8]
	);

	textures[9] = BmpToTexture("bmp/Titania 4cA.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex_id[9]);
	glBindTexture(GL_TEXTURE_2D, tex_id[9]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		w,
		h,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		textures[9]
	);

	textures[10] = BmpToTexture("bmp/Titania 4cB.bmp", &w, &h);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex_id[10]);
	glBindTexture(GL_TEXTURE_2D, tex_id[10]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		w,
		h,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		textures[10]
	);
	// SKYBOX
	int skyx = 3072;
	int skyy = 2048;
	textures[15] = BmpToTexture("bmp/skybox.bmp", &skyx, &skyy);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex_id[15]);
	glBindTexture(GL_TEXTURE_2D, tex_id[15]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		skyx,
		skyy,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		textures[15]
	);
	/////////////////////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////////////////////
	#ifdef WIN32
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			fprintf(stderr, "glewInit Error\n");
		}
		else
			fprintf(stderr, "GLEW initialized OK\n");
		fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	#endif


	// Shader assignment
	Pattern = new GLSLProgram();
	bool valid = Pattern->Create("pattern.vert", "pattern.frag");
	if (!valid)
	{
		fprintf(stderr, "Shader cannot be created!\n");
		DoMainMenu(QUIT);
	}
	else
	{
		fprintf(stderr, "Shader created.\n");
	}
	Pattern->SetVerbose(false);
}

#include "glslprogram.cpp"

void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	WhichColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoViewMenu(int id)
{
	isInside = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	WhichProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoShadowsMenu(int id)
{
	ShadowsOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}
void doMeshMenu(int id) {
	meshType = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(int));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 1);
	glutAddMenuEntry("On", 0);

	int insidemenu = glutCreateMenu(DoViewMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);

	int shadowsmenu = glutCreateMenu(DoShadowsMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int meshmenu = glutCreateMenu(doMeshMenu);
	glutAddMenuEntry("NAKED EARTH!", 0);
	glutAddMenuEntry("WITH CRUST!",   1);
	glutAddMenuEntry("TECTONIC PLATE SHIFTING!", 2);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Colors", colormenu);
	glutAddSubMenu("Meshes", meshmenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddSubMenu("Inside View", insidemenu);
	glutAddSubMenu("Projection", projmenu);

#ifdef ENABLE_SHADOWS
	glutAddSubMenu("Shadows", shadowsmenu);
#endif

	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )


void shipStat()
{
	//printf("Heading: %f, Vel(%f,%f)\n", PillarOfWinter->ang.y, PillarOfWinter->vel.x, PillarOfWinter->vel.z);
}

// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'f':
		Frozen = !Frozen;
		//printf("freezy breezy!\n");
		if (Frozen) {
			glutIdleFunc(NULL);
		}
		else {
			glutIdleFunc(Animate);
		}
		break;

	case 'b': {
		uFRAG = true;
		uVERT = true;
	} break;
	case 'l': {
		lines = !lines;
		//printf("curvy swervy!\n");
	} break;
	case 'p': {
		points = !points;
		//printf("pointy... um... pointy?\n");
	} break;

	case 'o':
	case 'O':
		WhichProjection = ORTHO;
		break;
	case 'P':
		WhichProjection = PERSP;
		break;

	//case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler
	case 'q': // turn counterclockwise
		PillarOfWinter->ang.y += SHIP_SPEED * DT * 8;
		shipStat();
		break;
	case 'e': // turn clockwise
		PillarOfWinter->ang.y -= SHIP_SPEED * DT * 8;
		shipStat();
		break;
	case 'w':
		PillarOfWinter->vel.z -= SHIP_SPEED * DT;
		shipStat();
		break;
	case 's':
		PillarOfWinter->vel.z += SHIP_SPEED * DT;
		shipStat();
		break;
	case 'a':
		PillarOfWinter->vel.x -= SHIP_SPEED * DT;
		shipStat();
		break;
	case 'd':
		PillarOfWinter->vel.x += SHIP_SPEED * DT;
		shipStat();
		break;
	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "MouseMotion: %d, %d\n", x, y);


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}


	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	if (DebugOn != 0)
		fprintf(stderr, "ReSize: %d, %d\n", width, height);

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
		0.f, 1.f, 0.f, 1.f
};

static float xy[] = {
		-.5f, .5f, .5f, -.5f
};

static int xorder[] = {
		1, 2, -3, 4
};

static float yx[] = {
		0.f, 0.f, -.5f, .5f
};

static float yy[] = {
		0.f, .6f, 1.f, 1.f
};

static int yorder[] = {
		1, 2, 3, -2, 4
};

static float zx[] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
};

static float zy[] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
};

static int zorder[] = {
		1, 2, 3, 4, -5, 6
};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}

struct bmfh
{
	short bfType;
	int bfSize;
	short bfReserved1;
	short bfReserved2;
	int bfOffBits;
} FileHeader;

struct bmih
{
	int biSize;
	int biWidth;
	int biHeight;
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
} InfoHeader;

const int birgb = { 0 };

// read a BMP file into a Texture:

unsigned char* BmpToTexture(char* filename, int* width, int* height)
{
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}

	FileHeader.bfType = ReadShort(fp);


	// if bfType is not 0x4d42, the file is not a bmp:

	if (FileHeader.bfType != 0x4d42)
	{
		fprintf(stderr, "File '%s' is the wrong type of file: 0x%0x\n", filename, FileHeader.bfType);
		fclose(fp);
		return NULL;
	}

	FileHeader.bfSize = ReadInt(fp);
	FileHeader.bfReserved1 = ReadShort(fp);
	FileHeader.bfReserved2 = ReadShort(fp);
	FileHeader.bfOffBits = ReadInt(fp);

	InfoHeader.biSize = ReadInt(fp);
	InfoHeader.biWidth = ReadInt(fp);
	InfoHeader.biHeight = ReadInt(fp);

	int nums = InfoHeader.biWidth;
	int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort(fp);
	InfoHeader.biBitCount = ReadShort(fp);
	InfoHeader.biCompression = ReadInt(fp);
	InfoHeader.biSizeImage = ReadInt(fp);
	InfoHeader.biXPelsPerMeter = ReadInt(fp);
	InfoHeader.biYPelsPerMeter = ReadInt(fp);
	InfoHeader.biClrUsed = ReadInt(fp);
	InfoHeader.biClrImportant = ReadInt(fp);

	fprintf(stderr, "Image size in file '%s' is: %d x %d\n", filename, nums, numt);

	unsigned char* texture = new unsigned char[3 * nums * numt];
	if (texture == NULL)
	{
		fprintf(stderr, "Cannot allocate the texture array!\b");
		return NULL;
	}

	// extra padding bytes:

	int numextra = 4 * (((3 * InfoHeader.biWidth) + 3) / 4) - 3 * InfoHeader.biWidth;

	// we do not support compression:

	if (InfoHeader.biCompression != birgb)
	{
		fprintf(stderr, "Image file '%s' has the wrong type of image compression: %d\n", filename, InfoHeader.biCompression);
		fclose(fp);
		return NULL;
	}

	rewind(fp);
	fseek(fp, 14 + 40, SEEK_SET);

	if (InfoHeader.biBitCount == 24)
	{
		unsigned char* tp = texture;
		for (int t = 0; t < numt; t++)
		{
			for (int s = 0; s < nums; s++, tp += 3)
			{
				*(tp + 2) = fgetc(fp);		// b
				*(tp + 1) = fgetc(fp);		// g
				*(tp + 0) = fgetc(fp);		// r
			}

			for (int e = 0; e < numextra; e++)
			{
				fgetc(fp);
			}
		}
	}

	fclose(fp);

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt(FILE* fp)
{
	unsigned char b3, b2, b1, b0;
	b0 = fgetc(fp);
	b1 = fgetc(fp);
	b2 = fgetc(fp);
	b3 = fgetc(fp);
	return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

short
ReadShort(FILE* fp)
{
	unsigned char b1, b0;
	b0 = fgetc(fp);
	b1 = fgetc(fp);
	return (b1 << 8) | b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


struct point2 {
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

int		NumLngs, NumLats;
struct point2 *	Pts;

struct point2 *
PtsPointer( int lat, int lng )
{
	if( lat < 0 )	lat += (NumLats-1);
	if( lng < 0 )	lng += (NumLngs-1);
	if( lat > NumLats-1 )	lat -= (NumLats-1);
	if( lng > NumLngs-1 )	lng -= (NumLngs-1);
	return &Pts[ NumLngs*lat + lng ];
}



void
DrawPoint( struct point2 *p )
{
	glNormal3f( p->nx, p->ny, p->nz );
	glTexCoord2f( p->s, p->t );
	glVertex3f( p->x, p->y, p->z );
}

void
MjbSphere( float radius, int slices, int stacks )
{
	struct point2 top, bot;		// top, bottom points
	struct point2 *p;

	// set the globals:

	NumLngs = slices;
	NumLats = stacks;

	if( NumLngs < 3 )
		NumLngs = 3;

	if( NumLats < 3 )
		NumLats = 3;


	// allocate the point data structure:

	Pts = new struct point2[ NumLngs * NumLats ];


	// fill the Pts structure:

	for( int ilat = 0; ilat < NumLats; ilat++ )
	{
		float lat = -M_PI/2.  +  M_PI * (float)ilat / (float)(NumLats-1);
		float xz = cos( lat );
		float y = sin( lat );
		for( int ilng = 0; ilng < NumLngs; ilng++ )
		{
			float lng = -M_PI  +  2. * M_PI * (float)ilng / (float)(NumLngs-1);
			float x =  xz * cos( lng );
			float z = -xz * sin( lng );
			p = PtsPointer( ilat, ilng );
			p->x  = radius * x;
			p->y  = radius * y;
			p->z  = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			if( Distort )
			{
				p->s = lng + 1 + M_PI*lat*sANIM;
				p->t = lat + 2 + M_PI*lng*tANIM;
			}
			else
			{
				p->s = ( lng + M_PI    ) / ( 2.*M_PI );
				p->t = ( lat + M_PI/2. ) / M_PI;
			}
		}
	}

	top.x =  0.;		top.y  = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s  = 0.;		top.t  = 1.;

	bot.x =  0.;		bot.y  = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s  = 0.;		bot.t  =  0.;


	// connect the north pole to the latitude NumLats-2:

	glBegin( GL_QUADS );
	for( int ilng = 0; ilng < NumLngs-1; ilng++ )
	{
		p = PtsPointer( NumLats-1, ilng );
		DrawPoint( p );

		p = PtsPointer( NumLats-2, ilng );
		DrawPoint( p );

		p = PtsPointer( NumLats-2, ilng+1 );
		DrawPoint( p );

		p = PtsPointer( NumLats-1, ilng+1 );
		DrawPoint( p );
	}
	glEnd( );

	// connect the south pole to the latitude 1:

	glBegin( GL_QUADS );
	for( int ilng = 0; ilng < NumLngs-1; ilng++ )
	{
		p = PtsPointer( 0, ilng );
		DrawPoint( p );

		p = PtsPointer( 0, ilng+1 );
		DrawPoint( p );

		p = PtsPointer( 1, ilng+1 );
		DrawPoint( p );

		p = PtsPointer( 1, ilng );
		DrawPoint( p );
	}
	glEnd( );


	// connect the other 4-sided polygons:

	glBegin( GL_QUADS );
	for( int ilat = 2; ilat < NumLats-1; ilat++ )
	{
		for( int ilng = 0; ilng < NumLngs-1; ilng++ )
		{
			p = PtsPointer( ilat-1, ilng );
			DrawPoint( p );

			p = PtsPointer( ilat-1, ilng+1 );
			DrawPoint( p );

			p = PtsPointer( ilat, ilng+1 );
			DrawPoint( p );

			p = PtsPointer( ilat, ilng );
			DrawPoint( p );
		}
	}
	glEnd( );

	delete [ ] Pts;
	Pts = NULL;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <GL/gl.h>

#include <vector>


// delimiters for parsing the obj file:

#define OBJDELIMS		" \t"


struct Vertex
{
	float x, y, z;
};


struct Normal
{
	float nx, ny, nz;
};


struct TextureCoord
{
	float s, t, p;
};


struct face
{
	int v, n, t;
};






int
LoadObjFile(char* name)
{
	char* cmd;		// the command string
	char* str;		// argument string

	std::vector <struct Vertex> Vertices(10000);
	std::vector <struct Normal> Normals(10000);
	std::vector <struct TextureCoord> TextureCoords(10000);

	Vertices.clear();
	Normals.clear();
	TextureCoords.clear();

	struct Vertex sv;
	struct Normal sn;
	struct TextureCoord st;


	// open the input file:

	FILE* fp = fopen(name, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open .obj file '%s'\n", name);
		return 1;
	}


	float xmin = 1.e+37f;
	float ymin = 1.e+37f;
	float zmin = 1.e+37f;
	float xmax = -xmin;
	float ymax = -ymin;
	float zmax = -zmin;

	glBegin(GL_TRIANGLES);

	for (; ; )
	{
		char* line = ReadRestOfLine(fp);
		if (line == NULL)
			break;


		// skip this line if it is a comment:

		if (line[0] == '#')
			continue;


		// skip this line if it is something we don't feel like handling today:

		if (line[0] == 'g')
			continue;

		if (line[0] == 'm')
			continue;

		if (line[0] == 's')
			continue;

		if (line[0] == 'u')
			continue;


		// get the command string:

		cmd = strtok(line, OBJDELIMS);


		// skip this line if it is empty:

		if (cmd == NULL)
			continue;


		if (strcmp(cmd, "v") == 0)
		{
			str = strtok(NULL, OBJDELIMS);
			sv.x = atof(str);

			str = strtok(NULL, OBJDELIMS);
			sv.y = atof(str);

			str = strtok(NULL, OBJDELIMS);
			sv.z = atof(str);

			Vertices.push_back(sv);

			if (sv.x < xmin)	xmin = sv.x;
			if (sv.x > xmax)	xmax = sv.x;
			if (sv.y < ymin)	ymin = sv.y;
			if (sv.y > ymax)	ymax = sv.y;
			if (sv.z < zmin)	zmin = sv.z;
			if (sv.z > zmax)	zmax = sv.z;

			continue;
		}


		if (strcmp(cmd, "vn") == 0)
		{
			str = strtok(NULL, OBJDELIMS);
			sn.nx = atof(str);

			str = strtok(NULL, OBJDELIMS);
			sn.ny = atof(str);

			str = strtok(NULL, OBJDELIMS);
			sn.nz = atof(str);

			Normals.push_back(sn);

			continue;
		}


		if (strcmp(cmd, "vt") == 0)
		{
			st.s = st.t = st.p = 0.;

			str = strtok(NULL, OBJDELIMS);
			st.s = atof(str);

			str = strtok(NULL, OBJDELIMS);
			if (str != NULL)
				st.t = atof(str);

			str = strtok(NULL, OBJDELIMS);
			if (str != NULL)
				st.p = atof(str);

			TextureCoords.push_back(st);

			continue;
		}


		if (strcmp(cmd, "f") == 0)
		{
			struct face vertices[10];
			for (int i = 0; i < 10; i++)
			{
				vertices[i].v = 0;
				vertices[i].n = 0;
				vertices[i].t = 0;
			}

			int sizev = (int)Vertices.size();
			int sizen = (int)Normals.size();
			int sizet = (int)TextureCoords.size();

			int numVertices = 0;
			bool valid = true;
			int vtx = 0;
			char* str;
			while ((str = strtok(NULL, OBJDELIMS)) != NULL)
			{
				int v, n, t;
				ReadObjVTN(str, &v, &t, &n);

				// if v, n, or t are negative, they are wrt the end of their respective list:

				if (v < 0)
					v += (sizev + 1);

				if (n < 0)
					n += (sizen + 1);

				if (t < 0)
					t += (sizet + 1);


				// be sure we are not out-of-bounds (<vector> will abort):

				if (t > sizet)
				{
					if (t != 0)
						fprintf(stderr, "Read texture coord %d, but only have %d so far\n", t, sizet);
					t = 0;
				}

				if (n > sizen)
				{
					if (n != 0)
						fprintf(stderr, "Read normal %d, but only have %d so far\n", n, sizen);
					n = 0;
				}

				if (v > sizev)
				{
					if (v != 0)
						fprintf(stderr, "Read vertex coord %d, but only have %d so far\n", v, sizev);
					v = 0;
					valid = false;
				}

				vertices[vtx].v = v;
				vertices[vtx].n = n;
				vertices[vtx].t = t;
				vtx++;

				if (vtx >= 10)
					break;

				numVertices++;
			}


			// if vertices are invalid, don't draw anything this time:

			if (!valid)
				continue;

			if (numVertices < 3)
				continue;


			// list the vertices:

			int numTriangles = numVertices - 2;

			for (int it = 0; it < numTriangles; it++)
			{
				int vv[3];
				vv[0] = 0;
				vv[1] = it + 1;
				vv[2] = it + 2;

				// get the planar normal, in case vertex normals are not defined:

				struct Vertex* v0 = &Vertices[vertices[vv[0]].v - 1];
				struct Vertex* v1 = &Vertices[vertices[vv[1]].v - 1];
				struct Vertex* v2 = &Vertices[vertices[vv[2]].v - 1];

				float v01[3], v02[3], norm[3];
				v01[0] = v1->x - v0->x;
				v01[1] = v1->y - v0->y;
				v01[2] = v1->z - v0->z;
				v02[0] = v2->x - v0->x;
				v02[1] = v2->y - v0->y;
				v02[2] = v2->z - v0->z;
				Cross(v01, v02, norm);
				Unit(norm, norm);
				glNormal3fv(norm);

				for (int vtx = 0; vtx < 3; vtx++)
				{
					if (vertices[vv[vtx]].t != 0)
					{
						struct TextureCoord* tp = &TextureCoords[vertices[vv[vtx]].t - 1];
						glTexCoord2f(tp->s, tp->t);
					}

					if (vertices[vv[vtx]].n != 0)
					{
						struct Normal* np = &Normals[vertices[vv[vtx]].n - 1];
						glNormal3f(np->nx, np->ny, np->nz);
					}

					struct Vertex* vp = &Vertices[vertices[vv[vtx]].v - 1];
					glVertex3f(vp->x, vp->y, vp->z);
				}
			}
			continue;
		}


		if (strcmp(cmd, "s") == 0)
		{
			continue;
		}

	}

	glEnd();
	fclose(fp);

	fprintf(stderr, "Obj file range: [%8.3f,%8.3f,%8.3f] -> [%8.3f,%8.3f,%8.3f]\n",
		xmin, ymin, zmin, xmax, ymax, zmax);
	fprintf(stderr, "Obj file center = (%8.3f,%8.3f,%8.3f)\n",
		(xmin + xmax) / 2., (ymin + ymax) / 2., (zmin + zmax) / 2.);
	fprintf(stderr, "Obj file  span = (%8.3f,%8.3f,%8.3f)\n",
		xmax - xmin, ymax - ymin, zmax - zmin);

	return 0;
}






float
Unit(float v[3])
{
	float dist;

	dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

	if (dist > 0.0)
	{
		dist = sqrt(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}

	return dist;
}





char*
ReadRestOfLine(FILE* fp)
{
	static char* line;
	std::vector<char> tmp(1000);
	tmp.clear();

	for (; ; )
	{
		int c = getc(fp);

		if (c == EOF && tmp.size() == 0)
		{
			return NULL;
		}

		if (c == EOF || c == '\n')
		{
			delete[] line;
			line = new char[tmp.size() + 1];
			for (int i = 0; i < (int)tmp.size(); i++)
			{
				line[i] = tmp[i];
			}
			line[tmp.size()] = '\0';	// terminating null
			return line;
		}
		else
		{
			tmp.push_back(c);
		}
	}

	return "";
}


void
ReadObjVTN(char* str, int* v, int* t, int* n)
{
	// can be one of v, v//n, v/t, v/t/n:

	if (strstr(str, "//"))				// v//n
	{
		*t = 0;
		sscanf(str, "%d//%d", v, n);
		return;
	}
	else if (sscanf(str, "%d/%d/%d", v, t, n) == 3)	// v/t/n
	{
		return;
	}
	else
	{
		*n = 0;
		if (sscanf(str, "%d/%d", v, t) == 2)		// v/t
		{
			return;
		}
		else						// v
		{
			*n = *t = 0;
			sscanf(str, "%d", v);
		}
	}
}