#pragma once 


#include <glm/glm.hpp>

namespace plt 

{
	struct MVPUniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

}
