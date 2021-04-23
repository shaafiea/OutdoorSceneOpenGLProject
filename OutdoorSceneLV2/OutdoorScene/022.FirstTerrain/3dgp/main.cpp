#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// GLSL Objects (Shader Program)
C3dglProgram ProgramBasic;
C3dglProgram ProgramParticle;
C3dglProgram ProgramWater;
C3dglProgram ProgramTerrain;

//Switches
int dayLight = 1;
int directionalLight = 1;
int pointLight = 1;
int ambientLight = 1;
int bulbambientLight = 1;

//Global Variables (Particle System)
GLuint idBufferVelocity;
GLuint idBufferStartTime;
GLuint idBufferPosition;

// Particle System Params
const float PERIOD = 0.00175f;
const float LIFETIME = 4;
const int NPARTICLES = (int)(LIFETIME / PERIOD);


// 3D Models
C3dglSkyBox skybox;
C3dglSkyBox nightbox;
C3dglTerrain terrain, road, water;
C3dglModel lamp;
C3dglModel car;
C3dglModel snowman;
C3dglBitmap bmSand;


//Bitmap textures
C3dglBitmap bmGround;
C3dglBitmap bmRoad;
C3dglBitmap bmSnow;//Level 2 Particle System Textures
C3dglBitmap bmSnowman;
C3dglBitmap bmSnowmanNormal;

GLuint idTexSnowman;
GLuint idTexSnowmanNormal;
GLuint idTexRoad;
GLuint idTexSnow;

GLuint idTexGround;
GLuint idTexSand;
GLuint idTexNone;

//Normal Map Textures
C3dglBitmap bmNormalSand;
GLuint idTexNormalSand;
C3dglBitmap bmNormalBed;
GLuint idTexNormalBed;

//Sleigh
C3dglModel santasleigh;
C3dglBitmap bmRide; //Santa Sleigh Bitmap
GLuint idTexSleigh;

// Water specific variables
float waterLevel = 3.6f;

// camera position (for first person type camera navigation)
mat4 matrixView;			// The View Matrix
float angleTilt = 15.f;		// Tilt Angle
vec3 cam(0);				// Camera movement values

