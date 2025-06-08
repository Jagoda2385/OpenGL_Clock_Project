#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Models {
	class ModelObj {
	public:
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned int> indices;
		GLuint vao, vbo, nbo, tbo, ebo;

		GLuint texture;

		void loadFromOBJ(const std::string& filename);
		void draw();
	};
}

#endif