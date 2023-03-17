cmake_minimum_required(VERSION 3.22)
project(ZuneCraft)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")
#link_directories("/opt/OpenZDK/ZUNE/Lib/ARMV4I" "/opt/OpenZDK/NVIDIA/lib/release")
#add_compile_definitions(ZUNE_HD _DEBUG _WIN32_WCE=0x600 UNDER_CE WINCE DEBUG _WINDOWS _ARM_ _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS ZC_PLATFORM_ZUNE)

file(GLOB_RECURSE zunecraft_SRC CONFIGURE_DEPENDS "src/*.cpp")
#list(FILTER zunecraft_SRC EXCLUDE REGEX "Windows")

add_executable(${PROJECT_NAME} ${zunecraft_SRC})
#target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "/opt/OpenZDK/ZUNE/Include/ZDK" "/opt/OpenZDK/NVIDIA/include")
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "ext/glm" "ext/Platform/Win/glad" "ext/nothings" "ext/Platform/Win/glfw/include" "ext/fastnoise" "ext/imgui/imgui" "ext/imgui")
target_include_directories(${PROJECT_NAME} PRIVATE "src")

#target_link_libraries(${PROJECT_NAME} zdksystem.lib compclient.lib libEGL.lib)