bool init()
{
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// switch on: transparency/blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* setup the point size (old)
	glEnable(GL_POINT_SPRITE);
	glPointSize(5);*/

	//Setting the shader (Enable Functionality)
	glEnable(0x8642);
	glEnable(GL_POINT_SPRITE);

	// Initialise Shaders
	C3dglShader VertexShader;
	C3dglShader FragmentShader;

	// Initialise Shader - Lighting
	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/basic.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/basic.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramBasic.Create()) return false;
	if (!ProgramBasic.Attach(VertexShader)) return false;
	if (!ProgramBasic.Attach(FragmentShader)) return false;
	if (!ProgramBasic.Link()) return false;
	if (!ProgramBasic.Use(true)) return false;

	//Initalize Water Shader
	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/water.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/water.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramWater.Create()) return false;
	if (!ProgramWater.Attach(VertexShader)) return false;
	if (!ProgramWater.Attach(FragmentShader)) return false;
	if (!ProgramWater.Link()) return false;
	if (!ProgramWater.Use(true)) return false;

	//Initalize Terrain Shader
	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/terrain.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/terrain.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramTerrain.Create()) return false;
	if (!ProgramTerrain.Attach(VertexShader)) return false;
	if (!ProgramTerrain.Attach(FragmentShader)) return false;
	if (!ProgramTerrain.Link()) return false;
	if (!ProgramTerrain.Use(true)) return false;

	// Initialise Shader - Particle
	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/particles.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/particles.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramParticle.Create()) return false;
	if (!ProgramParticle.Attach(VertexShader)) return false;
	if (!ProgramParticle.Attach(FragmentShader)) return false;
	if (!ProgramParticle.Link()) return false;
	if (!ProgramParticle.Use(true)) return false;

	// glut additional setup
	glutSetVertexAttribCoord3(ProgramBasic.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(ProgramBasic.GetAttribLocation("aNormal"));
	glutSetVertexAttribCoord3(ProgramTerrain.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(ProgramTerrain.GetAttribLocation("aNormal"));

	//Loading 3D Models
	ProgramBasic.Use();
	if (!lamp.load("models\\lamp2.obj")) return false;
	if (!snowman.load("models\\snowmanv2\\snowman.obj")) return false;
	if (!santasleigh.load("models\\santasleigh\\santasleigh.obj")) return false;

	// load your 3D models here!
	if (!terrain.loadHeightmap("models\\one.bmp", 10)) return false;
	if (!road.loadHeightmap("models\\two.bmp", 10)) return false;
	if (!water.loadHeightmap("models\\watermap.png", 10)) return false;

	// load Sky Box     
	if (!skybox.load("models\\TropicalSunnyDay\\TropicalSunnyDayFront1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayLeft1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayBack1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayRight1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayUp1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayDown1024.jpg")) return false;

	//load night box
	if (!nightbox.load("models\\FullMoon\\FullMoonFront2048.png",
		"models\\FullMoon\\FullMoonLeft2048.png",
		"models\\FullMoon\\FullMoonBack2048.png",
		"models\\FullMoon\\FullMoonRight2048.png",
		"models\\FullMoon\\FullMoonUp2048.png",
		"models\\FullMoon\\FullMoonDown2048.png")) return false;


	//Loading the bitmap
	bmGround.Load("models/snowysand.png", GL_RGBA);
	if (!bmGround.GetBits()) return false;
	bmSand.Load("models/snow.jpg", GL_RGBA);
	if (!bmSand.GetBits()) return false;
	bmRoad.Load("models/road.png", GL_RGBA);
	if (!bmRoad.GetBits()) return false;
	bmSnowman.Load("models/snowmanv2/map_snowman1_BaseColor.png", GL_RGBA);
	if (!bmSnowman.GetBits()) return false;
	bmSnowmanNormal.Load("models/snowmanv2/map_snowman1_Normal.png", GL_RGBA);
	if (!bmSnowmanNormal.GetBits()) return false;
	bmNormalSand.Load("models/snowNormalMap.png", GL_RGBA);
	if (!bmNormalSand.GetBits()) return false;
	bmNormalBed.Load("models/snowysandNormalMap.png", GL_RGBA);
	if (!bmNormalBed.GetBits()) return false;
	bmRide.Load("models/santasleigh/santasleigh_texture.bmp", GL_RGBA);
	if (!bmRide.GetBits()) return false;


	//Preparing the texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexRoad);
	glBindTexture(GL_TEXTURE_2D, idTexRoad);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmRoad.GetWidth(), bmRoad.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmRoad.GetBits());

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &idTexGround);
	glBindTexture(GL_TEXTURE_2D, idTexGround);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmGround.GetWidth(), bmGround.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmGround.GetBits());

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &idTexSand);
	glBindTexture(GL_TEXTURE_2D, idTexSand);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmSand.GetWidth(), bmSand.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmSand.GetBits());

	//Normal Map Texture

	//Shore
	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &idTexNormalSand);
	glBindTexture(GL_TEXTURE_2D, idTexNormalSand);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmNormalSand.GetWidth(), bmNormalSand.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmNormalSand.GetBits());

	//Bed
	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &idTexNormalBed);
	glBindTexture(GL_TEXTURE_2D, idTexNormalBed);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmNormalBed.GetWidth(), bmNormalBed.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmNormalBed.GetBits());

	//Sleigh
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexSleigh);
	glBindTexture(GL_TEXTURE_2D, idTexSleigh);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmRide.GetWidth(), bmRide.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmRide.GetBits());


	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexSnowman);
	glBindTexture(GL_TEXTURE_2D, idTexSnowman);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmSnowman.GetWidth(), bmSnowman.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmSnowman.GetBits());
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &idTexSnowmanNormal);
	glBindTexture(GL_TEXTURE_2D, idTexSnowmanNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmSnowmanNormal.GetWidth(), bmSnowmanNormal.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmSnowmanNormal.GetBits());


	// Setup the particle system
	//ProgramParticle.SendUniform("initialPos", 5.0, 8.0, 3.0); // not needed
	//ProgramParticle.SendUniform("gravity", 0.0, -1.0, 0.0);
	ProgramParticle.SendUniform("particleLifetime", LIFETIME);

	// setup the water colours and level
	ProgramWater.SendUniform("waterColor", 0.2f, 0.2f, 0.7f, 0.2f);
	ProgramWater.SendUniform("skyColor", 0.2f, 0.6f, 1.f, 1.0f);
	ProgramTerrain.SendUniform("waterLevel", waterLevel);

	// none (simple-white) texture
	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);

	//Loading the snow particles bitmap
	bmSnow.Load("models/snow.bmp", GL_RGBA);
	if (!bmSnow.GetBits()) return false;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexSnow);
	glBindTexture(GL_TEXTURE_2D, idTexSnow);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmSnow.GetWidth(), bmSnow.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bmSnow.GetBits());

	// Prepare the particle buffers
	std::vector<float> bufferVelocity;
	std::vector<float> bufferStartTime;
	std::vector<float> bufferPosition;
	float time = 0;
	for (int i = 0; i < NPARTICLES; i++)
	{
		float theta = (float)M_PI / 6.f * (float)rand() / (float)RAND_MAX;
		float phi = (float)M_PI * 2.f * (float)rand() / (float)RAND_MAX;
		float x = sin(theta) * cos(phi);
		float y = cos(theta);
		float z = sin(theta) * sin(phi);
		float v = 2 + 0.5f * (float)rand() / (float)RAND_MAX;

		bufferVelocity.push_back(x * v);
		bufferVelocity.push_back(-y * v); //-y will make the particles flipped so now it will be facing downwards
		bufferVelocity.push_back(z * v);

		//particle position
		bufferPosition.push_back(x * v);
		bufferPosition.push_back(-y);
		bufferPosition.push_back(z * v);

		bufferStartTime.push_back(time);
		time += PERIOD;
	}
	glGenBuffers(1, &idBufferVelocity);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferVelocity);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferVelocity.size(), &bufferVelocity[0],
		GL_STATIC_DRAW);
	glGenBuffers(1, &idBufferStartTime);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferStartTime);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferStartTime.size(), &bufferStartTime[0],
		GL_STATIC_DRAW);
	glGenBuffers(1, &idBufferPosition);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferPosition.size(), &bufferPosition[0],
		GL_STATIC_DRAW);

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(4.0, 10.5, 30.0),
		vec3(4.0, 10.5, 0.0),
		vec3(0.0, 10.0, 0.0));

	// Send the texture info to the shaders
	ProgramBasic.SendUniform("texture0", 0);
	/*ProgramTerrain.SendUniform("textureBed", 1)
	ProgramTerrain.SendUniform("textureShore", 2);
	ProgramTerrain.SendUniform("NormalShore", 3);*/

	// setup the screen background colour
	//glClearColor(0.2f, 0.6f, 1.f, 1.0f);   // blue sky background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << "Switches: " << endl;
	cout << "  N - Switch between daytime and nighttime" << endl;
	cout << "  K - turn directional light on or off" << endl;
	cout << "  L - turn ambient light on or off" << endl;
	cout << "  ; - turn spot light on or off" << endl;
	cout << endl;

	return true;
}

