#pragma once

#include "Shader.h"
#include "Transform.h"
#include "MeshOnGpu.h"

void RenderQuad(const ShaderProgram& shaderForRendering, const Mat4x4& quadTransformMatrix, const int& textureIndex, const MeshOnGPU& meshOnGPU) {

	int modelLoc = glGetUniformLocation(shaderForRendering.shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(quadTransformMatrix));

	glBindTexture(GL_TEXTURE_2D, Texture::textures[textureIndex].textureID);
	glBindVertexArray(meshOnGPU.VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void RenderCube(const ShaderProgram& shaderForRendering, const Mat4x4& cubeTransformMatrix, const int& textureIndex, const MeshOnGPU& meshOnGPU) {

	int modelLoc = glGetUniformLocation(shaderForRendering.shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeTransformMatrix));

	glBindTexture(GL_TEXTURE_2D, Texture::textures[textureIndex].textureID);
	glBindVertexArray(meshOnGPU.VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}


void RenderQuadInstance(const ShaderProgram& shaderForRendering, const Mat4x4& globalTransformMatrix, const int& textureIndex, const MeshOnGPU& meshOnGPU, const unsigned int& instanceCount) {

	int modelLoc = glGetUniformLocation(shaderForRendering.shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(globalTransformMatrix));

	glBindTexture(GL_TEXTURE_2D, Texture::textures[textureIndex].textureID);
	glBindVertexArray(meshOnGPU.VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanceCount);

}

void RenderCubeInstance(const ShaderProgram& shaderForRendering, const Mat4x4& globalTransformMatrix, const int& textureIndex, const MeshOnGPU& meshOnGPU, const unsigned int& instanceCount) {

	int modelLoc = glGetUniformLocation(shaderForRendering.shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(globalTransformMatrix));

	glBindTexture(GL_TEXTURE_2D, Texture::textures[textureIndex].textureID);
	glBindVertexArray(meshOnGPU.VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instanceCount);

}