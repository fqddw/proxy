#/bin/sh
g++ -o main HttpRequest.cc DataIOHandler.cc Stream.cc MemList.cc MasterThread.cc WorkThread.cc TimeLib.cc Task.cc TaskQueue.cc ServerConfig.cc Server.cc NetEngine.cc InetSocketAddress.cc IOHandler.cc IOEvent.cc main.cc EventPump.cc CriticalSection.cc ClientSide.cc  -lpthread -lrt