void done()
{
}

void render()
{

	// this global variable controls the animation
	float theta = glutGet(GLUT_ELAPSED_TIME) * 0.01f;

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));// switch tilt off
	m = translate(m, cam);												// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			// switch tilt on
	matrixView = m * matrixView;

	// move the camera up following the profile of terrain (Y coordinate of the terrain)
	float terrainY = -terrain.getInterpolatedHeight(inverse(matrixView)[3][0], inverse(matrixView)[3][2]);
	matrixView = translate(matrixView, vec3(0, terrainY, 0));

	// setup View Matrix
	ProgramBasic.SendUniform("matrixView", matrixView);
	ProgramWater.SendUniform("matrixView", matrixView);
	ProgramTerrain.SendUniform("matrixView", matrixView);

	//Daytime
	if (dayLight == 1)
	{
		//setup switches to the lights (Basic Shader)
		ProgramBasic.SendUniform("lightAmbient.on", ambientLight);
		ProgramBasic.SendUniform("lightAmbient1.on", ambientLight);
		ProgramBasic.SendUniform("lightDir.on", directionalLight);
		ProgramBasic.SendUniform("lightPoint.on", pointLight);
		ProgramBasic.SendUniform("lightPoint1.on", pointLight);
		ProgramBasic.SendUniform("lightPoint2.on", pointLight);
		ProgramBasic.SendUniform("lightPoint3.on", pointLight);

		//setup switches to the lights (Terrain Shader)
		ProgramTerrain.SendUniform("lightAmbient.on", ambientLight);
		ProgramTerrain.SendUniform("lightAmbient1.on", ambientLight);
		ProgramTerrain.SendUniform("lightDir.on", directionalLight);
		ProgramTerrain.SendUniform("lightPoint.on", pointLight);
		ProgramTerrain.SendUniform("lightPoint1.on", pointLight);
		ProgramTerrain.SendUniform("lightPoint2.on", pointLight);
		ProgramTerrain.SendUniform("lightPoint3.on", pointLight);

		//Daylight
		ProgramBasic.SendUniform("lightAmbient.on", 1);
		ProgramBasic.SendUniform("lightAmbient.color", 1.0, 1.0, 1.0);

		ProgramTerrain.SendUniform("lightAmbient.on", 1);
		ProgramTerrain.SendUniform("lightAmbient.color", 1.0, 1.0, 1.0);

		//Bulb Lights Off
		ProgramBasic.SendUniform("lightAmbient1.on", 0);
		ProgramBasic.SendUniform("lightAmbient1.color", 0.0, 0.0, 0.0);

		ProgramTerrain.SendUniform("lightAmbient1.on", 0);
		ProgramTerrain.SendUniform("lightAmbient1.color", 0.0, 0.0, 0.0);

		//Directional Light
		ProgramBasic.SendUniform("lightDir.on", 1);
		ProgramBasic.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
		ProgramBasic.SendUniform("lightDir.diffuse", 0.7, 0.7, 0.7);	  // dimmed white light

		ProgramTerrain.SendUniform("lightDir.on", 1);
		ProgramTerrain.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
		ProgramTerrain.SendUniform("lightDir.diffuse", 0.7, 0.7, 0.7);	  // dimmed white light

			////PointLight (Lamps)
		ProgramBasic.SendUniform("lightPoint.on", 0);
		ProgramBasic.SendUniform("lightPoint1.on", 0);
		ProgramBasic.SendUniform("lightPoint2.on", 0);
		ProgramBasic.SendUniform("lightPoint3.on", 0);
		ProgramBasic.SendUniform("materialSpecular", 0.3, 0.3, 0.7);
		ProgramBasic.SendUniform("shininess", 3.0);
		ProgramBasic.SendUniform("att_quadratic", 0.5);

		ProgramTerrain.SendUniform("lightPoint.on", 0);
		ProgramTerrain.SendUniform("lightPoint1.on", 0);
		ProgramTerrain.SendUniform("lightPoint2.on", 0);
		ProgramTerrain.SendUniform("lightPoint3.on", 0);
		ProgramTerrain.SendUniform("materialSpecular", 0.3, 0.3, 0.7);
		ProgramTerrain.SendUniform("shininess", 3.0);
		ProgramTerrain.SendUniform("att_quadratic", 0.5);
		ProgramTerrain.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);
		ProgramTerrain.SendUniform("materialAmbient", 0.1, 0.1, 0.1);

		//SpotLight
		ProgramBasic.SendUniform("lightSpot.on", 0);
		ProgramBasic.SendUniform("lightSpot1.on", 0);
		ProgramTerrain.SendUniform("lightSpot.on", 0);
		ProgramTerrain.SendUniform("lightSpot1.on", 0);

		//Render Skybox
		ProgramBasic.SendUniform("materialAmbient", 1.0, 1.0, 1.0);
		ProgramBasic.SendUniform("materialDiffuse", 0.0, 0.0, 0.0);
		m = matrixView;
		skybox.render(m);
		ProgramBasic.SendUniform("lightAmbient.color", 0.4, 0.4, 0.4);

		//Fog Colour and Density
		ProgramBasic.SendUniform("fogColor", 1.0, 1.0, 1.0);
		ProgramBasic.SendUniform("fogDensity", 0.0);
		ProgramWater.SendUniform("fogColor", 0.2, 0.2, 0.35);
		ProgramWater.SendUniform("fogDensity", 0.08);
		ProgramTerrain.SendUniform("fogColor", 0.2, 0.2, 0.35);
		ProgramTerrain.SendUniform("fogDensity", 0.08);
		ProgramTerrain.SendUniform("fogColorT", 1.0, 1.0, 1.0);
		ProgramTerrain.SendUniform("fogDensityT", 0.0);

	} else if (dayLight == 0) //Nighttime
	{
		//setup switches to the lightss
		ProgramBasic.SendUniform("lightAmbient.on", ambientLight);
		ProgramBasic.SendUniform("lightAmbient1.on", ambientLight);
		ProgramBasic.SendUniform("lightDir.on", directionalLight);
		ProgramBasic.SendUniform("lightPoint.on", pointLight);
		ProgramBasic.SendUniform("lightPoint1.on", pointLight);
		ProgramBasic.SendUniform("lightPoint2.on", pointLight);
		ProgramBasic.SendUniform("lightPoint3.on", pointLight);

		//setup switches to the lights (Terrain Shader)
		ProgramTerrain.SendUniform("lightAmbient.on", ambientLight);
		ProgramTerrain.SendUniform("lightAmbient1.on", ambientLight);
		ProgramTerrain.SendUniform("lightDir.on", directionalLight);
		ProgramTerrain.SendUniform("lightPoint.on", pointLight);
		ProgramTerrain.SendUniform("lightPoint1.on", pointLight);
		ProgramTerrain.SendUniform("lightPoint2.on", pointLight);
		ProgramTerrain.SendUniform("lightPoint3.on", pointLight);

		//Daylight
		ProgramBasic.SendUniform("lightAmbient.on", 0);
		ProgramBasic.SendUniform("lightAmbient.color", 0.0, 0.0, 0.0);

		ProgramTerrain.SendUniform("lightAmbient.on", 0);
		ProgramTerrain.SendUniform("lightAmbient.color", 0.0, 0.0, 0.0);

		//Bulb Lights On
		ProgramBasic.SendUniform("lightAmbient1.on", 1);
		ProgramBasic.SendUniform("lightAmbient1.color", 5.0, 5.0, 5.0);

		ProgramTerrain.SendUniform("lightAmbient1.on", 1);
		ProgramTerrain.SendUniform("lightAmbient1.color", 1.0, 1.0, 1.0);

		//Directional Light
		ProgramBasic.SendUniform("lightDir.on", 0);
		ProgramBasic.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
		ProgramBasic.SendUniform("lightDir.diffuse", 0.7, 0.7, 0.7);	  // dimmed white light

		ProgramTerrain.SendUniform("lightDir.on", 0);
		ProgramTerrain.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
		ProgramTerrain.SendUniform("lightDir.diffuse", 0.7, 0.7, 0.7);	  // dimmed white light

			////PointLight (Lamps)
		ProgramBasic.SendUniform("lightPoint.on", 1);
		ProgramBasic.SendUniform("lightPoint.position", -16.5, 7.2, 0.0); //lamp1
		ProgramBasic.SendUniform("lightPoint.diffuse", 0.5, 0.5, 0.5);
		ProgramBasic.SendUniform("lightPoint.specular", 1.0, 1.0, 1.0);
		ProgramBasic.SendUniform("lightPoint1.on", 1);
		ProgramBasic.SendUniform("lightPoint1.position", -14.3, 8.3, 22.0); //lamp2
		ProgramBasic.SendUniform("lightPoint1.diffuse", 0.5, 0.5, 0.5);
		ProgramBasic.SendUniform("lightPoint1.specular", 1.0, 1.0, 1.0);
		ProgramBasic.SendUniform("lightPoint2.on", 1);
		ProgramBasic.SendUniform("lightPoint2.position", -14.3, 9.0, -14.0); //lamp3
		ProgramBasic.SendUniform("lightPoint2.diffuse", 0.5, 0.5, 0.5);
		ProgramBasic.SendUniform("lightPoint2.specular", 1.0, 1.0, 1.0);
		ProgramBasic.SendUniform("lightPoint3.on", 1);
		ProgramBasic.SendUniform("lightPoint3.position", 14.9, 8.1, -2.0); //lamp4
		ProgramBasic.SendUniform("lightPoint3.diffuse", 0.5, 0.5, 0.5);
		ProgramBasic.SendUniform("lightPoint3.specular", 1.0, 1.0, 1.0);
		ProgramBasic.SendUniform("materialSpecular", 0.0, 0.0, 0.0);
		ProgramBasic.SendUniform("shininess", 3.0);
		ProgramBasic.SendUniform("att_quadratic", 0.5);

		ProgramTerrain.SendUniform("lightPoint.on", 1);
		ProgramTerrain.SendUniform("lightPoint.position", -16.5, 7.2, 0.0); //lamp1
		ProgramTerrain.SendUniform("lightPoint.diffuse", 0.5, 0.5, 0.5);
		ProgramTerrain.SendUniform("lightPoint.specular", 1.0, 1.0, 1.0);
		ProgramTerrain.SendUniform("lightPoint1.on", 1);
		ProgramTerrain.SendUniform("lightPoint1.position", -14.3, 8.3, 22.0); //lamp2
		ProgramTerrain.SendUniform("lightPoint1.diffuse", 1.5, 1.5, 1.5);
		ProgramTerrain.SendUniform("lightPoint1.specular", 1.0, 1.0, 1.0);
		ProgramTerrain.SendUniform("lightPoint2.on", 1);
		ProgramTerrain.SendUniform("lightPoint2.position", -14.3, 9.0, -14.0); //lamp3
		ProgramTerrain.SendUniform("lightPoint2.diffuse", 1.5, 1.5, 1.5);
		ProgramTerrain.SendUniform("lightPoint2.specular", 1.0, 1.0, 1.0);
		ProgramTerrain.SendUniform("lightPoint3.on", 1);
		ProgramTerrain.SendUniform("lightPoint3.position", 14.9, 8.1, -2.0); //lamp4
		ProgramTerrain.SendUniform("lightPoint3.diffuse", 0.5, 0.5, 0.5);
		ProgramTerrain.SendUniform("lightPoint3.specular", 1.0, 1.0, 1.0);
		ProgramTerrain.SendUniform("materialSpecular", 0.0, 0.0, 0.0);
		ProgramTerrain.SendUniform("shininess", 3.0);
		ProgramTerrain.SendUniform("att_quadratic", 0.5);

		//SpotLight
		ProgramBasic.SendUniform("lightSpot.on", 1);
		ProgramBasic.SendUniform("lightSpot.diffuse", 4.0, 2.7, 0.0);
		ProgramBasic.SendUniform("lightSpot.specular", 1.0, 1.0, 1.0);
		ProgramBasic.SendUniform("lightSpot.direction", 0.0, -0.1, 1.0);
		ProgramBasic.SendUniform("lightSpot.cutoff", 15.0);
		ProgramBasic.SendUniform("lightSpot.attenuation", 3.0);
		ProgramTerrain.SendUniform("lightSpot.on", 1);
		ProgramTerrain.SendUniform("lightSpot.diffuse", 4.0, 2.7, 0.0);
		ProgramTerrain.SendUniform("lightSpot.specular", 1.0, 1.0, 1.0);
		ProgramTerrain.SendUniform("lightSpot.direction", 0.0, -0.1, 1.0);
		ProgramTerrain.SendUniform("lightSpot.cutoff", 15.0);
		ProgramTerrain.SendUniform("lightSpot.attenuation", 3.0);


		//SpotLight
		ProgramBasic.SendUniform("lightSpot1.on", 1);
		ProgramBasic.SendUniform("lightSpot1.diffuse", 4.0, 2.7, 0.0);
		ProgramBasic.SendUniform("lightSpot1.specular", 1.0, 1.0, 1.0);
		ProgramBasic.SendUniform("lightSpot1.direction", 0.0, -0.1, 1.0);
		ProgramBasic.SendUniform("lightSpot1.cutoff", 15.0);
		ProgramBasic.SendUniform("lightSpot1.attenuation", 7.0);
		ProgramTerrain.SendUniform("lightSpot1.on", 1);
		ProgramTerrain.SendUniform("lightSpot1.diffuse", 4.0, 2.7, 0.0);
		ProgramTerrain.SendUniform("lightSpot1.specular", 1.0, 1.0, 1.0);
		ProgramTerrain.SendUniform("lightSpot1.direction", 0.0, -0.1, 1.0);
		ProgramTerrain.SendUniform("lightSpot1.cutoff", 15.0);
		ProgramTerrain.SendUniform("lightSpot1.attenuation", 7.0);

		// render the nightbox
		ProgramBasic.SendUniform("materialAmbient", 0.1, 0.1, 0.3);
		ProgramBasic.SendUniform("materialDiffuse", 0.0, 0.0, 0.0);
		m = matrixView;
		nightbox.render(m);
		
		//Set map to be very dark
		ProgramBasic.SendUniform("materialDiffuse", 0.12, 0.12, 0.12);
		ProgramBasic.SendUniform("materialAmbient", 0.1, 0.1, 0.1);
		ProgramTerrain.SendUniform("materialDiffuse", 0.12, 0.12, 0.12);
		ProgramTerrain.SendUniform("materialAmbient", 0.1, 0.1, 0.1);

		////Fog Colour and Density
		ProgramBasic.SendUniform("fogColor", 0.2, 0.2, 0.35);
		ProgramBasic.SendUniform("fogDensity", 0.08);
		ProgramWater.SendUniform("fogColor", 0.2, 0.2, 0.35);
		ProgramWater.SendUniform("fogDensity", 0.15);
		ProgramTerrain.SendUniform("fogColour", 0.2, 0.2, 0.35);
		ProgramTerrain.SendUniform("fogDensity", 0.15);
		ProgramTerrain.SendUniform("fogColorT", 0.2, 0.2, 0.35);
		ProgramTerrain.SendUniform("fogDensityT", 0.1);
	}

	// Render Particle Animation Time
	ProgramParticle.SendUniform("time", glutGet(GLUT_ELAPSED_TIME) / 1000.f - 2);

	// render the terrain
	ProgramTerrain.Use();
	//Textures For the Lower Half of the Terrain
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexGround);
	ProgramTerrain.SendUniform("textureBed", 1);
	//Textures above the water level
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, idTexSand);
	ProgramTerrain.SendUniform("textureShore", 2);
	//Normal Map Texture for above the water level
	//Shore
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, idTexNormalSand);
	ProgramTerrain.SendUniform("normalShore", 3);
	//Bed
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, idTexNormalBed);
	ProgramTerrain.SendUniform("normalBed", 4);
	m = matrixView;
	m = translate(matrixView, vec3(0, 0, 0));
	terrain.render(m);

	// render the road
	//Apply Road Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexRoad);
	//Road Placement
	m = translate(matrixView, vec3(0, 0, 0));
	m = translate(m, vec3(-15.0f, 0.01f, 0.0f));
	ProgramBasic.SendUniform("materialDiffuse", 0.12, 0.12, 0.12);
	road.render(m);


	//Bulb
	ProgramBasic.Use();
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	m = matrixView;
	m = translate(m, vec3(-16.5f, 8.38f, 0.0f));
	m = scale(m, vec3(0.23f, 0.23f, 0.23f));
	ProgramBasic.SendUniform("materialAmbient", 1.0, 1.0, 1.0);
	ProgramBasic.SendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);


	//Lamp Middle Lamp
	m = matrixView;
	m = translate(m, vec3(-16.5f, 6.75f, 0.0f));
	m = rotate(m, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.013f, 0.013f, 0.013f));
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);
	ProgramBasic.SendUniform("materialDiffuse", 0.1, 0.1, 0.1);
	lamp.render(m);


	////Bulb 2
	ProgramBasic.Use();
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	m = matrixView;
	m = translate(m, vec3(-14.3f, 9.28f, 22.0f));
	m = scale(m, vec3(0.23f, 0.23f, 0.23f));
	ProgramBasic.SendUniform("materialAmbient", 1.0, 1.0, 1.0);
	ProgramBasic.SendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);

	//Lamp2 Top of the Hill
	m = matrixView;
	m = translate(m, vec3(-14.3f, 7.65f, 22.0f));
	m = scale(m, vec3(0.013f, 0.013f, 0.013f));
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);
	ProgramBasic.SendUniform("materialDiffuse", 0.1, 0.1, 0.1);
	lamp.render(m);

	////Bulb 3
	ProgramBasic.Use();
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	m = matrixView;
	m = translate(m, vec3(-14.3f, 9.86f, -14.0f));
	m = scale(m, vec3(0.23f, 0.23f, 0.23f));
	ProgramBasic.SendUniform("materialAmbient", 1.0, 1.0, 1.0);
	ProgramBasic.SendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);

	//Lamp3 Bottom of Hill
	m = matrixView;
	m = translate(m, vec3(-14.3f, 8.23f, -14.0f));
	m = rotate(m, radians(10.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.013f, 0.013f, 0.013f));
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);
	ProgramBasic.SendUniform("materialDiffuse", 0.1, 0.1, 0.1);
	lamp.render(m);

	//Bulb 4
	ProgramBasic.Use();
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	m = matrixView;
	m = translate(m, vec3(14.9f, 9.285f, -2.0f));
	m = scale(m, vec3(0.23f, 0.23f, 0.23f));
	ProgramBasic.SendUniform("materialAmbient", 1.0, 1.0, 1.0);
	ProgramBasic.SendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);

	//Lamp 4 Right Side of Terrain
	m = matrixView;
	m = translate(m, vec3(14.9f, 7.655f, -2.0f));
	m = rotate(m, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.013f, 0.013f, 0.013f));
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);
	ProgramBasic.SendUniform("materialDiffuse", 0.1, 0.1, 0.1);
	lamp.render(m);

	////Snowman
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexSnowman);
	ProgramBasic.SendUniform("texture0", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexSnowmanNormal);
	ProgramBasic.SendUniform("textureNormal", 1);
	m = matrixView;
	m = translate(m, vec3(14.9f, 7.655f, -1.5f));
	m = rotate(m, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.7f, 0.7f, 0.7f));
	ProgramBasic.SendUniform("materialAmbient", 0.015, 0.015, 0.015);
	ProgramBasic.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);
	snowman.render(m);
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);

		////Santa Sleigh
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexSleigh);
	float z = -32 + (glutGet(GLUT_ELAPSED_TIME) % (64 * 200)) / 200.f;
	float h1 = terrain.getInterpolatedHeight(-15.5, z - 1);
	float h2 = terrain.getInterpolatedHeight(-15.5, z + 1);
	m = matrixView;
	m = translate(m, vec3(-15.5f, (h1 + h2) / 2 + 0.14, z));
	m = rotate(m, (float)atan2(h1 - h2, 2), vec3(1.0f, 0.0f, 0.0f));
	mat4 m1 = m;
	m = rotate(m, radians(0.0f), vec3(0, 1, 0));
	m = scale(m, vec3(0.0045, 0.0045, 0.0045));
	ProgramBasic.SendUniform("materialAmbient", 0.015, 0.015, 0.015);
	santasleigh.render(m);
	ProgramBasic.SendUniform("materialAmbient", 0.0, 0.0, 0.0);

	//// render the headlights
	m = m1;
	m = scale(m, vec3(2.0f, 2.0f, 2.0f));
	m = translate(m, vec3(0.12f, 0.28f, 0.68f));
	ProgramBasic.SendUniform("materialAmbient", 1.0, 7.0, 0.0);
	ProgramBasic.SendUniform("matrixModelView", m);
	ProgramBasic.SendUniform("lightSpot.matrix", m);
	glutSolidSphere(0.03, 42, 42);

	m = translate(m, vec3(-0.24f, 0.0f, 0.0f));
	ProgramBasic.SendUniform("matrixModelView", m);
	ProgramBasic.SendUniform("lightSpot1.matrix", m);
	glutSolidSphere(0.03, 42, 42);


	// send the animation time to shaders
	ProgramWater.SendUniform("t", glutGet(GLUT_ELAPSED_TIME) / 1000.f);


	// render the water
	m = matrixView;
	m = translate(m, vec3(0, waterLevel, 0));
	m = scale(m, vec3(0.5f, 1.0f, 0.5f));
	ProgramTerrain.SendUniform("matrixModelView", m);
	ProgramWater.SendUniform("matrixModelView", m);
	water.render(m);

	///////////////////////////////////
