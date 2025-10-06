#pragma once

#include "GLMIncludes.h"
#include "MeshOnGpu.h"

#include "FastNoise/FastNoise.h"
#include "VoxelFunctions.h"

unsigned int GetFlattenedChunkIndexForChunksVoxelsDataPoolMetadatas(const Vector3Int& worldSizeInChunks, const Vector3Int& chunkIndex) {
	return chunkIndex.y * worldSizeInChunks.x * worldSizeInChunks.z + chunkIndex.z * worldSizeInChunks.x + chunkIndex.x;
}

void GenerateChunksAndAddToIndirectRenderCommandVectorOnCPU(const Vector3Int& worldSizeInChunks, const Vector3Int& chunkSizeInVoxels, VoxelsDataPool& voxelsDataPool, ChunksPerFaceIndirectDrawCommands& chunksPerFaceIndirectDrawCommands, std::vector<ChunkVoxelsDataPoolMetadata>& chunksVoxelsDataPoolMetadatas) {

	chunksVoxelsDataPoolMetadatas.resize(worldSizeInChunks.x * worldSizeInChunks.y * worldSizeInChunks.z);

	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(5);

	for (unsigned int j = 0; j < worldSizeInChunks.y; j++)
	{
		for (unsigned int k = 0; k < worldSizeInChunks.z; k++)
		{
			for (unsigned int i = 0; i < worldSizeInChunks.x; i++)
			{
				std::vector<float> noiseOutput(chunkSizeInVoxels.x * chunkSizeInVoxels.z);

				Vector3Int chunkStartWorldPos = Vector3Int{ i * chunkSizeInVoxels.x, j * chunkSizeInVoxels.y, k * chunkSizeInVoxels.z };
				fnFractal->GenUniformGrid2D(noiseOutput.data(), chunkStartWorldPos.x, chunkStartWorldPos.z, chunkSizeInVoxels.x, chunkSizeInVoxels.z, 0.2f, 1337);

				unsigned int numVoxelDatasPerBucket = chunkSizeInVoxels.x * chunkSizeInVoxels.y * chunkSizeInVoxels.z;
				unsigned int numBuckets = worldSizeInChunks.x * worldSizeInChunks.y * worldSizeInChunks.z;
				unsigned int megaVoxelsDataBufferObjectBindingLocation = 2;


				Vector3Int chunkIndex = Vector3Int{ i, 0, k };
				unsigned int flattenedChunkIndexForVoxelsDataPoolMetadatas = GetFlattenedChunkIndexForChunksVoxelsDataPoolMetadatas(worldSizeInChunks, chunkIndex);

				chunksVoxelsDataPoolMetadatas[flattenedChunkIndexForVoxelsDataPoolMetadatas].packedChunkIndex = chunkIndex.x + (chunkIndex.y << 5) + (chunkIndex.z << 10);
				GenerateChunkVoxelPositionsOnGPUAsSSBOAsTriangleWithVoxelDataPoolForIndirectDrawCommands(noiseOutput, chunkSizeInVoxels, voxelsDataPool, chunksVoxelsDataPoolMetadatas[flattenedChunkIndexForVoxelsDataPoolMetadatas]);
				WriteChunkDataToDrawCommand(chunksVoxelsDataPoolMetadatas[flattenedChunkIndexForVoxelsDataPoolMetadatas], chunksPerFaceIndirectDrawCommands);

			}
		}
	}
}