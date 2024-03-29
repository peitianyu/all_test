# 指定cmake版本
cmake_minimum_required(VERSION 3.3)
# 工程名
project(all_tests)

#cmake的c++设置
# 告知當前使用的是交叉編譯方式，必須配置
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
# 执行路径设置
# SET(EXECUTABLE_OUTPUT_PATH ../bin)
# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -std=c++14 -fPIC -pthread")
SET(CMAKE_BUILD_TYPE Release)
# 运行测试
add_definitions(-DTEST_ENABLE)

# 开启图优化打印与断言
option(ENABLE_LOGGING "Enable logging." ON )
if(ENABLE_LOGGING)
add_definitions(-DGRAPH_ENABLE_LOG)
endif()

option(ENABLE_ASSERTIONS "Enable assertions." ON )
if(ENABLE_ASSERTIONS)
add_definitions(-DGRAPH_ENABLE_ASSERT)
endif()


# 添加eigen库
find_package(Eigen3 REQUIRED)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})

# 添加opencv库
find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# 添加openMP库
find_package(OpenMP REQUIRED)

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(Pangolin REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

# 添加Sophus库
find_package(Sophus REQUIRED)
include_directories(${Sophus_INCLUDE_DIRS})

# 添加.h文件
include_directories(src)

file(GLOB_RECURSE ALL_LIBRIRY_SRCS "src/*.c*")

# 执行文件
add_executable(${PROJECT_NAME} ${ALL_LIBRIRY_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX  ${EIGEN3_LIBRARY} ${OpenCV_LIBS} 
                        ${PYTHON_LIBRARY} ${Pangolin_LIBRARIES} ${Sophus_LIBRARIES} fmt::fmt ${CERES_LIBRARIES})
