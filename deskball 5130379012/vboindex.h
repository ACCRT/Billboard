#pragma once
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <GL/glut.h>

void indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,

	std::vector<unsigned int> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
	);

void indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec3> & in_uvs,
	std::vector<glm::vec3> & in_normals,

	std::vector<unsigned int> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec3> & out_uvs,
	std::vector<glm::vec3> & out_normals
	);