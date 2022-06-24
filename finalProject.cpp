#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL2/SOIL2.h>

using namespace std;

int width, height;
const double PI = 3.14159;
const float toRadians = PI / 180.0f;

// Declare Input Callback Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);

// Declare View Matrix
glm::mat4 viewMatrix;

// Camera Field of View
GLfloat fov = 45.0f;

void initiateCamera();

// Define Camera Attributes
glm::vec3 cameraPosition = glm::vec3(0.0f, 2.5f, 5.0f); // Move 3 units back in z towards screen
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // What the camera points to
glm::vec3 cameraDirection = glm::normalize(cameraPosition - target); // direction z
glm::vec3 worldUp = glm::vec3(0.0, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));// right vector x
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight)); // up vector y
glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 1 unit away from lense


													  // Camera Transformation Prototype
void TransformCamera();

// Boolean array for keys and mouse buttons
bool keys[1024], mouseButtons[3];

// Input state booleans
bool isPanning = false, isOrbiting = false;

// Pitch and Yaw
GLfloat radius = 3.0f, rawYaw = 0.0f, rawPitch = 0.0f, degYaw, degPitch;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = 320, lastY = 240, xChange, yChange; // Center mouse cursor
bool firstMouseMove = true;

// Light Position
glm::vec3 lightPosition(1.0, 1., 1.);


