// Preprocessor Directives
#ifndef GLITTER
#define GLITTER
#pragma once

// System Headers
// GLEW
//#define GLEW_STATIC
//#include <GL/glew.h>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
//     #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Define Some Constants
const int mWidth = 1280;
const int mHeight = 800;

#endif //~ Glitter Header
