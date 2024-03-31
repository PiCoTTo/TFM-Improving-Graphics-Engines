#include "Context.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "core/Log.h"

nimo::Context::Context(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        NIMO_CRITICAL("Failed to initialize OpenGL context");
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
}
nimo::Context::~Context(){
    
}