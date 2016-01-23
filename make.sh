#/bin/sh
c++ -c MasterThread.cc -o MasterThread.o
c++ -c WorkThread.cc -o WorkThread.o
c++ -c TimeLib.cc -o TimeLib.o
c++ -c Task.cc -o Task.o
c++ -c TaskQueue.cc -o TaskQueue.o
c++ -c ServerConfig.cc -o ServerConfig.o
c++ -c Server.cc -o Server.o
c++ -c NetEngine.cc -o NetEngine.o
c++ -c InetSocketAddress.cc -o InetSocketAddress.o
c++ -c IOHandler.cc -o IOHandler.o
c++ -c IOEvent.cc -o IOEvent.o
c++ -c main.cc -o main.o
c++ -c EventPump.cc -o EventPump.o
c++ -c CriticalSection.cc -o CriticalSection.o
c++ -c ClientSide.cc -o ClientSide.o
c++ -c ClientSideTask.cc -o ClientSideTask.o
c++ -O2 -o main main.o ClientSideTask.o ClientSide.o IOEvent.o IOHandler.o InetSocketAddress.o Server.o ServerConfig.o NetEngine.o CriticalSection.o EventPump.o TimeLib.o Task.o TaskQueue.o WorkThread.o MasterThread.o -lpthread -lrt
