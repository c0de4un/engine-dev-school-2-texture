# = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# CMAKE
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

set(CMAKE_VERSION 3.22)
cmake_minimum_required(VERSION ${CMAKE_VERSION})

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# PROEJCT
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

project("Hello OpenGL" VERSION 0.1.0)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# SOURCES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

set(SOURCES_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(SOURCES
  "${SOURCES_DIR}/main.cpp"
)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# DEPENDENCIES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# - - - GLFW - - - -
set(GLFW_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/GLFW")

#set(GLFW_IMPORTED_LOCATION "${}")
if (MSVC) # Windows MSVC
  set(GLFW_IMPORTED_LOCATION "${GLFW_DIR}/lib/windows/msvc/glfw3.lib")
elseif (MINGW) # Windows MinGW
  set(GLFW_IMPORTED_LOCATION "${GLFW_DIR}/lib/windows/mingw-w64/libglfw3.a")
else () # Linux or Darwin
  set(GLFW_IMPORTED_LOCATION "${GLFW_DIR}/lib/nix/libglfw3.a")
endif ()

# Check location
if (NOT EXISTS ${GLFW_IMPORTED_LOCATION})
  message(FATAL_ERROR "${PROJECT_NAME} - GLFW not found at ${GLFW_IMPORTED_LOCATION}")
endif ()

# GLFW Interface
set(GLFW_INCLUDE "${GLFW_DIR}/include")
if (NOT EXISTS "${GLFW_INCLUDE}/GLFW/glfw3.h")
  message(FATAL_ERROR "${PROJECT_NAME} - glfw.h not found at ${GLFW_INCLUDE}")
endif ()

# Import GLFW STATIC Library
add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
  IMPORTED_LOCATION ${GLFW_IMPORTED_LOCATION}
  INTERFACE_INCLUDE_DIRECTORIES ${GLFW_INCLUDE}
)
message(STATUS "${PROJECT_NAME} - GLFW3 imported as STATIC library")

# - - - GLAD - - - -
set(GLAD_LOCATION "${CMAKE_CURRENT_LIST_DIR}/libs/GLAD")
set(GLAD_SOURCES "${GLAD_LOCATION}/src/gl.c")
set(GLAD_INCLUDE "${GLAD_LOCATION}/include")

# Add GLAD to Sources of project
set(SOURCES
  ${GLAD_SOURCES}
  ${SOURCES}
)

# OpenGL
if (MINGW) # Windows MinGW
  find_library(opengl "libopengl32.a")
elseif (MSVC) # Windows MSVC
  find_library(opengl "OpenGL32.lib")
else () # Linux or Darwin
  find_library(opengl "libopengl.a")
endif ()

# Check OpenGL Library
if (opengl)
  message(STATUS "${PROJECT_NAME} - OpenGL Library found at ${opengl}")
else ()
  message(FATAL_ERROR "${PROJECT_NAME} - OpenGL not found. Configure SDK and check packages")
endif ()

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# BUILD
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Add Executable Object
add_executable(app ${SOURCES})

# Configure Executable Object
set_target_properties(app PROPERTIES
  RUNTIME_OUTPUT_NAME "Hello_OpenGL_v${PROJECT_VERSION}"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin"
)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# LINK
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Link with GLFW
target_link_libraries(app
  glfw3
  ${opengl}
)

# Include GLAD Headers
target_include_directories(app PRIVATE ${GLAD_INCLUDE})

# = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
