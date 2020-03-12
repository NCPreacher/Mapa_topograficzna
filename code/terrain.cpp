#include "./terrain.hpp"

#include <fstream>

#include <string.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

Terrain::Terrain( string filePath )
{
	readFile( filePath );
	setMesh();

	mat4 rotation = rotate( mat4( 1.0 ), radians( 180.0f ), vec3( 1.0f, 0.0f, 0.0f ) );
	baseMatrix = rotate( rotation, radians( 90.0f ), vec3( 0.0f, 0.0f, 1.0f ) );
}

Terrain::~Terrain()
{
	glDeleteBuffers( 1, &vertexbuffer );
	glDeleteBuffers( 1, &indexbuffer );

	vertices.empty();
	indices.empty();

	for(int i = 0; i < 1201; i++) { delete[] height[i]; }

	delete[] height;
}

void Terrain::draw( GLuint &VertexArrayID, GLuint &ModelMatrixID, mat4 &ModelMatrix )
{
	ModelMatrix = translate( baseMatrix, position );

	glUniformMatrix4fv( ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0] );
	glBindVertexArray( VertexArrayID );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0 );

	glDisableVertexAttribArray( 0 );
	glBindVertexArray( 0 );
}

void Terrain::readFile( string filePath )
{
    ifstream file( filePath, ios::in|ios::binary );

    if(!file) { cout << "Error opening file!" << endl; }

    unsigned char buffer[2];

    for( int i = 0; i < 1201; ++i )
    {
        for( int j = 0; j < 1201; ++j )
        {
            if( !file.read( reinterpret_cast<char*>( buffer ), sizeof( buffer ) ) ) { cout << "Error reading file!" << endl; }
            height[i][j] = ( buffer[0] << 8 ) | buffer[1];

			if( height[i][j] == -32768) { height[i][j] = height[i][j - 1]; }
        }
    }

	int y = stoi( filePath.substr(6, 2) );
    int x = stoi( filePath.substr(9, 3) );

	if( filePath.at( 5 ) == 'S' ) { y *= -1; }
	if( filePath.at( 8 ) == 'E' ) { x *= -1; }

    position = vec3( -y , -x, 0 ); // Zamiana orientacji spowodowana obrotem
}

void Terrain::setMesh()
{
	vertices.clear();
	indices.clear();

	float wide = 1201.0;

	int index = 0;
	int offset = 0;

	for( int y = 0; y < wide; y += lod )
	{
		for( int x = 0; x < wide; x += lod )
		{
			vertices.push_back( vec3( x / wide, y / wide, height[x][y] / 9000.0f ) );
		}
	}

	if( lod == 1 )
	{
		for( int k = 1; k < wide / ( lod * lod ); k++)
		{
			index = wide * offset; // parzyste wide // parzyste lod
			//index = wide * offset + 1 * offset; //nieparzyste wide // nieparzyste lod

			for( int u = 1; u < wide / lod; u++ )
			{
				//indices.push_back( index + 0 ); // parzyste wide
				//indices.push_back( index + 1 );
				//indices.push_back( index + wide - 1 );

				indices.push_back( index + 0 ); // nieparzyste wide
				indices.push_back( index + 1 );
				indices.push_back( index + wide );

				////////////////
						
				//indices.push_back( index + wide + 1 ); // parzyste wide
				//indices.push_back( index + 1 );
				//indices.push_back( index + wide );
						
				indices.push_back( index + wide + 1 ); // nieparzyste wide
				indices.push_back( index + 1 );
				indices.push_back( index + wide + 0 );
				index++;
			}
					
			offset++;
		}
	}

	if( lod == 2 )
	{
		for( int k = 1; k < wide / ( lod * lod ); k++)
		{

			index = wide * offset + 1 * offset;

			for( int u = 1; u < wide / lod; u++ )
			{
				indices.push_back( index + 0 ); 
				indices.push_back( index + 1 );
				indices.push_back( index + wide + 1 );
						
				indices.push_back( index + wide + 1 );
				indices.push_back( index + 1 );
				indices.push_back( index + wide + 2 );
				index++;
			}
					
			offset++;
		}
	}

	if( lod == 3 )
	{
		for( int k = 1; k < wide / ( lod * lod ); k++)
		{
			index = wide * offset + 2 * offset;

			for( int u = 1; u < wide / lod; u++ )
			{
				indices.push_back( index + 0 ); 
				indices.push_back( index + 1 );
				indices.push_back( index + wide + 2 );

				indices.push_back( index + 1 ); 
				indices.push_back( index + wide + 2 );
				indices.push_back( index + wide + 3 );
						
				index++;
			}
					
			offset++;
		}
	}

	if( lod == 4 )
	{
		for( int k = 1; k < wide / ( lod * lod ); k++)
		{
			index = wide * offset + 3 * offset;

			for( int u = 1; u < wide / lod; u++ )
			{
				indices.push_back( index + 0 ); 
				indices.push_back( index + 1 );
				indices.push_back( index + wide + 3 );

				indices.push_back( index + 1 ); 
				indices.push_back( index + wide + 3 );
				indices.push_back( index + wide + 4 );
						
				index++;
			}
					
			offset++;
		}
	}


	glGenBuffers( 1, &vertexbuffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( vec3 ), &vertices[0], GL_STATIC_DRAW );

	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned int ), &indices[0] , GL_STATIC_DRAW);
}

void Terrain::setPosition( vec3 position ) { this -> position = position; }
vec3 Terrain::getPosition() { return position; }

int Terrain::getTrianglesAmount() { return indices.size() / 3; }

void Terrain::setLOD( int lod ) { this -> lod = lod; }