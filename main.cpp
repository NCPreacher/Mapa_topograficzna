#include <iostream>
#include <vector>

#include <dirent.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "code/shader.hpp"
#include "code/camera.hpp"
#include "code/terrain.hpp"

//////////////////////////////////////////////////////////////////////////////

using namespace glm;
using namespace std;

GLFWwindow* window;

vector<string> files;
vector<Terrain> map;

float fieldOfView = 45;

static double limitFPS = 1.0 / 60.0;

double lastTime = glfwGetTime(), timer = lastTime;
double deltaTime = 0, nowTime = 0;
int frames = 0 , updates = 0, ticks = 0;

double triangles = 0;
int currentLOD = 1;

void init();
void checkFiles();
void frameCounter();
void createWindow();
void generateTerrain();

void changeLOD( int LOD );
void adjustWindow( GLFWwindow *window, int newWidth, int newHeight );
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );
void keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );

//////////////////////////////////////////////////////////////////////////////

int main( void )
{
	init();

	GLuint VertexArrayID;
	glGenVertexArrays( 1, &VertexArrayID );
	glBindVertexArray( VertexArrayID );

	GLuint ObjectShader = LoadShaders( "./shaders/object.vertexshader", "./shaders/object.fragmentshader" );
	GLuint ObjectTextureID  = glGetUniformLocation( ObjectShader, "myTextureSampler" );
	GLuint ObjectProjectionID = glGetUniformLocation( ObjectShader, "projection" );
	GLuint ObjectModelMatrixID = glGetUniformLocation( ObjectShader, "model" );
	GLuint ObjectViewMatrixID = glGetUniformLocation( ObjectShader, "view" );

	checkFiles();
	generateTerrain();

	Camera outsideCamera( {-16.0, 51.0, -3.0}, 0, 0 );

	mat4 ViewProjectionMatrix;
	mat4 ProjectionMatrix;
	mat4 ModelMatrix;
	mat4 ViewMatrix;

	while( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose( window ) == 0 )
	{
		frameCounter();

		glClear( GL_COLOR_BUFFER_BIT );

		outsideCamera.update();

		ProjectionMatrix = outsideCamera.getProjectionMatrix();
		ViewMatrix = outsideCamera.getViewMatrix();

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

		glUseProgram( ObjectShader );
		glUniform1i( ObjectTextureID, 0 );

		glUniformMatrix4fv( ObjectProjectionID, 1, GL_FALSE, &ProjectionMatrix[0][0] );
		glUniformMatrix4fv( ObjectViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0] );
		
		for( int i = 0; i < map.size(); i++ )
		{
			map[i].draw( VertexArrayID, ObjectModelMatrixID, ModelMatrix );
		}

		glfwSwapBuffers( window );
		glfwPollEvents();
	}

	glDeleteProgram( ObjectShader );
	glDeleteVertexArrays( 1, &VertexArrayID );
	glDeleteVertexArrays( 1, &VertexArrayID );

	glfwTerminate();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void init()
{	
	srand (time(NULL));

	if( !glfwInit() ) cout << "Failed to initialize GLFW\n";
	
	createWindow();

	if ( glewInit() != GLEW_OK ) cout << "Failed to initialize GLEW\n";
	
	glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetKeyCallback(window, keyCallback);

	glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);
}

void checkFiles()
{
	struct dirent* plik;
    DIR* sciezka;
   
    if( ( sciezka = opendir( "./data" ) ) )
	{
        while( ( plik = readdir( sciezka ) ) )
		{
			if( plik -> d_name != string( ".." ) && plik -> d_name != string( "." ) )
			{
				files.push_back( plik -> d_name );
			}
		}

		closedir( sciezka );

		for( int i = 0; i < files.size(); i++ ) { files[i].insert(0, "data/"); }
    }
}

void frameCounter()
{
	nowTime = glfwGetTime();
	deltaTime += ( nowTime - lastTime ) / limitFPS;
    lastTime = nowTime;
    
    while ( deltaTime >= 1.0 )
    {
		if( frames < 10 && ticks == 0 && currentLOD < 4 )
		{
			triangles = 0;
			currentLOD++;

			for( int i = 0; i < map.size(); i++ )
			{
				map[i].setLOD( currentLOD );
				map[i].setMesh();

				triangles += map[i].getTrianglesAmount();
			}

			ticks = 100;
		}
        
		ticks--;
        updates++;
        deltaTime--;
    }
    
    frames++;

    if ( glfwGetTime() - timer > 1.0 )
    {
        timer ++;

		cout << "FPS: " << frames << "	  ";
		cout << "Updates: " << updates << "	 ";
		cout << "LOD: " << currentLOD << "	   ";
		cout << "Rendered triangles: " << triangles << "\n";

        updates = 0, frames = 0;
    }
}

void createWindow()
{
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1024, 768, "Map", NULL, NULL );

	if( window == NULL )
	{
		cout << "Failed to open GLFW window.\n";
		glfwTerminate( );
	}

	glfwMakeContextCurrent( window );
	
	glfwSetWindowSizeCallback(window, adjustWindow);
	glfwSetScrollCallback(window, scroll_callback);
	
	glClearColor( 0.0f, 0.0f, 1.0f, 0.0f );
}

void generateTerrain()
{
	map.reserve( files.size() );

	for( int i = 0; i < files.size(); i++ )
	{
		map.emplace_back( files[i] );
	}
}

void changeLOD( int LOD )
{
	currentLOD = LOD;
	triangles = 0;
	
	for( int i = 0; i < map.size(); i++ )
	{
		map[i].setLOD( currentLOD );
		map[i].setMesh();

		triangles += map[i].getTrianglesAmount();
	}
}

void adjustWindow( GLFWwindow *window, int newWidth, int newHeight )
{	
	glfwGetFramebufferSize(window, &newWidth, &newHeight);
	glViewport(0, 0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(-1 * newWidth/newHeight, 1 * newWidth/newHeight, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void scroll_callback( GLFWwindow* window, double xoffset, double yoffset )
{
	if( yoffset > 0 && fieldOfView < 80 ) { fieldOfView += yoffset; }
	if( yoffset < 0 && fieldOfView > 45 ) { fieldOfView += yoffset; }
}

void keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods )
{
	if ( key == GLFW_KEY_1 && action == GLFW_PRESS ) { changeLOD( 1 ); }
	if ( key == GLFW_KEY_2 && action == GLFW_PRESS ) { changeLOD( 2 ); }
	if ( key == GLFW_KEY_3 && action == GLFW_PRESS ) { changeLOD( 3 ); }
	if ( key == GLFW_KEY_4 && action == GLFW_PRESS ) { changeLOD( 4 ); }
}