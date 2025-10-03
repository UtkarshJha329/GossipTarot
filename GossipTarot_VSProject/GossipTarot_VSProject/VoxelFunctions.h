#pragma once

#include "GLMIncludes.h"
#include "MeshUtils.h"

#include "BakedMeshData.h"

void GenerateFaceQuadMeshOnGPU(MeshOnGPU& faceQuadMeshOnGPU) {

	MeshOnCPU curFaceMeshOnCPU;

	for (int i = 0; i < 6; i++)
	{
		curFaceMeshOnCPU.indices.push_back(QuadIndices[i]);
	}

	for (int i = 0; i < 20; i++)
	{
		curFaceMeshOnCPU.vertices.push_back(QuadVerticesWithTexCoords[i]);
	};

	CreateMeshOnGPU(curFaceMeshOnCPU, faceQuadMeshOnGPU);
}

void AddFaceIndicesAndVerticesToMeshOnCPUAtPosition(MeshOnCPU& meshOnCPU, const std::vector<float>& cubeFaceVertices, const std::vector<unsigned int>& cubeFaceIndices, const Vector3Int& position) {

}

//void GenerateChunkVoxelPositionsOnGPUAsVBO(const MeshOnGPU& meshOnGPU, unsigned int& numVoxelsInChunk, const std::vector<float>& chunkNoise, const Vector3Int& chunkSizeInVoxels) {
//
//	numVoxelsInChunk = 0;
//	std::vector<unsigned int> compressedChunkVoxelPositions;
//
//	for (int z = 0; z < chunkSizeInVoxels.z; z++) {
//		for (int x = 0; x < chunkSizeInVoxels.x; x++) {
//			int currentNoiseIndex = x + z * chunkSizeInVoxels.x;
//			float voxelHeight = chunkNoise[currentNoiseIndex] * chunkSizeInVoxels.y;
//
//			for (int y = 0; y < chunkSizeInVoxels.y; y++)
//			{
//				if (y <= voxelHeight) {
//					unsigned int curVoxelCompactPos = (x << 0);
//					curVoxelCompactPos += (y << 5);
//					curVoxelCompactPos += (z << 10);
//
//					compressedChunkVoxelPositions.push_back(curVoxelCompactPos);
//					numVoxelsInChunk++;
//				}
//			}
//		}
//	}
//
//	glBindVertexArray(meshOnGPU.VAO);
//	unsigned int voxelPositionsInstanceVBO;
//	glGenBuffers(1, &voxelPositionsInstanceVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, voxelPositionsInstanceVBO);
//	glBufferData(GL_ARRAY_BUFFER, compressedChunkVoxelPositions.size() * sizeof(compressedChunkVoxelPositions[0]), compressedChunkVoxelPositions.data(), GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(2);
//	glBindBuffer(GL_ARRAY_BUFFER, voxelPositionsInstanceVBO);
//	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, GL_FALSE, (void*)0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glVertexAttribDivisor(2, 1);
//}

bool VoxelIndexLiesInsideChunk(const Vector3Int& chunkSizeInVoxels, const Vector3Int& curVoxelIndex) {

	return !((curVoxelIndex.x < 0 ||
				curVoxelIndex.y < 0 ||
				curVoxelIndex.z < 0 ||
				curVoxelIndex.x >= chunkSizeInVoxels.x ||
				curVoxelIndex.y >= chunkSizeInVoxels.y ||
				curVoxelIndex.z >= chunkSizeInVoxels.z));

}

bool NoiseExistsInNeighbour(const std::vector<float>& chunkNoise, const Vector3Int& chunkSizeInVoxels, const Vector3Int& curVoxelIndex, const Vector3Int& directionToCheck) {

	Vector3Int neighbouringVoxelIndex = curVoxelIndex + directionToCheck;

	if (VoxelIndexLiesInsideChunk(chunkSizeInVoxels, neighbouringVoxelIndex)) {
		int neightbourNoiseIndex = neighbouringVoxelIndex.x + (neighbouringVoxelIndex.z * chunkSizeInVoxels.x);
		float voxelHeight = chunkNoise[neightbourNoiseIndex] * chunkSizeInVoxels.y;

		return (neighbouringVoxelIndex.y <= voxelHeight);
	}
	return false;
}

void AddBitShiftFaceIDToCompressedVoxelPosition(std::vector<unsigned int>& compressedChunkVoxelPositions, unsigned int curVoxelCompactPos, const unsigned int& bitshiftedFaceID, unsigned int& numVoxelsInChunk) {
	curVoxelCompactPos += bitshiftedFaceID;
	compressedChunkVoxelPositions.push_back(curVoxelCompactPos);
	numVoxelsInChunk++;
}