// TO DO: RENDER THE PARTICLE SYSTEM

// particles
	glDepthMask(GL_FALSE);				// disable depth buffer updates
	glActiveTexture(GL_TEXTURE0);			// choose the active texture
	glBindTexture(GL_TEXTURE_2D, idTexSnow);	// bind the texture

	ProgramParticle.Use();
	m = matrixView;
	m = translate(m, vec3(5.0, 50.0, 0.0));
	m = scale(m, vec3(10.2f, 5.2f, 10.2f));
	ProgramParticle.SendUniform("matrixModelView", m);

	// render the buffer
	glEnableVertexAttribArray(0);	// velocity
	glEnableVertexAttribArray(1);	// start time
	glEnableVertexAttribArray(2);	// position
	glBindBuffer(GL_ARRAY_BUFFER, idBufferVelocity);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferStartTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferPosition);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_POINTS, 0, NPARTICLES);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);



	glDepthMask(GL_TRUE);		// don't forget to switch the depth test updates back on


	// the camera must be moved down by terrainY to avoid unwanted effects
	matrixView = translate(matrixView, vec3(0, -terrainY, 0));

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void reshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(60.f), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	ProgramBasic.SendUniform("matrixProjection", matrixProjection);

	mat4 m = perspective(radians(60.f), ratio, 0.02f, 1000.f);
	ProgramBasic.SendUniform("matrixProjection", m);
	ProgramParticle.SendUniform("matrixProjection", m);
	ProgramWater.SendUniform("matrixProjection", m);
	ProgramTerrain.SendUniform("matrixProjection", m);
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;
	case 'n': dayLight = 1 - dayLight; break;
	//case 'k': directionalLight = 1 - directionalLight; break;
	//case 'l': ambientLight = 1 - ambientLight; break;
	//case ';': pointLight = 1 - pointLight; break;
	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': cam.z = 0; break;
	case 'a':
	case 'd': cam.x = 0; break;
	case 'q':
	case 'e': cam.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
bool bJustClicked = false;
void onMouse(int button, int state, int x, int y)
{
	bJustClicked = (state == GLUT_DOWN);
	glutSetCursor(bJustClicked ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
}

// handle mouse move
void onMotion(int x, int y)
{
	if (bJustClicked)
		bJustClicked = false;
	else
	{
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// find delta (change to) pan & tilt
		float deltaPan = 0.25f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
		float deltaTilt = 0.25f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// View = Tilt * DeltaPan * Tilt^-1 * DeltaTilt * View;
		angleTilt += deltaTilt;
		mat4 m = mat4(1.f);
		m = rotate(m, radians(angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaPan), vec3(0.f, 1.f, 0.f));
		m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaTilt), vec3(1.f, 0.f, 0.f));
		matrixView = m * matrixView;
	}
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CI5520 3D Graphics Programming");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "GLEW Error: " << glewGetErrorString(err) << endl;
		return 0;
	}
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	// register callbacks
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);

	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Version: " << glGetString(GL_VERSION) << endl;

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		cerr << "Application failed to initialise" << endl;
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	done();

	return 1;
}

