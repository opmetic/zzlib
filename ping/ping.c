//Module Name: Ping.c

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#include "../printdata/printdata.h"

#define IP_RECORD_ROUTE 0x7

//
// IP header structure
//

typedef struct _iphdr
{
	unsigned int h_len:4; // Length of the header
	unsigned int version:4; // Version of IP
	unsigned char tos; //Type of service
	unsigned short total_len; //Total length of the packet
	unsigned short ident; // Unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char ttl; //Time to live
	unsigned char proto; // Protocol (TCP. UDP . etc.)
	unsigned short checksum; // IP checksum

	unsigned int sourceIP;
	unsigned int destIP;
}IpHeader;

#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
#define ICMP_MIN 8 //Minimum 8-byte ICMP packet (header)

// 
// ICMP header structure
//
typedef struct _icmphdr
{
	BYTE i_type; // 1�ֽ�
	BYTE i_code;
	USHORT i_cksum;
	USHORT i_id;
	USHORT i_seq;
	ULONG timestamp;
} IcmpHeader;


//
// IP option header--use with socket option IP_OPTIONS
//
typedef struct _ipoptionhdr
{
	unsigned char code; //Option type
	unsigned char len; //Length of option hdr
	unsigned char ptr; // Offset into options
	unsigned long addr[9]; //List of IP addrs��4�ֽ�
}IpOptionHeader;

#define DEF_PACKET_SIZE 32 // Default packet size
#define MAX_PACKET 1024 // Max ICMP packet size
#define MAX_IP_HDR_SIZE 60 // Max IP header size w/options

BOOL bRecordRoute;
int datasize; //��ping ���ݴ�С
char *lpdest; //��Ŀ¼��ַ�ġ����ַ���ָ��

//
//Function : usage
//
//Description:
//	Print usage information
//
void usage(char *progname)
{
	printf("usage: p -r <host> [data size] \n");
	printf("	-r		record route \n");
	printf("	host	remote machine to Ping \n");
	printf("	datasize	can be up to 1 KB\n");
	ExitProcess(-1);
}

//
// Function : FillICMPdata
//
//Description;
// Helper function to fill in various fields for our ICMP request
//

void FillICMPData(char *icmp_data, int datasize)
{
	IcmpHeader *icmp_hdr = NULL;
	char *datapart = NULL; //

	icmp_hdr = (IcmpHeader*)icmp_data;
	icmp_hdr->i_type = ICMP_ECHO;
	icmp_hdr->i_code = 0;
	icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
	icmp_hdr->i_cksum = 0;
	icmp_hdr->i_seq = 0;

	datapart = icmp_data + sizeof(IcmpHeader); //��ָ������ICMPͷ����Ϣ
	
	//Place some junk in the buffer
	memset(datapart,'E', datasize - sizeof(IcmpHeader));
}

//
// Function: checksum
//
// Description:
//	This function calculatesthe 16-bit one's complement sum
//	of the supplied buffer (ICMP) header
//
/*
unsigned short

cksum (struct ip *ip, int len)

{

        long sum = 0;                                // assume 32 bit long, 16 bit short 

       while ( len >1 ) {

                sum += *((unsigned short *) ip)++;

                if (sum & 8x00000000)       // if high-order bit set, fold 

                        sum = (sum & 0xFFFF) + (sum>> 16) ;

                len -= 2;

        }

        if ( len )                                      // take care of left over byte 

                sum += ( unsigned  short  ) * (unsignedl char *) ip;

        while ( sum >> 16)

                sum =(sum & 0xFFFF) + (sum>> 16);

   return ~sum;
   }
*/

USHORT checksum(USHORT *buffer, int size)
{
	unsigned long cksum = 0;
	
	while(size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size) //�����������ʣ��һ���ֽ�
	{
		cksum += *(UCHAR*)buffer;
	}

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (USHORT)(~cksum);
}

//
// Function: DecodeIPOptions
//
// Description:
//	If the IP option header is present, find the IP options
//	within the IP header and print the record route option
//	values
//
void DecodeIPOptions(char *buf, int bytes)
{
	IpOptionHeader *ipopt = NULL;
	IN_ADDR inaddr;
	int i;
	HOSTENT *host = NULL;

	ipopt = (IpOptionHeader *)(buf + 20);
	printf("RR:	");
	for (i=0; i<(ipopt->ptr / 4) - 1; i++)
	{
		inaddr.S_un.S_addr = ipopt->addr[i];
		if (i != 0)
		{
			printf("	");
		}
		host = gethostbyaddr((char *)&inaddr.S_un.S_addr, sizeof(inaddr.S_un.S_addr), AF_INET);
		if (host)
		{
			printf("%-15s) %s\n", inet_ntoa(inaddr), host->h_name);
		}
		else
		{
			printf("%-15s)\n", inet_ntoa(inaddr));
		}
		
	}
	return ;
}

