#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#define PI 3.14159265358

using namespace std;

int windowSize[2] = { 600, 600 };
double angle=0;
double lightmove = 0;
double time = 0;
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void textureInit();

GLuint program;
GLuint partprogram;
GLuint makemakeprogram;
GLuint universeprogram;
GLuint vao;
GLuint makemakevao;
GLuint partvao;
GLuint universevao;
GLuint vboName;
GLuint makemakevboName;
GLuint partvboName;
GLuint universevboName;
GLuint texture[8];

//Initialize the shaders 
void shaderInit();
void makemakeshaderInit();
void partshaderInit();
void universeshaderInit();

//Number of vertices that will be sent to shader
int verticeNumber = 360*(180+1)*2;

//Storing vertex datas that will be sent to shader
class VertexAttribute {
public:
	GLfloat position[3];
	GLfloat texcoord[2];
	GLfloat normal[3];
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
	void setCoord(float stack, float slice) {
		texcoord[0] = stack;
		texcoord[1] = slice;
	};
	void setNormal(float x, float y, float z) {
		normal[0] = x;
		normal[1] = y;
		normal[2] = z;
	};
};

struct ParticleAttribute
{
	GLfloat position[3] = {};
	GLfloat speed[3];
	GLfloat fade;
	GLfloat life;

	/*void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
	void setSpeed(float x, float y, float z) {
		speed[0] = x;
		speed[1] = y;
		speed[2] = z;
	};*/

	ParticleAttribute()
		: position{ 0.0f, 0.0f, 0.0f }, speed{ 0.0f, 0.0f, 0.0f }, fade(0.0f), life(0.0f) { }
};

ParticleAttribute particles[20000];
GLuint lastUsedParticle = 0;

//Collecting vertex datas
VertexAttribute* drawEarth();
VertexAttribute* drawMakemake();
VertexAttribute* drawUniverse();

