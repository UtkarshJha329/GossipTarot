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

class IndirectDrawCommands {
	
public:

	std::vector<DrawElementsIndirectCommand> cpu_drawElementsIndirectCommands;
	unsigned int gpu_drawElementsIndirectCommandsBuffer;

	void GPU_InitCommandBuffer() {
		glCreateBuffers(1, &gpu_drawElementsIndirectCommandsBuffer);

		glNamedBufferStorage(gpu_drawElementsIndirectCommandsBuffer,
			sizeof(DrawElementsIndirectCommand) * cpu_drawElementsIndirectCommands.size(),
			(const void*)cpu_drawElementsIndirectCommands.data(),
			GL_DYNAMIC_STORAGE_BIT);
	}

};