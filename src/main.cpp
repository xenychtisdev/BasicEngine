/*//#define GLAD_IMPLEMENTATION
//#include <glad/gl.h>

#include <iostream>


int main()
{
    std::cout << "Hello World" << std::endl;

    return 0;
}

*/

#include <iostream>
#include <GLFW/glfw3.h>

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a window
    GLFWwindow* window = glfwCreateWindow(
        800, 600,
        "BasicEngine - Hello GLFW!",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Make the window's OpenGL context current
    glfwMakeContextCurrent(window);

    std::cout << "Hello, GLFW!\n";

    // Main application loop
    while (!glfwWindowShouldClose(window))
    {
        // Process keyboard, mouse, and window events
        glfwPollEvents();

        // Present the rendered frame
        glfwSwapBuffers(window);
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}