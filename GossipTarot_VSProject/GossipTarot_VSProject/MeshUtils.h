#pragma once

#include "glad/glad.h"

#include "MeshOnCPU.h"
#include "MeshOnGpu.h"

/// <summary>
/// The vertex array in MeshOnCPU must contain vertex data in the format, (vertexCoordA, vertexCoordB,  vertexCoordC,  texCoordA,  texCoordB)
/// </summary>
/// <param name="meshOnCPU"></param>
/// <param name="meshOnGPU"></param>
void CreateMeshOnGPU(const MeshOnCPU& meshOnCPU, MeshOnGPU& meshOnGPU) {

	glGenVertexArrays(1, &meshOnGPU.VAO);
	glBindVertexArray(meshOnGPU.VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, meshOnCPU.vertices.size() * sizeof(meshOnCPU.vertices[0]), meshOnCPU.vertices.data(), GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	for (int i = 0; i < MeshOnCPU::defaultVertexLayout.numVertexAttribs; i++)
	{
		glVertexAttribPointer(MeshOnCPU::defaultVertexLayout.vertexAttribsPositions[i],
								MeshOnCPU::defaultVertexLayout.vertexAttribNumComponents[i],
								GL_FLOAT,
								GL_FALSE,
								MeshOnCPU::defaultVertexLayout.vertexAttributeStride[i],
								MeshOnCPU::defaultVertexLayout.pointerToFirstAttributeInVertex[i]);

		glEnableVertexAttribArray(MeshOnCPU::defaultVertexLayout.vertexAttribsPositions[i]);
	}

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshOnCPU.indices.size() * sizeof(meshOnCPU.indices[0]), meshOnCPU.indices.data(), GL_STATIC_DRAW);

}