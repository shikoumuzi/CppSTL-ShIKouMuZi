include("src/MLog/MLog.cmake")


set(MSTL_HEAD_FILE_PATH
	"src/MSTL/include/MTreeMap.h" 
	"src/MSTL/include/MHashMap.h" 
	"src/MSTL/include/MList.h" 
	"src/MSTL/include/MStack.h" 
	"src/MSTL/include/MDeque.h" 
	"src/MSTL/include/MSyncAnnularQueue.h" 
	"src/MSTL/include/MVector.h" 
	"src/MSTL/include/MSkipList.h"  
	"src/MSTL/include/MArray.h"
	"src/MSTL/include/MAtomicLock.h" 
	"src/MSTL/h/MPriorityQueue.h" 
	"src/MSTL/h/MHeap.h" 


	"src/MNet/MKCP/MKCP.h" 
	"src/MSTL/include/MBaseString.h"    
	"src/MSTL/include/MTypeTraits.h" 
	"src/MSTL/include/MPolymorphicFunctional.h"

	"src/MSTL/example/MSyncAnnularQueueExample.h" 

)

set(H_FILE_PATH,
	${H_FILE_PATH}
	${MSTL_HEAD_FILE_PATH}
)

set(CPP_FILE_PATH,
	${CPP_FILE_PATH}

	"src/MNet/example/MSyncSocketExample.h" 
)

# set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${MSTL_HEAD_FILE_PATH})
message(STATUS "MSTL is included")