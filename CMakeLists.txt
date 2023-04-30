# CMakeList.txt: CppSTL-shikounuzi 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CppSTL-shikounuzi")

file(GLOB_RECURSE SRC_PATH src/*.h src/*.cpp)
file(GLOB_RECURSE TEST_PATH test/*.h test/*.cpp)
file(GLOB_RECURSE H_FILE_PATH src/*.h)

# 导入头文件搜索路径
include_directories(src/*/h)
include_directories(src/MAllocator)
include_directories(src/MATimer)
message(STATUS " H_FILE_PATH IS ${H_FILE_PATH}")

# 将源代码添加到此项目的可执行文件。
add_executable (CppSTL-shikounuzi "CppSTL-shikounuzi.cpp" "CppSTL-shikounuzi.h" ${SRC_PATH})

include_directories(${H_FILE_PATH})
# 将测试添加到此项目的可执行文件。
add_executable (TESTMAIN ${TEST_PATH} ${H_FILE_PATH})



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CppSTL-shikounuzi PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
