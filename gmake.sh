#/bin/sh
g++ -g -o main Stream.cc MemList.cc MasterThread.cc WorkThread.cc TimeLib.cc Task.cc TaskQueue.cc ServerConfig.cc Server.cc NetEngine.cc InetSocketAddress.cc IOHandler.cc IOEvent.cc main.cc EventPump.cc CriticalSection.cc ClientSide.cc ClientSideTask.cc -lpthread -lrt
