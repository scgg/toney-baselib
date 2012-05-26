
# 笨笨基础库 工程CMakeLists文件 
# toney 2012-5-25
#
 
# 工程名
project(TYLib)
set(LIB_VERSION 0.0.1)

# CMake 安全版本要求
cmake_minimum_required(VERSION 2.8)

# 强制在非源码目录cmake
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" BUILDING_IN_SOURCE)
if(BUILDING_IN_SOURCE)
  message(FATAL_ERROR
    "sgame要求在非源码目录编译. 移除当前目录的'CMakeCache.txt', 创建一个单独的bulid目录，在build目录下执行'cmake <srcs> [options]'"
  )
endif()

# 添加编译选项
option(DEBUG "Debug mode" 1)
option(PCH "Use precompiled headers" 0)

if(DEBUG)
  message("Build in debug-mode   : Yes")
  set(CMAKE_BUILD_TYPE Debug)
else()
  set(CMAKE_BUILD_TYPE Release)
  message("Build in debug-mode   : No  (default)")
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} --no-warnings")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} --no-warnings")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wfatal-errors -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wfatal-errors -Wextra")  
  
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



include_directories(
  ${PROJECT_SOURCE_DIR}/algorithm
  ${PROJECT_SOURCE_DIR}/base
  ${PROJECT_SOURCE_DIR}/config
  ${PROJECT_SOURCE_DIR}/net
  ${PROJECT_SOURCE_DIR}/system
  
)

link_directories(
  ${PROJECT_SOURCE_DIR}/lib
)


add_subdirectory(algorithm)
add_subdirectory(base)
#add_subdirectory(config)
add_subdirectory(system)
#add_subdirectory(net)


# 生成静态库
add_library(TYLib STATIC )
# 生成动态库
#add_library(TYLib SHARED )

target_link_libraries(TYLib
	base
	system
	algorithm
)

#add_subdirectory(test)

# 安装配置 （暂时不需要 toney）









