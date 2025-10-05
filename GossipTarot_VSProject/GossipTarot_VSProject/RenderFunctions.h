#pragma once

#include "Shader.h"
#include "Transform.h"
#include "MeshOnGpu.h"

#include "DrawElementsIndirect.h"

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

void RenderMeshOnGPUWithNumIndices(const ShaderProgram& shaderForRendering, const Mat4x4& globalTransformMatrix, const int& textureIndex, const MeshOnGPU& meshOnGPU, const unsigned int& numIndices) {

	int modelLoc = glGetUniformLocation(shaderForRendering.shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(globalTransformMatrix));

	glBindTexture(GL_TEXTURE_2D, Texture::textures[textureIndex].textureID);
	glBindVertexArray(meshOnGPU.VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

}


void RenderMeshOnGPUWithNumIndicesAndSSBO(const ShaderProgram& shaderForRendering, const Mat4x4& globalTransformMatrix, const int& textureIndex, const MeshOnGPU& meshOnGPU, const unsigned int& numIndices, const unsigned int& ssboID, const unsigned int& ssboBindingLocation) {

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssboBindingLocation, ssboID);

	int modelLoc = glGetUniformLocation(shaderForRendering.shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(globalTransformMatrix));

	glBindTexture(GL_TEXTURE_2D, Texture::textures[textureIndex].textureID);
	glBindVertexArray(meshOnGPU.VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

}

void RenderMeshOnGPUWithDrawElementsIndirectCommands(const ShaderProgram& shaderForRendering, const Mat4x4& globalTransformMatrix, const int& textureIndex, const MeshOnGPU& meshOnGPU, const IndirectDrawCommands& indirectDrawCommands, const unsigned int& ssboID, const unsigned int& ssboBindingLocation) {

	int modelLoc = glGetUniformLocation(shaderForRendering.shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(globalTransformMatrix));

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssboBindingLocation, ssboID);

	glBindTexture(GL_TEXTURE_2D, Texture::textures[textureIndex].textureID);

	glBindVertexArray(meshOnGPU.VAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectDrawCommands.gpu_drawElementsIndirectCommandsBuffer);

	glMultiDrawElementsIndirect(
		GL_TRIANGLES,
		GL_UNSIGNED_INT,												// Type of data in indicesBuffer
		(const void*)0,													// No offset into draw command buffer
		indirectDrawCommands.cpu_drawElementsIndirectCommands.size(),	// Indirect Buffer Count
		0																// No stride as data is tightly packed, for now.
	);
}
