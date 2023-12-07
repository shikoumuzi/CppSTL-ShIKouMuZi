include("src/MLog/MLog.cmake")
set(${H_FILE_PATH},
	${H_FILE_PATH}
	"src/MSTL/h/MTreeMap.h" 
	"src/MSTL/h/MHashMap.h" 
	"src/MSTL/h/MList.h" 
	"src/MSTL/h/MStack.h" 
	"src/MSTL/h/MDeque.h" 
	"src/MSTL/h/MSyncAnnularQueue.h" 
	"src/MSTL/h/MVector.h" 
	"src/MSTL/h/MSkipList.h"  
	"src/MSTL/h/MArray.h"
	"src/MSTL/h/MAtomicLock.h" 
)

set(${CPP_FILE_PATH},
	${CPP_FILE_PATH}
	"src/MAllocator/MPoolAllocator.cpp" 
	"src/MAllocator/MLOKIAllocator.cpp" 
)
message(STATUS "MSTL is included")