# CMakeList.txt: CppSTL-shikounuzi 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.0)

# Enable Hot Reload for MSVC compilers if supported.
# if (POLICY CMP0141)
#   cmake_policy(SET CMP0141 NEW)
#   set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
#   "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
# endif()

project (CppSTL-shikounuzi)
set(PROJECT_NAME CppSTL-shikounuzi)

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
set(RAPIDJSON_CODE_LIBRARY_PATH F:/CodeLibrary/rapidjson)
set(FFMPEG_CODE_LIBRARY_PATH F:/CodeLibrary/ffmpeg)

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

# 设置颜色
# string(ASCII 27 Esc)
# set(Yellow "${Esc}")

# 导入模块
macro(importModule)
	include("src/MAllocator/MAllocator.cmake")
	include("src/MBase/MBase.cmake")
	include("src/MDataBase/MSQLite/MSQLite.cmake")
	include("src/MDesignModel/MDesignModel.cmake")
	include("src/MEventLoop/MEventLoop.cmake")
	include("src/MLog/MLog.cmake")
	include("src/MFileSystem/MFileSystem.cmake")
	include("src/MFTP/MFTP.cmake")
	include("src/MSTL/MSTL.cmake")
	include("src/MNet/MNet.cmake")
	include("src/MMedia/MMPlayer/MMPlayer.cmake")
endmacro()
importModule()

macro(initBoost)
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
endmacro()
initBoost()

macro(initSQLite)
	# sqlte配置
	set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${SQLITE3_CODE_LIBRARY_PATH}/sqlite-amalgamation-3440200)
	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${SQLITE3_CODE_LIBRARY_PATH}/sqlite-dll-win-x64-3440200)
	set(SQLITE3_ROOT ${SQLITE3_CODE_LIBRARY_PATH})

	# 连接sqlite
	find_package(SQLite3 REQUIRED)

	if(SQLite3_FOUND)
		include_directories(${SQLite3_INCLUDE_DIRS}) # Assume sqlite header is in /usr/include
		link_directories(${SQLite3_LIBRARIES}) # Assume sqlite library is in /usr/lib
		message(STATUS "Sqlite3 库信息:")
		message(STATUS "Sqlite3_INCLUDE_DIRS=${SQLite3_INCLUDE_DIRS}")
		message(STATUS "Sqlite3_LIBRARY_DIRS=${SQLite3_LIBRARIES}")
		# add_library(sqlite3 SHARED imported)
		# set_target_properties(sqlite3 PROPERTIES IMPORTED_LOCATION ${SQLITE3_CODE_LIBRARY_PATH}/sqlite-dll-win-x64-3440200/sqlite3.dll)

	endif()

endmacro()
initSQLite()

macro(initRapidJson)
	# rapidcpp 配置
	
	set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${RAPIDJSON_CODE_LIBRARY_PATH}/include/rapidjson)
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${RAPIDJSON_CODE_LIBRARY_PATH}/build)
	set(RapidJSON_ROOT ${RAPIDJSON_CODE_LIBRARY_PATH}/build)

	find_package(RapidJSON REQUIRED)
	# 连接rapidjson
	if(RapidJSON_FOUND)
		include_directories(${RapidJSON_INCLUDE_DIRS})
		message(STATUS "rapidjson 库信息:")
		message(STATUS "rapidjson_INCLUDE_DIRS=${RapidJSON_INCLUDE_DIRS}")
	endif()
endmacro()
initRapidJson()

# 由于部分库采用pkg_config配置 所以需要通过查找 pkg_config导入到cmake当中
macro(importPkgConfig)
	set(PKG_CONFIG_EXECUTABLE D:/pkg-config-lite-0.28-1/bin/pkg-config.exe)
	find_package(PkgConfig  REQUIRED)
	message(STATUS "PKG_CONFIG import: ${PKG_CONFIG_FOUND}")
	message(STATUS "PKG_CONFIG version: ${PKG_CONFIG_VERSION_STRING}")
endmacro()
importPkgConfig()
 