//
// Function: DecodeICMPHeader
//
// Description:
//		The response is an IP packet. We must decode the IP header to locate the ICMP data;
//
void DecodeICMPHeader(char *buf, int bytes, struct sockaddr_in *from)
{
	IpHeader *iphdr = NULL;
	IcmpHeader *icmphdr = NULL;
	unsigned short iphdrlen;
	DWORD tick;
	static int icmpcount = 0;

	iphdr = (IpHeader *) buf;
	iphdrlen = iphdr->h_len * 4;
	tick = GetTickCount();
	if ((iphdrlen == MAX_IP_HDR_SIZE) && (!icmpcount))
	{
		DecodeIPOptions(buf, bytes);
	}

	if (bytes < iphdrlen + ICMP_MIN)
	{
		printf("Too few byte from %s \n", inet_ntoa(from->sin_addr));
	}
	icmphdr = (IcmpHeader *)(buf + iphdrlen);
	if (icmphdr->i_type != ICMP_ECHOREPLY)
	{
		printf("nonecho type %d recvd \n", icmphdr->i_type);
		return ;
	}

	//Make sure this is an ICMP reply to sometying we sent!
	if (icmphdr->i_id != (USHORT)GetCurrentProcessId())
	{
		printf("someone else's packet!\n");
		return;
	}

	printf("%d bytes from %s:", bytes, inet_ntoa(from->sin_addr));
	printf(" icmp_seq = %d. ", icmphdr->i_seq);
	printf(" time: %d ms", tick - icmphdr->timestamp);
	printf("\n");

	icmpcount++;
	return;
}

void ValidateArgs(int argc, char **argv)
{
	int i;
	bRecordRoute = FALSE;
	lpdest = NULL;
	datasize = DEF_PACKET_SIZE;

	for(i=1; i<argc; i++)
	{
		if ((argv[i][0] == '-') || (argv[i][0] == '/'))
		{
			switch(tolower(argv[i][1]))
			{
			case 'r':
				bRecordRoute = TRUE;
				break;
			default:
				usage(argv[0]);
				break;
			}
		}
		else if (isdigit(argv[i][0])) //�ж� ����argv[i][0] �Ƿ�Ϊ����
		{
			datasize = atoi(argv[i]);
		}
		else
		{
			lpdest = argv[i];
		}
	}
}









