#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <iostream>
#include <stdlib.h>
#include <time.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void generate_vectors(float positions[], int phrase_length);

struct vectorNode {
    float normalized_value_x;
    float normalized_value_y;
    vectorNode* next = nullptr;
};

const char *phrase = ("The highway is full of big cars going nowhere fast And folks is smoking anything that'll burn \
Some people wrap their lives around a cocktail glass And you sit wondering where you're going to turn. \
I got it Come. And be my baby?");
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600; 

const char* vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0"; 
const char* fragment_shader_source = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Graphical", NULL, NULL); 

    if (window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL); 

    glCompileShader(vertex_shader); 

    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    int phrase_length = 227; //sizeof(phrase) - 1;
    float positions[phrase_length * 2];
    generate_vectors(positions, phrase_length);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO); 
    glBindVertexArray(VAO); 

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(colors), NULL, GL_STATIC_DRAW); 
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(colors), colors);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.75f, 0.74f, 0.74f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glPointSize(20.0f);
        glDrawArrays(GL_POINTS, 0, phrase_length);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void generate_vectors(float positions[], int phrase_length) {
    vectorNode* head = nullptr;
    vectorNode* marker = nullptr;

    srand(time(NULL));
    
    for (int i = 0; i < phrase_length; i++) {
        if ((phrase[i] >= 'a' && phrase[i] <= 'z') || (phrase[i] >= 'A' && phrase[i] <= 'Z')) {
            vectorNode* newNode = new vectorNode;

            int randInt = rand() % 10 - 4;
            float normalized_x = ((phrase[i] - (phrase[i] > 'a' ? 114 : 82) + randInt) / 18.0);
            float normalized_y = randInt / 7.0;
            

            // std::cout << phrase[i] << ": " << i << std::endl;
            // std::cout << normalized_x << std::endl;
            // std::cout << normalized_y << std::endl;

            newNode->normalized_value_x = normalized_x;
            newNode->normalized_value_y = normalized_y;
 
            if (head == nullptr) {
                head = newNode;
                marker = head;
            } else {
                marker->next = newNode;
                marker = newNode;
            }
        }
    }
    marker = head;
    int i = 0;
    while (marker != nullptr) {
        positions[i] = marker->normalized_value_x;
        positions[i+1] = marker->normalized_value_y; 
        positions[i+2] = 1.0f;
        positions[i+3] = 0.0f;
        positions[i+4] = 0.0f;
        i+=5;
        vectorNode* next = marker->next;
        delete marker;
        marker = next;
    }
}
