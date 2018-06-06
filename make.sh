#/bin/sh
g++ -O3 -o main ConnectionResetProccessor.cc HttpResponseLine.cc HttpResponse.cc HttpBody.cc NetUtils.cc SendProccessor.cc ReceiveProccessor.cc RemoteSide.cc HttpUrl.cc HttpRequestLine.cc HttpHeader.cc HttpRequest.cc Stream.cc MemList.cc MasterThread.cc WorkThread.cc TimeLib.cc Task.cc TaskQueue.cc ServerConfig.cc Server.cc NetEngine.cc InetSocketAddress.cc IOHandler.cc IOEvent.cc main.cc EventPump.cc CriticalSection.cc ClientSide.cc DNSCache.cc Digest.cc MD5imp.cc MD5.cc User.cc Auth.cc AuthManager.cc UserSession.cc QueuedNetTask.cc PublicCookie.cc NetEngineTask.cc Mysql.cc AdminServer.cc AdminClient.cc BlackListIp.cc UrlProject.cc -lpthread -lrt -lmysqlclient
