
#include "Mesh.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

Mesh::Mesh()
	:mLoaded(false), mVAO(0), mVBO(0)
{
}

Mesh::~Mesh()
{
	if (mVAO) glDeleteVertexArrays(1, &mVAO);
	if (mVBO) glDeleteBuffers(1, &mVBO);
}

bool Mesh::loadOBJ(const std::string& filename)
{
	if (filename.find(".obj") == std::string::npos)
		return false;

	FILE* file = fopen(filename.c_str(), "rb");
	if (!file)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		return false;
	}

	std::cout << "Loading OBJ file " << filename << " ..." << std::endl;

	const size_t bufSize = 1024 * 1024;
	char* streamBuf = new char[bufSize];
	setvbuf(file, streamBuf, _IOFBF, bufSize);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUVs;
	std::vector<glm::vec3> tempNormals;

	tempVertices.reserve(120000);
	tempUVs.reserve(120000);
	tempNormals.reserve(120000);
	vertexIndices.reserve(300000);
	uvIndices.reserve(300000);
	normalIndices.reserve(300000);

	char line[1024];
	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == 'v')
		{
			if (line[1] == ' ' || line[1] == '\t')
			{
				glm::vec3 vertex;
				if (sscanf(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z) >= 3)
					tempVertices.push_back(vertex);
			}
			else if (line[1] == 't' && (line[2] == ' ' || line[2] == '\t'))
			{
				glm::vec2 uv;
				if (sscanf(line + 3, "%f %f", &uv.x, &uv.y) >= 2)
					tempUVs.push_back(uv);
			}
			else if (line[1] == 'n' && (line[2] == ' ' || line[2] == '\t'))
			{
				glm::vec3 normal;
				if (sscanf(line + 3, "%f %f %f", &normal.x, &normal.y, &normal.z) >= 3)
					tempNormals.push_back(glm::normalize(normal));
			}
		}
		else if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t'))
		{
			char* ptr = line + 2;
			int v[4]  = {0, 0, 0, 0};
			int vt[4] = {0, 0, 0, 0};
			int vn[4] = {0, 0, 0, 0};
			int count = 0;

			while (*ptr && count < 4)
			{
				while (*ptr == ' ' || *ptr == '\t') ptr++;
				if (*ptr == '\0' || *ptr == '\r' || *ptr == '\n') break;

				int vi = 0, ti = 0, ni = 0;
				if (sscanf(ptr, "%d/%d/%d", &vi, &ti, &ni) == 3) {

				} else if (sscanf(ptr, "%d//%d", &vi, &ni) == 2) {
					ti = 0;
				} else if (sscanf(ptr, "%d/%d", &vi, &ti) == 2) {
					ni = 0;
				} else if (sscanf(ptr, "%d", &vi) == 1) {
					ti = 0; ni = 0;
				} else {
					break;
				}

				v[count]  = vi;
				vt[count] = ti;
				vn[count] = ni;
				count++;

				while (*ptr && *ptr != ' ' && *ptr != '\t' && *ptr != '\r' && *ptr != '\n') ptr++;
			}

			if (count >= 3)
			{

				vertexIndices.push_back(v[0]); vertexIndices.push_back(v[1]); vertexIndices.push_back(v[2]);
				uvIndices.push_back(vt[0]);     uvIndices.push_back(vt[1]);     uvIndices.push_back(vt[2]);
				normalIndices.push_back(vn[0]); normalIndices.push_back(vn[1]); normalIndices.push_back(vn[2]);

				if (count == 4)
				{
					vertexIndices.push_back(v[0]); vertexIndices.push_back(v[2]); vertexIndices.push_back(v[3]);
					uvIndices.push_back(vt[0]);     uvIndices.push_back(vt[2]);     uvIndices.push_back(vt[3]);
					normalIndices.push_back(vn[0]); normalIndices.push_back(vn[2]); normalIndices.push_back(vn[3]);
				}
			}
		}
	}

	fclose(file);
	delete[] streamBuf;

	mVertices.clear();
	mVertices.reserve(vertexIndices.size());

	for (size_t i = 0; i < vertexIndices.size(); i++)
	{
		Vertex meshVertex;
		meshVertex.position  = glm::vec3(0.0f);
		meshVertex.normal    = glm::vec3(0.0f, 1.0f, 0.0f);
		meshVertex.texCoords = glm::vec2(0.0f);

		if (vertexIndices[i] > 0 && (vertexIndices[i] - 1) < tempVertices.size())
			meshVertex.position = tempVertices[vertexIndices[i] - 1];

		if (normalIndices[i] > 0 && (normalIndices[i] - 1) < tempNormals.size())
			meshVertex.normal = tempNormals[normalIndices[i] - 1];

		if (uvIndices[i] > 0 && (uvIndices[i] - 1) < tempUVs.size())
			meshVertex.texCoords = tempUVs[uvIndices[i] - 1];

		mVertices.push_back(meshVertex);
	}

	initBuffers();
	return (mLoaded = true);
}

void Mesh::initBuffers()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.empty() ? nullptr : &mVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::draw()
{
	if (!mLoaded) return;
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mVertices.size());
	glBindVertexArray(0);
}
