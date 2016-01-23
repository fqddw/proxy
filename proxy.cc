#include "sys/socket.h"
#include "stdio.h"
#include "netdb.h"
#include "stdlib.h"
#include "netinet/in.h"
#include "unistd.h"
#include "pthread.h"
#include "string.h"
#include "signal.h"
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
typedef int SOCKET;
typedef long DWORD;
SOCKET mainsocket;
int pMemCount=0;
int mode=1;
typedef int BOOL;
#define FALSE 0L
#define TRUE true
int globalindex=0;
BOOL bFlag=FALSE;
int cSocket=0;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct hostent HOSTENT;
typedef struct in_addr IN_ADDR;
typedef struct sockaddr SOCKADDR;
int closesocket(SOCKET fd)
{ 
	return close(fd);
}
typedef struct _socklist
{
	SOCKET sockfd;
	char* hostname;
	int state;
	struct _socklist* next;
}SOCKETNODE;
SOCKETNODE* g_pSockListHeader;

int AppendList(SOCKETNODE* pList,SOCKET fd,char* hostname)
{
	int len=strlen(hostname);
	SOCKETNODE* ptmpList=pList;
	while(ptmpList->next!=NULL)
	{
		ptmpList=ptmpList->next;
	}

	if(ptmpList->sockfd!=NULL)
	{
		SOCKETNODE* pNewSocketNode=new SOCKETNODE;

		pNewSocketNode->sockfd=fd;
		pNewSocketNode->hostname=new char[len+1];
		memcpy(pNewSocketNode->hostname,hostname,len);
		pNewSocketNode->hostname[len]='\0';
		pNewSocketNode->next=NULL;
		ptmpList->next=NULL;
	}
	else
	{
		ptmpList->sockfd=fd;
		ptmpList->next=NULL;
	}
	return 0;//Always Success!!!

}
int GetHostName(char* buffer,char* pHostName)
{
	char* prefix="http://";
	char* pstart=strstr(buffer,"http://");

	if(pstart==NULL){
		return NULL;
	}
	char* prevstart=pstart;
	pstart+=strlen(prefix);
	char* pend=NULL;

	pend=strstr(pstart,"/");

	if(pend==NULL){
		return NULL;
	}
	int stlen=pend-pstart+1;

	char* str=pHostName;
	if(str==NULL){
		return NULL;
	}
	memset(str,'\0',stlen);
	strncpy(str,pstart,pend-pstart);


	return TRUE;
}

int FixURI(char* buffer,char *buf)
{
	memset(buf,'\0',4*1024);

	if(strstr(buffer,"CONNECT")==buffer)
		return NULL;
	char str[128]={'\0'};
	GetHostName(buffer,str);
	char* prevstart=strstr(buffer,"http://");
	int scheme=7;
	if(prevstart==NULL)
	{
		prevstart=strstr(buffer,"https://");
		if(prevstart==NULL)
			return -1;
		else
			scheme=8;
	}

	char* p=prevstart;
	strncpy(buf,buffer,p-buffer);
	char* c=p+strlen(str)+scheme;
	c=strstr(c,"/");
	char ptr[128]={'\0'};
	sprintf(ptr,"Host: %s",str);
	char *tmp=strstr(buffer,ptr);
	if(tmp==NULL){
		return NULL;
	}
	strncpy(buf+(p-buffer),c,tmp-c);
	char*host;
	if(mode==1)
		host="HOST:";
	else
		host="Host: ";
	sprintf(buf,"%s%s%s%s",buf,host,str,tmp+strlen(ptr));

	return TRUE;
}
typedef struct http_request_
{
	char* reqContent;
	int port;
	char* hostname;
}HTTPREQUEST;

HTTPREQUEST* PullAndParseClientRequest(SOCKET s)
{
	char* buffer=new char[16*1024];
	char* buf=new char[16*1024];
	if(!buffer || !buf)
		return 0;
	memset(buffer,'\0',16*1024);	
	int nRecv=1;
	nRecv=recv(s,buffer,16*1024,0);
	if(nRecv==0||nRecv==-1)
	{
		return 0;
	}
	if(strstr(buffer,"CONNECT")==buffer)
		return NULL;
	FixURI(buffer,buf);
	if(buf[0]=='\0')
		return NULL;
	char hostname[128]={'\0'};
	GetHostName(buffer,hostname);

	int port=80;//URI.port;
	if(strstr(buffer,"https://"))
		port=443;
	HTTPREQUEST* req=new HTTPREQUEST;
	req->hostname=new char[strlen(hostname)+1];
	memcpy(req->hostname,hostname,strlen(hostname));
	req->hostname[strlen(hostname)]='\0';
	req->port=port;
	req->reqContent=new char[strlen(buf)+1];
	memcpy(req->reqContent,buf,strlen(buf));
	req->reqContent[strlen(buf)]='\0';
	delete buffer;
	delete buf;
	return req;
}