# 连接ffmpeg
macro(initFFmpeg)
	
	# 将目标地址地址导入搜索路径
	set(ENV{PKG_CONFIG_PATH} "F:/CodeLibrary/ffmpeg/lib/pkgconfig")
	
	# 通过pkg_search_module搜索
	pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil)
	#pkg_search_module(ffmpeg REQUIRED IMPORTED_TARGET libavcodec)
	message(STATUS "FFMPEG_LIBRARIES: ${FFMPEG_LIBRARIES}")
	message(STATUS "FFMPEG_INCLUDE_DIRS: ${FFMPEG_INCLUDE_DIRS}")
	message(STATUS "FFMPEG_LINK_LIBRARIES: ${FFMPEG_LINK_LIBRARIES}")
	message(STATUS "FFMPEG_LIBRARY_DIRS: ${FFMPEG_LIBRARY_DIRS}")

	# 为每个都添加上.lib后缀
	set(FFMPEG_LIBRARIES_WITH_EXTEND)
	foreach(var ${FFMPEG_LIBRARIES})
		list( APPEND FFMPEG_LIBRARIES_WITH_EXTEND ${var}.lib)
	endforeach()
	foreach(var ${FFMPEG_LIBRARIES_WITH_EXTEND})
		message("var is :" ${var})
	endforeach()
	message("FFMPEG_LIBRARIES_WITH_EXTEND: " ${FFMPEG_LIBRARIES_WITH_EXTEND})

	# 查询动态库路径
	set(FFMEPG_TARGET_DLLS)
	file(GLOB_RECURSE FFMPEG_DLLS ${FFMPEG_CODE_LIBRARY_PATH}/bin/*.dll)
	foreach(var_i ${FFMPEG_LIBRARIES})
		foreach(var_j ${FFMPEG_DLLS})
			string(FIND ${var_j} ${var_i} result REVERSE)
			if(NOT (result EQUAL -1))
				list( APPEND FFMEPG_TARGET_DLLS ${var_j})
			endif()
		endforeach()
	endforeach()
	# message(STATUS "FFMPEG_DLLS:${FFMPEG_DLLS}" )
	message(STATUS "FFMEPG_TARGET_DLLS:${FFMEPG_TARGET_DLLS}" )

	include_directories(${FFMPEG_INCLUDE_DIRS})
	link_directories(${FFMPEG_LIBRARY_DIRS})

endmacro()
initFFmpeg()

# 导入子目录
# add_subdirectory("src/MAllocator")
# add_subdirectory("src/MBase")

set(HEADERS
	${H_FILE_PATH}
	#"src/MLog/MLog.h"

	# "src/MSTL/h/MTreeMap.h" 
	# "src/MSTL/h/MHashMap.h" 
	# "src/MSTL/h/MList.h" 
	# "src/MSTL/h/MStack.h" 
	# "src/MSTL/h/MDeque.h" 
	# "src/MSTL/h/MSyncAnnularQueue.h" 
	# "src/MSTL/h/MVector.h" 
	# "src/MSTL/h/MSkipList.h"  
	# "src/MSTL/h/MArray.h"
	# "src/MSTL/h/MAtomicLock.h"
	
	# "src/MFileSystem/MFileDataBase.h" 
	# "src/MFileSystem/MFileInput.h" 
	# "src/MFileSystem/MFileOutput.h"  
	# "src/MFileSystem/FileIOQueue.h"  
	# "src/MFileSystem/MFileBase/MFileInWin.h" 
	# "src/MDataBase/MSQLite/MSQLiite.h"

	# "src/MNet/MEndPoint/MClientEndPoint.h"
	# "src/MNet/MEndPoint/MServerEndPoint.h"
	# "src/MNet/MNetBase.h"
	# "src/MNet/MSyncSocket/MSyncSocket.h"
	# "src/MNet/MAsyncSocket/MAsyncSocket.h"
	# "src/MNet/MAsyncSocket/MAsyncServer.h"
	# "src/MNet/MEndPoint/MEndPoint.h"
	# "src/MNet/MAsyncSocket/MAsyncClient.h"
	# "src/MNet/MMsgNode/MMsgNode.h" 
	# "src/MNet/MAsyncSocket/MSession.h"
	# "src/MNet/MMsgNode/MSendMsgNode.h" 
	# "src/MNet/MMsgNode/MRecvMsgNode.h" 
	# "src/MNet/MLogicSystem/MLogicSystem.h"
	# "src/MNet/MLogicSystem/MLogicNode.h" 
	# "src/MNet/MIOPool/MAsioIOServerPool/MAsioIOServerPool.h"
	# "src/MNet/MIOPool/MAsioIOThreadPool/MAsioIOThreadPool.h"
	# "src/MNet/MCoroutineSocket/MCoroutineSocket.h"
	#"src/MNet/MCoroutineSocket/MSession.h" 
	#"src/MNet/MHttpSocket/MHttpServer.h" 
	#"src/MNet/MReliableUDP/MReliableUDP.h" 
	#"src/MNet/MWebSocket/MWebSocket.h" 
	#"src/MNet/MCoroutineSocket/MCoroutineServer.h" 
	#"src/MNet/MCoroutineSocket/MCoroutineClient.h" 
	#"src/MNet/MHttpSocket/MHttpClient.h" 

	# "src/MDesignModel/MObserver/MObserver.h"
	# "src/MDesignModel/MObserver/MSubject.h" 
	# "src/MDesignModel/MSingleton/MSingleton.h"


	"src/MSignal/MSignalUtils.h"


)

set(SOURCES
	${CPP_FILE_PATH}
	#"src/MLog/MLog.cpp"
	
	#"src/MFileSystem/FileIOQueue.cpp"
	#"src/MFileSystem/MFileInput.cpp" 
	#"src/MFileSystem/MFile.cpp" 
	#"src/MFileSystem/MFileOutput.cpp" 
	#"src/MFileSystem/MFileDataBase.cpp" 
	# "src/MFTP/MFTP.cpp" 

	#"src/MNet/MEndPoint/MClientEndPoint.cpp"
	#"src/MNet/MEndPoint/MServerEndPoint.cpp" 
	#"src/MNet/MSyncSocket/MSyncSocket.cpp"  
	#"src/MNet/MAsyncSocket/MAsyncSocket.cpp"
	#"src/MNet/MAsyncSocket/MAsyncServer.cpp"
	#"src/MNet/MAsyncSocket/MAsyncClient.cpp"
	#"src/MNet/MAsyncSocket/MSession.cpp"
	#"src/MNet/MIOPool/MAsioIOServerPool/MAsioIOServerPool.cpp"  
	#"src/MNet/MIOPool/MAsioIOThreadPool/MAsioIOThreadPool.cpp"
	#"src/MNet/MCoroutineSocket/MCoroutineSocket.cpp"
	#"src/MNet/MHttpSocket/MHttpServer.cpp" 
	#"src/MNet/MHttpSocket/MHttpClient.cpp"
	#"src/MNet/MCoroutineSocket/MCoroutineServer.cpp" 
	#"src/MNet/MCoroutineSocket/MCoroutineClient.cpp" 
	#"src/MNet/MWebSocket/MWebSocket.cpp" 
	#"src/MNet/MCoroutineSocket/MSession.cpp" 

	#"src/MEventLoop/MEventLoop.cpp"
	#"src/MEventLoop/MEvent.cpp"

	
	"src/MSignal/MSignalUtils.cpp"

)


# 将源代码添加到此项目的可执行文件。
# 将测试添加到此项目的可执行文件。
# add_executable (TESTMAIN ${TEST_PATH} ${H_FILE_PATH} "src/MAllocator/MPoolAllocator.h" "src/MAllocator/MLOKIAllocator.h" "src/MAllocator/MArrayAllocaor.h" "src/MAllocator/MPoolAllocator.cpp" "src/MAllocator/MLOKIAllocator.cpp")
add_executable (CppSTL-shikounuzi 
	"CppSTL-shikounuzi.cpp" 
	"CppSTL-shikounuzi.h" 
	${SOURCES}
	${HEADERS}


 "src/MMedia/MMPlayer/MMAV/MMAVPackage.h"
 "src/MMedia/MMPlayer/MMAV/MMAVReader.cpp"
 "src/MMedia/MMPlayer/MMAV/MMAVPackage.cpp" 
 "src/MMedia/MMPlayer/MMAV/MMAVReader.h" 
 "src/MMedia/MMPlayer/MMAV/MMAV.h" 
 "src/MMedia/MMPlayer/MMAV/MMAVDecoder.h" 
 "src/MMedia/MMPlayer/MMAV/MMAVDecoder.cpp" 
 "src/MMedia/MMPlayer/MMAV/MMAVStream.h" 
 "src/MMedia/MMPlayer/MMAV/MMAVStream.cpp" 
 "src/MMedia/MMPlayer/MMAV/MMAVBase.h" "src/MMedia/MMPlayer/MMAV/MMAVFrame.h" "src/MMedia/MMPlayer/MMAV/MMAVFrame.cpp" "src/MMedia/MMPlayer/MMAV/example/MMAVExample.h" "src/MSTL/example/MSyncAnnularQueueExample.h")



# cmake中的相关顺序一定要正确
# 顺序是：
# include_directories
# link_directories
# add_executable
# target_link_libraries
# ————————————————————
# link_directories一定要放在add_executable前面，target_link_libraries一定要放在add_executable后面

# cmake变量名是区分大小写的
macro(targetLinkLib)
	target_link_libraries(CppSTL-shikounuzi PUBLIC ${SQLite3_LIBRARIES}) # Link with sqlite3 library
	message(STATUS "sqlite is targeted")

	target_link_libraries(CppSTL-shikounuzi PUBLIC ${BOOST_LIBRARIES})
	message(STATUS "boost is targeted")

	target_link_libraries(CppSTL-shikounuzi PUBLIC ${rapidjson_LIRARIES})
	message(STATUS "rapidjson is targeted")
	
	#ffmpeg_LINK_LIBRARIES 是cmake文档中的 所提及的变量 the libraries and their absolute paths 包含库及其路径
	#target_link_libraries(CppSTL-shikounuzi PUBLIC ${FFMPEG_LIBRARY_DIRS})
	#target_link_libraries(CppSTL-shikounuzi PUBLIC ${FFMPEG_LINK_LIBRARIES})
	
	target_link_libraries(CppSTL-shikounuzi PUBLIC ${FFMPEG_LIBRARIES_WITH_EXTEND})
	# target_link_libraries(CppSTL-shikounuzi PRIVATE PkgConfg::FFMPEG)
	message(STATUS "FFMPEG is targeted")
endmacro()
targetLinkLib()


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CppSTL-shikounuzi PROPERTY CXX_STANDARD 20)
endif()

# 设置dll拷贝
# file(copy ${SQLite3_LIBRARIES}/sqlite3.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH})
# 安装sqlite dll
install(FILES ${SQLITE3_CODE_LIBRARY_PATH}/sqlite-dll-win-x64-3440200/sqlite3.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH}/../bin)
# 安装ffmpeg dll
foreach(var ${FFMPEG_DLLS})
	install(FILES ${var} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/../bin)
endforeach()

message(STATUS "CMAKE Install 项目信息：")
message(STATUS "SQLITE.dll=${SQLITE3_CODE_LIBRARY_PATH}/sqlite-dll-win-x64-3440200/sqlite3.dll")
foreach(var ${FFMPEG_DLLS})
	message(STATUS "FFmpeg.dll=${var}")
endforeach()

message(STATUS "CMAKE 项目输出信息：")
message(STATUS "EXECUTABLE_OUTPUT_PATH=${EXECUTABLE_OUTPUT_PATH}")
message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

# TODO: 如有需要，请添加测试并安装目标。
