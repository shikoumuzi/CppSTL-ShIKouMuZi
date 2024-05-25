macro(initMMplayer)
set(H_FILE_PAT
	${H_FILE_PATH}
	"src/MMedia/MMPlayer/MMAV/MMAVPackage.h"

	"src/MMedia/MMPlayer/MMAV/MMAVReader.h" 
	"src/MMedia/MMPlayer/MMAV/MMAV.h"
	"src/MMedia/MMPlayer/MMAV/MMAVEncoder.h" 

	"src/MMedia/MMPlayer/MMAV/MMAVEncoderManager.h" 
	"src/MMedia/MMPlayer/MMAV/MMAVDecoderManager.h" 

	"src/MMedia/MMPlayer/MMAV/MMAVVideoPrinter.h" 
	"src/MMedia/MMPlayer/MMAV/MMAVPackage.h"

	"src/MMedia/MMPlayer/MMAV/MMAVReader.h" 
	"src/MMedia/MMPlayer/MMAV/MMAV.h" 
	"src/MMedia/MMPlayer/MMAV/MMAVDecoder.h" 

	"src/MMedia/MMPlayer/MMAV/MMAVStream.h" 
	"src/MMedia/MMPlayer/MMAV/MMAVBase.h" 
	"src/MMedia/MMPlayer/MMAV/MMAVFrame.h" 
	"src/MMedia/MMPlayer/MMAV/example/MMAVExample.h"

)

set(CPP_FILE_PATH
	${CPP_FILE_PATH}
	"src/MMedia/MMPlayer/MMAV/MMAVReader.cpp"
	"src/MMedia/MMPlayer/MMAV/MMAVPackage.cpp" 
	"src/MMedia/MMPlayer/MMAV/MMAVEncoder.cpp" 
	"src/MMedia/MMPlayer/MMAV/MMAVEncoderManager.cpp" 
	"src/MMedia/MMPlayer/MMAV/MMAVVideoPrinter.cpp" 
	"src/MMedia/MMPlayer/MMAV/MMAVReader.cpp"
	"src/MMedia/MMPlayer/MMAV/MMAVPackage.cpp" 
	"src/MMedia/MMPlayer/MMAV/MMAVStream.cpp" 
	"src/MMedia/MMPlayer/MMAV/MMAVFrame.cpp" 
	"src/MMedia/MMPlayer/MMAV/MMAVDecoder.cpp" 

)
endmacro()
initMMplayer()