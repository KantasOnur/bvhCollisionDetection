# CMakeList.txt : CMake project for bvhCollisionDetection, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("bvhCollisionDetection")
set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")

file(GLOB_RECURSE SRC_FILES src/*.cpp)


# Add source to this project's executable.
add_executable (bvhCollisionDetection ${SRC_FILES})
add_compile_definitions(SRC="${CMAKE_CURRENT_SOURCE_DIR}/src")

target_include_directories(bvhCollisionDetection PRIVATE "${DEPENDENCIES_DIR}/glew/include")
target_include_directories(bvhCollisionDetection PRIVATE "${DEPENDENCIES_DIR}/glfw/include")
target_include_directories(bvhCollisionDetection PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

find_package(OpenGL REQUIRED)

if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit
  set(GLEW_LIB_PATH "${DEPENDENCIES_DIR}/glew/lib/Release/x64/glew32s.lib")
  set(GLFW_LIB_PATH "${DEPENDENCIES_DIR}/glfw/lib/WIN64/glfw3.lib")
else() # 32-bit
  set(GLEW_LIB_PATH "${DEPENDENCIES_DIR}/glew/lib/Release/Win32/glew32s.lib")
  set(GLFW_LIB_PATH "${DEPENDENCIES_DIR}/glfw/lib/WIN32/glfw3.lib")
endif()

# Link glm
include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

include(${CMAKE_SOURCE_DIR}/dependencies/imgui/get_imgui.cmake)


target_link_libraries(bvhCollisionDetection PRIVATE glm::glm)
target_compile_definitions(bvhCollisionDetection PRIVATE GLEW_STATIC)
target_link_libraries(bvhCollisionDetection PRIVATE ${GLFW_LIB_PATH})
target_link_libraries(bvhCollisionDetection PRIVATE ${GLEW_LIB_PATH})
target_link_libraries(bvhCollisionDetection PUBLIC OpenGL::GL)
target_link_libraries(bvhCollisionDetection PUBLIC ImGui)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET bvhCollisionDetection PROPERTY CXX_STANDARD 20)
endif()
