#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getView() {
    return glm::lookAt(
        glm::vec3(2.0f, 1.0f, 6.0f),  
        glm::vec3(0.0f, 1.0f, 0.0f),  
        glm::vec3(0.0f, 1.0f, 0.0f)   
    );
}
