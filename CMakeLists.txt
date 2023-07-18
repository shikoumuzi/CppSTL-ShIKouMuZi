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

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)# 可执行文件
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})# 静态库输出


# 基础设置
# add_definitions("-DUNICODE" "-D_UNICODE") # 设置为unicode字符集

# 指定平台工具集
 set(CMAKE_GENERATOR_PLATFORM v143) 

# 路径变量设置
set(BOOST_CODE_LIBRARY_PATH F:/CodeLibrary/boost_1_79_0)
set(SQLITE3_CODE_LIBRARY_PATH D:/sqlite)


# 添加头文件路径
file(GLOB_RECURSE CPP_FILE_PATH src/*.cpp)
file(GLOB_RECURSE TEST_PATH test/*.h test/*.cpp)
file(GLOB_RECURSE H_FILE_PATH src/*.h)

# 设置cpp标准
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD 20)

# 导入头文件搜索路径
include_directories(src/MSTL/h)
include_directories(src/MAllocator)
include_directories(src/MTimer)
include_directories(src/)
include_directories(${H_FILE_PATH})
# message(STATUS " H_FILE_PATH IS ${H_FILE_PATH}")


# BOOST的设置
# 设置不使用静态库
set (Boost_USE_STATIC_LIBS OFF)

# 设置使用多线程库
set (Boost_USE_MULTITHREADED ON)

# 设置不使用静态运行时库
set (Boost_USE_STATIC_RUNTIME OFF)

# 添加boost路径
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${BOOST_CODE_LIBRARY_PATH})
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${BOOST_CODE_LIBRARY_PATH}/stage/lib)
set(BOOST_ROOT ${BOOST_CODE_LIBRARY_PATH})# 设置自己的路径

find_package(BOOST REQUIRED  COMPONENTS  system filesystem asio)
# target_link_libraries 需要在add_excutable后面
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS}) 
	link_directories(${Boost_LIBRARY_DIRS})
	message(STATUS "Boost 库信息:")
	message(STATUS "Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost_LIBRARY_DIRS=${Boost_LIBRARY_DIRS}")
endif()

# sqlte配置
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${SQLITE3_CODE_LIBRARY_PATH}/sqlite-amalgamation-3410200)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${SQLITE3_CODE_LIBRARY_PATH}/sqlite-tools-win32-x86-3410200)
set(SQLITE3_ROOT ${SQLITE3_CODE_LIBRARY_PATH})

# 连接sqlite
find_package(SQLite3 REQUIRED)

if(SQLite3_FOUND)
	include_directories(${SQLite3_INCLUDE_DIRS}) # Assume sqlite header is in /usr/include
	link_directories(${SQLite3_LIBRARIES}) # Assume sqlite library is in /usr/lib
	message(STATUS "Sqlite3 库信息:")
	message(STATUS "Sqlite3_INCLUDE_DIRS=${SQLite3_INCLUDE_DIRS}")
	message(STATUS "Sqlite3_LIBRARY_DIRS=${SQLite3_LIBRARIES}")
endif()

set(HEADERS
	${H_FILE_PATH}
	"src/MSTL/h/MTreeMap.h" 
	"src/MSTL/h/MHashMap.h" 
	"src/MSTL/h/MList.h" 
	"src/MBase/MError.h" 
	"src/MSTL/h/MStack.h" 
	"src/MSTL/h/MDeque.h" 
	"src/MFTP/MFTPProto.h" 
	"src/MFileSystem/MFileDataBase.h" 
	"src/MFileSystem/MFileInput.h" 
	"src/MFileSystem/MFileOutput.h"  
	"src/MFileSystem/FileIOQueue.h"  
	"src/MFileSystem/MFileBase/MFileInWin.h" 
	"src/MSQLite/MSQLiite.h"
	"src/MAllocator/MPoolAllocator.h" 
	"src/MAllocator/MLOKIAllocator.h" 
	"src/MAllocator/MArrayAllocaor.h"
	"src/MNet/MEndPoint/MClientEndPoint.h"
	"src/MNet/MEndPoint/MServerEndPoint.h"
	"src/MNet/MNetBase.h"
	"src/MNet/MSyncSocket/MSyncSocket.h"
	"src/MLog/MLog.h"
	"src/MNet/MAsyncSocket/MAsyncSocket.h"
	"src/MNet/MAsyncSocket/MAsyncServer.h"

	
)

set(SOURCES
	${CPP_FILE_PATH}
	"src/MSQLite/MSQLite.cpp"
	"src/MFileSystem/FileIOQueue.cpp"
	"src/MFileSystem/MFileInput.cpp" 
	"src/MFileSystem/MFile.cpp" 
	"src/MAllocator/MPoolAllocator.cpp" 
	"src/MAllocator/MLOKIAllocator.cpp" 
	"src/MFileSystem/MFileOutput.cpp" 
	"src/MFileSystem/MFileDataBase.cpp" 
	"src/MFTP/MFTP.cpp" 
	"src/MNet/MEndPoint/MClientEndPoint.cpp"
	"src/MNet/MEndPoint/MServerEndPoint.cpp"
	"src/MNet/MSyncSocket/MSyncSocket.cpp"  
	"src/MLog/MLog.cpp"
	"src/MNet/MAsyncSocket/MAsyncSocket.cpp"
	"src/MNet/MAsyncSocket/MAsyncServer.cpp"
)


# 将源代码添加到此项目的可执行文件。
# 将测试添加到此项目的可执行文件。
# add_executable (TESTMAIN ${TEST_PATH} ${H_FILE_PATH} "src/MAllocator/MPoolAllocator.h" "src/MAllocator/MLOKIAllocator.h" "src/MAllocator/MArrayAllocaor.h" "src/MAllocator/MPoolAllocator.cpp" "src/MAllocator/MLOKIAllocator.cpp")
add_executable (CppSTL-shikounuzi 
	"CppSTL-shikounuzi.cpp" 
	"CppSTL-shikounuzi.h" 
	${SOURCES}
	${HEADERS}
       "src/MNet/MEndPoint/MEndPoint.h" "src/MNet/MAsyncSocket/MAsyncClient.h" "src/MNet/MAsyncSocket/MAsyncClient.cpp" "src/MNet/MAsyncSocket/MMsgNode.h" "src/MNet/MAsyncSocket/MSession.h" "src/MNet/MAsyncSocket/MSession.cpp")



# cmake中的相关顺序一定要正确
# 顺序是：
# include_directories
# link_directories
# add_executable
# target_link_libraries
# ————————————————————
# link_directories一定要放在add_executable前面，target_link_libraries一定要放在add_executable后面
if(SQLite3_FOUND)
# cmake变量名是区分大小写的
	target_link_libraries(CppSTL-shikounuzi PUBLIC ${SQLite3_LIBRARIES}) # Link with sqlite3 library
	message(STATUS "sqlite is targeted")
endif()

if(Boost_FOUND)
	target_link_libraries(CppSTL-shikounuzi PUBLIC ${BOOST_LIBRARIES})
	message(STATUS "boost is targeted")
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CppSTL-shikounuzi PROPERTY CXX_STANDARD 20)
endif()

# 设置dll拷贝
# file(copy ${SQLite3_LIBRARIES}/sqlite3.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH})
install(FILES ${SQLITE3_CODE_LIBRARY_PATH}/sqlite-tools-win32-x86-3410200/sqlite3.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH}/../bin)



message(STATUS "CMAKE Install 项目信息：")
message(STATUS "SQLITE.LIB=${SQLITE3_CODE_LIBRARY_PATH}/sqlite-tools-win32-x86-3410200/sqlite3.dll")

message(STATUS "CMAKE 项目输出信息：")
message(STATUS "EXECUTABLE_OUTPUT_PATH=${EXECUTABLE_OUTPUT_PATH}")
message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

# TODO: 如有需要，请添加测试并安装目标。
