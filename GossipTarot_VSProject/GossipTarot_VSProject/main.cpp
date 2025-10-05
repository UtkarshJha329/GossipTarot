
#include <iostream>

#include <sstream>
#include <string>
#include <iomanip>

#include <cmath>
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

#include "FastNoise/FastNoise.h"
#include "flecs/flecs.h"

#include "BakedMeshData.h"
#include "VoxelFunctions.h"
#include "ChunkGeneration.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

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

	int qKeyState = glfwGetKey(window, GLFW_KEY_Q);
	SetKeyBasedOnState(KeyCode::KEY_Q, qKeyState > 0 ? PRESSED_OR_HELD : RELEASED);

	int eKeyState = glfwGetKey(window, GLFW_KEY_E);
	SetKeyBasedOnState(KeyCode::KEY_E, eKeyState > 0 ? PRESSED_OR_HELD : RELEASED);

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

void RenderDeltaTimeInMS(const ShaderProgram& shaderForRendering, const int& rootUIRectIndex, const MeshOnGPU& meshOnGPU, const float& deltaTime) {

	Vector3 deltaTimeTextPosition = { -200.0f, -60.0f, 0.0f };
	float deltaTimeInMs = deltaTime * 1000.0f;
	float roundedDeltaTimeInMs = std::round(deltaTimeInMs * std::pow(10, 2)) / std::pow(10, 2);
	std::stringstream deltaTimeStringStream;
	deltaTimeStringStream << std::fixed << std::setprecision(2) << roundedDeltaTimeInMs;
	std::string deltaTimeString = deltaTimeStringStream.str() + " ms.";

	RenderText(shaderForRendering, AnchorPosition::TopRight, meshOnGPU, deltaTimeString, deltaTimeTextPosition, rootUIRectIndex);
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

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Enable face culling
	glEnable(GL_CULL_FACE);

	// Cull only back faces
	glCullFace(GL_BACK);

	// Define front face as counter-clockwise
	glFrontFace(GL_CCW);





	std::string woodContainerTexturePath = "Assets/Textures/WoodContainer.jpg";
	int woodContainerTextureIndex = TextureFromFile(woodContainerTexturePath);

	std::string stickmanTexturePath = "Assets/Textures/Stickman.jpg";
	int stickmanTextureIndex = TextureFromFile(stickmanTexturePath);

	std::string courierFontPath = "Assets/Fonts/cour.ttf";



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

	
	//Shader voxelRenderingVertexShaderWithCameraWithTextures;
	//voxelRenderingVertexShaderWithCameraWithTextures.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShader/CompressedVoxelWithTexturesShader.vert";
	//voxelRenderingVertexShaderWithCameraWithTextures.shaderType = SHADER_TYPE::VERTEX_SHADER;
	//voxelRenderingVertexShaderWithCameraWithTextures.CreateShader();

	//Shader voxelRenderingFragmentShaderWithCameraWithTextures;
	//voxelRenderingFragmentShaderWithCameraWithTextures.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShader/CompressedVoxelWithTexturesShader.frag";
	//voxelRenderingFragmentShaderWithCameraWithTextures.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	//voxelRenderingFragmentShaderWithCameraWithTextures.CreateShader();

	//std::vector<Shader> shadersForVoxelRenderingShaderProgram = { voxelRenderingVertexShaderWithCameraWithTextures, voxelRenderingFragmentShaderWithCameraWithTextures };
	//ShaderProgram voxelRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram;

	//voxelRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram.CreateShaderProgram(shadersForVoxelRenderingShaderProgram);


	//Shader voxelRenderingVertexShaderWithCameraWithTexturesPerFace;
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFace.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFace/CompressedVoxelWithTexturesShaderPerFace.vert";
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFace.shaderType = SHADER_TYPE::VERTEX_SHADER;
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFace.CreateShader();

	//Shader voxelRenderingFragmentShaderWithCameraWithTexturesPerFace;
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFace.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFace/CompressedVoxelWithTexturesShaderPerFace.frag";
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFace.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFace.CreateShader();

	//std::vector<Shader> shadersForVoxelPerFaceRenderingShaderProgram = { voxelRenderingVertexShaderWithCameraWithTexturesPerFace, voxelRenderingFragmentShaderWithCameraWithTexturesPerFace };
	//ShaderProgram voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceShaderProgram;

	//voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceShaderProgram.CreateShaderProgram(shadersForVoxelPerFaceRenderingShaderProgram);

	//Shader voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBOData;
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBOData.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFaceVBOData/CompressedVoxelWithTexturesShaderPerFaceVBOData.vert";
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBOData.shaderType = SHADER_TYPE::VERTEX_SHADER;
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBOData.CreateShader();

	//Shader voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBOData;
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBOData.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFaceVBOData/CompressedVoxelWithTexturesShaderPerFaceVBOData.frag";
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBOData.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBOData.CreateShader();

	//std::vector<Shader> shadersForVoxelPerFaceVBODataRenderingShaderProgram = { voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBOData, voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBOData };
	//ShaderProgram voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceVBODataShaderProgram;

	//voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceVBODataShaderProgram.CreateShaderProgram(shadersForVoxelPerFaceVBODataRenderingShaderProgram);


	//Shader voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBODataAsTriangle;
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBODataAsTriangle.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFaceVBODataAsTriangle/CompressedVoxelWithTexturesShaderPerFaceVBODataAsTriangle.vert";
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBODataAsTriangle.shaderType = SHADER_TYPE::VERTEX_SHADER;
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBODataAsTriangle.CreateShader();

	//Shader voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBODataAsTriangle;
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBODataAsTriangle.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFaceVBODataAsTriangle/CompressedVoxelWithTexturesShaderPerFaceVBODataAsTriangle.frag";
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBODataAsTriangle.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBODataAsTriangle.CreateShader();

	//std::vector<Shader> shadersForVoxelPerFaceVBODataAsTriangleRenderingShaderProgram = { voxelRenderingVertexShaderWithCameraWithTexturesPerFaceVBODataAsTriangle, voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceVBODataAsTriangle };
	//ShaderProgram voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceVBODataAsTriangleShaderProgram;

	//voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceVBODataAsTriangleShaderProgram.CreateShaderProgram(shadersForVoxelPerFaceVBODataAsTriangleRenderingShaderProgram);

	//Shader voxelRenderingVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle;
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFaceSSBODataAsTriangle/CompressedVoxelWithTexturesShaderPerFaceSSBODataAsTriangle.vert";
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle.shaderType = SHADER_TYPE::VERTEX_SHADER;
	//voxelRenderingVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle.CreateShader();

	//Shader voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle;
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFaceSSBODataAsTriangle/CompressedVoxelWithTexturesShaderPerFaceSSBODataAsTriangle.frag";
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	//voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle.CreateShader();

	//std::vector<Shader> shadersForVoxelPerFaceSSBODataAsTriangleRenderingShaderProgram = { voxelRenderingVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle, voxelRenderingFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangle };
	//ShaderProgram voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleShaderProgram;

	//voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleShaderProgram.CreateShaderProgram(shadersForVoxelPerFaceSSBODataAsTriangleRenderingShaderProgram);

	Shader voxelVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw;
	voxelVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFaceSSBODataAsTriangleIndirectDraw/VoxelWithTexturesShaderPerFaceSSBODataAsTriangleIndirectDraw.vert";
	voxelVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw.shaderType = SHADER_TYPE::VERTEX_SHADER;
	voxelVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw.CreateShader();

	Shader voxelFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw;
	voxelFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw.shaderFilePath = "Assets/Shaders/CompressedVoxelWithTexturesShaderPerFaceSSBODataAsTriangleIndirectDraw/VoxelWithTexturesShaderPerFaceSSBODataAsTriangleIndirectDraw.frag";
	voxelFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
	voxelFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw.CreateShader();

	std::vector<Shader> shadersForVoxelPerFaceSSBODataAsTriangleIndirectDrawShaderProgram = { voxelVertexShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw, voxelFragmentShaderWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDraw };
	ShaderProgram voxelVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDrawShaderProgram;

	voxelVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDrawShaderProgram.CreateShaderProgram(shadersForVoxelPerFaceSSBODataAsTriangleIndirectDrawShaderProgram);


	MeshOnCPU simpleQuadMeshCPU;
	for (int i = 0; i < 20; i++)
	{
		//simpleQuadMeshCPU.vertices.push_back(vertices[i]);
		simpleQuadMeshCPU.vertices.push_back(QuadVerticesWithTexCoords[i]);
	};

	for (int i = 0; i < 6; i++)
	{
		simpleQuadMeshCPU.indices.push_back(QuadIndices[i]);
	}

	MeshOnGPU simpleQuadMeshGPU;

	CreateMeshOnGPU(simpleQuadMeshCPU, simpleQuadMeshGPU);


	MeshOnCPU simpleCubeMeshCPU;
	for (int i = 0; i < 120; i++)
	{
		simpleCubeMeshCPU.vertices.push_back(CubeVerticesWithTexCoords[i]);
	};

	for (int i = 0; i < 36; i++)
	{
		simpleCubeMeshCPU.indices.push_back(CubeIndices[i]);
	}

	MeshOnGPU simpleCubeMeshGPU;

	CreateMeshOnGPU(simpleCubeMeshCPU, simpleCubeMeshGPU);


	Transform modelTransform;
	modelTransform.position = { 0.0f, 0.0f, 3.0f };
	modelTransform.rotation = { 0.0f, 0.0f, 0.0f };
	modelTransform.scale = { 1.0f, 1.0f, 1.0f };



	Transform cameraTransform;
	cameraTransform.position = { 0.0f, 3.0f, -3.0f };
	cameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
	cameraTransform.scale = { 1.0f, 1.0f, 1.0f };

	Camera mainCamera;
	mainCamera.viewport.dimensions = { WINDOW_WIDTH, WINDOW_HEIGHT };
	mainCamera.SetProjectionMatrixToPerspectiveProjection(45.0f, 0.1f, 1000.0f);


	Transform uiModelTransform;
	uiModelTransform.position = { 0.0f, 0.0f, 0.0f };
	uiModelTransform.rotation = { 0.0f, 0.0f, 0.0f };
	uiModelTransform.scale = { 100.0f, 100.0f, 100.0f };

	Camera uiCamera;
	uiCamera.viewport.dimensions = { WINDOW_WIDTH, WINDOW_HEIGHT };
	uiCamera.SetProjectionMatrixToOrthographicProjection(0.1f, 1000.0f);

	Transform uiCameraTransform;
	uiCameraTransform.position = { 0.0f, 0.0f, -3.0f };
	uiCameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
	uiCameraTransform.scale = { 1.0f, 1.0f, 1.0f };

	const int rootUIRectIndex = UI_Rect::uiRects.size();
	Vector3 rootStart = { -WINDOW_WIDTH / 2.0f, -WINDOW_HEIGHT / 2.0f, 0.0f };
	Vector3 rootEnd = { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f };
	UI_Rect::uiRects.push_back({ rootUIRectIndex, rootStart, rootEnd, MiddleMiddle });
	UI_Rect::uiRects[0].worldStartPos = rootStart;
	UI_Rect::uiRects[0].worldEndPos = rootEnd;

	InitializeCharacterRects(courierFontPath, rootUIRectIndex);

	int numChildrenUIRects = 4;
	for (int i = 0; i < numChildrenUIRects; i++)
	{
		if (i == 2) {
			continue;
		}
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




	//auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	//auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

	//fnFractal->SetSource(fnSimplex);
	//fnFractal->SetOctaveCount(5);

	Vector3Int worldSizeInChunks = { 16, 1, 16 };
	Vector3Int chunkSizeInVoxels = { 32, 32, 32 };
	//Vector3Int chunkSizeInVoxels = { 64, 64, 64 };
	//std::vector<float> noiseOutput(chunkSizeInVoxels.x * chunkSizeInVoxels.z);

	//fnFractal->GenUniformGrid2D(noiseOutput.data(), 0, 0, chunkSizeInVoxels.x, chunkSizeInVoxels.z, 0.2f, 1337);

	//unsigned int numVoxelsInChunk = 0;
	//GenerateChunkVoxelPositionsOnGPUAsVBO(simpleCubeMeshGPU, numVoxelsInChunk, noiseOutput, chunkSizeInVoxels);
	//GenerateChunkVoxelInstancePositionsOnGPUAsVBO(simpleQuadMeshGPU, numVoxelsInChunk, noiseOutput, chunkSizeInVoxels);
	//std::cout << "Num Voxels in chunk : " << numVoxelsInChunk << std::endl;

	//unsigned int numIndicesInChunk = 0;
	//MeshOnGPU chunkMeshOnGPU;
	//GenerateChunkVoxelPositionsOnGPUAsVBO(chunkMeshOnGPU, numIndicesInChunk, noiseOutput, chunkSizeInVoxels);

	//unsigned int numIndicesInChunk = 0;
	//MeshOnGPU chunkMeshOnGPU;
	//GenerateChunkVoxelPositionsOnGPUAsVBOAsTriangle(chunkMeshOnGPU, numIndicesInChunk, noiseOutput, chunkSizeInVoxels);

	//unsigned int numIndicesInChunk = 0;
	//unsigned int megaVoxelsQuadPositionsBufferObjectID = 0;
	//unsigned int megaVoxelsQuadPositionsBufferObjectBindingLocation = 2;
	//MeshOnGPU chunkMeshOnGPU;
	//GenerateChunkVoxelPositionsOnGPUAsSSBOAsTriangle(noiseOutput, chunkSizeInVoxels, chunkMeshOnGPU, numIndicesInChunk, megaVoxelsQuadPositionsBufferObjectID);

	unsigned int numVoxelDatasPerBucket = chunkSizeInVoxels.x * chunkSizeInVoxels.y * chunkSizeInVoxels.z;
	unsigned int numBuckets = worldSizeInChunks.x * worldSizeInChunks.y * worldSizeInChunks.z;
	unsigned int megaVoxelsDataBufferObjectBindingLocation = 2;
	VoxelsDataPool voxelsDataPool(numVoxelDatasPerBucket, numBuckets, megaVoxelsDataBufferObjectBindingLocation);

	//unsigned int numIndicesInChunk = 0;
	//MeshOnGPU chunkMeshOnGPU;
	//ChunkVoxelsDataPoolMetadata chunkVoxelsDataPoolMetadata;
	//GenerateChunkVoxelPositionsOnGPUAsSSBOAsTriangleWithVoxelDataPool(noiseOutput, chunkSizeInVoxels, voxelsDataPool, chunkMeshOnGPU, numIndicesInChunk, chunkVoxelsDataPoolMetadata);

	//unsigned int numIndicesInChunk = 0;
	//MeshOnGPU chunkMeshOnGPU;
	//ChunkVoxelsDataPoolMetadata chunkVoxelsDataPoolMetadata;
	//GenerateChunkVoxelPositionsOnGPUAsSSBOAsTriangleWithVoxelDataPool(noiseOutput, chunkSizeInVoxels, voxelsDataPool, chunkMeshOnGPU, numIndicesInChunk, chunkVoxelsDataPoolMetadata);


	IndirectDrawCommands chunksIndirectDrawCommands;
	//AddChunkToDrawCommand(Vector3Int{ 1, 0, 1 }, chunkVoxelsDataPoolMetadata, chunksIndirectDrawCommands);

	MeshOnGPU commonChunkMeshOnGPU;
	GenerateCommonChunkMeshOnGPU(chunkSizeInVoxels, commonChunkMeshOnGPU);
	
	GenerateChunksAndAddToIndirectRenderCommandVectorOnCPU(worldSizeInChunks, chunkSizeInVoxels, voxelsDataPool, chunksIndirectDrawCommands);
	chunksIndirectDrawCommands.GPU_InitCommandBuffer();

	//chunksIndirectDrawCommands.GPU_InitCommandBuffer();

	// TODO 2 : Make Multi Draw Elements Indirect Command


	std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameEndTime = std::chrono::high_resolution_clock::now();
	float deltaTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		// Find best placement for this piece of code below VVV. Should it be at the end or at the begining or even somewhere in between?
		if (GetKeyPressedInThisFrame(KeyCode::KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true);
		}

		UpdateKeyStates(window);

		float mouseSensitivity = 75.0f;
		float moveSpeed = 3.0f;


		if (GetKeyHeld(KeyCode::MOUSE_BUTTON_RIGHT)) {

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			cameraTransform.rotation.y -= mouseXDelta * mouseSensitivity * deltaTime;
			cameraTransform.rotation.x += mouseYDelta * mouseSensitivity * deltaTime;

			if (cameraTransform.rotation.x > 90.0f)
				cameraTransform.rotation.x = 90.0f;
			if (cameraTransform.rotation.x < -90.0f)
				cameraTransform.rotation.x = -90.0f;

			Vector3 cameraFront = Vector3{ 0.0f, 0.0f, 1.0f };

			glm::quat rotQuat = glm::quat(glm::radians(cameraTransform.rotation));
			Mat4x4 rotationMatrix = glm::mat4_cast(rotQuat);
			cameraFront = glm::vec3(rotationMatrix * glm::vec4(cameraFront, 1.0f));

			mainCamera.SetCameraDirectionVectors(Transform::worldUp, cameraFront);

			if (GetKeyHeld(KeyCode::KEY_W)) {
				cameraTransform.position += mainCamera.cameraPointingDirection * moveSpeed * deltaTime;
			}
			if (GetKeyHeld(KeyCode::KEY_A)) {
				cameraTransform.position -= mainCamera.cameraRight * moveSpeed * deltaTime;
			}
			if (GetKeyHeld(KeyCode::KEY_S)) {
				cameraTransform.position -= mainCamera.cameraPointingDirection * moveSpeed * deltaTime;
			}
			if (GetKeyHeld(KeyCode::KEY_D)) {
				cameraTransform.position += mainCamera.cameraRight * moveSpeed * deltaTime;
			}

			if (GetKeyHeld(KeyCode::KEY_Q)) {
				cameraTransform.position += cameraTransform.worldUp * moveSpeed * deltaTime;
			}
			if (GetKeyHeld(KeyCode::KEY_E)) {
				cameraTransform.position -= cameraTransform.worldUp * moveSpeed * deltaTime;
			}
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}


		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use Shader Program
		glUseProgram(voxelVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDrawShaderProgram.shaderProgramID);


		// Render World Geometry
		int mainCameraViewLoc = glGetUniformLocation(voxelVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDrawShaderProgram.shaderProgramID, "view");
		//Mat4x4 viewMatrix = glm::inverse(cameraTransform.GetTransformMatrix());
		Mat4x4 viewMatrix = glm::lookAt(cameraTransform.position, glm::normalize(mainCamera.cameraPointingDirection) + cameraTransform.position, mainCamera.cameraUp);
		glUniformMatrix4fv(mainCameraViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		int mainCameraProjectionLoc = glGetUniformLocation(voxelVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDrawShaderProgram.shaderProgramID, "projection");
		glUniformMatrix4fv(mainCameraProjectionLoc, 1, GL_FALSE, glm::value_ptr(mainCamera.GetProjectionMatrix()));

		// VVVV Rendering whole cube for each voxel takes ~5.2 MS
		//Vector3 startingChunkPos = modelTransform.position;
		//int numChunksToRenderOnHorizontalAxis = 8;
		//for (int i = 0; i < numChunksToRenderOnHorizontalAxis; i++)
		//{
		//	for (int j = 0; j < numChunksToRenderOnHorizontalAxis; j++)
		//	{
		//		modelTransform.position = Vector3{ i * chunkSizeInVoxels.x, 0.0f, j * chunkSizeInVoxels.z };
		//		RenderCubeInstance(voxelRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram, modelTransform.GetTransformMatrix(), stickmanTextureIndex, simpleCubeMeshGPU, numVoxelsInChunk);

		//	}
		//}
		//modelTransform.position = startingChunkPos;

		// VVVV Rendering all only visible to air faces takes ~2.4 MS
		//Vector3 startingChunkPos = modelTransform.position;
		//int numChunksToRenderOnHorizontalAxis = 8;
		//for (int i = 0; i < numChunksToRenderOnHorizontalAxis; i++)
		//{
		//	for (int j = 0; j < numChunksToRenderOnHorizontalAxis; j++)
		//	{
		//		modelTransform.position = Vector3{ i * chunkSizeInVoxels.x, 0.0f, j * chunkSizeInVoxels.z };
		//		RenderQuadInstance(voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceShaderProgram, modelTransform.GetTransformMatrix(), stickmanTextureIndex, simpleQuadMeshGPU, numVoxelsInChunk);

		//	}
		//}
		//modelTransform.position = startingChunkPos;

		// Rendering all only visible to air faces using a VBO instead of Instancing takes ~2.01 MS
		//Vector3 startingChunkPos = modelTransform.position;
		//int numChunksToRenderOnHorizontalAxis = 8;
		//for (int i = 0; i < numChunksToRenderOnHorizontalAxis; i++)
		//{
		//	for (int j = 0; j < numChunksToRenderOnHorizontalAxis; j++)
		//	{
		//		modelTransform.position = Vector3{ i * chunkSizeInVoxels.x, 0.0f, j * chunkSizeInVoxels.z };
		//		RenderMeshOnGPUWithNumIndices(voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceVBODataShaderProgram, modelTransform.GetTransformMatrix(), stickmanTextureIndex, chunkMeshOnGPU, numIndicesInChunk);

		//	}
		//}
		//modelTransform.position = startingChunkPos;

		// Rendering all only visible to air faces using a VBO and as a single large triangle instead of Quad takes ~1.90 MS
		//Vector3 startingChunkPos = modelTransform.position;
		//int numChunksToRenderOnHorizontalAxis = 8;
		//for (int i = 0; i < numChunksToRenderOnHorizontalAxis; i++)
		//{
		//	for (int j = 0; j < numChunksToRenderOnHorizontalAxis; j++)
		//	{
		//		modelTransform.position = Vector3{ i * chunkSizeInVoxels.x, 0.0f, j * chunkSizeInVoxels.z };
		//		RenderMeshOnGPUWithNumIndices(voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceVBODataAsTriangleShaderProgram, modelTransform.GetTransformMatrix(), stickmanTextureIndex, chunkMeshOnGPU, numIndicesInChunk);

		//	}
		//}
		//modelTransform.position = startingChunkPos;

		//Vector3 startingChunkPos = modelTransform.position;
		//int numChunksToRenderOnHorizontalAxis = 8;
		//for (int i = 0; i < numChunksToRenderOnHorizontalAxis; i++)
		//{
		//	for (int j = 0; j < numChunksToRenderOnHorizontalAxis; j++)
		//	{
		//		modelTransform.position = Vector3{ i * chunkSizeInVoxels.x, 0.0f, j * chunkSizeInVoxels.z };
		//		RenderMeshOnGPUWithNumIndicesAndSSBO(voxelRenderingVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleShaderProgram, modelTransform.GetTransformMatrix(), stickmanTextureIndex, chunkMeshOnGPU, numIndicesInChunk, voxelsDataPool.megaVoxelsDataBufferObjectID, voxelsDataPool.megaVoxelsDataBufferObjectBindingLocation);
		//	}
		//}
		//modelTransform.position = startingChunkPos;

		//RenderMeshOnGPUWithDrawElementsIndirectCommands(voxelVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDrawShaderProgram, modelTransform.GetTransformMatrix(), stickmanTextureIndex, chunkMeshOnGPU, chunksIndirectDrawCommands, voxelsDataPool.megaVoxelsDataBufferObjectID, voxelsDataPool.megaVoxelsDataBufferObjectBindingLocation);

		RenderMeshOnGPUWithDrawElementsIndirectCommands(voxelVertexAndFragmentWithCameraWithTexturesPerFaceSSBODataAsTriangleIndirectDrawShaderProgram, modelTransform.GetTransformMatrix(), stickmanTextureIndex, commonChunkMeshOnGPU, chunksIndirectDrawCommands, voxelsDataPool.megaVoxelsDataBufferObjectID, voxelsDataPool.megaVoxelsDataBufferObjectBindingLocation);



		// Render UI
		glUseProgram(simpleVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID);

		int viewLoc = glGetUniformLocation(simpleVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(uiCameraTransform.GetTransformMatrix()));

		int projectionLoc = glGetUniformLocation(simpleVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(uiCamera.GetProjectionMatrix()));

		Vector2 mousePosInUISpace = ConvertMousePosToScreenUIPos(mouseX, mouseY, WINDOW_WIDTH, WINDOW_HEIGHT);
		UpdateUITreeStates(UI_Rect::uiRects[rootUIRectIndex], mousePosInUISpace.x, mousePosInUISpace.y);
		HandleUIEvents(mouseXDelta, mouseYDelta);
		RenderUI(UI_Rect::uiRects[rootUIRectIndex], simpleVertexAndFragmentWithCameraWithTexturesShaderProgram, stickmanTextureIndex, simpleQuadMeshGPU);

		// Render Text
		glUseProgram(textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID);
		int viewLocInTextShader = glGetUniformLocation(textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "view");
		glUniformMatrix4fv(viewLocInTextShader, 1, GL_FALSE, glm::value_ptr(uiCameraTransform.GetTransformMatrix()));

		int projectionLocInTextShader = glGetUniformLocation(textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram.shaderProgramID, "projection");
		glUniformMatrix4fv(projectionLocInTextShader, 1, GL_FALSE, glm::value_ptr(uiCamera.GetProjectionMatrix()));

		RenderDeltaTimeInMS(textRenderingVertexAndFragmentWithCameraWithTexturesShaderProgram, rootUIRectIndex, simpleQuadMeshGPU, deltaTime);

		// Swap Buffers for display
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Calculate frame delta time
		std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsedTime = currentTime - lastFrameEndTime;
		deltaTime = elapsedTime.count();
		lastFrameEndTime = currentTime;

		// Reset Keyboard State for next frame
		ResetKeysReleased();
	}

	glfwTerminate();
	return 0;
}