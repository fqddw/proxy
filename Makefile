.PHONY : clean

CC=g++
LDFLAGS=-L/usr/lib64/mysql/ 
LIBS=-lmysqlclient -lpthread -lrt
ELFNAME=main
OBJECTS=Mysql.o HttpUrl.o Server.o EventPump.o NetEngine.o HttpBody.o NetUtils.o HttpRequest.o HttpRequestLine.o TaskQueue.o Stream.o ConnectionResetProccessor.o HttpResponseLine.o ClientSide.o RemoteSide.o MemList.o PublicCookie.o DNSCache.o HttpHeader.o ServerConfig.o InetSocketAddress.o QueuedNetTask.o Digest.o MD5imp.o MD5.o User.o TimeLib.o Task.o UserSession.o Auth.o AuthManager.o NetEngineTask.o BlackListIp.o UrlProject.o AdminServer.o AdminClient.o MasterThread.o WorkThread.o IOHandler.o IOEvent.o AccessLog.o MemStore.o ReceiveProccessor.o SendProccessor.o CriticalSection.o HttpResponse.o main.o

main: $(OBJECTS)
	$(CC) -o $(ELFNAME) $(OBJECTS) $(LDFLAGS) $(LIBS)


Mysql.o: Mysql.cc
	g++ -Wall -c -o Mysql.o Mysql.cc
HttpUrl.o: HttpUrl.cc
	g++ -Wall -c -o HttpUrl.o HttpUrl.cc
Server.o: Server.cc
	g++ -Wall -c -o Server.o Server.cc
EventPump.o: EventPump.cc
	g++ -Wall -c -o EventPump.o EventPump.cc
NetEngine.o: NetEngine.cc
	g++ -Wall -c -o NetEngine.o NetEngine.cc
HttpBody.o: HttpBody.cc
	g++ -Wall -c -o HttpBody.o HttpBody.cc
NetUtils.o: NetUtils.cc
	g++ -Wall -c -o NetUtils.o NetUtils.cc
HttpRequest.o: HttpRequest.cc
	g++ -Wall -c -o HttpRequest.o HttpRequest.cc
HttpRequestLine.o: HttpRequestLine.cc
	g++ -Wall -c -o HttpRequestLine.o HttpRequestLine.cc
TaskQueue.o: TaskQueue.cc
	g++ -Wall -c -o TaskQueue.o TaskQueue.cc
Stream.o: Stream.cc
	g++ -Wall -c -o Stream.o Stream.cc
ConnectionResetProccessor.o: ConnectionResetProccessor.cc
	g++ -Wall -c -o ConnectionResetProccessor.o ConnectionResetProccessor.cc
HttpResponseLine.o: HttpResponseLine.cc
	g++ -Wall -c -o HttpResponseLine.o HttpResponseLine.cc
ClientSide.o: ClientSide.cc
	g++ -Wall -c -o ClientSide.o ClientSide.cc
RemoteSide.o: RemoteSide.cc
	g++ -Wall -c -o RemoteSide.o RemoteSide.cc
MemList.o: MemList.cc
	g++ -Wall -c -o MemList.o MemList.cc
PublicCookie.o: PublicCookie.cc
	g++ -Wall -c -o PublicCookie.o PublicCookie.cc
DNSCache.o: DNSCache.cc
	g++ -Wall -c -o DNSCache.o DNSCache.cc
HttpHeader.o: HttpHeader.cc
	g++ -Wall -c -o HttpHeader.o HttpHeader.cc
ServerConfig.o: ServerConfig.cc
	g++ -Wall -c -o ServerConfig.o ServerConfig.cc
InetSocketAddress.o: InetSocketAddress.cc
	g++ -Wall -c -o InetSocketAddress.o InetSocketAddress.cc
QueuedNetTask.o: QueuedNetTask.cc
	g++ -Wall -c -o QueuedNetTask.o QueuedNetTask.cc
Digest.o: Digest.cc
	g++ -Wall -c -o Digest.o Digest.cc
MD5imp.o: MD5imp.cc
	g++ -Wall -c -o MD5imp.o MD5imp.cc
MD5.o: MD5.cc
	g++ -Wall -c -o MD5.o MD5.cc
User.o: User.cc
	g++ -Wall -c -o User.o User.cc
TimeLib.o: TimeLib.cc
	g++ -Wall -c -o TimeLib.o TimeLib.cc
Task.o: Task.cc
	g++ -Wall -c -o Task.o Task.cc
UserSession.o: UserSession.cc
	g++ -Wall -c -o UserSession.o UserSession.cc
Auth.o: Auth.cc
	g++ -Wall -c -o Auth.o Auth.cc
AuthManager.o: AuthManager.cc
	g++ -Wall -c -o AuthManager.o AuthManager.cc
NetEngineTask.o: NetEngineTask.cc
	g++ -Wall -c -o NetEngineTask.o NetEngineTask.cc
BlackListIp.o: BlackListIp.cc
	g++ -Wall -c -o BlackListIp.o BlackListIp.cc
UrlProject.o: UrlProject.cc
	g++ -Wall -c -o UrlProject.o UrlProject.cc
AdminServer.o: AdminServer.cc
	g++ -Wall -c -o AdminServer.o AdminServer.cc
AdminClient.o: AdminClient.cc
	g++ -Wall -c -o AdminClient.o AdminClient.cc
MasterThread.o: MasterThread.cc
	g++ -Wall -c -o MasterThread.o MasterThread.cc
WorkThread.o: WorkThread.cc
	g++ -Wall -c -o WorkThread.o WorkThread.cc
IOHandler.o: IOHandler.cc
	g++ -Wall -c -o IOHandler.o IOHandler.cc
IOEvent.o: IOEvent.cc
	g++ -Wall -c -o IOEvent.o IOEvent.cc
AccessLog.o: AccessLog.cc
	g++ -Wall -c -o AccessLog.o AccessLog.cc
MemStore.o: MemStore.cc
	g++ -Wall -c -o MemStore.o MemStore.cc
ReceiveProccessor.o: ReceiveProccessor.cc
	g++ -Wall -c -o ReceiveProccessor.o ReceiveProccessor.cc
SendProccessor.o: SendProccessor.cc
	g++ -Wall -c -o SendProccessor.o SendProccessor.cc
CriticalSection.o: CriticalSection.cc
	g++ -Wall -c -o CriticalSection.o CriticalSection.cc
HttpResponse.o: HttpResponse.cc
	g++ -Wall -c -o HttpResponse.o HttpResponse.cc
main.o: main.cc
	g++ -Wall -c -o main.o main.cc

clean:
	rm -f *.o