void initParticlesPosition()
{
	for (int i = 0; i < 20000; i++)
	{
		particles[i].position[0] = 0.0;
		particles[i].position[1] = 0.0;
		particles[i].position[2] = 0.0;
		//particles[i].setPosition(0.0, 0.0, 0.0);
		float stepi = 2 * PI / 180;
		int randomi = rand() % 180;
		float stepj = 2 * PI / 180;
		int randomj = rand() % 180;
		particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;
		particles[i].speed[1] = cos(randomj * stepj) * 100.0;
		particles[i].speed[2] = sin(randomj * stepj) * sin(randomi * stepi) * 100.0;
		//particles[i].setSpeed(sin(randomj * stepj) * cos(randomi * stepi) * 100.0, cos(randomj * stepj) * 100.0, sin(randomj * stepj) * sin(randomi * stepi) * 100.0);
		if (i < 5000)
		{
			particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 500.0;
		}
		particles[i].fade = GLfloat(rand() % 100) / 1000.0f + 0.003f;
		particles[i].life = 2.0f;
	}
}

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();

	shaderInit();
	makemakeshaderInit();
	textureInit();
	partshaderInit();
	universeshaderInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint geom = createShader("Shaders/example.geom", "geometry");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, geom, frag);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Generate a new buffer object
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	//Copy vertex data to the buffer object
	VertexAttribute *vertices;
	vertices = drawEarth();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void makemakeshaderInit() {
	GLuint makemakevert = createShader("Shaders/makemake.vert", "vertex");
	GLuint makemakefrag = createShader("Shaders/makemake.frag", "fragment");
	makemakeprogram = createProgram(makemakevert, makemakefrag);

	glGenVertexArrays(1, &makemakevao);
	glBindVertexArray(makemakevao);

	//Generate a new buffer object
	glGenBuffers(1, &makemakevboName);
	glBindBuffer(GL_ARRAY_BUFFER, makemakevboName);

	//Copy vertex data to the buffer object
	VertexAttribute* vertice;
	vertice = drawMakemake();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertice, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void partshaderInit() {
	GLuint partvert = createShader("Shaders/particles.vert", "vertex");
	GLuint partfrag = createShader("Shaders/particles.frag", "fragment");
	partprogram = createProgram(partvert, partfrag);

	glGenVertexArrays(1, &partvao);
	glBindVertexArray(partvao);

	//Generate a new buffer object
	glGenBuffers(1, &partvboName);
	glBindBuffer(GL_ARRAY_BUFFER, partvboName);

	//Copy vertex data to the buffer object
	initParticlesPosition();
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, particles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, speed)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, fade)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void universeshaderInit() {
	GLuint universevert = createShader("Shaders/universe.vert", "vertex");
	GLuint universefrag = createShader("Shaders/universe.frag", "fragment");
	universeprogram = createProgram(universevert, universefrag);

	glGenVertexArrays(1, &universevao);
	glBindVertexArray(universevao);

	//Generate a new buffer object
	glGenBuffers(1, &universevboName);
	glBindBuffer(GL_ARRAY_BUFFER, universevboName);

	//Copy vertex data to the buffer object
	VertexAttribute* vertices;
	vertices = drawUniverse();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void textureInit()
{
	glEnable(GL_TEXTURE_2D);

	FIBITMAP* pIimage = FreeImage_Load(FreeImage_GetFileType("earth_texture_map.jpg", 0), "earth_texture_map.jpg");
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glGenTextures(8, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);


	pIimage = FreeImage_Load(FreeImage_GetFileType("earth_normal_map.tif", 0), "earth_normal_map.tif");
	p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);


	pIimage = FreeImage_Load(FreeImage_GetFileType("earth_specular_map.tif", 0), "earth_specular_map.tif");
	p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);


	pIimage = FreeImage_Load(FreeImage_GetFileType("earth_clouds_map.jpg", 0), "earth_clouds_map.jpg");
	p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glBindTexture(GL_TEXTURE_2D, texture[3]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);


	pIimage = FreeImage_Load(FreeImage_GetFileType("earth_night_map.jpg", 0), "earth_night_map.jpg");
	p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glBindTexture(GL_TEXTURE_2D, texture[4]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);


	pIimage = FreeImage_Load(FreeImage_GetFileType("makemake_texture_map.jpg", 0), "makemake_texture_map.jpg");
	p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glBindTexture(GL_TEXTURE_2D, texture[5]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);


	pIimage = FreeImage_Load(FreeImage_GetFileType("sun_texture_map.jpg", 0), "sun_texture_map.jpg");
	p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glBindTexture(GL_TEXTURE_2D, texture[6]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	pIimage = FreeImage_Load(FreeImage_GetFileType("stars_milky_way.jpg", 0), "stars_milky_way.jpg");
	p32BitsImage = FreeImage_ConvertTo32Bits(pIimage);
	iWidth = FreeImage_GetWidth(p32BitsImage);
	iHeight = FreeImage_GetHeight(p32BitsImage);
	glBindTexture(GL_TEXTURE_2D, texture[7]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pIimage);
}

