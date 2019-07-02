#ifndef MESH_H
#define MESH_H

#include <GL\glew.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "rply.h"
#include "util.h"

using namespace std;

// usage: Mesh a(meshPath), a.Draw(); 

// type
struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
};


class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	GLuint VAO;
	double                 _left, _right, _top, _down, _near, _far;
	glm::vec3              _center;


	/*  Functions  */
	// constructor
	Mesh(string meshPath)
	{
		cout << "load Mesh: " << meshPath << endl;
		this->loadMesh(meshPath);

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

	// render the mesh
	double Draw(Shader shader) const;
	
private:
	/*  Render data  */
	GLuint VBO, EBO;

	/*  Loading ply  */
	enum {
		PLY_PX, PLY_PY, PLY_PZ, PLY_NX, PLY_NY, PLY_NZ, PLY_TX, PLY_TY
	};
	// loading ply
	static int vertex_cb(p_ply_argument argument);
	static int index_cb(p_ply_argument argument);
	void loadPly(string meshPath);

	// compute mesh geometry info
	void bounds();

	/*  Functions    */
	// load vertices, indices
	void loadMesh(string meshPath);

	// initializes all the buffer objects/arrays
	void setupMesh();
};

#endif