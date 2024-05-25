#macro(initMDesignModel)
set(H_FILE_PATH,
	${H_FILE_PATH}
	"src/MDesignModel/MObserver/MObserver.h"
	"src/MDesignModel/MObserver/MSubject.h" 
	"src/MDesignModel/MSingleton/MSingleton.h"
)
message(STATUS "MDesignModel is included")
#endmacro()
#initMDesignModel()
