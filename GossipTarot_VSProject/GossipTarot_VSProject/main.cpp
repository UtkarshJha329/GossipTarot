
#include <iostream>
#include <string>
#include <chrono>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "GlobalConstants.h"

#include "GLMIncludes.h"

#include "Transform.h"
#include "MeshUtils.h"
#include "Shader.h"
#include "TextureUtils.h"
#include "Camera.h"

#include "RenderUI.h"
#include "UISimulation.h"
#include "UIText.h"

#include "Input.h"

#include "flecs/flecs.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

float vertices[] = {
	 0.5f,  0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f
};

unsigned int indices[] = {
	0, 1, 3,
	1, 2, 3
};


float verticesWithTexCoords[] = {
	// positions			// texture coords
	 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,   // top right
	 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f    // top left 
};

void UpdateKeyStates(GLFWwindow* window) {

	//---------------------------------Keyboard Input--------------------------------------

	int wKeyState = glfwGetKey(window, GLFW_KEY_W);
	SetKeyBasedOnState(KeyCode::KEY_W, wKeyState > 0 ? PRESSED_OR_HELD : RELEASED);

	int sKeyState = glfwGetKey(window, GLFW_KEY_S);
	SetKeyBasedOnState(KeyCode::KEY_S, sKeyState > 0 ? PRESSED_OR_HELD : RELEASED);

	int dKeyState = glfwGetKey(window, GLFW_KEY_D);
	SetKeyBasedOnState(KeyCode::KEY_D, dKeyState > 0 ? PRESSED_OR_HELD : RELEASED);

	int aKeyState = glfwGetKey(window, GLFW_KEY_A);
	SetKeyBasedOnState(KeyCode::KEY_A, aKeyState > 0 ? PRESSED_OR_HELD : RELEASED);

	int pKeyState = glfwGetKey(window, GLFW_KEY_P);
	SetKeyBasedOnState(KeyCode::KEY_P, pKeyState > 0 ? PRESSED_OR_HELD : RELEASED);

	int escKeyState = glfwGetKey(window, GLFW_KEY_ESCAPE);
	SetKeyBasedOnState(KeyCode::KEY_ESCAPE, escKeyState > 0 ? PRESSED_OR_HELD : RELEASED);

	//---------------------------------------Mouse Input------------------------------------------

	int mouseButtonLeftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	SetKeyBasedOnState(KeyCode::MOUSE_BUTTON_LEFT, mouseButtonLeftState > 0 ? PRESSED_OR_HELD : RELEASED);

	int mouseButtonRightState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	SetKeyBasedOnState(KeyCode::MOUSE_BUTTON_RIGHT, mouseButtonRightState > 0 ? PRESSED_OR_HELD : RELEASED);

	mouseXFromPreviousFrame = mouseX;
	mouseYFromPreviousFrame = mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	mouseXDelta = mouseX - mouseXFromPreviousFrame;
	mouseYDelta = mouseY - mouseYFromPreviousFrame;
}

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::string woodContainerTexturePath = "Assets/Textures/WoodContainer.jpg";
	int woodContainerTextureIndex = TextureFromFile(woodContainerTexturePath);

	std::string stickmanTexturePath = "Assets/Textures/Stickman.jpg";
	int stickmanTextureIndex = TextureFromFile(stickmanTexturePath);

	std::string courierFontPath = "Assets/Fonts/cour.ttf";

	//Shader simpleVertexShader;
	//simpleVertexShader.shaderFilePath = "Assets/Shaders/SimpleShaderNoCamera/SimpleShaderNoCamera.vert";
	//simpleVertexShader.shaderType = SHADER_TYPE::VERTEX_SHADER;
	//simpleVertexShader.CreateShader();

	//Shader simpleFragmentShader;
	//simpleFragmentShader.shaderFilePath = "Assets/Shaders/SimpleShaderNoCamera/SimpleShaderNoCamera.frag";
	//simpleFragmentShader.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	//simpleFragmentShader.CreateShader();

	//std::vector<Shader> shadersForThisShaderProgram = { simpleVertexShader, simpleFragmentShader };
	//ShaderProgram simpleVertexAndFragmentNoCameraShaderProgram;

	//simpleVertexAndFragmentNoCameraShaderProgram.CreateShaderProgram(shadersForThisShaderProgram);

	//Shader simpleVertexShaderWithTextures;
	//simpleVertexShaderWithTextures.shaderFilePath = "Assets/Shaders/SimpleShaderNoCameraWithTextures/SimpleShaderNoCameraWithTextures.vert";
	//simpleVertexShaderWithTextures.shaderType = SHADER_TYPE::VERTEX_SHADER;
	//simpleVertexShaderWithTextures.CreateShader();

	//Shader simpleFragmentShaderWithTextures;
	//simpleFragmentShaderWithTextures.shaderFilePath = "Assets/Shaders/SimpleShaderNoCameraWithTextures/SimpleShaderNoCameraWithTextures.frag";
	//simpleFragmentShaderWithTextures.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	//simpleFragmentShaderWithTextures.CreateShader();

	//std::vector<Shader> shadersForThisShaderProgram = { simpleVertexShaderWithTextures, simpleFragmentShaderWithTextures };
	//ShaderProgram simpleVertexAndFragmentNoCameraWithTexturesShaderProgram;

	//simpleVertexAndFragmentNoCameraWithTexturesShaderProgram.CreateShaderProgram(shadersForThisShaderProgram);


	Shader simpleVertexShaderWithCameraWithTextures;
	simpleVertexShaderWithCameraWithTextures.shaderFilePath = "Assets/Shaders/SimpleShaderWithCameraWithTextures/SimpleShaderWithCameraWithTextures.vert";
	simpleVertexShaderWithCameraWithTextures.shaderType = SHADER_TYPE::VERTEX_SHADER;
	simpleVertexShaderWithCameraWithTextures.CreateShader();

	Shader simpleFragmentShaderWithCameraWithTextures;
	simpleFragmentShaderWithCameraWithTextures.shaderFilePath = "Assets/Shaders/SimpleShaderWithCameraWithTextures/SimpleShaderWithCameraWithTextures.frag";
	simpleFragmentShaderWithCameraWithTextures.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	simpleFragmentShaderWithCameraWithTextures.CreateShader();

	std::vector<Shader> shadersForThisShaderProgram = { simpleVertexShaderWithCameraWithTextures, simpleFragmentShaderWithCameraWithTextures };
	ShaderProgram simpleVertexAndFragmentWithCameraWithTexturesShaderProgram;

	simpleVertexAndFragmentWithCameraWithTexturesShaderProgram.CreateShaderProgram(shadersForThisShaderProgram);

	Shader textRenderingVertexShaderWithCameraWithTextures;
	textRenderingVertexShaderWithCameraWithTextures.shaderFilePath = "Assets/Shaders/TextRenderingShaderWithCameraWithTextures/TextRenderingShaderWithCameraWithTextures.vert";
	textRenderingVertexShaderWithCameraWithTextures.shaderType = SHADER_TYPE::VERTEX_SHADER;
	textRenderingVertexShaderWithCameraWithTextures.CreateShader();

	Shader textRenderingFragmentShaderWithCameraWithTextures;
	textRenderingFragmentShaderWithCameraWithTextures.shaderFilePath = "Assets/Shaders/TextRenderingShaderWithCameraWithTextures/TextRenderingShaderWithCameraWithTextures.frag";
	textRenderingFragmentShaderWithCameraWithTextures.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	textRenderingFragmentShaderWithCameraWithTextures.CreateShader();

	std::vector<Shader> shadersForTextRenderingShaderProgram = { textRenderingVertexShaderWithCameraWithTextures, textRenderingFragmentShaderWithCameraWithTextures };
	ShaderProgram textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram;

	textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram.CreateShaderProgram(shadersForTextRenderingShaderProgram);


	MeshOnCPU simpleQuadMeshCPU;
	for (int i = 0; i < 20; i++)
	{
		//simpleQuadMeshCPU.vertices.push_back(vertices[i]);
		simpleQuadMeshCPU.vertices.push_back(verticesWithTexCoords[i]);
	};

	for (int i = 0; i < 6; i++)
	{
		simpleQuadMeshCPU.indices.push_back(indices[i]);
	}

	MeshOnGPU simpleQuadMeshGPU;

	CreateMeshOnGPU(simpleQuadMeshCPU, simpleQuadMeshGPU);


	//Transform modelTransform;
	//modelTransform.position = { 0.0f, -1.0f, 0.0f };
	//modelTransform.rotation = { 90.0f, 0.0f, 0.0f };
	//modelTransform.scale = { 1.0f, 1.0f, 1.0f };

	Transform uiModelTransform;
	uiModelTransform.position = { 0.0f, 0.0f, 0.0f };
	uiModelTransform.rotation = { 0.0f, 0.0f, 0.0f };
	uiModelTransform.scale = { 100.0f, 100.0f, 100.0f };


	//Transform cameraTransform;
	//cameraTransform.position = { 0.0f, 0.0f, -3.0f };
	//cameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
	//cameraTransform.scale = { 1.0f, 1.0f, 1.0f };

	//Camera mainCamera;
	//mainCamera.viewport.dimensions = { WINDOW_WIDTH, WINDOW_HEIGHT };
	//mainCamera.SetProjectionMatrixToPerspectiveProjection(45.0f, 0.1f, 1000.0f);

	Camera uiCamera;
	uiCamera.viewport.dimensions = { WINDOW_WIDTH, WINDOW_HEIGHT };
	uiCamera.SetProjectionMatrixToOrthographicProjection(0.1f, 1000.0f);

	Transform uiCameraTransform;
	uiCameraTransform.position = { 0.0f, 0.0f, -3.0f };
	uiCameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
	uiCameraTransform.scale = { 1.0f, 1.0f, 1.0f };

	//Vector3 cameraTargetPosition = { 0.0f, 0.0f, 0.0f };
	//Vector3 cameraDirection = glm::normalize(cameraTargetPosition - cameraTransform.position);
	//mainCamera.SetCameraDirectionVectors(Transform::worldUp, cameraDirection);

	const int rootUIRectIndex = UI_Rect::uiRects.size();
	Vector3 rootStart = { -WINDOW_WIDTH / 2.0f, -WINDOW_HEIGHT / 2.0f, 0.0f };
	Vector3 rootEnd = { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f };
	UI_Rect::uiRects.push_back({ rootUIRectIndex, rootStart, rootEnd, MiddleMiddle });
	UI_Rect::uiRects[0].worldStartPos = rootStart;
	UI_Rect::uiRects[0].worldEndPos = rootEnd;

	InitializeCharacterRects(courierFontPath, rootUIRectIndex);

	int numChildrenUIRects = 0;
	for (int i = 0; i < numChildrenUIRects; i++)
	{
		UI_Rect someChildUIRect;
		someChildUIRect.index = UI_Rect::uiRects.size();

		if (i == 0) {
			//someChildUIRect.anchorPosition = TopMiddle;
			someChildUIRect.anchorPosition = TopLeft;

			someChildUIRect.start = { 0.0f, 0.0f, 0.0f };
			someChildUIRect.end = { 100.0f, -100.0f, 0.0f };
		}
		else if (i == 1) {
			//someChildUIRect.anchorPosition = MiddleLeft;
			someChildUIRect.anchorPosition = BottomLeft;

			someChildUIRect.start = { 0.0f, 0.0f, 0.0f };
			someChildUIRect.end = { 100.0f, 100.0f, 0.0f };
		}
		else if (i == 2) {
			//someChildUIRect.anchorPosition = MiddleRight;
			someChildUIRect.anchorPosition = TopRight;

			someChildUIRect.start = { 0.0f, 0.0f, 0.0f };
			someChildUIRect.end = { -100.0f, -100.0f, 0.0f };
		}
		else if (i == 3) {
			//someChildUIRect.anchorPosition = BottomMiddle;
			someChildUIRect.anchorPosition = BottomRight;

			someChildUIRect.start = { 0.0f, 0.0f, 0.0f };
			someChildUIRect.end = { -100.0f, 100.0f, 0.0f };
		}

		AddUIRectAsChildToUIRect(someChildUIRect, rootUIRectIndex);
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameEndTime = std::chrono::high_resolution_clock::now();
	float deltaTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		// Find best placement for this piece of code below VVV. Should it be at the end or at the begining or even somewhere in between?
		if (GetKeyPressedInThisFrame(KeyCode::KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true);
		}

		UpdateKeyStates(window);

		float moveSpeed = 5.0f;
		//if (GetKeyHeld(KeyCode::KEY_W)) {
		//	modelTransform.position.z += deltaTime * moveSpeed;
		//}
		//if (GetKeyHeld(KeyCode::KEY_A)) {
		//	modelTransform.position.x -= deltaTime * moveSpeed;
		//}
		//if (GetKeyHeld(KeyCode::KEY_S)) {
		//	modelTransform.position.z -= deltaTime * moveSpeed;
		//}
		//if (GetKeyHeld(KeyCode::KEY_D)) {
		//	modelTransform.position.x += deltaTime * moveSpeed;
		//}		

		//float mouseSensitivity = 50.0f;

		//cameraTransform.rotation.y -= mouseXDelta * mouseSensitivity * deltaTime;
		//cameraTransform.rotation.x += mouseYDelta * mouseSensitivity * deltaTime;

		//if (cameraTransform.rotation.x > 90.0f)
		//	cameraTransform.rotation.x = 90.0f;
		//if (cameraTransform.rotation.x < -90.0f)
		//	cameraTransform.rotation.x = -90.0f;

		//Vector3 cameraFront = Vector3{ 0.0f, 0.0f, 1.0f };

		//glm::quat rotQuat = glm::quat(glm::radians(cameraTransform.rotation));
		//Mat4x4 rotationMatrix = glm::mat4_cast(rotQuat);
		//cameraFront = glm::vec3(rotationMatrix * glm::vec4(cameraFront, 1.0f));

		//mainCamera.SetCameraDirectionVectors(Transform::worldUp, cameraFront);

		//if (GetKeyHeld(KeyCode::KEY_W)) {
		//	cameraTransform.position += mainCamera.cameraPointingDirection * moveSpeed * deltaTime;
		//}
		//if (GetKeyHeld(KeyCode::KEY_A)) {
		//	cameraTransform.position -= mainCamera.cameraRight * moveSpeed * deltaTime;
		//}
		//if (GetKeyHeld(KeyCode::KEY_S)) {
		//	cameraTransform.position -= mainCamera.cameraPointingDirection * moveSpeed * deltaTime;
		//}
		//if (GetKeyHeld(KeyCode::KEY_D)) {
		//	cameraTransform.position += mainCamera.cameraRight * moveSpeed * deltaTime;
		//}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//glUseProgram(simpleVertexAndFragmentNoCameraShaderProgram.shaderProgramID);
		//glUseProgram(simpleVertexAndFragmentNoCameraWithTexturesShaderProgram.shaderProgramID);
		glUseProgram(simpleVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID);

		//float numDegreesToTurnInASecond = 100.0f;
		//modelTransform.rotation.y += deltaTime * (numDegreesToTurnInASecond);
		//if (modelTransform.rotation.y > 360.0f) {
		//	modelTransform.rotation.y = 0.0f;
		//}

		//Mat4x4 cameraViewMat = glm::lookAt(cameraTransform.position, cameraTransform.position + mainCamera.cameraPointingDirection, mainCamera.cameraUp);

		//int modelLoc = glGetUniformLocation(simpleVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "model");
		////glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTransform.GetTransformMatrix()));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(uiModelTransform.GetTransformMatrix()));

		int viewLoc = glGetUniformLocation(simpleVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "view");
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cameraTransform.GetTransformMatrix()));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(uiCameraTransform.GetTransformMatrix()));
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));

		int projectionLoc = glGetUniformLocation(simpleVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "projection");
		//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mainCamera.GetProjectionMatrix()));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(uiCamera.GetProjectionMatrix()));

		////glBindTexture(GL_TEXTURE_2D, Texture::textures[woodContainerTextureIndex].textureID);
		//glBindTexture(GL_TEXTURE_2D, Texture::textures[stickmanTextureIndex].textureID);
		//glBindVertexArray(simpleQuadMeshGPU.VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//std::cout << mouseX << ", " << mouseY << std::endl;

		Vector2 mousePosInUISpace = ConvertMousePosToScreenUIPos(mouseX, mouseY, WINDOW_WIDTH, WINDOW_HEIGHT);
		//std::cout << mousePosInUISpace.x << ", " << mousePosInUISpace.y << std::endl;
		UpdateUITreeStates(UI_Rect::uiRects[rootUIRectIndex], mousePosInUISpace.x, mousePosInUISpace.y);
		HandleUIEvents(mouseXDelta, mouseYDelta);
		RenderUI(UI_Rect::uiRects[rootUIRectIndex], simpleVertexAndFragmentWithCameraWithTexturesShaderProgram, stickmanTextureIndex, simpleQuadMeshGPU);

		//int characterIndex = static_cast<int>('c');
		//int characterIndex = 99;	//-> C
		//int characterIndex = 65; //-> A
		//int characterIndex = 126; //-> ~
		// VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV MAKE NEW SHADERS FOR UI TEXT WITH Y AXIS FLIPPED!!!!!!!!!!!!
		//RenderUIRectAndSubTree(UI_Rect::uiRects[charactersPerCharacter[characterIndex].characterUIRectIndex], UI_Rect::uiRects[rootUIRectIndex], simpleVertexAndFragmentWithCameraWithTexturesShaderProgram, charactersPerCharacter[characterIndex].textureIndex, simpleQuadMeshGPU);

		glUseProgram(textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID);
		int viewLocInTextShader = glGetUniformLocation(textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "view");
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cameraTransform.GetTransformMatrix()));
		glUniformMatrix4fv(viewLocInTextShader, 1, GL_FALSE, glm::value_ptr(uiCameraTransform.GetTransformMatrix()));
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cameraViewMat));

		int projectionLocInTextShader = glGetUniformLocation(textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "projection");
		//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mainCamera.GetProjectionMatrix()));
		glUniformMatrix4fv(projectionLocInTextShader, 1, GL_FALSE, glm::value_ptr(uiCamera.GetProjectionMatrix()));

		Vector3 helloWorldTextPosition = { -180.0f, 0.0f, 0.0f };
		RenderText(textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram, simpleQuadMeshGPU, "Hello World!", helloWorldTextPosition, rootUIRectIndex);


		glfwSwapBuffers(window);
		glfwPollEvents();

		std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsedTime = currentTime - lastFrameEndTime;
		deltaTime = elapsedTime.count();
		lastFrameEndTime = currentTime;

		ResetKeysReleased();
	}

	glfwTerminate();
	return 0;
}