void display() {   
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);

	if(10.0f - 0.003 * time>=5)
	{
		//cout << 11.0f + 5 * cos(PI / 2 + 0.003 * time / 3.2) << " " << 5 * sin( 0.003 * time / 3.2) << endl;
		glLoadIdentity();
		gluLookAt(11.0f + 5 * cos(PI / 2 + 0.003 * time / 3.2), 0.0f, 5 * sin(0.003 * time / 3.2),// eye
			5.0f - 0.003 * time, 0.0f, 0.0f,// center
			0.0f, 1.0f, 0.0f);// up
	}
	else
	{
		glLoadIdentity();
		gluLookAt(6.00017f, 0.0f, 4.99983f - 0.003 * time,// eye
			0.0f, 0.0f, 0.0f,// center
			0.0f, 1.0f, 0.0f);// up
	}

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	glMatrixMode(GL_MODELVIEW);

	GLfloat mtx[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mtx);

	glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		glRotatef(-lightmove, 0.0f, 1.0f, 0.0f);
		glTranslatef(10.0f, 0.0f, 10.0f);
		//gluSphere(gluNewQuadric(), 0.1f, 360, 180);

		GLfloat lpmtx[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, lpmtx);
		GLint lpmatLoc = glGetUniformLocation(program, "LightModelView");
	glPopMatrix();

	glRotatef(23.5, 0.0f, 0.0f, 1.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
		GLfloat pmtx[16];
		GLfloat mmtx[16];
		GLfloat lmtx[3] = { 10.0f, 0.0f, 10.0f };
		glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
		glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

 		GLint pmatLoc = glGetUniformLocation(program, "Projection");
		GLint mmatLoc = glGetUniformLocation(program, "ModelView");
		GLint texLoc = glGetUniformLocation(program, "Texture");
		GLint bumpLoc = glGetUniformLocation(program, "Bump");
		GLint specLoc = glGetUniformLocation(program, "Specular");
		GLint cloudLoc = glGetUniformLocation(program, "Cloud");
		GLint nightLoc = glGetUniformLocation(program, "Night");
		GLint lightLoc = glGetUniformLocation(program, "Light");
		GLint timeLoc = glGetUniformLocation(program, "Time");
		GLint expLoc = glGetUniformLocation(program, "Explosion");

		glUseProgram(program);
		glBindVertexArray(vao);

		glUniformMatrix4fv(lpmatLoc, 1, GL_FALSE, lpmtx);

		//input the modelview matrix into vertex shader
		glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
		//input the rotation matrix into vertex shader
		glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
		glUniform3fv(lightLoc, 1, lmtx);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glUniform1i(texLoc, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glUniform1i(bumpLoc, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glUniform1i(specLoc, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		glUniform1i(cloudLoc, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glUniform1i(nightLoc, 4);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, texture[6]);
		glUniform1i(expLoc, 5);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, verticeNumber-2);

		glUniform1d(timeLoc, time);

		glBindVertexArray(0);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, NULL);
	glPopMatrix();

	glLoadMatrixf(mtx);

	glPushMatrix();
		GLint partpmatLoc = glGetUniformLocation(partprogram, "Projection");
		GLint partmmatLoc = glGetUniformLocation(partprogram, "ModelView");

		if (10.0f - 0.003 * time <= 1)
		{
			glEnable(GL_PROGRAM_POINT_SIZE);  
			glUseProgram(partprogram);
			glBindVertexArray(partvao);

			//input the modelview matrix into vertex shader
			glUniformMatrix4fv(partpmatLoc, 1, GL_FALSE, pmtx);
			//input the rotation matrix into vertex shader
			glUniformMatrix4fv(partmmatLoc, 1, GL_FALSE, mmtx);

			glDrawArrays(GL_POINTS, 0, 20000);

			glBindVertexArray(0);
			glUseProgram(0);
		}
	glPopMatrix();

	glPushMatrix();
		if(10 - 0.003 * time>0.75)
			glTranslatef(10-0.003*time, 0, 0);
		else
			glTranslatef(0.75, 0, 0);
		glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
		glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

		GLint makemakepmatLoc = glGetUniformLocation(makemakeprogram, "Projection");
		GLint makemakemmatLoc = glGetUniformLocation(makemakeprogram, "ModelView");
		GLint makemaketexLoc = glGetUniformLocation(makemakeprogram, "MakeMake");

		glUseProgram(makemakeprogram);
		glBindVertexArray(makemakevao);

		glUniformMatrix4fv(lpmatLoc, 1, GL_FALSE, lpmtx);

		//input the modelview matrix into vertex shader
		glUniformMatrix4fv(makemakepmatLoc, 1, GL_FALSE, pmtx);
		//input the rotation matrix into vertex shader
		glUniformMatrix4fv(makemakemmatLoc, 1, GL_FALSE, mmtx);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[5]);
		glUniform1i(makemaketexLoc, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, verticeNumber - 2);

		glBindVertexArray(0);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, NULL);
	glPopMatrix();

	glPushMatrix();
		GLint universepmatLoc = glGetUniformLocation(universeprogram, "Projection");
		GLint universemmatLoc = glGetUniformLocation(universeprogram, "ModelView");
		GLint universetexLoc = glGetUniformLocation(makemakeprogram, "Universe");

		glUseProgram(universeprogram);
		glBindVertexArray(universevao);

		//input the modelview matrix into vertex shader
		glUniformMatrix4fv(universepmatLoc, 1, GL_FALSE, pmtx);
		//input the rotation matrix into vertex shader
		glUniformMatrix4fv(universemmatLoc, 1, GL_FALSE, mmtx);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[7]);
		glUniform1i(universetexLoc, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, verticeNumber - 2);

		glBindVertexArray(0);
		glUseProgram(0);
	glPopMatrix();

	glutSwapBuffers();	
}

