#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define SIZE 512

GLint HITS;
int flag_xyz = 1;
int flag_light = 1;
int mousex, mousey;
float movex, movey, movez;

typedef struct {
	GLubyte* image_data;
	GLsizei width;
	GLsizei height;
} Data;
Data* data[4];
GLuint textures[4];

float PI = 3.1415926;
void display();
void drawSphere(GLfloat, GLfloat, GLfloat, int);

float wide = 0.8, height = 0.8, Dept = 0.8;

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(4, textures);
	for(int i = 0; i < 4; i++){
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0, GL_RGB, data[i]->width,
			data[i]->height , 0,
			GL_RGB, GL_UNSIGNED_BYTE,
			data[i]->image_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

int wide_screen = 300, height_screen = 300;

float eyex = 0, eyey = 0, eyez = 1.7;

void reshape(int w, int h)
{
	wide_screen = w;
	height_screen = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 1, 30);
	gluLookAt(eyex, eyey, eyez, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

float light_postion[4] = {wide - 0.2, height - 0.2, -Dept, 1};
float light_ambient [4] = {1.0, 1.0, 1.0, 0.8};
float light_diffuse [4] = {1.0, 1.0, 1.0, 0.8};
float light_specular [4] = {1.0, 1.0, 1.0, 0.8};

float proxy[3] = {wide - 0.2, height - 0.2, -Dept};

void createLightAndProxy(GLenum mode)
{
	glTranslatef(2 * movex / wide_screen, 2 * movey / height_screen, 2 * movez / wide_screen);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_postion);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	if(mode == GL_SELECT)
	{
		glLoadName(1);
	}
	if(flag_light == 1)
		drawSphere(proxy[0], proxy[1]-wide/12, proxy[2], 1);

	glTranslatef(-2 * movex / wide_screen, -2 * movey / height_screen, -2 * movez / wide_screen);

	if(mode == GL_SELECT)
	{
		glLoadName(2);
	}
}

void createWall()
{
	GLfloat Material_ambient[4] = {0.6, 0.2, 0.5, 0.5};
	GLfloat Material_diffuse[4] = {0.8, 0.2, 0.5, 0.5};
	GLfloat Material_specular[4] = {0.0, 0.0, 0.5, 1.0};
	GLfloat Material_shiness = 64.0;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Material_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Material_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Material_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Material_shiness);

	glBegin(GL_QUADS);

		glColor3f(0.27, 0.27, 0.38);
		glNormal3f(0, 0, -1);
		glVertex3f(wide, -height, Dept);
		glVertex3f(wide, height, Dept);
		glVertex3f(-wide, height, Dept);
		glVertex3f(-wide, -height, Dept);

		glColor3f(0.19, 0.19, 0.27);
		glNormal3f(-1, 0, 0);
		glVertex3f(wide, -height, Dept);
		glVertex3f(wide, height, Dept);
		glVertex3f(wide, height, -Dept);
		glVertex3f(wide, -height, -Dept);

		glColor3f(0.33, 0.33, 0.47);
		glNormal3f(1, 0, 0);
		glVertex3f(-wide, -height, -Dept);
		glVertex3f(-wide, height, -Dept);
		glVertex3f(-wide, height, Dept);
		glVertex3f(-wide, -height, Dept);

		glColor3f(0.28, 0.28, 0.4);
		glNormal3f(0, 0, 1);
		glVertex3f(wide, -height, -Dept);
		glVertex3f(wide, height, -Dept);
		glVertex3f(-wide, height, -Dept);
		glVertex3f(-wide, -height, -Dept);
	glEnd();
}

void createGround()
{
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glBegin(GL_QUADS);
	glColor4f(0.4, 0.3, 0.3, 0.6);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-wide, -height, Dept);
	glTexCoord2f(16.0, 0);
	glVertex3f(wide, -height, Dept);
	glTexCoord2f(16.0, 16.0);
	glVertex3f(wide, -height, -Dept);
	glTexCoord2f(0.0, 16.0);
	glVertex3f(-wide, -height, -Dept);
	glEnd();
}

