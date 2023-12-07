macro(initMSQLite)
	set(${H_FILE_PATH},
		${H_FILE_PATH}
		"src/MDataBase/MSQLite/MSQLite.h"  
	)
	set(${CPP_FILE_PATH},
		${CPP_FILE_PATH}
		"src/MDataBase/MSQLite/MSQLite.cpp"
	)
	message(STATUS "MSQLite is included")
endmacro()
initMSQLite()