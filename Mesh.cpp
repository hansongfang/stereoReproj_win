#include "Mesh.h"

int Mesh::vertex_cb(p_ply_argument argument)
{
	long i, j;
	vector<Vertex>* ver;
	ply_get_argument_element(argument, NULL, &i);
	ply_get_argument_user_data(argument, (void**)&ver, &j);
	if (j < 3) {
		j -= PLY_PX;
		//cout << (float)ply_get_argument_value(argument) << endl;
		(*ver)[i].Position[j] = (float)ply_get_argument_value(argument);
	}
	else if (j >= 3 && j < 6) {
		j -= PLY_NX;
		//cout << (float)ply_get_argument_value(argument) << endl;
		(*ver)[i].Normal[j] = (float)ply_get_argument_value(argument);
	}
	else if (j >= 6 && j < 8) {
		j -= PLY_TX;
		//cout << (float)ply_get_argument_value(argument) << endl;
		(*ver)[i].TexCoords[j] = (float)ply_get_argument_value(argument);
	}
	else {
		cout << "not support vertex attribute " << endl;
	}
	return 1;
}

int Mesh::index_cb(p_ply_argument argument)
{
	long i, j;
	vector<GLuint>* ind;
	ply_get_argument_element(argument, NULL, &i);
	ply_get_argument_property(argument, NULL, NULL, &j);
	ply_get_argument_user_data(argument, (void**)&ind, NULL);
	if (j < 0) return 1;
	ind->push_back((GLuint)ply_get_argument_value(argument));
	return 1;
}

void Mesh::loadMesh(string meshPath)
{
	vertices.clear();
	indices.clear();
	
	// loadPly
	loadPly(meshPath);
	// this->vertices
	// this->indices
}

void Mesh::loadPly(string meshPath)
{
	int nVertices, nIndices;
	p_ply ply = ply_open(meshPath.c_str(), NULL);
	ply_read_header(ply);
	nVertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, (void*)&vertices, PLY_PX);
	ply_set_read_cb(ply, "vertex", "y", vertex_cb, (void*)&vertices, PLY_PY);
	ply_set_read_cb(ply, "vertex", "z", vertex_cb, (void*)&vertices, PLY_PZ);
	ply_set_read_cb(ply, "vertex", "nx", vertex_cb, (void*)&vertices, PLY_NX);
	ply_set_read_cb(ply, "vertex", "ny", vertex_cb, (void*)&vertices, PLY_NY);
	ply_set_read_cb(ply, "vertex", "nz", vertex_cb, (void*)&vertices, PLY_NZ);

	int numTriangles = ply_set_read_cb(ply, "face", "vertex_indices", index_cb, (void*)&indices, 0);
	nIndices = 3 * numTriangles;

	vertices.resize(nVertices);
	indices.reserve(nIndices);
	ply_read(ply);
	ply_close(ply);

	cout << "model with " << nVertices << " Vertices and " << numTriangles << " faces" << endl;

	/*cout << "check vertices value" << endl;
	cout << vertices[0].Position[0] << " " << vertices[0].Position[1] << " " << vertices[0].Position[2] << endl;
	cout << vertices[0].Normal[0] << " " << vertices[0].Normal[1] << " " << vertices[0].Normal[2] << endl;*/
	// model bounding info
	bounds();
}

void Mesh::bounds()
{
	double xMax = -std::numeric_limits<double>::max();
	double yMax = -std::numeric_limits<double>::max();
	double zMax = -std::numeric_limits<double>::max();

	double xMin = std::numeric_limits<double>::max();
	double yMin = std::numeric_limits<double>::max();
	double zMin = std::numeric_limits<double>::max();

	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	int numVerts = static_cast<int>(vertices.size());

	for (int i = 0; i < numVerts; ++i)
	{
		x = vertices[i].Position[0];
		y = vertices[i].Position[1];
		z = vertices[i].Position[2];

		if (x < xMin)
			xMin = x;

		if (x > xMax)
			xMax = x;

		if (y < yMin)
			yMin = y;

		if (y > yMax)
			yMax = y;

		if (z < zMin)
			zMin = z;

		if (z > zMax)
			zMax = z;
	}

	_left = xMin; _right = xMax;
	_down = yMin; _top = yMax;
	_near = zMin; _far = zMax;

	_center[0] = (xMin + xMax) / 2.0;
	_center[1] = (yMin + yMax) / 2.0;
	_center[2] = (zMin + zMax) / 2.0;

	if (1) {
		std::cout << "Bounding box: " << std::endl;
		cout << "x axis: " << xMin << " " << xMax << endl;
		cout << "y axis: " << yMin << " " << yMax << endl;
		cout << "z axis: " << zMin << " " << zMax << endl;
		cout << "center" << endl;
		cout << _center[0] << " " << _center[1] << " " << _center[2] << endl;
	}
}

void Mesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	/*glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));*/
	glBindVertexArray(0);
}

double Mesh::Draw(Shader shader) const
{
	shader.use();

	// query object
	GLuint64 startTime, stopTime;
	GLuint queryID[2];
	glGenQueries(2, queryID);

	glQueryCounter(queryID[0], GL_TIMESTAMP);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glQueryCounter(queryID[1], GL_TIMESTAMP);
	GLint stopTimerAvailable = 0; //get result until avaible, could be used to test latency
	while (!stopTimerAvailable) {
		glGetQueryObjectiv(queryID[1],
			GL_QUERY_RESULT_AVAILABLE,
			&stopTimerAvailable);
	}
	glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
	glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &stopTime);

	double time = (stopTime - startTime) / 1000000.0;
	return time;
}