void create_quad(int i, int j)
{
	if((i + j) % 2 == 1 )
		glColor3f(0.0, 0.5, 0.5);
	else
		glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);
		glVertex3f(-wide / 16, 0, Dept / 16);
		glVertex3f(wide / 16, 0, Dept / 16);
		glVertex3f(wide / 16, 0, -Dept / 16);
		glVertex3f(-wide / 16, 0, -Dept / 16);
	glEnd();
}

void createCeiling()
{
	GLfloat Material_ambient[4]={0.8, 0.8, 0.8, 1.0};
	GLfloat Material_diffuse[4]={0.8, 0.8, 0.8, 1.0};
	GLfloat Material_specular[4]={0.4, 0.3, 0.5, 1.0};
	GLfloat Material_shiness = 1.0;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Material_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Material_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Material_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Material_shiness);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glBegin(GL_QUADS);
		glColor4f(0.4, 0.3, 0.3, 0.6);
		glNormal3f(0, -1, 0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-wide, height, -Dept);
		glTexCoord2f(16.0, 0);
		glVertex3f(wide, height, -Dept);
		glTexCoord2f(16.0, 16.0);
		glVertex3f(wide, height, Dept);
		glTexCoord2f(0.0, 16.0);
		glVertex3f(-wide, height, Dept);
	glEnd();

}

void drawSphere(GLfloat xx = 0, GLfloat yy = 0, GLfloat zz = 0, int flag = 0)
{
	GLfloat radius;
	if(flag == 0)
		radius = 2 * wide / 8;
	else
		radius=wide / 16;
	GLfloat M = 20;
	GLfloat N = 20;
	float step_z = PI/M;
	float step_xy = 2*PI/N;
	float x[4], y[4], z[4];

	float angle_z = 0.0;
	float angle_xy = 0.0;
	int i = 0, j = 0;
	if(flag == 0)
		glColor3f(0.5, 0.25, 0);
	else
		glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	for(i = 0; i < M; i++)
	{
		angle_z = i * step_z;

		for(j = 0; j < N; j++)
		{
			angle_xy = j * step_xy;

			x[0] = radius * sin(angle_z) * cos(angle_xy);
			y[0] = radius * sin(angle_z) * sin(angle_xy);
			z[0] = radius * cos(angle_z);

			x[1] = radius * sin(angle_z + step_z) * cos(angle_xy);
			y[1] = radius * sin(angle_z + step_z) * sin(angle_xy);
			z[1] = radius * cos(angle_z + step_z);

			x[2] = radius * sin(angle_z + step_z) * cos(angle_xy + step_xy);
			y[2] = radius * sin(angle_z + step_z) * sin(angle_xy + step_xy);
			z[2] = radius  * cos(angle_z + step_z);

			x[3] = radius * sin(angle_z) * cos(angle_xy + step_xy);
			y[3] = radius * sin(angle_z) * sin(angle_xy + step_xy);
			z[3] = radius * cos(angle_z);

			for(int k = 0; k < 4; k++)
			{
				glNormal3f(x[k], y[k], z[k]);
				glVertex3f(xx + x[k], yy + y[k], zz + z[k]);
			}
		}
	}
	glEnd();
}

void yuanzhui()
{
	glTranslatef(-wide / 2, -height, -Dept / 2);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0, 0.25, 0.5);
	glNormal3f(1, 1, 1);
	glVertex3f(0, height / 1.2, 0);
	for(int i = 0; i <= 32; i++)
	{
		glVertex3f(0.2 * cos(i * PI / 16), 0, 0.2 * sin(i * PI / 16));
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
		glColor3f(0, 0.25, 0.5);
		glNormal3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		for(int i = 0; i <= 32; i++)
		{
			glVertex3f(0.2 * cos(i * PI / 16), 0.01 , 0.2 * sin(i * PI / 16));
		}
	glEnd();
	glTranslatef(wide / 2, height, Dept/2);
}

void createFurnishings()
{
	glTranslatef(0, -height + 2 * wide / 8, -Dept + 2 * Dept / 8);
	drawSphere();
	glTranslatef(0, height - 2 * wide / 8, Dept - 2 * Dept / 8);
	yuanzhui();
}

void draw(GLenum mode)
{
	glMatrixMode(GL_MODELVIEW);
	if(flag_light == 1)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	createLightAndProxy(mode);
	createWall();
	createGround();
	createCeiling();
	createFurnishings();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 1, 30);
	gluLookAt(eyex, eyey, eyez, 0, 0, 0, 0, 1, 0);
	draw(GL_RENDER);
	glFlush();
	glutSwapBuffers();
}

