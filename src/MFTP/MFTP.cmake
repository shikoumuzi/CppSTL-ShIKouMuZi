macro(initMFTP)
include("src/MLog/MLog.cmake")
	set(H_FILE_PATH,
		${H_FILE_PATH}
		"src/MFTP/MFTPProto.h" 
		"src/MFTP/MFTP.h" 
	)

	set(CPP_FILE_PATH,
		${CPP_FILE_PATH}
		"src/MFTP/MFTP.cpp" 
	)
	message(STATUS "MFTP is included")

endmacro()
initMFTP()
