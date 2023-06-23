cmake_minimum_required(VERSION 3.22)
project(ZuneCraft)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall")
#link_directories("/opt/OpenZDK/ZUNE/Lib/ARMV4I" "/opt/OpenZDK/NVIDIA/lib/release")
#add_compile_definitions(ZUNE_HD _DEBUG _WIN32_WCE=0x600 UNDER_CE WINCE DEBUG _WINDOWS _ARM_ _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS ZC_PLATFORM_ZUNE)
add_compile_definitions(ZC_PLATFORM_LINUX DEBUG)

file(GLOB_RECURSE zunecraft_SRC CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER zunecraft_SRC EXCLUDE REGEX "Platform/Zune")

add_executable(${PROJECT_NAME} ${zunecraft_SRC})
#target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "/opt/OpenZDK/ZUNE/Include/ZDK" "/opt/OpenZDK/NVIDIA/include")
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "ext/glm" "ext/glad" "ext/nothings" "ext/glfw/include" "ext/fastnoise" "ext/imgui/imgui")
target_include_directories(${PROJECT_NAME} PRIVATE "src")

add_subdirectory(ext/glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw)

file(COPY src/assets DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

#target_link_libraries(${PROJECT_NAME} zdksystem.lib compclient.lib libEGL.lib)
