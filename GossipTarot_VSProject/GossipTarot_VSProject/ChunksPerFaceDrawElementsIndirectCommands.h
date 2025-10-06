#pragma once

#include <vector>

#include "glad/glad.h"

struct DrawElementsIndirectCommand {

public:
	unsigned int  count;			// Number of Elements to be rendered.
	unsigned int  instanceCount;	// Number of instances of said element to be redered.
	unsigned int  firstIndex;		// Index of first Element.
	int  baseVertex;				// Base vertex to offset to add when accessing vertex array.
	unsigned int  baseInstance;		// Base instance offset to add to each element for instanced attributes.
};

class ChunksPerFaceIndirectDrawCommands {
	
public:

	std::vector<DrawElementsIndirectCommand> cpu_drawElementsIndirectCommands;

	unsigned int numDrawCommandsFilled;

	unsigned int gpu_drawElementsIndirectCommandsBufferID;
	DrawElementsIndirectCommand* gpu_drawElementsIndirectCommandsBufferPointer;

	ChunksPerFaceIndirectDrawCommands(unsigned int maxNumDrawCommands) {
		cpu_drawElementsIndirectCommands.resize(maxNumDrawCommands);
		
		numDrawCommandsFilled = 0;

		gpu_drawElementsIndirectCommandsBufferID = 0;
		gpu_drawElementsIndirectCommandsBufferPointer = nullptr;
	}

	void GPU_InitCommandBuffer() {
		glCreateBuffers(1, &gpu_drawElementsIndirectCommandsBufferID);

		size_t sizeOfCommandsBufferInBytes = cpu_drawElementsIndirectCommands.size() * sizeof(DrawElementsIndirectCommand);

		glNamedBufferStorage(gpu_drawElementsIndirectCommandsBufferID,
			sizeOfCommandsBufferInBytes,
			(const void*)cpu_drawElementsIndirectCommands.data(),
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

		void* mappedPointerToBuffer = glMapNamedBufferRange(
			gpu_drawElementsIndirectCommandsBufferID, 0, sizeOfCommandsBufferInBytes,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		);
		gpu_drawElementsIndirectCommandsBufferPointer = reinterpret_cast<DrawElementsIndirectCommand*>(mappedPointerToBuffer);
	}

	void GPU_UpdateIndirectCommandsBuffer(unsigned int _numDrawCommandsFilled) {
		numDrawCommandsFilled = _numDrawCommandsFilled;
		memcpy(gpu_drawElementsIndirectCommandsBufferPointer, cpu_drawElementsIndirectCommands.data(), numDrawCommandsFilled * sizeof(DrawElementsIndirectCommand));
	}

};