// Draw Primitive(s)
void draw()
{
	GLenum mode = GL_TRIANGLES;
	GLsizei indices = 6;
	glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

GLuint verts;
void drawDesk() {
	GLenum mode = GL_TRIANGLES;
	glDrawArrays(mode, 0, verts);
}
GLuint iMacVerts;
void drawiMac() {
	GLenum mode = GL_TRIANGLES;
	glDrawArrays(mode, 0, iMacVerts);
}
GLuint screenVerts;
void drawScreen() {
	GLenum mode = GL_TRIANGLES;
	glDrawArrays(mode, 0, screenVerts);
}
GLuint kBVerts;
void drawKB() {
	GLenum mode = GL_TRIANGLES;
	glDrawArrays(mode, 0, kBVerts);
}

GLuint mouseVerts;

void drawMouse() {
	GLenum mode = GL_TRIANGLES;
	glDrawArrays(mode, 0, mouseVerts);
}


// Create and Compile Shaders
static GLuint CompileShader(const string& source, GLuint shaderType)
{
	// Create Shader object
	GLuint shaderID = glCreateShader(shaderType);
	const char* src = source.c_str();

	// Attach source code to Shader object
	glShaderSource(shaderID, 1, &src, nullptr);

	// Compile Shader
	glCompileShader(shaderID);

	// Return ID of Compiled shader
	return shaderID;

}

// Create Program Object
static GLuint CreateShaderProgram(const string& vertexShader, const string& fragmentShader)
{
	// Compile vertex shader
	GLuint vertexShaderComp = CompileShader(vertexShader, GL_VERTEX_SHADER);

	// Compile fragment shader
	GLuint fragmentShaderComp = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	// Create program object
	GLuint shaderProgram = glCreateProgram();

	// Attach vertex and fragment shaders to program object
	glAttachShader(shaderProgram, vertexShaderComp);
	glAttachShader(shaderProgram, fragmentShaderComp);

	// Link shaders to create executable
	glLinkProgram(shaderProgram);

	// Delete compiled vertex and fragment shaders
	glDeleteShader(vertexShaderComp);
	glDeleteShader(fragmentShaderComp);

	// Return Shader Program
	return shaderProgram;

}


int main(void)
{
	width = 640; height = 480;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Main Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set input callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		cout << "Error!" << endl;


	GLfloat vertices[] = {
		 //Positions			   //colors     			 //Texture Coordinates   //Normal
		
						//bottom plane of desk
		 //Positions               //Color				//Texture			//Normals  bb  b         
		 -5.5f,  -0.5f, -2.0f,    1.0f,1.0f,1.0f,      0.0f,0.0f,           0.0f,  0.0f, 1.0f, // Bottom front  left corner 
		 5.0f,  -0.5f, -2.0f,     1.0f,1.0f,1.0f,      1.0f,0.0f,           0.0f,  0.0f, 1.0f, // Bottom Front  Right corner 
		 5.0f,  -0.5f,  2.0f,     1.0f,1.0f,1.0f,       1.0f,1.0f,          0.0f,  0.0f, 1.0f,  // Bottom Back   Right corner 

		 -5.5f,  -0.5f, -2.0f,    1.0f,1.0f,1.0f,     0.0f,0.0f,            0.0f,  0.0f, 1.0f,  // Bottom front  left corner
		 5.0f,  -0.5f,  2.0f,     1.0f,1.0f,1.0f,       1.0f,1.0f,          0.0f,  0.0f, 1.0f, // Bottom Back   Right corner 
		-5.5f,  -0.5f,  2.0f,     1.0f,1.0f,1.0f,       0.0f,1.0f,          0.0f,  0.0f, 1.0f,  // Bottom Back   Left corner 

						//top plane of desk
									//Texture Coordinates
		-5.5f,  0.0f, -2.0f,      1.0f,1.0f,1.0f,        0.0f,0.0f,         0.0f,  0.0f, 1.0f,  //     Top front  left corner 
		 5.0f,  0.0f, -2.0f,      1.0f,1.0f,1.0f,        1.0f,0.0f,         0.0f,  0.0f, 1.0f,  //     Top Front  Right corner 
		 5.0f,  0.0f,  2.0f,      1.0f,1.0f,1.0f,        1.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Back   Right corner 

		 -5.5f,  0.0f, -2.0f,     1.0f,1.0f,1.0f,        0.0f,0.0f,         0.0f,  0.0f, 1.0f,  //     Top front  left corner 
		 5.0f,  0.0f,  2.0f,      1.0f,1.0f,1.0f,        1.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Back   Right corner 
		-5.5f,  0.0f,  2.0f,      1.0f,1.0f,1.0f,        0.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Back   Left corner 

						// left side
		-5.5f,  -0.5f, -2.0f,     1.0f,1.0f,1.0f,       1.0f,0.0f,          0.0f,  0.0f, 1.0f, // Bottom front  left corner
		-5.5f,  0.0f, -2.0f,      1.0f,1.0f,1.0f,        1.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top front  left corner
		-5.5f,  0.0f,  2.0f,      1.0f,1.0f,1.0f,        0.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Back   Left corner

		 -5.5f,  -0.5f,  2.0f,    1.0f,1.0f,1.0f,       0.0f,0.0f,          0.0f,  0.0f, 1.0f, // Bottom Back   Left corner
		 -5.5f,  -0.5f, -2.0f,    1.0f,1.0f,1.0f,      1.0f,0.0f,           0.0f,  0.0f, 1.0f, // Bottom front  left corner
		 -5.5f,  0.0f,  2.0f,     1.0f,1.0f,1.0f,        0.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Back   Left corner
						//right side
		 5.0f,  -0.5f, -2.0f,     1.0f,1.0f,1.0f,       0.0f,0.0f,          0.0f,  0.0f, 1.0f,  // Bottom Front  Right corner 
		 5.0f,  -0.5f,  2.0f,     1.0f,1.0f,1.0f,       1.0f,0.0f,          0.0f,  0.0f, 1.0f,  // Bottom Back   Right corner 
		 5.0f,  0.0f, -2.0f,      1.0f,1.0f,1.0f,       0.0f,1.0f,          0.0f,  0.0f, 1.0f,  // Top Front  Right corner 

		 5.0f,  0.0f,  2.0f,      1.0f,1.0f,1.0f,        1.0f,1.0f,         0.0f,  0.0f, 1.0f,  // Top Back   Right corner
		 5.0f,  -0.5f,  2.0f,     1.0f,1.0f,1.0f,       1.0f,0.0f,          0.0f,  0.0f, 1.0f, // Bottom Back   Right corner 
		 5.0f,  0.0f, -2.0f,      1.0f,1.0f,1.0f,       0.0f,1.0f,          0.0f,  0.0f, 1.0f, // Top Front  Right corner 
						//front side
		 -5.5f,  0.0f, -2.0f,     1.0f,1.0f,1.0f,        0.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top front  left corner 
		 5.0f,  0.0f, -2.0f,      1.0f,1.0f,1.0f,        1.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Front  Right corner
		 -5.5f,  -0.5f, -2.0f,    1.0f,1.0f,1.0f,      0.0f,0.0f,           0.0f,  0.0f, 1.0f, // Bottom front  left corner

		 -5.5f,  -0.5f, -2.0f,    1.0f,1.0f,1.0f,      0.0f,0.0f,           0.0f,  0.0f, 1.0f,  // Bottom front  left corner
		 5.0f,  -0.5f, -2.0f,     1.0f,1.0f,1.0f,       1.0f,0.0f,          0.0f,  0.0f, 1.0f,  // Bottom Front  Right corner 
		  5.0f,  0.0f, -2.0f,     1.0f,1.0f,1.0f,        1.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Front  Right corner
						//back side
		 5.0f,  0.0f,  2.0f,      1.0f,1.0f,1.0f,       0.0f,1.0f,          0.0f,  0.0f, 1.0f,  //     Top Back   Right corner 
		 -5.5f,  0.0f,  2.0f,     1.0f,1.0f,1.0f,        1.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Back   Left corner 
		 -5.5f,  -0.5f,  2.0f,    1.0f,1.0f,1.0f,       1.0f,0.0f,          0.0f,  0.0f, 1.0f,  // Bottom Back   Left corner

		 5.0f,  -0.5f,  2.0f,     1.0f,1.0f,1.0f,       0.0f,0.0f,          0.0f,  0.0f, 1.0f,  // Bottom Back   Right corner
		 -5.5f,  -0.5f,  2.0f,    1.0f,1.0f,1.0f,      1.0f,0.0f,           0.0f,  0.0f, 1.0f, // Bottom Back   Left corner
		 5.0f,  0.0f,  2.0f,      1.0f,1.0f,1.0f,        0.0f,1.0f,         0.0f,  0.0f, 1.0f,  //     Top Back   Right corner 
		 

	};

	verts = sizeof(vertices);

	//iMac computer verticies
	GLfloat pcVertices[] = {
		// Index				//Color		        	//Texture		//Normals
					//Base
	//Bottom of Base
		-1.25f, 0.5f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.25f, 0.5f,  1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.25f, 0.5f,  1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.25f, 0.5f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.25f, 0.5f,  1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.25f, 0.5f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		 //Top of Base
		 -1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 -1.25f, 0.6f,  1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		  1.25f, 0.6f,  1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 -1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		  1.25f, 0.6f,  1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		  1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		  //Left of Base
		-1.25f, 0.5f,  1.5f, 	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		-1.25f, 0.6f,  1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		-1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.25f, 0.5f,  1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		-1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.25f, 0.5f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		  //Right of Base
		 1.25f, 0.5f,  1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 1.25f, 0.6f,  1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.25f, 0.5f,  1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.25f, 0.5f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		   //Front of Base
		  -1.25f, 0.5f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		  -1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		   1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		  -1.25f, 0.5f, 0.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   1.25f, 0.6f, 0.0f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		   1.25f, 0.5f, 0.0f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		   //Back of Base
		  -1.25f, 0.5f, 1.5f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		  -1.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		   1.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		  -1.25f, 0.5f, 1.5f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   1.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		   1.25f, 0.5f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
					   //Neck
		   //Bottom of Neck
		   -0.25f, 0.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 0.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 0.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   //Top of Neck
		   -0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
			0.25f, 2.6f, 1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
			0.25f, 2.6f, 1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
			0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   //Front of Neck
		   -0.25f, 0.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 0.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 0.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		   //Back of Neck
		   -0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
			0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
			0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
			0.25f, 2.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		   //Left of Neck
		   -0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 0.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   //Right of Neck
		    0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 0.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 0.6f, 1.5f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		    0.25f, 2.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
						// Monitor/Bezel
		   //Bottom of bezel
		   -2.0f, 1.6f, 1.3f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -2.0f, 1.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		    2.0f, 1.6f, 1.3f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		   -2.0f, 1.6f, 1.4f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		    2.0f, 1.6f, 1.3f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		    2.0f, 1.6f, 1.4f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		   //Top of bezel
			-2.0f, 3.6f, 1.3f,	0.75f, 0.75f, 0.75f,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 3.6f, 1.4f,  0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 3.6f, 1.3f,  0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 3.6f, 1.4f,  0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 3.6f, 1.3f,  0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 3.6f, 1.4f,  0.75f, 0.75f, 0.75f,	1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		   //Front of bezel
		    -2.0f, 1.6f, 1.3f,	0.75f, 0.75f, 0.75f,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		    -2.0f, 3.6f, 1.3f,	0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		     2.0f, 1.6f, 1.3f,	0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 3.6f, 1.3f,	0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 1.6f, 1.3f,	0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 3.6f, 1.3f,	0.75f, 0.75f, 0.75f,	1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		   //Back of bezel
			-2.0f, 1.6f, 1.4f,	0.75f, 0.75f, 0.75f,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 3.6f, 1.4f,	0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 1.6f, 1.4f,	0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 3.6f, 1.4f,	0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 1.6f, 1.4f,	0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 3.6f, 1.4f,	0.75f, 0.75f, 0.75f,	1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		   //Left of bezel
		    -2.0f, 1.6f, 1.4f,	0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 3.6f, 1.4f,	0.75f, 0.75f, 0.75f,	1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 3.6f, 1.3f,	0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 1.6f, 1.4f,	0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 1.6f, 1.3f,	0.75f, 0.75f, 0.75f,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			-2.0f, 3.6f, 1.3f,	0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		   //Right of bezel
			 2.0f, 1.6f, 1.4f,	0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 3.6f, 1.4f,	0.75f, 0.75f, 0.75f,	1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 3.6f, 1.3f,	0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 1.6f, 1.4f,	0.75f, 0.75f, 0.75f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 1.6f, 1.3f,	0.75f, 0.75f, 0.75f,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			 2.0f, 3.6f, 1.3f,	0.75f, 0.75f, 0.75f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,			
	};
	 iMacVerts = sizeof(pcVertices);



	GLfloat screenInfo[] = {
						//Screen
			// front of screen
			-1.8f, 1.8f, 1.29f,		0.19f, 0.84f, 0.78f,	0.0f,0.0f,		0.0f, 0.0f, 1.0f,
			-1.8f, 3.4f, 1.29f,		0.19f, 0.84f, 0.78f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 1.4f, 3.4f, 1.29f,		0.19f, 0.84f, 0.78f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,

			-1.8f, 1.8f, 1.29f,		0.19f, 0.84f, 0.78f,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			 1.4f, 3.4f, 1.29f,		0.19f, 0.84f, 0.78f,	1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 1.4f, 1.8f, 1.29f,		0.19f, 0.84f, 0.78f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,

	};
	 screenVerts = sizeof(screenInfo);

	 GLfloat keyboardVertices[] = {

		 //Keyboard
		//Positions				//Color					//Texture		//Normals
		//Bottom of kb
		-1.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.5f,  2.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.5f,  2.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.5f,  2.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		 //Top of kb
		 -1.0f, 0.6f,  1.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 -1.0f, 0.6f,  2.0f,	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		  1.0f, 0.6f,  2.0f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 -1.0f, 0.6f,  1.0f,	0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		  1.0f, 0.6f,  2.0f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		  1.0f, 0.6f,  1.0f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		  //Left of kb
		-1.0f, 0.5f,  2.0f, 	0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.6f,  2.0f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.5f,  2.0f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		//Right of kb
		1.0f, 0.5f,  2.0f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		1.0f, 0.6f,  2.0f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		1.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		1.0f, 0.5f,  2.0f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		1.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		1.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		//Front of kb
		-1.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		 //Back of kb
		-1.0f, 0.5f, 2.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.6f, 2.0f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.6f, 2.0f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		-1.0f, 0.5f, 2.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.6f, 2.0f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 1.0f, 0.5f, 2.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
			 };
	  kBVerts = sizeof(keyboardVertices);

	 GLfloat mouseVertices[] = {
							//Mouse
		 //Positions				//Color					//Texture		//Normals
		//Bottom of mouse
		3.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.5f,  1.5f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.5f,  1.5f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.5f,  1.5f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		 //Top of mouse
		 3.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 3.0f, 0.6f,  1.5f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 3.25f, 0.6f,  1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 3.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		 3.25f, 0.6f,  1.5f,	0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		 3.25f, 0.6f,  1.0f,	0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		  //Left of mouse
		3.0f, 0.5f,  1.5f, 		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.6f,  1.5f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.5f,  1.5f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		//Right of mouse
		3.25f, 0.5f,  1.5f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.6f,  1.5f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.5f,  1.5f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		//Front of mouse
		3.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.6f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.5f,  1.0f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,

		 //Back of mouse
		3.0f, 0.5f, 1.5f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.6f, 1.5f,		0.75f,0.75f,0.75f,		1.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.6f, 1.5f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.0f, 0.5f, 1.5f,		0.75f,0.75f,0.75f,		1.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.6f, 1.5f,		0.75f,0.75f,0.75f,		0.0f, 1.0f,		 0.0f,  0.0f, 1.0f,
		3.25f, 0.5f, 1.5f,		0.75f,0.75f,0.75f,		0.0f, 0.0f,		 0.0f,  0.0f, 1.0f,
	 };

		 mouseVerts = sizeof(mouseVertices);


	GLfloat lampVertices[] = {

		// Triangle 1
		1.5, -0.5, 0.0, // index 0
		1.5, 0.5, 0.0, // index 1
		2.0, -0.5, 0.0,  // index 2	

						 // Triangle 2	
						 2.0, 0.5, 0.0  // index 3	
	};


	// Define element indices
	GLubyte indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	// Plane Transforms
	glm::vec3 planePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.5f),
		glm::vec3(0.5f,  0.0f,  0.0f),
		glm::vec3(0.0f,  0.0f,  -0.5f),
		glm::vec3(-0.5f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.5f,  0.0f),
		glm::vec3(0.0f, -0.5f,  0.0f)
	};

	glm::float32 planeRotations[] = {
		0.0f, 90.0f, 180.0f, -90.0f, -90.f, 90.f
	};

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint deskVBO, deskEBO, deskVAO, floorVBO, floorEBO, floorVAO, lampVBO, lampEBO, lampVAO,
		iMacVBO, iMacEBO, iMacVAO, screenVBO, screenEBO, screenVAO ,kBVBO, kBEBO, kBVAO, mouseVBO, mouseEBO, mouseVAO;

	glGenBuffers(1, &deskVBO); // Create VBO
	glGenBuffers(1, &deskEBO); // Create EBO

	glGenBuffers(1, &floorVBO); // Create VBO
	glGenBuffers(1, &floorEBO); // Create EBO

	glGenBuffers(1, &lampVBO); // Create VBO
	glGenBuffers(1, &lampEBO); // Create EBO

	glGenBuffers(1, &iMacVBO); // Create VBO
	glGenBuffers(1, &iMacEBO); // Create EBO

	glGenBuffers(1, &screenVBO); // Create VBO
	glGenBuffers(1, &screenEBO); // Create EBO

	glGenBuffers(1, &kBVBO); // Create VBO
	glGenBuffers(1, &kBEBO); // Create EBO

	glGenBuffers(1, &mouseVBO); // Create VBO
	glGenBuffers(1, &mouseEBO); // Create EBO

	glGenVertexArrays(1, &deskVAO); // Create VOA
	glGenVertexArrays(1, &iMacVAO); //create VAO
	glGenVertexArrays(1, &floorVAO); // Create VOA
	glGenVertexArrays(1, &lampVAO); // Create VOA
	glGenVertexArrays(1, &screenVAO); // Create VAO
	glGenVertexArrays(1, &kBVAO); // Create VOA
	glGenVertexArrays(1, &mouseVAO); // Create VOA

	glBindVertexArray(deskVAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, deskVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, deskEBO); // Select EBO


	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Load vertex attributes
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

																					 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)


	glBindVertexArray(lampVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lampVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO); // Select EBO

	glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW); // Load vertex attributes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	glBindVertexArray(iMacVAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, iMacVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iMacEBO); // Select EBO


	glBufferData(GL_ARRAY_BUFFER, sizeof(pcVertices), pcVertices, GL_STATIC_DRAW); // Load vertex attributes
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

																					 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)




	glBindVertexArray(screenVAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenEBO); // Select EBO


	glBufferData(GL_ARRAY_BUFFER, sizeof(screenInfo), screenInfo, GL_STATIC_DRAW); // Load vertex attributes
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

																					 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)


	glBindVertexArray(kBVAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, kBVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kBEBO); // Select EBO


	glBufferData(GL_ARRAY_BUFFER, sizeof(keyboardVertices), keyboardVertices, GL_STATIC_DRAW); // Load vertex attributes
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

																					 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)


	glBindVertexArray(mouseVAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, mouseVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mouseEBO); // Select EBO


	glBufferData(GL_ARRAY_BUFFER, sizeof(mouseVertices), mouseVertices, GL_STATIC_DRAW); // Load vertex attributes
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

																					 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)


	// Load texture maps
	int deskTexWidth, deskTexHeight, gridTexWidth, gridTexHeight, iMacTexWidth, iMacTexHeight,
		screenTexWidth, screenTexHeight, kBTexWidth, kBTexHeight, mouseTexWidth, mouseTexHeight;
	unsigned char* deskImage = SOIL_load_image("wood.jpg", &deskTexWidth, &deskTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* iMacImage = SOIL_load_image("test2.jpg", &iMacTexWidth, &iMacTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* screenImage = SOIL_load_image("screen3.jpg", &screenTexWidth, &screenTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* kBImage = SOIL_load_image("keyboard.jpg", &kBTexWidth, &kBTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* mouseImage = SOIL_load_image("mouse.jpg", &mouseTexWidth, &mouseTexHeight, 0, SOIL_LOAD_RGB);

	//Testing image loading
	if (iMacImage == nullptr){ std::cout << "Failed to load iMac Image "; }
	if (screenImage == nullptr) { std::cout << "Failed to load screen Image "; }
	if (kBImage == nullptr) { std::cout << "Failed to load Keyboard Image "; }
	if (mouseImage == nullptr) { std::cout << "Failed to load mouse Image "; }

	// Generate Textures  
	GLuint deskTexture; // for texture ID
	glGenTextures(1, &deskTexture);// Generate texture id
	glBindTexture(GL_TEXTURE_2D, deskTexture); // Activate texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, deskTexWidth, deskTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, deskImage); // Generate texture
	glGenerateMipmap(GL_TEXTURE_2D); // Texture resolution managment
	SOIL_free_image_data(deskImage); // Free imge from memory
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind or close texture object

	GLuint iMacTexture; // for texture ID
	glGenTextures(1, &iMacTexture);// Generate texture id
	glBindTexture(GL_TEXTURE_2D, iMacTexture); // Activate texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iMacTexWidth, iMacTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, iMacImage); // Generate texture
	glGenerateMipmap(GL_TEXTURE_2D); // Texture resolution managment
	SOIL_free_image_data(iMacImage); // Free imge from memory
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind or close texture object
 
	GLuint screenTexture; // for texture ID
	glGenTextures(1, &screenTexture);// Generate texture id
	glBindTexture(GL_TEXTURE_2D, screenTexture); // Activate texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenTexWidth, screenTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, screenImage); // Generate texture
	glGenerateMipmap(GL_TEXTURE_2D); // Texture resolution managment
	SOIL_free_image_data(screenImage); // Free imge from memory
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind or close texture object\

	GLuint kBTexture; // for texture ID
	glGenTextures(1, &kBTexture);// Generate texture id
	glBindTexture(GL_TEXTURE_2D, kBTexture); // Activate texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kBTexWidth, kBTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, kBImage); // Generate texture
	glGenerateMipmap(GL_TEXTURE_2D); // Texture resolution managment
	SOIL_free_image_data(kBImage); // Free imge from memory
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind or close texture object

	GLuint mouseTexture; // for texture ID
	glGenTextures(1, &mouseTexture);// Generate texture id
	glBindTexture(GL_TEXTURE_2D, mouseTexture); // Activate texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mouseTexWidth, mouseTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mouseImage); // Generate texture
	glGenerateMipmap(GL_TEXTURE_2D); // Texture resolution managment
	//SOIL_free_image_data(kBImage); // Free imge from memory       //This throws an exception for some reason now.
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind or close texture object

	// desk Vertex shader source code
	string vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;"
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	// desk Fragment shader source code
	string fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 objectColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"// Ambient\n"
		"float ambientStrength = 0.1f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"// Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"vec3 diffuse = diff * 2.5f * lightColor;"
		"// Specular\n"
		"float specularStrength = 1.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result = (ambient + diffuse + specular) * objectColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

		// iMac vertex shader source code
	string iMacVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;"
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	// iMac  Fragment shader source code
	string iMacFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 iMacColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"// Ambient\n"
		"float ambientStrength = 0.5f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"// Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"vec3 diffuse = diff * 5.0 * lightColor;"
		"// Specular\n"
		"float specularStrength = 1.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result =(ambient + diffuse + specular) * 3 * iMacColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

	//Screen vertex shader
	string screenVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;"
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	// Screen Fragment shader source code
	string screenFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 screenColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"// Ambient\n"
		"float ambientStrength = 0.5f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"// Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"vec3 diffuse = diff * 5.0f * lightColor;"
		"// Specular\n"
		"float specularStrength = 1.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result = (ambient + diffuse + specular)* 3 * screenColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

	//Keyboard Vertex Shader
	string kBVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;"
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	// Keyboard Fragment shader source code
	string kBFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 kBColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"// Ambient\n"
		"float ambientStrength = 0.5f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"// Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"vec3 diffuse = diff * 5.0f * lightColor;"
		"// Specular\n"
		"float specularStrength = 1.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result = (ambient + diffuse + specular)* 3 * kBColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

	//Mouse Vertex Shader
	string mouseVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;"
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	// Mouse Fragment shader source code
	string mouseFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 mouseColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"// Ambient\n"
		"float ambientStrength = 0.5f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"// Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"vec3 diffuse = diff * 5.0f * lightColor;"
		"// Specular\n"
		"float specularStrength = 1.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result = (ambient + diffuse + specular)* 3 * mouseColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

	// Lamp Vertex shader source code
	string lampVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"}\n";

	// Lamp Fragment shader source code
	string lampFragmentShaderSource =
		"#version 330 core\n"
		"out vec4 fragColor;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(1.0f);"
		"}\n";

	// Creating Shader Program
	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

	// Creating Lamp Shader Program
	GLuint lampShaderProgram = CreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource);

	//createing iMac Shader Program 
	GLuint iMacShaderProgram = CreateShaderProgram(iMacVertexShaderSource, iMacFragmentShaderSource);

	//creating screen Shader Program
	GLuint screenShaderProgram = CreateShaderProgram(screenVertexShaderSource, screenFragmentShaderSource);

	//creating Keyboard Shader Program
	GLuint kBShaderProgram = CreateShaderProgram(kBVertexShaderSource, kBFragmentShaderSource);
	
	//creating mouse Shader Program
	GLuint mouseShaderProgram = CreateShaderProgram(mouseVertexShaderSource, mouseFragmentShaderSource);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Resize window and graphics simultaneously
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use Shader Program exe and select VAO before drawing 
		glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes


									 // Declare transformations (can be initialized outside loop)		
		glm::mat4 projectionMatrix;

		// Define LookAt Matrix
		viewMatrix = glm::lookAt(cameraPosition, target, worldUp);

		// Define projection matrix
		projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);




		// Get matrix's uniform location and set matrix
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

		// Get light and object color reference in Fragment Shader
		GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

		// Specify light and object color
		glUniform3f(objectColorLoc, 0.46f, 0.36f, 0.25f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(viewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


		glBindTexture(GL_TEXTURE_2D, deskTexture); // Apply crate texture (Auto detected by Uniform Sampler)
		glBindVertexArray(deskVAO); // User-defined VAO must be called before draw. 

		drawDesk();

		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -2.0f, -5.0f));
		modelMatrix = glm::rotate(modelMatrix, 0.f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2.f, 2.f, 2.f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		
		glUseProgram(0); //closes shader program probably
		
		//glBindTexture(GL_TEXTURE_2D, 0); // MAYBE WORKS?


		glUseProgram(iMacShaderProgram); //SHADER PROGRAM
		// Get matrix's uniform location and set matrix
		GLint iMacModelLoc = glGetUniformLocation(iMacShaderProgram, "model");
		GLint iMacViewLoc = glGetUniformLocation(iMacShaderProgram, "view");
		GLint iMacProjLoc = glGetUniformLocation(iMacShaderProgram, "projection");

		// Get light and object color reference in Fragment Shader
		GLint iMacColorLoc = glGetUniformLocation(iMacShaderProgram, "iMacColor");
		GLint iMacLightColorLoc = glGetUniformLocation(iMacShaderProgram, "lightColor");
		GLint iMacLightPosLoc = glGetUniformLocation(iMacShaderProgram, "lightPos");
		GLint iMacViewPosLoc = glGetUniformLocation(iMacShaderProgram, "viewPos");

		// Specify light and object color
		glUniform3f(iMacColorLoc, 0.75f, 0.75f, 0.75f);
		glUniform3f(iMacLightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(iMacLightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(iMacViewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		glUniformMatrix4fv(iMacViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(iMacProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


		glBindTexture(GL_TEXTURE_2D, iMacTexture); // Apply crate texture (Auto detected by Uniform Sampler)
		glBindVertexArray(iMacVAO); // User-defined VAO must be called before draw. 

		drawiMac();

		glm::mat4 iMacModelMatrix;
		iMacModelMatrix = glm::translate(iMacModelMatrix, glm::vec3(0.0f, -3.0f, -5.0f));
		iMacModelMatrix = glm::rotate(iMacModelMatrix, 180.f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		iMacModelMatrix = glm::scale(iMacModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(iMacModelLoc, 1, GL_FALSE, glm::value_ptr(iMacModelMatrix));

		glUseProgram(0); //probably closes shader program



		glUseProgram(screenShaderProgram); //SHADER PROGRAM!

		// Get matrix's uniform location and set matrix
		GLint screenModelLoc = glGetUniformLocation(screenShaderProgram, "model");
		GLint screenViewLoc = glGetUniformLocation(screenShaderProgram, "view");
		GLint screenProjLoc = glGetUniformLocation(screenShaderProgram, "projection");

		// Get light and object color reference in Fragment Shader
		GLint screenColorLoc = glGetUniformLocation(screenShaderProgram, "screenColor");
		GLint screenLightColorLoc = glGetUniformLocation(screenShaderProgram, "lightColor");
		GLint screenLightPosLoc = glGetUniformLocation(screenShaderProgram, "lightPos");
		GLint screenViewPosLoc = glGetUniformLocation(screenShaderProgram, "viewPos");

		// Specify light and object color
		glUniform3f(screenColorLoc, 0.19f, 0.84f, 0.78f);
		glUniform3f(screenLightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(screenLightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(screenViewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		glUniformMatrix4fv(screenViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(screenProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glm::mat4 screenModelMatrix;
		screenModelMatrix = glm::translate(screenModelMatrix, glm::vec3(-0.35f, -3.0f, -5.0f));
		screenModelMatrix = glm::rotate(screenModelMatrix, 180.f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		screenModelMatrix = glm::scale(screenModelMatrix, glm::vec3(2.f, 2.f, 2.f));
		glUniformMatrix4fv(screenModelLoc, 1, GL_FALSE, glm::value_ptr(screenModelMatrix));

		glBindTexture(GL_TEXTURE_2D, screenTexture); // Apply crate texture (Auto detected by Uniform Sampler)
		glBindVertexArray(screenVAO); // User-defined VAO must be called before draw. 

		drawScreen();

		glUseProgram(0);

		glBindVertexArray(0); //Incase different VAO will be used after



		glUseProgram(kBShaderProgram); //SHADER PROGRAM!

		// Get matrix's uniform location and set matrix
		GLint kBModelLoc = glGetUniformLocation(kBShaderProgram, "model");
		GLint kBViewLoc = glGetUniformLocation(kBShaderProgram, "view");
		GLint kBProjLoc = glGetUniformLocation(kBShaderProgram, "projection");

		// Get light and object color reference in Fragment Shader
		GLint kBColorLoc = glGetUniformLocation(kBShaderProgram, "kBColor");
		GLint kBLightColorLoc = glGetUniformLocation(kBShaderProgram, "lightColor");
		GLint kBLightPosLoc = glGetUniformLocation(kBShaderProgram, "lightPos");
		GLint kBViewPosLoc = glGetUniformLocation(kBShaderProgram, "viewPos");

		// Specify light and object color
		glUniform3f(kBColorLoc, 0.75f, 0.75f, 0.75f);
		glUniform3f(kBLightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(kBLightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(kBViewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		glUniformMatrix4fv(kBViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(kBProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glm::mat4 kBModelMatrix;
		kBModelMatrix = glm::translate(kBModelMatrix, glm::vec3(0.0f, -3.0f, 0.0f));
		kBModelMatrix = glm::rotate(kBModelMatrix, 180.f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		kBModelMatrix = glm::scale(kBModelMatrix, glm::vec3(2.f, 2.f, 2.f));
		glUniformMatrix4fv(kBModelLoc, 1, GL_FALSE, glm::value_ptr(kBModelMatrix));

		glBindTexture(GL_TEXTURE_2D, kBTexture); // Apply crate texture (Auto detected by Uniform Sampler)
		glBindVertexArray(kBVAO); // User-defined VAO must be called before draw. 

		drawKB();

		glUseProgram(0);

		glBindVertexArray(0); //Incase different VAO will be used after

		glUseProgram(mouseShaderProgram); //SHADER PROGRAM!

		// Get matrix's uniform location and set matrix
		GLint mouseModelLoc = glGetUniformLocation(mouseShaderProgram, "model");
		GLint mouseViewLoc = glGetUniformLocation(mouseShaderProgram, "view");
		GLint mouseProjLoc = glGetUniformLocation(mouseShaderProgram, "projection");

		// Get light and object color reference in Fragment Shader
		GLint mouseColorLoc = glGetUniformLocation(mouseShaderProgram, "mouseColor");
		GLint mouseLightColorLoc = glGetUniformLocation(mouseShaderProgram, "lightColor");
		GLint mouseLightPosLoc = glGetUniformLocation(mouseShaderProgram, "lightPos");
		GLint mouseViewPosLoc = glGetUniformLocation(mouseShaderProgram, "viewPos");

		// Specify light and object color
		glUniform3f(mouseColorLoc, 0.75f, 0.75f, 0.75f);
		glUniform3f(mouseLightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(mouseLightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(mouseViewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		glUniformMatrix4fv(mouseViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(mouseProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


		glBindTexture(GL_TEXTURE_2D, mouseTexture); // Apply crate texture (Auto detected by Uniform Sampler)
		glBindVertexArray(mouseVAO); // User-defined VAO must be called before draw. 

		glm::mat4 mouseModelMatrix;
		mouseModelMatrix = glm::translate(mouseModelMatrix, glm::vec3(9.0f, -3.0f, -1.0f));
		mouseModelMatrix = glm::rotate(mouseModelMatrix, 180.f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		mouseModelMatrix = glm::scale(mouseModelMatrix, glm::vec3(2.f, 2.f, 2.f));
		glUniformMatrix4fv(mouseModelLoc, 1, GL_FALSE, glm::value_ptr(mouseModelMatrix));

		drawMouse();

		glUseProgram(0);

		glBindVertexArray(0); //Incase different VAO will be used after


		glUseProgram(lampShaderProgram); // Call Shader per-frame when updating attributes

		GLint lampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
		GLint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
		GLint lampProjLoc = glGetUniformLocation(lampShaderProgram, "projection");

		glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(lampProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(lampVAO); // User-defined VAO must be called before draw. 

									// Transform planes to form cube
		for (GLuint i = 0; i < 1; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, planePositions[i] / glm::vec3(8., 8., 8.) + lightPosition);
			modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(.125f, .125f, .125f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

			glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			draw();
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0); // Incase different shader will be used after

						 /* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		// Poll Camera Transformations
		TransformCamera();

	}

	//Clear GPU resources
	glDeleteVertexArrays(1, &deskVAO);
	glDeleteBuffers(1, &deskVBO);
	glDeleteBuffers(1, &deskEBO);
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
	glDeleteBuffers(1, &floorEBO);
	glDeleteBuffers(1, &iMacVAO);
	glDeleteBuffers(1, &iMacVBO);
	glDeleteBuffers(1, &iMacEBO);
	glDeleteBuffers(1, &screenVAO);
	glDeleteBuffers(1, &screenVBO);
	glDeleteBuffers(1, &screenEBO);


	glfwTerminate();
	return 0;
}

// Define input functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Display ASCII Key code
	//std::cout <<"ASCII: "<< key << std::endl;	

	// Close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Assign true to Element ASCII if key pressed
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE) // Assign false to Element ASCII if key released
		keys[key] = false;

}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	// Clamp FOV
	if (fov >= 1.0f && fov <= 55.0f)
		fov -= yoffset * 0.01;

	// Default FOV
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 55.0f)
		fov = 55.0f;

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouseMove)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}
	// Calculate mouse offset (Easing effect)
	xChange = xpos - lastX;
	yChange = lastY - ypos; // Inverted cam

							// Get current mouse (always starts at 0)
	lastX = xpos;
	lastY = ypos;


	if (isOrbiting)
	{
		// Update raw yaw and pitch with mouse movement
		rawYaw += xChange;
		rawPitch += yChange;

		// Conver yaw and pitch to degrees, and clamp pitch
		degYaw = glm::radians(rawYaw);
		degPitch = glm::clamp(glm::radians(rawPitch), -glm::pi<float>() / 2.f + .1f, glm::pi<float>() / 2.f - .1f);

		// Azimuth Altitude formula
		cameraPosition.x = target.x + radius * cosf(degPitch) * sinf(degYaw);
		cameraPosition.y = target.y + radius * sinf(degPitch);
		cameraPosition.z = target.z + radius * cosf(degPitch) * cosf(degYaw);
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	// Assign boolean state to element Button code
	if (action == GLFW_PRESS)
		mouseButtons[button] = true;
	else if (action == GLFW_RELEASE)
		mouseButtons[button] = false;
}


// Define TransformCamera function
void TransformCamera()
{

	// Orbit camera
	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
		isOrbiting = true;
	else
		isOrbiting = false;

	// Focus camera
	if (keys[GLFW_KEY_F])
		initiateCamera();
}

// Define 
void initiateCamera()
{	// Define Camera Attributes
	cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f); // Move 3 units back in z towards screen
	target = glm::vec3(0.0f, 0.0f, 0.0f); // What the camera points to
	cameraDirection = glm::normalize(cameraPosition - cameraDirection); // direction z
	worldUp = glm::vec3(0.0, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));// right vector x
	cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight)); // up vector y
	CameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 1 unit away from lense
}
