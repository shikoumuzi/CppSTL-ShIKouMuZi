macro(initMMplayer)
set(${H_FILE_PATH}
	${H_FILE_PATH}
	"src/MMedia/MMPlayer/MMAV/MMAVPackage.h"

	"src/MMedia/MMPlayer/MMAV/MMAVReader.h" 
	"src/MMedia/MMPlayer/MMAV/MMAV.h"

)

set(${CPP_FILE_PATH}
	${CPP_FILE_PATH}
	 "src/MMedia/MMPlayer/MMAV/MMAVReader.cpp"
	"src/MMedia/MMPlayer/MMAV/MMAVPackage.cpp" 
)
endmacro()
initMMplayer()