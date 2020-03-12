#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Terrain
{
	GLuint vertexbuffer, indexbuffer;

	short height[1201][1201];

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;

	glm::mat4 baseMatrix;
	glm::vec3 position;

	int lod = 1;

	public:

		Terrain( std::string filePath );
		~Terrain();

		void draw( GLuint &VertexArrayID, GLuint &ModelMatrixID, glm::mat4 &ModelMatrix );
		void readFile( std::string filePath );
		void setMesh();

		void setPosition( glm::vec3 position );
		glm::vec3 getPosition();

		int getTrianglesAmount();

		void setLOD( int lod );
};