void GenerateChunkVoxelPositionsOnGPUAsVBO(const MeshOnGPU& meshOnGPU, unsigned int& numVoxelsInChunk, const std::vector<float>& chunkNoise, const Vector3Int& chunkSizeInVoxels) {

	const unsigned int bitShiftPosX = 0;
	const unsigned int bitShiftPosY = 5;
	const unsigned int bitShiftPosZ = 10;
	const unsigned int bitShiftPosFace = 15;

	const unsigned int bitShiftFaceIDTop = 0;
	const unsigned int bitShiftFaceIDBottom = 1;
	const unsigned int bitShiftFaceIDLeft = 2;
	const unsigned int bitShiftFaceIDRight = 3;
	const unsigned int bitShiftFaceIDFront = 4;
	const unsigned int bitShiftFaceIDBack = 5;

	numVoxelsInChunk = 0;
	std::vector<unsigned int> compressedChunkVoxelPositions;

	for (int z = 0; z < chunkSizeInVoxels.z; z++) {
		for (int x = 0; x < chunkSizeInVoxels.x; x++) {
			int currentNoiseIndex = x + (z * chunkSizeInVoxels.x);
			float voxelHeight = chunkNoise[currentNoiseIndex] * chunkSizeInVoxels.y;

			for (int y = 0; y < chunkSizeInVoxels.y; y++)
			{
				if (y <= voxelHeight) {
					unsigned int curVoxelCompactPos = (x << bitShiftPosX);
					curVoxelCompactPos += (y << bitShiftPosY);
					curVoxelCompactPos += (z << bitShiftPosZ);

					if (!NoiseExistsInNeighbour(chunkNoise, chunkSizeInVoxels, Vector3Int{x, y, z}, Vector3Int{0, 1, 0})) {
						AddBitShiftFaceIDToCompressedVoxelPosition(compressedChunkVoxelPositions, curVoxelCompactPos, (bitShiftFaceIDTop << bitShiftPosFace), numVoxelsInChunk);
					}
					if (!NoiseExistsInNeighbour(chunkNoise, chunkSizeInVoxels, Vector3Int{x, y, z}, Vector3Int{0, -1, 0})) {
						AddBitShiftFaceIDToCompressedVoxelPosition(compressedChunkVoxelPositions, curVoxelCompactPos, (bitShiftFaceIDBottom << bitShiftPosFace), numVoxelsInChunk);
					}
					if (!NoiseExistsInNeighbour(chunkNoise, chunkSizeInVoxels, Vector3Int{x, y, z}, Vector3Int{-1, 0, 0})) {
						AddBitShiftFaceIDToCompressedVoxelPosition(compressedChunkVoxelPositions, curVoxelCompactPos, (bitShiftFaceIDLeft << bitShiftPosFace), numVoxelsInChunk);
					}
					if (!NoiseExistsInNeighbour(chunkNoise, chunkSizeInVoxels, Vector3Int{x, y, z}, Vector3Int{1, 0, 0})) {
						AddBitShiftFaceIDToCompressedVoxelPosition(compressedChunkVoxelPositions, curVoxelCompactPos, (bitShiftFaceIDRight << bitShiftPosFace), numVoxelsInChunk);
					}
					if (!NoiseExistsInNeighbour(chunkNoise, chunkSizeInVoxels, Vector3Int{x, y, z}, Vector3Int{0, 0, 1})) {
						AddBitShiftFaceIDToCompressedVoxelPosition(compressedChunkVoxelPositions, curVoxelCompactPos, (bitShiftFaceIDFront << bitShiftPosFace), numVoxelsInChunk);
					}
					if (!NoiseExistsInNeighbour(chunkNoise, chunkSizeInVoxels, Vector3Int{x, y, z}, Vector3Int{0, 0, -1})) {
						AddBitShiftFaceIDToCompressedVoxelPosition(compressedChunkVoxelPositions, curVoxelCompactPos, (bitShiftFaceIDBack << bitShiftPosFace), numVoxelsInChunk);
					}
				}
			}
		}
	}

	glBindVertexArray(meshOnGPU.VAO);
	unsigned int voxelPositionsInstanceVBO;
	glGenBuffers(1, &voxelPositionsInstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, voxelPositionsInstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, compressedChunkVoxelPositions.size() * sizeof(compressedChunkVoxelPositions[0]), compressedChunkVoxelPositions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, voxelPositionsInstanceVBO);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, GL_FALSE, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);
}