int flag_move = 0 ;

void process(GLint hits, GLuint buffer[])
{
	unsigned int i, j;
	GLint  names, *ptr;

	ptr = (GLint *) buffer;
	for (i = 0; i < hits; i++)
  {
		names = *ptr;
		ptr += 3;
		for (j = 0; j < names; j++)
    {
			if(*ptr == 1)
			{
				flag_move = 1;
			}
			ptr++;
		}
	}
}

GLuint select_Buffer[SIZE];

void mouse(int key, int state, int x, int y)
{
	GLint hits;
	GLint viewport[4];
	if(key == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mousex = x;
		mousey = y;
		glGetIntegerv(GL_VIEWPORT, viewport);

		glSelectBuffer(SIZE, select_Buffer);
		glRenderMode(GL_SELECT);

		glInitNames();
		glPushName(0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5, 5, viewport);
		gluPerspective(60, 1, 1, 30);
		gluLookAt(eyex, eyey, eyez, 0, 0, 0, 0, 1, 0);

		draw(GL_SELECT);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glFlush();

		hits = glRenderMode(GL_RENDER);
		process(hits, select_Buffer);
		HITS = hits;
		glutPostRedisplay();
	}
	if(key == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		flag_move = 0;
	}
}

void motion(int x, int y)
{
	if(flag_move == 0)
		return;
	else
	{
		switch(flag_xyz)
		{
		case 1:
		case 2:
			movex = x - mousex + movex;
			movey = -y + mousey + movey;
			break;
		case 3:
			movez = x - mousex+movez;
			break;
		}
		glutPostRedisplay();
		mousex = x;
		mousey = y;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'x':
		eyex -=0.1;
		break;
	case 'y':
		eyey -=0.1;
		break;
	case 'z':
		eyez -=0.1;
		break;
	case 'X':
		eyex +=0.1;
		break;
	case 'Y':
		eyey +=0.1;
		break;
	case 'Z':
		eyez +=0.1;
		break;
	case 'h':
	case 'H':
		flag_xyz = 1;
		break;
	case 's':
	case 'S':
		flag_xyz = 2;
		break;
	case 'l':
	case 'L':
		flag_xyz = 3;
		break;
	default:
		return;
		break;
	}
	glutPostRedisplay();
}

void MenuFunc(int data)
{
	switch(data)
	{
	case 1:
		flag_light = 0;
		break;
	case 2:
		flag_light = 1;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

static GLint    ImageWidth;
static GLint    ImageHeight;
static GLint    PixelLength;
static GLubyte* PixelData;

Data* imageInit(char * path_name){

	FILE* pFile = fopen(path_name, "rb");
	if( pFile == 0 )
	{
		printf("File not found");
		exit(0);
	}

	fseek(pFile, 0x0012, SEEK_SET);
	fread(&ImageWidth, sizeof(ImageWidth), 1, pFile);
	fread(&ImageHeight, sizeof(ImageHeight), 1, pFile);

	PixelLength = ImageWidth * 3;
	while( PixelLength % 4 != 0 )
		++PixelLength;
	PixelLength *= ImageHeight;

	PixelData = (GLubyte*)malloc(PixelLength);
	if( PixelData == 0 )
		exit(0);

	fseek(pFile, 54, SEEK_SET);
	fread(PixelData, PixelLength, 1, pFile);

	fclose(pFile);
	Data* rv = (Data*)malloc(sizeof(Data));
	rv->image_data = PixelData;
	rv->width = ImageWidth;
	rv->height = ImageHeight;
	return rv;
}

int Menu;

int main(int argc, char* argv[])
{

	char* tupian[4] = {"1.bmp", "2.bmp", "3.bmp", "4.bmp"};
	for(int i = 0; i < 4; i++){
		data[i] = imageInit(tupian[i]);
	}

	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInit(&argc, argv);
	glutInitWindowSize(wide_screen, height_screen);
	glutCreateWindow("4");
	init();
	Menu = glutCreateMenu(MenuFunc);
	glutAddMenuEntry("open", 1);
	glutAddMenuEntry("close", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
