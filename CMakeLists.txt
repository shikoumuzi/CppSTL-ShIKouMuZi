# CMakeList.txt: CppSTL-shikounuzi 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (CppSTL-shikounuzi)

set(BOOST_CODE_LIBRARY_PATH F:/CodeLibrary/boost_1_79_0)
set(SQLITE_CODE_LIBRARY_PATH D:/sqlite)


# 添加头文件路径
file(GLOB_RECURSE SRC_PATH src/*.h src/*.cpp)
file(GLOB_RECURSE TEST_PATH test/*.h test/*.cpp)
file(GLOB_RECURSE H_FILE_PATH src/*.h)

# BOOST的设置
# 设置不使用静态库
set (Boost_USE_STATIC_LIBS OFF)

# 设置使用多线程库
set (Boost_USE_MULTITHREADED ON)

# 设置不使用静态运行时库
set (Boost_USE_STATIC_RUNTIME OFF)

# 添加boost路径
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${BOOST_CODE_LIBRARY_PATH})
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${BOOST_CODE_LIBRARY_PATH}/stage/lib)
set(BOOST_ROOT ${BOOST_CODE_LIBRARY_PATH})# 设置自己的路径

find_package(BOOST REQUIRED  COMPONENTS  system filesystem asio)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS}) 
	link_directories(${Boost_LIBRARY_DIRS})
	message(STATUS "Boost 库信息:")
	message(STATUS "Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost_LIBRARY_DIRS=${Boost_LIBRARY_DIRS}")
 
	# message(STATUS "Boost_FILESYSTEM_LIBRARIES=${Boost_FILESYSTEM_LIBRARY}")#为空
	# message(STATUS "Boost_LIBRARIES=${BOOST_LIBRARIES}")#为空
	#target_link_libraries(testBoost Boost::filesystem)#有问题
	# target_link_libraries(CppSTL-shikounuzi PUBLIC ${BOOST_LIBRARIES} PUBLIC ${BOOST_LIBRARIES})
endif()

# sqlte配置
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${SQLITE_CODE_LIBRARY_PATH}/sqlite-amalgamation-3410200)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${SQLITE_CODE_LIBRARY_PATH})
set(SQLITE_ROOT ${SQLITE_CODE_LIBRARY_PATH})

# 连接sqlite
find_package(sqlite3 REQUIRED)
if(Sqlite3_FOUND)
	include_directories(${Boost_INCLUDE_DIRS}) 
	link_directories(${Boost_LIBRARY_DIRS})
	message(STATUS "sqlite3 库信息:")
	message(STATUS "sqlite3_INCLUDE_DIRS=${SQLITE_INCLUDE_DIRS}")
	message(STATUS "sqlite3_LIBRARY_DIRS=${SQLITE_LIBRARY_DIRS}")
 
	message(STATUS "sqlite3_FILESYSTEM_LIBRARIES=${SQLITE_FILESYSTEM_LIBRARY}")#为空
	message(STATUS "sqlite3_LIBRARIES=${SQLITE_LIBRARIES}")#为空
	#target_link_libraries(testBoost Boost::filesystem)#有问题
	#target_link_libraries(testBoost PUBLIC ${BOOST_LIBRARIES} PUBLIC ${BOOST_LIBRARIES}
endif()

# 设置cpp标准
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD 20)

# 导入头文件搜索路径
include_directories(src/MSTL/h)
include_directories(src/MAllocator)
include_directories(src/MTimer)
include_directories(src/)
# message(STATUS " H_FILE_PATH IS ${H_FILE_PATH}")

# 将源代码添加到此项目的可执行文件。
add_executable (CppSTL-shikounuzi "CppSTL-shikounuzi.cpp" "CppSTL-shikounuzi.h" ${SRC_PATH} "src/MAllocator/MPoolAllocator.h" "src/MAllocator/MLOKIAllocator.h" "src/MAllocator/MArrayAllocaor.h" "src/MAllocator/MPoolAllocator.cpp" "src/MAllocator/MLOKIAllocator.cpp" "src/MSTL/h/MTreeMap.h" "src/MSTL/h/MHashMap.h" "src/MSTL/h/MList.h" "src/MBase/MError.h" "src/MSTL/h/MStack.h" "src/MSTL/h/MDeque.h" "src/MFTP/MFTP.cpp" "src/MFTP/MFTPProto.h" "src/MFileSystem/MFileDataBase.h" "src/MFileSystem/MFileDataBase.cpp")

include_directories(${H_FILE_PATH})
# 将测试添加到此项目的可执行文件。
# add_executable (TESTMAIN ${TEST_PATH} ${H_FILE_PATH} "src/MAllocator/MPoolAllocator.h" "src/MAllocator/MLOKIAllocator.h" "src/MAllocator/MArrayAllocaor.h" "src/MAllocator/MPoolAllocator.cpp" "src/MAllocator/MLOKIAllocator.cpp")



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CppSTL-shikounuzi PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
