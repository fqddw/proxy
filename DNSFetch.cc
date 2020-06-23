DNSFetch::DNSFetch()
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	int cflags = fcntl(fd,F_GETFL,0);
	fcntl(fd,F_SETFL, cflags|O_NONBLOCK);

	GetEvent()->SetIOHandler(this);
	GetEvent()->SetFD(fd);
}

int DNSFetch::sendReq(char* url)
{
	int fd = GetEvent()->GetFD();
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
	delete pStream;
	socklen_t lent = sizeof(sai);
	sendto(fd, dnsflat, 512, 0,(struct sockaddr*)&sai, lent);
	return 0;
}

long DNSFetch::getfirstip(char* resp)
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
	while(type != 1)
	{
		int datalen = ntohs(*((unsigned short*)(pkg_start+10)));
		//printf("datalen %d\n", datalen);
		pkg_start = pkg_start+12+datalen;
		type= ntohs(*((unsigned short*)(pkg_start+2)));
		//printf("type %d\n", type);
	}

	long ip = *(int*)(pkg_start+12);
	return ip;
}
Stream* DNSFetch::getURLBuffer(char* url)
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

int DNSFetch::ProcessReceive(Stream* pStream)
{
	char* pData = pStream->GetData();
	int ip = getip(pData);
	return ip;
}