int main(int argc, char **argv)
{
	
	WSADATA wsaData;
	SOCKET sockRaw = INVALID_SOCKET; //��ʼΪ�����socket
	struct sockaddr_in dest, //Ŀ���ַ
						from; //Դ��ַ
	int bread, 
		fromlen = sizeof(from), //��ַ��С
		timeout = 1000,  //��ʱʱ��
		ret; //����ֵ
	char *icmp_data = NULL, //icmp����
		*recvbuf = NULL; //�������ݡ�
	unsigned int addr = 0; //int�������ַ
	USHORT seq_no = 0; //2�ֽ� , 16λ
	BOOL flag = TRUE;
	
	/*
	struct hostent {
����char *h_name;                    // ��ַ����ʽ���ơ� 
����char **h_aliases;                // ���ֽ�-��ַ��Ԥ�����Ƶ�ָ�롣
����int h_addrtype;                  // ��ַ����; ͨ����AF_INET
����int h_length;                    // ��ַ�ı��س���
����char **h_addr_list;              // ���ֽ�-���������ַָ�롣�����ֽ�˳��
����#define h_addr h_addr_list[0]    // h_addr_list�еĵ�һ��ַ
����};
	*/
	struct hostent *hp = NULL;
	IpOptionHeader ipopt; //ip�Ŀ�ѡ����

	//printf("%d", sizeof( long ));return 0; /////////////////////////// test 

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup() failed : %d\n", GetLastError());
		return -1;
	}

	ValidateArgs(argc, argv); //��������

	sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sockRaw == INVALID_SOCKET)
	{
		printf("WSASocket() failed: %d\n", WSAGetLastError());
		return -1;
	}

	if (bRecordRoute) // �Ƿ��¼·����Ϣ
	{
		ZeroMemory(&ipopt, sizeof(ipopt));
		ipopt.code = IP_RECORD_ROUTE; // Record route option
		ipopt.ptr = 4; // Point to the first addr offset
		ipopt.len = 39;  // Length of option header

		// ��Ʋ���(Ŀ��socket�������ȼ������������������ݣ�����ռ���ڴ��С);
		//IP_OPTIONS ���Ǽ�¼·�ɵ�ַ
		ret = setsockopt(sockRaw, IPPROTO_IP, IP_OPTIONS, (char *)&ipopt, sizeof(ipopt));
		if (ret == SOCKET_ERROR)
		{
			printf("setsockopt(IP_OPTIONS) failed : %d\n", WSAGetLastError());
		}
	}

	//���ԡ�SO_RCVTIMEO �����׽��������ݽ��ն�Ӧ�ĳ�ʱʱ��ֵ���Ժ���Ϊ��λ��
	/*bread = setsockopt(sockRaw, IPPROTO_IP, IP_HDRINCL, (char *)&flag, sizeof(BOOL));
	
	if (bread == SOCKET_ERROR)
	{
		printf("setsockopt(SO_RCVTIMEO) failed : %d \n", WSAGetLastError());
		return -1;
	}*/

	//SO_RCVTIMEO �����׽��������ݽ��ն�Ӧ�ĳ�ʱʱ��ֵ���Ժ���Ϊ��λ��
	bread = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	
	if (bread == SOCKET_ERROR)
	{
		printf("setsockopt(SO_RCVTIMEO) failed : %d \n", WSAGetLastError());
		return -1;
	}

	timeout = 1000;
	//SO_SNDTIMEO �����׽����ϵ����ݷ��ͳ�ʱʱ�䣨�Ժ���Ϊ��λ��
	bread = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	if (bread == SOCKET_ERROR)
	{
		printf("setsockopt(SO_SNDTIMEO) failed : %d\n", WSAGetLastError());
		return -1;
	}

	memset(&dest, 0, sizeof(dest));
	//Resolve the endpoint's name if necessary
	dest.sin_family = AF_INET; //Э����
	if ((dest.sin_addr.s_addr = inet_addr(lpdest)) == INADDR_NONE) //���ַ�����Ч��Ϊ32λ�����������ֽ����IPV4��ַ,����ΪINADDR_NONE
	{
		if ((hp = gethostbyname(lpdest)) != NULL) //gethostbyname ���ض�Ӧ�ڸ����������İ����������ֺ͵�ַ��Ϣ��hostent�ṹָ��
		{
			memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
			dest.sin_family = hp->h_addrtype;
			printf("dest.sin_addr = %s\n", inet_ntoa(dest.sin_addr));
		}
		else
		{
			printf("gethostbyname() failed; %d\n", WSAGetLastError());
			return -1;
		}
	}
	

	//Create the ICMP packet
	datasize += sizeof(IcmpHeader);
	
	//printf("%d %d datasize=%d", sizeof(BYTE), sizeof(IcmpHeader), datasize);return 0;///////////////////////////////////
	
	icmp_data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_PACKET); //��ָ���Ķ��Ϸ����ڴ�.
	recvbuf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_PACKET);
	if (!icmp_data)
	{
		printf("HeapAlloc() failed: %d\n", GetLastError());
		return -1;
	}

	memset(icmp_data, 0, MAX_PACKET);
	FillICMPData(icmp_data, datasize); //���ICMPͷ����Ϣ�����ѿռ���ȥ

	while(1)
	{
		static int nCount = 0;
		int bwrote;
		if (nCount++ == 1) 
		{
			break;
		}
		((IcmpHeader*)icmp_data)->i_cksum = 0;
		((IcmpHeader*)icmp_data)->timestamp =GetTickCount(); //�Ӳ���ϵͳ������������������elapsed���ĺ�����
		((IcmpHeader*)icmp_data)->i_seq = seq_no ++; //����seq
		((IcmpHeader*)icmp_data)->i_cksum = checksum((USHORT *)icmp_data, datasize); // 16λICMPУ��� datasizeΪ44

		bwrote = sendto(sockRaw, icmp_data, datasize, 0, (struct sockaddr*)&dest, sizeof(dest)); //��������
		printf("send-----------------------------------------\n");
		printf("bread=%d\n", bwrote);
		PrintRawData(icmp_data, bwrote);
		printf("-----------------------------------------send\n");

		if (bwrote == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT)
			{
				printf("timed out\n");
				continue;
			}
			printf("sendto() failed: %d\n", WSAGetLastError());
			return -1;
		}
		if (bwrote < datasize) //����������͵�����С��Ҫ���͵�����
		{
			printf("Wrote %d bytes \n", bwrote);
		}
		
		bread = recvfrom(sockRaw, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&from, &fromlen); //�����ȷ���շ��ؽ��յ����ֽ�����ʧ�ܷ���0. 
		if (bread == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT)
			{
				printf("timed out \n");
				continue;
			}
			printf("recvfrom() FAILED : %d\n", WSAGetLastError());
			return -1;
		}
		printf("recv-----------------------------------------\n");
		printf("bread=%d\n", bread);
		PrintRawData(recvbuf, bread);
		PrintBinaryData(recvbuf, bread);
		printf("-----------------------------------------recv\n");
		DecodeICMPHeader(recvbuf, bread, & from);
		Sleep(1000);
	}
	
	//Cleanup
	if (sockRaw != INVALID_SOCKET)
	{
		closesocket(sockRaw);
	}
	HeapFree(GetProcessHeap(), 0, recvbuf);
	HeapFree(GetProcessHeap(), 0, icmp_data);

	WSACleanup();


	
	return 0;
}