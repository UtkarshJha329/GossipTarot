#pragma once

#include "GLMIncludes.h"
#include "MeshOnGpu.h"

#include "FastNoise/FastNoise.h"
#include "VoxelFunctions.h"

void GenerateChunksAndAddToIndirectRenderCommandVectorOnCPU(const Vector3Int& worldSizeInChunks, const Vector3Int& chunkSizeInVoxels, VoxelsDataPool& voxelsDataPool, IndirectDrawCommands& chunksIndirectDrawCommands) {

	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

	fnFractal->SetSource(fnSimplex);
	fnFractal->SetOctaveCount(5);

	for (unsigned int k = 0; k < worldSizeInChunks.y; k++)
	{
		for (unsigned int j = 0; j < worldSizeInChunks.x; j++)
		{
			for (unsigned int i = 0; i < worldSizeInChunks.z; i++)
			{
				std::vector<float> noiseOutput(chunkSizeInVoxels.x * chunkSizeInVoxels.z);

				Vector3Int chunkStartWorldPos = Vector3Int{ i * chunkSizeInVoxels.x, k * chunkSizeInVoxels.y, j * chunkSizeInVoxels.z };
				fnFractal->GenUniformGrid2D(noiseOutput.data(), chunkStartWorldPos.x, chunkStartWorldPos.z, chunkSizeInVoxels.x, chunkSizeInVoxels.z, 0.2f, 1337);

				unsigned int numVoxelDatasPerBucket = chunkSizeInVoxels.x * chunkSizeInVoxels.y * chunkSizeInVoxels.z;
				unsigned int numBuckets = worldSizeInChunks.x * worldSizeInChunks.y * worldSizeInChunks.z;
				unsigned int megaVoxelsDataBufferObjectBindingLocation = 2;


				unsigned int numIndicesInChunk = 0;
				MeshOnGPU chunkMeshOnGPU;
				ChunkVoxelsDataPoolMetadata chunkVoxelsDataPoolMetadata;
				GenerateChunkVoxelPositionsOnGPUAsSSBOAsTriangleWithVoxelDataPoolForIndirectDrawCommands(noiseOutput, chunkSizeInVoxels, voxelsDataPool, chunkVoxelsDataPoolMetadata);


				AddChunkToDrawCommand(Vector3Int{ i, 0, j }, chunkVoxelsDataPoolMetadata, chunksIndirectDrawCommands);
			}
		}
	}
}