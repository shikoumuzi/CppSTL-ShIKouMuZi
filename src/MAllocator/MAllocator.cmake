
include("src/MLog/MLog.cmake")
set(${H_FILE_PATH},
	${H_FILE_PATH}
	"src/MAllocator/MPoolAllocator.h" 
	"src/MAllocator/MLOKIAllocator.h" 
	"src/MAllocator/MArrayAllocaor.h")

set(${CPP_FILE_PATH},
	${CPP_FILE_PATH}
	"src/MAllocator/MPoolAllocator.cpp" 
	"src/MAllocator/MLOKIAllocator.cpp" )
message(STATUS "MAllocator is included")

