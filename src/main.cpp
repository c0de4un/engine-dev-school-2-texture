/**
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INCLUDES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Include STL
#include <iostream>

// Include OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DEFINES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

constexpr const int RESULT_OK = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// MAIN
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int OPEN_GL_VERSION_MAJOR = 3;
const int OPEN_GL_VERSION_MINOR = 0;
GLFWwindow *mWindow;
int mFrameBufferWidth = 0;
int mFrameBufferHeight = 0;

const float mVertices[] = {
  0.0f, 0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  -0.5f, -0.5f, 0.0f
};
static constexpr const int NUMBER_OF_VERTICES_PER_TRIANGLE = 3;
static constexpr const int NUMBER_OF_ATTRIBUTES_PER_VERTEX = 3;
static constexpr const int GL_TRIANGLE_BUFFER_SIZE = (NUMBER_OF_VERTICES_PER_TRIANGLE * NUMBER_OF_ATTRIBUTES_PER_VERTEX) * sizeof(float);
static constexpr const int CHAR_SIZE = sizeof(char);

GLuint mVertexBufferObject;
GLuint mVertexArrayObject;

GLuint mVertexShaderID;
GLuint mFragmentShaderID;
GLuint mShaderProgramID;

static void handleGLFWErrors(int error, const char* description)
{
  std::cout << "error_callback: " << description << "\n";
}

static void handleKeyEvents(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (!mWindow)
  {
    return;
  }

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
  }
}

void handleFramebufferSizeChange(GLFWwindow *pWindow, int width, int height)
{
  if (!mWindow)
  {
    return;
  }

  mFrameBufferWidth = width;
  mFrameBufferHeight = height;
  glViewport(0, 0, mFrameBufferWidth, mFrameBufferHeight);
}

GLuint loadShader(const GLenum _type, const char *const _src)
{
  GLint compilationStatus;

  GLuint shaderId = glCreateShader(GL_VERTEX_SHADER);
  if (!shaderId)
  {
    std::cout << "loadShaders: failed to generate Vertex Shader ID\n";
    return false;
  }

  glShaderSource(shaderId, 1, &_src, nullptr);
  glCompileShader(shaderId);
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilationStatus);

  if (!compilationStatus)
  {
    GLint infoLen;

    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen)
    {
      char *const infoLog = static_cast<char*>(
        malloc(CHAR_SIZE * infoLen)
      );
      glGetShaderInfoLog(shaderId, infoLen, nullptr, infoLog);

      std::cout << "loadShader: " << infoLog << "\n";

      free(infoLog);
    }

    glDeleteShader(shaderId);
  }

  return shaderId;
}

bool loadShaders()
{
  std::cout << "loadShaders: loading vertex shader\n";
  const char *const vertexShaderCode =
  "#version 150                    \n"
  "in vec3 vp;                     \n"
  "void main()                     \n"
  "{                               \n"
  "    gl_Position = vec4(vp, 1.0);\n"
  "}                               \n";

  mVertexShaderID = loadShader(GL_VERTEX_SHADER, vertexShaderCode);
  if (!mVertexShaderID)
  {
    std::cout << "loadShaders: failed to load vertex shader\n";
    return false;
  }
  std::cout << "loadShaders: vertex shader loaded\n";

  std::cout << "loadShaders: loading fragment shader\n";
  const char *const fragmentShaderCode =
  "#version 150                               \n"
  "out vec4 frag_colour;                      \n"
  "void main()                                \n"
  "{                                          \n"
  "    frag_colour = vec4(0.5, 0.0, 0.5, 1.0);\n"
  "}                                          \n";
  mFragmentShaderID = loadShader(GL_FRAGMENT_SHADER, fragmentShaderCode);
  if (!mFragmentShaderID)
  {
    std::cout << "loadShaders: failed to load fragment shader\n";
    return false;
  }
  std::cout << "loadShaders: fragment shader loaded\n";

  mShaderProgramID = glCreateProgram();
  if (!mShaderProgramID)
  {
    std::cout << "loadShaders: failed to create shader program\n";
    return false;
  }
  glAttachShader(mShaderProgramID, mVertexShaderID);
  glAttachShader(mShaderProgramID, mFragmentShaderID);
  glLinkProgram(mShaderProgramID);

  return true;
}

bool onSurfaceReady()
{
  glfwGetFramebufferSize(mWindow, &mFrameBufferWidth, &mFrameBufferHeight);
  glViewport(0, 0, mFrameBufferWidth, mFrameBufferHeight);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Load Vertices Data to GPU
  glGenBuffers(1, &mVertexBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(
    GL_ARRAY_BUFFER,
    GL_TRIANGLE_BUFFER_SIZE,
    mVertices,
    GL_STATIC_DRAW
  );

  // Create Mesh Array Object & Bind Buffers to it
  glGenVertexArrays(1, &mVertexArrayObject);
  glBindVertexArray(mVertexArrayObject);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glVertexAttribPointer(
    0,
    NUMBER_OF_VERTICES_PER_TRIANGLE,
    GL_FLOAT,
    GL_FALSE,
    0,
    nullptr
  );

  if (!loadShaders())
  {
    std::cout << "onSurfaceReady: failed to laod shaders\n";
    return false;
  }

  return true;
}

bool init()
{
  if (!glfwInit())
  {
    std::cout << "init: failed to init\n";
    return false;
  }

  glfwSetErrorCallback(handleGLFWErrors);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPEN_GL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPEN_GL_VERSION_MINOR);
  mWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Triangle", nullptr, nullptr);
  if (!mWindow)
  {
    std::cout << "init: failed to create window\n";
    return false;
  }

  glfwSetKeyCallback(mWindow, handleKeyEvents);

  glfwMakeContextCurrent(mWindow);
  gladLoadGL(glfwGetProcAddress);

  glfwSetFramebufferSizeCallback(mWindow, handleFramebufferSizeChange);
  if (!onSurfaceReady())
  {
    std::cout << "init: failed to load assets\n";
    return false;
  }

  return true;
}

void render()
{
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(mShaderProgramID);

  glBindVertexArray(mVertexArrayObject);

  // Draw Mesh
  glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES_PER_TRIANGLE);
}

void mainLoop()
{
  while (!glfwWindowShouldClose(mWindow))
  {
    render();

    glfwSwapBuffers(mWindow);
    glfwPollEvents();
  }
}

void terminate()
{
  if (mWindow)
  {
    if (mVertexShaderID)
    {
      glDeleteShader(mVertexShaderID);
    }
    mVertexShaderID = 0;

    if (mFragmentShaderID)
    {
      glDeleteShader(mFragmentShaderID);
    }
    mFragmentShaderID = 0;

    if (mShaderProgramID)
    {
      glDeleteProgram(mShaderProgramID);
    }
    mShaderProgramID = 0;

    glfwDestroyWindow(mWindow);
  }

  glfwTerminate();
}

int main()
{
  if (init())
  {
    mainLoop();
  }

  terminate();

  return RESULT_OK;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