SOCKET SendRequestToServer(HTTPREQUEST* pReq)
{
	SOCKET sclient=socket(AF_INET,SOCK_STREAM,0);
	printf("Client Socket %d\n",sclient);
	SOCKADDR_IN saclient;
	memset(&saclient,0,sizeof(saclient));
	saclient.sin_family=AF_INET;
	saclient.sin_port=htons(pReq->port);
	HOSTENT* hp=gethostbyname(pReq->hostname);
	if(!hp)
	{
		return 0;
	}
	saclient.sin_addr=*(IN_ADDR*)hp->h_addr;
	connect(sclient,(SOCKADDR*)&saclient,sizeof(SOCKADDR));
	send(sclient,pReq->reqContent,strlen(pReq->reqContent),0);
	return sclient;
}
typedef struct _partHTMLBuffer
{
	char* pHeader;
	int HeaderLen;

	char* pBody;
	int BodyLen;
}PARTBUFFER;
PARTBUFFER* TransGetResponseHeaderAndFollow(SOCKET SockRemote,SOCKET SockClient)
{
	int n=0;
	int sum=0;
	char pool[128]={'\0'};
	int bflag=FALSE;
	char* pBody=NULL;
	char* replyHeader=new char[4096];
	if(!replyHeader)
		return 0;
	memset(replyHeader,'\0',4096);
	while(bflag==FALSE)
	{
		n=recv(SockRemote,pool,128,0);
		send(SockClient,pool,n,0);
		if(n==-1 || n==0)
		{
			return 0;
		}
		memcpy(replyHeader+sum,pool,n);
		sum+=n;
		char* pHeaderEnd=strstr(replyHeader,"\r\n\r\n");
		if(pHeaderEnd)
		{
			bflag=TRUE;
			char* pTmpReplyHeader=new char[pHeaderEnd+4-replyHeader];
			memcpy(pTmpReplyHeader,replyHeader,pHeaderEnd+4-replyHeader);
			int BodyLen=sum+replyHeader-pHeaderEnd-4;
			pBody=new char[BodyLen];
			memcpy(pBody,pHeaderEnd+4,BodyLen);
			int HeaderLen=pHeaderEnd-replyHeader+4;
			delete replyHeader;
			replyHeader=pTmpReplyHeader;
			PARTBUFFER* pPB=new PARTBUFFER;
			pPB->pHeader=replyHeader;
			pPB->HeaderLen=HeaderLen;
			pPB->pBody=pBody;
			pPB->BodyLen=BodyLen;
			return pPB;
		}
	}
	return 0;
}

enum Transfer_Type{TRANS_CHUNKED,TRANS_CONTENT_LENGTH,TRANS_CONNECTION_CLOSE};
typedef struct _transfer_param
{
	int code;
	int arg;
}TRANS_PARAM;
char* GetHttpField(char* pHTTPHeader,char* name)
{
	char tag[128]={'\0'};
	sprintf(tag,"%s: ",name);
	char* pPosBegin=strstr(pHTTPHeader,tag);
	if(!pPosBegin)
		return NULL;
	char* pPosEnd=strstr(pPosBegin,"\r\n");
	char* pvalue=new char[pPosEnd-pPosBegin];
	memcpy(pvalue,pPosBegin+strlen(tag),pPosEnd-pPosBegin-strlen(tag));
	return pvalue;
}

TRANS_PARAM* GetTransferType(PARTBUFFER* pBuffer)
{

	char* pHeader=pBuffer->pHeader;
	char* pFlag=NULL;

	TRANS_PARAM* pArg=new TRANS_PARAM;
	pArg->code=TRANS_CONNECTION_CLOSE;
	pFlag=GetHttpField(pHeader,"Content-Length");
	if(pFlag)
	{
		pArg->code=TRANS_CONTENT_LENGTH;
		pArg->arg=atoi(pFlag);
		return pArg;
	}


	pFlag=GetHttpField(pHeader,"Transfer-Encoding");
	if(pFlag)
	{
		if(strstr(pFlag,"chunked"))
		{
			pArg->code=TRANS_CHUNKED;
			return pArg;
		}
	}
	pFlag=GetHttpField(pHeader,"Connection");
	if(pFlag)
	{
		if(strstr(pFlag,"close"))
		{
			pArg->code=TRANS_CONNECTION_CLOSE;
			return pArg;
		}
	}
	delete pFlag;
	return pArg;
}

