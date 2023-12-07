macro(initMEventLoop)
	set(${H_FILE_PATH},
		${H_FILE_PATH}
		"src/MEventLoop/MEvent.h" 
		"src/MEventLoop/MEventLoop.h" 
		"src/MEventLoop/MSlot/MSlot.h"
		"src/MEventLoop/MSlot/MSignal.h" 
		"src/MEventLoop/MSlot/MConnect.h"
		"src/MEventLoop/MEventObject.h" 
	)

	set(${CPP_FILE_PATH},
		${CPP_FILE_PATH}
		"src/MEventLoop/MEventLoop.cpp"
		"src/MEventLoop/MEvent.cpp" 

	)
	message(STATUS "MEventLoop is included")
endmacro()
initMEventLoop()
