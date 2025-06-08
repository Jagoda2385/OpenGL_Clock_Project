#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace Models;

void ModelObj::loadFromOBJ(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open OBJ file: " << filename << std::endl;
		return;
	}

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec2> temp_texCoords;

	std::string line;
	while (getline(file, line)) {
		std::istringstream ss(line);
		std::string prefix;
		ss >> prefix;
		if (prefix == "v") {
			glm::vec3 v;
			ss >> v.x >> v.y >> v.z;
			temp_vertices.push_back(v);
		}
		else if (prefix == "vn") {
			glm::vec3 n;
			ss >> n.x >> n.y >> n.z;
			temp_normals.push_back(n);
		}
		else if (prefix == "vt") {
			glm::vec2 t;
			ss >> t.x >> t.y;
			temp_texCoords.push_back(t);
		}
		else if (prefix == "f") {
			std::string v1, v2, v3;
			ss >> v1 >> v2 >> v3;
			std::vector<std::string> vertices_str = { v1, v2, v3 };

			for (std::string& v : vertices_str) {
				int vi = 0, ti = 0, ni = 0;
				size_t pos1 = v.find('/');
				size_t pos2 = v.find('/', pos1 + 1);

				if (pos1 == std::string::npos) {
					vi = std::stoi(v);
				}
				else {
					vi = std::stoi(v.substr(0, pos1));
					if (pos2 == std::string::npos) {
						ti = std::stoi(v.substr(pos1 + 1));
					}
					else {
						std::string tex = v.substr(pos1 + 1, pos2 - pos1 - 1);
						std::string norm = v.substr(pos2 + 1);
						if (!tex.empty()) ti = std::stoi(tex);
						if (!norm.empty()) ni = std::stoi(norm);
					}
				}

				if (vi > 0 && vi <= temp_vertices.size())
					vertices.push_back(temp_vertices[vi - 1]);
				if (ti > 0 && ti <= temp_texCoords.size())
					texCoords.push_back(temp_texCoords[ti - 1]);
				if (ni > 0 && ni <= temp_normals.size())
					normals.push_back(temp_normals[ni - 1]);
			}
		}

	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

void ModelObj::draw() {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}
