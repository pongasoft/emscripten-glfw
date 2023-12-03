#include <GLFW/glfw3.h>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

GLFWAPI int glfwInit()
{
  printf("glfwInit()\n");
  return GLFW_TRUE;
}

GLFWAPI void glfwTerminate(void)
{
  printf("glfwTerminate()\n");
}

#ifdef __cplusplus
} // extern "C"
#endif
