#include "NetUtils.h"
#include "CommonType.h"
#include "stdio.h"
#include "memory.h"
#include "TimeLib.h"
#include "DNSCache.h"

#include "string.h"
#include "stdlib.h"
#include "unistd.h"

#include "arpa/inet.h"
#include <netinet/in.h>

#include "Stream.h"

extern DNSCache* g_pDNSCache;
typedef struct _buffer
{
	char* buffer;
	int len;
}URLBUFFER;

long getfirstip(char* resp)
{
	int i = 12;

	int val = (int)resp[i];
	while(val != 0)
	{
		i=i+(val+1);
		val = (int)(resp[i]);
	}
	i+=5;
	char* pkg_start = resp+i;
	int type= ntohs(*((unsigned short*)(pkg_start+2)));
	int anscount = ntohs(*(unsigned short*)(resp+6));
	int offset = 0;
	while(type != 1)
	{
		int datalen = ntohs(*((unsigned short*)(pkg_start+10)));
		//printf("datalen %d\n", datalen);
		pkg_start = pkg_start+12+datalen;
		type= ntohs(*((unsigned short*)(pkg_start+2)));
		//printf("type %d\n", type);
		offset++;
		if(offset >= anscount)
			return 0;
	}

	long ip = *(int*)(pkg_start+12);
	return ip;
}
Stream* getURLBuffer(char* url)
{
	Stream* pStream = new Stream(strlen(url)+2);
	int i = 0;
	int start = 0;
	int offset = 0;
	char* pData = pStream->GetData();
	while(i<strlen(url))
	{
		if(url[i] == '.')
		{
			pData[offset] = start;
			offset+=start+1;
			start = 0;
		}
		else{
			pData[offset+start+1]=url[i];
			start++;
		}
		i++;
	}
			pData[offset] = start;
			pData[pStream->GetLength()-1] = 0;
	return pStream;
}

int getip(char* purl)
{
	int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct timeval tv_out;
	tv_out.tv_sec = 1;
	tv_out.tv_usec = 0;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
	struct sockaddr_in sai={0};

	sai.sin_family = AF_INET;
	sai.sin_addr.s_addr = inet_addr("119.29.29.29");
	sai.sin_port = htons(53);
	int ret = 0;//connect(fd, (struct sockaddr*)&sai, sizeof(sai));
	char* dnsflat = (char*)malloc(512);
	memset(dnsflat, 0, 512);
	unsigned short qr = 0x0000;

	((unsigned short*)dnsflat)[0] = 0xdcad;
	((unsigned short*)dnsflat)[1] = (qr);
	((unsigned short*)dnsflat)[2] = htons(1);
	((unsigned short*)dnsflat)[3] = 0;
	((unsigned short*)dnsflat)[4] = 0;
	((unsigned short*)dnsflat)[5] = 0;
	Stream* pStream = getURLBuffer(purl);
	char* pData = pStream->GetData();
	memcpy(dnsflat+12, pStream->GetData(),pStream->GetLength());
	*((short*)(dnsflat+12+pStream->GetLength())) = htons(1);
	*((short*)(dnsflat+12+pStream->GetLength()+2)) = htons(1);
	socklen_t lent = sizeof(sai);
	sendto(fd, dnsflat, 12+pStream->GetLength()+2+2, 0,(struct sockaddr*)&sai, lent);
	delete pStream;
	char buf[1024] = {0};
	socklen_t len = sizeof(sai);
	ret = recvfrom(fd, buf,1024,0,(struct sockaddr*)&sai,&len);
	printf("return %d\n", ret);
	close(fd);
	if(ret == 0 || ret == -1)
	{
		return 0;
	}
	if(buf[3] != 0xffffff80 || (buf[6] == 0 && buf[7] == 0))
	{
		printf("recv %d bytes %x\n", ret, buf[3]);
		return 0;
	}
	int i = 0;


	int ip = getfirstip(buf);
	return ip;
}


InetSocketAddress* NetUtils::GetHostByName(char* pHostName,int port)
{
	if(strlen(pHostName) == 0)
	{
		return NULL;
	}
	DNSItem* pItem = g_pDNSCache->getItemByHost(pHostName);
	if(pItem){
		int ip = pItem->GetSaddr();
		int bValid = pItem->IsValid();

		if(1)//bValid)
			return new InetSocketAddress(port, ip);
		else
			return NULL;
	}
	else{
		return NULL;
		int ip = getip(pHostName);
		if(ip !=0)
			g_pDNSCache->AddRecord(pHostName, ip, TRUE);
		else
			g_pDNSCache->AddRecord(pHostName, 0, FALSE);
		return new InetSocketAddress(port,ip);

	}/*{

		struct addrinfo hints;
		memset(&hints,0,sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		struct addrinfo* ptr,*result;

		//struct timespec start = Time::GetNow();
		int ret = getaddrinfo(pHostName,NULL,&hints,&result);
		//struct timespec end = Time::GetNow();
		//struct timespec sub = Time::Sub(end, start);
		if (ret != 0) {

			g_pDNSCache->AddRecord(pHostName, 0, FALSE);
			//fprintf(stderr,"getaddrinfo: %s %s/n", pHostName,
					//gai_strerror(ret));
			return NULL;
		}
		int index = 0;
		for(ptr = result;ptr!=NULL;ptr = ptr->ai_next)
		{
			index++;
			if(ptr->ai_family == AF_INET)
			{
				struct sockaddr_in *psa = (struct sockaddr_in*)ptr->ai_addr;
				int ip = g_pDNSCache->getSaddrByHost(pHostName);
				if(ip == FALSE)
					g_pDNSCache->AddRecord(pHostName, psa->sin_addr.s_addr, TRUE);
				InetSocketAddress* pAddr = new InetSocketAddress(port,psa->sin_addr.s_addr);
				freeaddrinfo(result);
				return pAddr;
			}
		}
		freeaddrinfo(result);
	}*/

	g_pDNSCache->AddRecord(pHostName, 0, FALSE);
	return NULL;
}