int DecodeAndDispatchChunkedBody(SOCKET SockRemote,SOCKET SockClient,PARTBUFFER* pBuffer)
{

	char* pBody=pBuffer->pBody;
	char* pEnd=pBody+pBuffer->BodyLen;
	char* pBodyOrg=pBody;
	int loopEnd=FALSE;
	int lastlen=0;
	int bexit=FALSE;
	int nBytesRecv=0;
	char pool[256*1024]={'\0'};
	int RemainLen=0;
	nBytesRecv=pBuffer->BodyLen;
	if(pBuffer->BodyLen==0)
		loopEnd=TRUE;
	char tmpPool2[2048]={'\0'};
	while(bexit==FALSE)
	{
		while(loopEnd==FALSE)
		{
			if(!pBody)
				return 0;
			int n=nBytesRecv;
			char* oldBody=pBody;
			pBody+=lastlen;
			char* tagEnd=NULL;
			tagEnd=strstr(pBody,"\r\n");
			if(tagEnd==NULL)
			{
				char tmpPool[1024]={'\0'};
				int nRecv=recv(SockRemote,tmpPool,1024,0);
				send(SockClient,tmpPool,nRecv,0);
				int delta=pEnd-pBody;
				if(delta<0)
					delta=0;

				memcpy(tmpPool2,pBody,delta);
				memcpy(tmpPool2+delta,tmpPool,nRecv);

				pBody=tmpPool2;
				pEnd=pBody+delta+nRecv;
				tagEnd=strstr(pBody,"\r\n");
			}
			char strnum[16]={'\0'};
			int len=0;
			memcpy(strnum,pBody,tagEnd-pBody);
			sscanf(strnum,"%x",&len);
			if(len==0)
			{
				//closesocket(SockClient);
				return 0;

			}
			if(pEnd-(tagEnd+2)<len)
			{
				lastlen=len-(pEnd-tagEnd-2)+2;
				loopEnd=TRUE;
			}
			else if(pEnd-(tagEnd+2)<=len+2)
			{
				lastlen=2+len-(pEnd-(tagEnd+2));
				loopEnd=TRUE;
			}
			else
			{
				pBody=(tagEnd+4+len);
				lastlen=0;
			}
		}
		memset(pool,'\0',256*1024);
		nBytesRecv=recv(SockRemote,pool,256*1024,0);
		if(nBytesRecv==-1 || nBytesRecv==0)
			return 0;
		send(SockClient,pool,nBytesRecv,0);
		pBody=pool;
		RemainLen+=nBytesRecv;
		if(RemainLen>=lastlen)
		{
			loopEnd=FALSE;
			lastlen=nBytesRecv-(RemainLen-lastlen);
			pEnd=pBody+nBytesRecv;
			RemainLen=0;
		}
	}

	return 0;
}

int DecodeAndDispatchCLBody(SOCKET SockRemote,SOCKET SockClient,PARTBUFFER* pBuffer,int len )
{
	//send(SockClient,pBuffer->pBody,pBuffer->BodyLen,0);
	int sum=pBuffer->BodyLen;
	int bFlag=FALSE;
	while(sum<len)//bFlag!=TRUE)//sum<len)
	{
		char pool[128]={'\0'};
		int n=recv(SockRemote,pool,128,0);
		if(n!=0 && n!=-1)
		{
			sum+=n;
			send(SockClient,pool,n,0);
			//if(sum>len)
			//continue;
		}
		else
			return 0;
	}
	return 0;
}

