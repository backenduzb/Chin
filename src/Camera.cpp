#include "../include/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getView() {
    // Position the camera further back and slightly higher
    // Look directly at a point that is likely the center of a character (e.g., y=1.0)
    return glm::lookAt(
        glm::vec3(0.0f, 1.0f, 5.0f),  // Eye position (x, y, z)
        glm::vec3(0.0f, 1.0f, 0.0f),  // Look at point (centered on character height)
        glm::vec3(0.0f, 1.0f, 0.0f)   // Up vector (perfectly vertical)
    );
}
