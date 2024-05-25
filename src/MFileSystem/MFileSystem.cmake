macro(initMFileSystem)
	set(H_FILE_PATH,
		${H_FILE_PATH}
		"src/MFileSystem/MFileDataBase.h" 
		"src/MFileSystem/MFileInput.h" 
		"src/MFileSystem/MFileOutput.h"  
		"src/MFileSystem/FileIOQueue.h"  
		"src/MFileSystem/MFileBase/MFileInWin.h" 
	)

	set(CPP_FILE_PATH,
		${CPP_FILE_PATH}
		"src/MFileSystem/FileIOQueue.cpp"
		"src/MFileSystem/MFileInput.cpp" 
		"src/MFileSystem/MFile.cpp" 

		"src/MFileSystem/MFileOutput.cpp" 
		"src/MFileSystem/MFileDataBase.cpp" 
	)
	message(STATUS "MFileSystem is included")
endmacro()
initMFileSystem()
