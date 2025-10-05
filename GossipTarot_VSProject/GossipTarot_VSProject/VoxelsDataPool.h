#pragma once

#include <queue>

#include "glad/glad.h"

struct ChunkVoxelsDataPoolMetadata {

public:
	unsigned int voxelDataBucketIndex = 0;
	unsigned int voxelDataBucketOffsetIntoMegaArray = 0;
	unsigned int numVoxelDataInBucket = 0;
};

class VoxelsDataPool {

public:

	unsigned int megaVoxelsDataBufferObjectID;
	unsigned int megaVoxelsDataBufferObjectBindingLocation;

	uint32_t* megaVoxelsDataBufferGPUPointer;

	unsigned int numVoxelDataPerBucket;
	unsigned int numBuckets;

	size_t sizeOfBucketInBytes;
	size_t sizeOfPoolInBytes;

	std::queue<unsigned int> freeDataBuckets;


	VoxelsDataPool(unsigned int _numVoxelDataPerBucket, unsigned int _numBuckets, unsigned int _megaVoxelsDataBufferObjectBindingLocation) {

		glCreateBuffers(1, &megaVoxelsDataBufferObjectID);

		numVoxelDataPerBucket = _numVoxelDataPerBucket;
		numBuckets = _numBuckets;

		megaVoxelsDataBufferObjectBindingLocation = _megaVoxelsDataBufferObjectBindingLocation;

		sizeOfBucketInBytes = sizeof(unsigned int) * numVoxelDataPerBucket;
		sizeOfPoolInBytes =  sizeOfBucketInBytes * numBuckets;

		glNamedBufferStorage(megaVoxelsDataBufferObjectID, sizeOfPoolInBytes, nullptr,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

		void* mappedPointerToBuffer = glMapNamedBufferRange(
			megaVoxelsDataBufferObjectID, 0, sizeOfPoolInBytes,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		);
		megaVoxelsDataBufferGPUPointer = reinterpret_cast<uint32_t*>(mappedPointerToBuffer);

		for (unsigned int i = 0; i < numBuckets; i++)
		{
			freeDataBuckets.push(i);
		}
	}

	~VoxelsDataPool() {
		glUnmapNamedBuffer(megaVoxelsDataBufferObjectID);
		glDeleteBuffers(1, &megaVoxelsDataBufferObjectID);
	}

	bool WriteDataToFreePool(const std::vector<unsigned int>& chunkVoxelDataToCopyToPool, ChunkVoxelsDataPoolMetadata& chunkVoxelDataPoolMetadata) {

		unsigned int newFreeBucketIndex;
		if (GetFreePool(newFreeBucketIndex)) {
			uint32_t* pointerToBucket = megaVoxelsDataBufferGPUPointer + (newFreeBucketIndex * numVoxelDataPerBucket);
			memcpy(pointerToBucket, chunkVoxelDataToCopyToPool.data(), chunkVoxelDataToCopyToPool.size() * sizeof(unsigned int));
			
			chunkVoxelDataPoolMetadata.voxelDataBucketIndex = newFreeBucketIndex;
			chunkVoxelDataPoolMetadata.voxelDataBucketOffsetIntoMegaArray = newFreeBucketIndex * numVoxelDataPerBucket;

			return true;
		}
		else {
			return false;
		}

	}

private:

	bool GetFreePool(unsigned int& freePoolIndex) {
		if (freeDataBuckets.empty()) {
			return false;
		}

		freePoolIndex = freeDataBuckets.front();
		freeDataBuckets.pop();

		return true;
	}

};