VertexAttribute* drawEarth() {
	VertexAttribute* vertices;
	vertices = new VertexAttribute[verticeNumber];
	int num = 0;
	double x, y, z;
	double slice1 = 2 * PI / 360, stack1 = PI / 180;
	double slice2 = 1.0 / 360, stack2 = 1.0 / 180;

	for (int i = 0; i < 360; i++)//slice
	{
		for (int j = 0; j < 180 + 1; j++)//stack
		{
			x = sin(j * stack1) * cos(i * slice1);
			y = cos(j * stack1);
			z = sin(j * stack1) * sin(i * slice1);
			vertices[num].setPosition(x, y, z);
			vertices[num].setCoord(1-(slice2 * i), 1-(stack2 * j));
			vertices[num].setNormal(x, y, z);

			x = sin(j * stack1) * cos((i + 1) * slice1);
			y = cos(j * stack1);
			z = sin(j * stack1) * sin((i + 1) * slice1);
			vertices[num+1].setPosition(x, y, z);
			vertices[num+1].setCoord(1-(slice2 * (i+1)), 1-(stack2 * j));
			vertices[num+1].setNormal(x, y, z);
		
			num += 2;
		}
	}
	return vertices;
}

VertexAttribute* drawMakemake() {
	VertexAttribute* vertice;
	vertice = new VertexAttribute[verticeNumber];
	int num = 0;
	double x, y, z;
	double slice1 = 2 * PI / 360, stack1 = PI / 180;
	double slice2 = 1.0 / 360, stack2 = 1.0 / 180;

	for (int i = 0; i < 360; i++)//slice
	{
		for (int j = 0; j < 180 + 1; j++)//stack
		{
			x = 0.25 * sin(j * stack1) * cos(i * slice1);
			y = 0.25 * cos(j * stack1);
			z = 0.25 * sin(j * stack1) * sin(i * slice1);
			vertice[num].setPosition(x, y, z);
			vertice[num].setCoord(1 - (slice2 * i), 1 - (stack2 * j));
			vertice[num].setNormal(x, y, z);

			x = 0.25 * sin(j * stack1) * cos((i + 1) * slice1);
			y = 0.25 * cos(j * stack1);
			z = 0.25 * sin(j * stack1) * sin((i + 1) * slice1);
			vertice[num + 1].setPosition(x, y, z);
			vertice[num + 1].setCoord(1 - (slice2 * (i + 1)), 1 - (stack2 * j));
			vertice[num + 1].setNormal(x, y, z);

			num += 2;
		}
	}
	return vertice;
}

VertexAttribute* drawUniverse() {
	VertexAttribute* vertice;
	vertice = new VertexAttribute[verticeNumber];
	int num = 0;
	double x, y, z;
	double slice1 = 2 * PI / 360, stack1 = PI / 180;
	double slice2 = 1.0 / 360, stack2 = 1.0 / 180;

	for (int i = 0; i < 360; i++)//slice
	{
		for (int j = 0; j < 180 + 1; j++)//stack
		{
			x = 50 * sin(j * stack1) * cos(i * slice1);
			y = 50 * cos(j * stack1);
			z = 50 * sin(j * stack1) * sin(i * slice1);
			vertice[num].setPosition(x, y, z);
			vertice[num].setCoord(1 - (slice2 * i), 1 - (stack2 * j));
			vertice[num].setNormal(x, y, z);

			x = 50 * sin(j * stack1) * cos((i + 1) * slice1);
			y = 50 * cos(j * stack1);
			z = 50 * sin(j * stack1) * sin((i + 1) * slice1);
			vertice[num + 1].setPosition(x, y, z);
			vertice[num + 1].setCoord(1 - (slice2 * (i + 1)), 1 - (stack2 * j));
			vertice[num + 1].setNormal(x, y, z);

			num += 2;
		}
	}
	return vertice;
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {
}

void idle() {
	angle += 0.087;
	lightmove += 0.1;
	time += 1.0;

	for (int i = 0; i < 20000; i++)
	{
		particles[i].position[0] = particles[i].position[0] + particles[i].speed[0] / 10000.0;
		particles[i].position[1] = particles[i].position[1] + particles[i].speed[1] / 10000.0;
		particles[i].position[2] = particles[i].position[2] + particles[i].speed[2] / 10000.0;

		particles[i].life -= particles[i].fade;
		if (particles[i].life < 0.0)
		{
			particles[i].life = 1.0f;
			particles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;
			particles[i].position[0] = 0.0;
			particles[i].position[1] = 0.0;
			particles[i].position[2] = 0.0;
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, partvboName);

	glGenVertexArrays(1, &partvao);
	glBindVertexArray(partvao);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, particles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, speed)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, fade)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glutPostRedisplay();
}
