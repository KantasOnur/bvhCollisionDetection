include(FetchContent)

# Declare Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.89.6 # Replace with the desired version
)

# Download and make Dear ImGui available
FetchContent_MakeAvailable(imgui)

# Add a static library target for ImGui
add_library(ImGui STATIC)

# Add ImGui source files to the static library
target_sources(ImGui PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Include directories for ImGui
target_include_directories(ImGui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include"
)

# Optionally link to required libraries (like OpenGL and GLFW) for backend implementations
find_package(OpenGL REQUIRED)
target_link_libraries(ImGui PUBLIC OpenGL::GL)

if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit
  set(GLEW_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/lib/Release/x64/glew32s.lib")
  set(GLFW_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/lib/WIN64/glfw3.lib")
else() # 32-bit
  set(GLEW_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/lib/Release/Win32/glew32s.lib")
  set(GLFW_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/lib/WIN32/glfw3.lib")
endif()

target_link_libraries(ImGui PRIVATE ${GLFW_LIB_PATH})