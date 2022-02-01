#include <iostream>
#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>

using namespace std;

int main()
{
    vector<string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const string& word : msg) {
        cout << word << " ";
    }
    cout << endl;

    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window) {
        cout << "No window";
    }
}
