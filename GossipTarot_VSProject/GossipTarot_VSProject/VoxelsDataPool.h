#pragma once

#include <queue>
#include <vector>

#include "glad/glad.h"

#include "ChunksVoxelsDataPoolMetadata.h"

class VoxelsDataPool {

public:

	unsigned int megaVoxelsPerFaceDataBufferObjectID;
	unsigned int megaVoxelsPerFaceDataBufferObjectBindingLocation;

	uint32_t* megaVoxelsPerFaceDataBufferGPUPointer;

	unsigned int numVoxelDataPerBucket;
	unsigned int numBuckets;

	size_t sizeOfBucketInBytes;
	size_t sizeOfPoolInBytes;

	std::queue<unsigned int> freeDataBuckets;


	VoxelsDataPool(unsigned int _numVoxelDataPerBucket, unsigned int _numBuckets, unsigned int _megaVoxelsPerFaceDataBufferObjectBindingLocation) {

		glCreateBuffers(1, &megaVoxelsPerFaceDataBufferObjectID);

		numVoxelDataPerBucket = _numVoxelDataPerBucket;
		numBuckets = _numBuckets;

		megaVoxelsPerFaceDataBufferObjectBindingLocation = _megaVoxelsPerFaceDataBufferObjectBindingLocation;

		sizeOfBucketInBytes = sizeof(unsigned int) * numVoxelDataPerBucket;
		sizeOfPoolInBytes =  sizeOfBucketInBytes * numBuckets;

		glNamedBufferStorage(megaVoxelsPerFaceDataBufferObjectID, sizeOfPoolInBytes, nullptr,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

		void* mappedPointerToBuffer = glMapNamedBufferRange(
			megaVoxelsPerFaceDataBufferObjectID, 0, sizeOfPoolInBytes,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		);
		megaVoxelsPerFaceDataBufferGPUPointer = reinterpret_cast<uint32_t*>(mappedPointerToBuffer);

		for (unsigned int i = 0; i < numBuckets; i++)
		{
			freeDataBuckets.push(i);
		}
	}

	~VoxelsDataPool() {
		glUnmapNamedBuffer(megaVoxelsPerFaceDataBufferObjectID);
		glDeleteBuffers(1, &megaVoxelsPerFaceDataBufferObjectID);
	}

	bool WriteFaceVoxelDataToFreeBucket(const std::vector<unsigned int>& chunkVoxelFaceDataToCopyToPool, FaceVoxelsDataPoolMetadata& chunkFaceVoxelDataPoolMetadata) {

		unsigned int newFreeBucketIndex;
		if (GetFreeBucket(newFreeBucketIndex)) {
			uint32_t* pointerToBucket = megaVoxelsPerFaceDataBufferGPUPointer + (newFreeBucketIndex * numVoxelDataPerBucket);
			memcpy(pointerToBucket, chunkVoxelFaceDataToCopyToPool.data(), chunkVoxelFaceDataToCopyToPool.size() * sizeof(unsigned int));
			
			chunkFaceVoxelDataPoolMetadata.voxelDataBucketIndex = newFreeBucketIndex;
			chunkFaceVoxelDataPoolMetadata.voxelDataBucketOffsetIntoMegaArray = newFreeBucketIndex * numVoxelDataPerBucket;

			return true;
		}
		else {
			return false;
		}

	}

private:

	bool GetFreeBucket(unsigned int& freePoolIndex) {
		if (freeDataBuckets.empty()) {
			return false;
		}

		freePoolIndex = freeDataBuckets.front();
		freeDataBuckets.pop();

		return true;
	}

};