int DecodeAndDispatchHTTP_VER_1_0_Body(SOCKET SockRemote,SOCKET SockClient,PARTBUFFER* pBuffer)
{
	char* pBody=pBuffer->pBody;
	int nBytesRecv=1;
	while(nBytesRecv!=-1 && nBytesRecv!=0)
	{
		char pool[128]={'\0'};
		nBytesRecv=recv(SockRemote,pool,128,0);
		if(nBytesRecv!=-1 && nBytesRecv!=0 )
			send(SockClient,pool,nBytesRecv,0);
	}
	return 0;
}
int PullAndParseServerData(SOCKET SockRemote,SOCKET SockClient)
{
	int contentlen=0;
	PARTBUFFER* pPartBuf=TransGetResponseHeaderAndFollow(SockRemote,SockClient);
	if(pPartBuf==NULL)
		return 0;
	TRANS_PARAM *pArg=GetTransferType(pPartBuf);
	if(!pArg)
	{
		return 0;
	}
	switch(pArg->code)
	{
		case TRANS_CHUNKED:
			DecodeAndDispatchChunkedBody(SockRemote,SockClient,pPartBuf);
			return 1;
			break;
		case TRANS_CONTENT_LENGTH:
			DecodeAndDispatchCLBody(SockRemote,SockClient,pPartBuf,pArg->arg);
			return 1;
			break;
		case TRANS_CONNECTION_CLOSE:
			DecodeAndDispatchHTTP_VER_1_0_Body(SockRemote,SockClient,pPartBuf);
			return 0;
			break;
		default:
			DecodeAndDispatchHTTP_VER_1_0_Body(SockRemote,SockClient,pPartBuf);
			return 0;
	}
	if(pPartBuf->pHeader!=NULL)
		delete pPartBuf->pHeader;
	if(pPartBuf->pBody!=NULL)
		delete pPartBuf->pBody;
	if(pPartBuf=NULL)
		delete pPartBuf;
	if(pArg!=NULL)
		delete pArg;
	return 0;
}
void* work(SOCKET pv)
{
	SOCKET clientSideSocket=pv;
	int flag = 1;
	while(flag){
	HTTPREQUEST* pReq=PullAndParseClientRequest(clientSideSocket);

	if(!pReq)
	{
		closesocket(clientSideSocket);
		return 0;
	}
	int n=strlen(pReq->reqContent);
	SOCKET clSocket=SendRequestToServer(pReq);
	if(clSocket==0)
	{
		closesocket(clientSideSocket);
		return 0;
	}
	flag = PullAndParseServerData(clSocket,clientSideSocket);
	}
	return 0;
}
typedef struct _ENTRYPARAM
{
	int sockfd;
	int state;
	int index;
	pthread_t t;
	pthread_cond_t ct;
}ENTRYPARAM;
void*  Work_Static(void* param)
{
	ENTRYPARAM* ep=(ENTRYPARAM*)param;
	while(1)
	{
		pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_lock(&m);
		pthread_cond_wait(&(ep->ct),&m);
		pthread_mutex_unlock(&m);
		work(ep->sockfd);
		pthread_cond_init(&(ep->ct),NULL);
		ep->state=0;
	}
	return NULL;
}
class ThreadPool
{
	private:
		ENTRYPARAM *pEntry;
		int	  num;
	public:
		int FindIdle();
		ThreadPool();
		int Init();
		int ActiveThread(int,int);

};
int ThreadPool::FindIdle()
{
	int i=0;
	for(;i<num;i++)
	{
		if(pEntry[i].state==0)
			return i;
	}
	return -1;
}
int ThreadPool::ActiveThread(int index,int sockfd)
{
	pEntry[index].sockfd=sockfd;
	pEntry[index].state=1;
	pthread_cond_broadcast(&(pEntry[index].ct));
	return 0;
}
int ThreadPool::Init()
{
	int i=0;
	for(;i<num;i++)
	{
		pthread_create(&(pEntry[i].t),0,Work_Static,pEntry+i);
		pthread_cond_init(&(pEntry[i].ct),NULL);
		pEntry[i].state=0;
	}
	return 0;
}
ThreadPool::ThreadPool()
{
	num=100;
	pEntry=new ENTRYPARAM[num];
}


int Dispatch()
{
	socklen_t len=sizeof(SOCKADDR);
	ThreadPool* pTP=new ThreadPool();
	pTP->Init();
	while(1)
	{
		SOCKADDR sa;
		DWORD tid=0;
		SOCKET *s=new SOCKET;


		if(s==NULL) continue;
		*s=accept(mainsocket,&sa,&len);
		int index=pTP->FindIdle();
		if(index!=-1)
		{
			pTP->ActiveThread(index,*s);
		}
	}
	return 0;
}
int main(int argc,char** argv)
{
	signal(SIGPIPE,SIG_IGN);
	if(argc>=2)
	{	
		if(strstr(argv[1],"1")==NULL)
			mode=0;
	}
	mainsocket=socket(AF_INET,SOCK_STREAM,0);

	SOCKADDR_IN sa;
	sa.sin_family=AF_INET;
	sa.sin_port=htons(8000);
	sa.sin_addr.s_addr=INADDR_ANY;
	int flag=1;
	int len=sizeof(int);
	setsockopt(mainsocket,SOL_SOCKET,SO_REUSEADDR,&flag,len);
	bind(mainsocket,(SOCKADDR*)&sa,sizeof(sa));
	listen(mainsocket,1024);
	Dispatch();

}

