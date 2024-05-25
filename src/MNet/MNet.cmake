
include("src/MLog/MLog.cmake")
include("src/MSTL/MSTL.cmake")
include("src/MDesignModel/MDesignModel.cmake")
set(H_FILE_PATH,
	${H_FILE_PATH}
		
	"src/MNet/MEndPoint/MClientEndPoint.h"
	"src/MNet/MEndPoint/MServerEndPoint.h"
	"src/MNet/MNetBase.h"
	"src/MNet/MSyncSocket/MSyncSocket.h"
	"src/MNet/MAsyncSocket/MAsyncSocket.h"
	"src/MNet/MAsyncSocket/MAsyncServer.h"
	"src/MNet/MEndPoint/MEndPoint.h"
	"src/MNet/MAsyncSocket/MAsyncClient.h"
	"src/MNet/MMsgNode/MMsgNode.h" 
	"src/MNet/MAsyncSocket/MSession.h"
	"src/MNet/MMsgNode/MSendMsgNode.h" 
	"src/MNet/MMsgNode/MRecvMsgNode.h" 
	"src/MNet/MLogicSystem/MLogicSystem.h"
	"src/MNet/MLogicSystem/MLogicNode.h" 
	"src/MNet/MIOPool/MAsioIOServerPool/MAsioIOServerPool.h"
	"src/MNet/MIOPool/MAsioIOThreadPool/MAsioIOThreadPool.h"
	"src/MNet/MCoroutineSocket/MCoroutineSocket.h"
	"src/MNet/MCoroutineSocket/MSession.h" 
	"src/MNet/MHttpSocket/MHttpServer.h" 
	"src/MNet/MReliableUDP/MReliableUDP.h" 
	"src/MNet/MWebSocket/MWebSocket.h" 
	"src/MNet/MCoroutineSocket/MCoroutineServer.h" 
	"src/MNet/MCoroutineSocket/MCoroutineClient.h" 
	"src/MNet/MHttpSocket/MHttpClient.h" 
	"src/MNet/example/MAsyncSocketExample.h" 

)

set(CPP_FILE_PATH,
	${CPP_FILE_PATH}
	"src/MNet/MEndPoint/MClientEndPoint.cpp"
	"src/MNet/MEndPoint/MServerEndPoint.cpp" 
	"src/MNet/MSyncSocket/MSyncSocket.cpp"  
	"src/MNet/MAsyncSocket/MAsyncSocket.cpp"
	"src/MNet/MAsyncSocket/MAsyncServer.cpp"
	"src/MNet/MAsyncSocket/MAsyncClient.cpp"
	"src/MNet/MAsyncSocket/MSession.cpp"
	"src/MNet/MIOPool/MAsioIOServerPool/MAsioIOServerPool.cpp"  
	"src/MNet/MIOPool/MAsioIOThreadPool/MAsioIOThreadPool.cpp"
	"src/MNet/MCoroutineSocket/MCoroutineSocket.cpp"
	"src/MNet/MHttpSocket/MHttpServer.cpp" 
	"src/MNet/MHttpSocket/MHttpClient.cpp"
	"src/MNet/MCoroutineSocket/MCoroutineServer.cpp" 
	"src/MNet/MCoroutineSocket/MCoroutineClient.cpp" 
	"src/MNet/MWebSocket/MWebSocket.cpp" 
	"src/MNet/MCoroutineSocket/MSession.cpp" 
)
message(STATUS "MNet is included")

