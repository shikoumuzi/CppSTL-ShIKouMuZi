macro(initMBase)
	set(H_FILE_PATH,
		${H_FILE_PATH}
		"src/MBase/MError.h"  
		"src/MBase/MObjectBase.h" 
	)
	message(STATUS "MBase is included")
endmacro()
initMBase()