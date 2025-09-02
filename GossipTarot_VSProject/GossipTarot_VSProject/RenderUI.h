#pragma once

#include <vector>

#include "GLFW/glfw3.h"

#include "GlobalConstants.h"
#include "UISimulation.h"

#include "Shader.h"
#include "Transform.h"
#include "MeshOnGpu.h"

void RenderQuad(const ShaderProgram& shaderForRendering, const Mat4x4& quadTransformMatrix, const int& textureIndex, const MeshOnGPU& meshOnGPU) {

	int modelLoc = glGetUniformLocation(shaderForRendering.shaderProgramID, "model");
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTransform.GetTransformMatrix()));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(quadTransformMatrix));

	//glBindTexture(GL_TEXTURE_2D, Texture::textures[woodContainerTextureIndex].textureID);
	glBindTexture(GL_TEXTURE_2D, Texture::textures[textureIndex].textureID);
	glBindVertexArray(meshOnGPU.VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

Mat4x4 MatrixOfUIRectToRender(const UI_Rect& uiRectToRender) {

	Vector2 uiDims = { GetUIRectWidth(uiRectToRender), GetUIRectHeight(uiRectToRender) };
	Vector3 offset = (uiRectToRender.worldEndPos - uiRectToRender.worldStartPos) * 0.5f;

	Transform transform;
	transform.position = { 0.0f, 0.0f, 0.0f };
	//transform.position = uiRectToRender.worldStartPos + Vector3{ uiDims.x, uiDims.y, 0.0f };
	transform.rotation = { 0.0f, 0.0f, 0.0f };
	transform.scale = { uiDims.x, uiDims.y, 1.0f };
	//transform.position += offset;

	transform.position = GetUIRectCentrePos(uiRectToRender);

	return transform.GetTransformMatrix();
}

Mat4x4 MatrixOfUIRectToRenderWithPositionOffset(const UI_Rect& uiRectToRender, const Vector3& positionOffset) {

	Vector2 uiDims = { GetUIRectWidth(uiRectToRender), GetUIRectHeight(uiRectToRender) };
	Vector3 offset = (uiRectToRender.worldEndPos - uiRectToRender.worldStartPos) * 0.5f;

	Transform transform;
	transform.position = { 0.0f, 0.0f, 0.0f };
	//transform.position = uiRectToRender.worldStartPos + Vector3{ uiDims.x, uiDims.y, 0.0f };
	transform.rotation = { 0.0f, 0.0f, 0.0f };
	transform.scale = { uiDims.x, uiDims.y, 1.0f };
	//transform.position += offset;

	transform.position = GetUIRectCentrePos(uiRectToRender);
	transform.position += positionOffset;

	return transform.GetTransformMatrix();
}


void RenderUIRectAndSubTree(UI_Rect& uiRect, const UI_Rect& parentUIRect, const ShaderProgram& shaderProgramForRendering, const int& textureIndex, const MeshOnGPU& meshOnGPU) {

	Vector3 start = uiRect.start;
	Vector3 end = uiRect.end;

	TransformUIPositionsToParentSpaceBasedOnAnchor(start, end, uiRect, parentUIRect);

	uiRect.worldStartPos = GetUIRectCentrePos(parentUIRect) + start;
	uiRect.worldEndPos = GetUIRectCentrePos(parentUIRect) + end;

	Mat4x4 uiRectTransformMatrix = MatrixOfUIRectToRender(uiRect);
	RenderQuad(shaderProgramForRendering, uiRectTransformMatrix, textureIndex, meshOnGPU);


	for (int i = 0; i < uiRect.children.size(); i++)
	{
		RenderUIRectAndSubTree(UI_Rect::uiRects[uiRect.children[i]], uiRect, shaderProgramForRendering, textureIndex, meshOnGPU);
	}
}

void RenderUIRoot(UI_Rect& rootUIRect, const ShaderProgram& shaderProgramForRendering, const int& textureIndex, const MeshOnGPU& meshOnGPU) {

	Vector3 start = rootUIRect.start;
	Vector3 end = rootUIRect.end;

	UI_Rect fakeScreenRect;

	fakeScreenRect.anchorPosition = MiddleMiddle;
	fakeScreenRect.start = { -WINDOW_WIDTH / 2.0f, -WINDOW_HEIGHT / 2.0f, 0.0f };
	fakeScreenRect.end = { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f };

	TransformUIPositionsToParentSpaceBasedOnAnchor(start, end, rootUIRect, fakeScreenRect);

	rootUIRect.worldStartPos = start;
	rootUIRect.worldEndPos = end;

	Mat4x4 rootUIRectTransformMatrix = MatrixOfUIRectToRender(rootUIRect);
	RenderQuad(shaderProgramForRendering, rootUIRectTransformMatrix, textureIndex, meshOnGPU);
}

void RenderUI(UI_Rect& rootUIRect, const ShaderProgram& shaderProgramForRendering, const int& textureIndex, const MeshOnGPU& meshOnGPU) {

	//RenderUIRoot(rootUIRect, shaderProgramForRendering, textureIndex, meshOnGPU);

	for (int i = 0; i < rootUIRect.children.size(); i++)
	{
		RenderUIRectAndSubTree(UI_Rect::uiRects[rootUIRect.children[i]], rootUIRect, shaderProgramForRendering, textureIndex, meshOnGPU);
	}
}