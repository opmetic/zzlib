#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_COUNT 20
#define DEFAULT_PORT 5150
#define DEFAULT_BUFFER 2048
#define DEFAULT_MESSAGE "This is a test of the emergency \
broadcasting system"

char szServer[128],
    szMessage[1024];
int iPort = DEFAULT_PORT;
DWORD dwCount = DEFAULT_COUNT;
BOOL bSendOnly = FALSE;

void usage()
{
    printf("usage: client [-p:x] [-s:IP] [-n:x] [-o]\n\n");
    printf("        -p:x    Remote port to send to \n");
    printf("        -s:IP   Server's IP address or host name \n");
    printf("        -n:x    Number of times to send message\n");
    printf("        -o      Send message only; don't recvive\n");
    ExitProcess(1);
}

void ValidateArgs( int argc, char **argv)
{
    int i;
    for(i=1; i<argc; i++)
    {  
        if ((argv[i][0] == '-' || argv[i][0] == '/'))
        {
            switch ( tolower(argv[i][1]))
            {
                case 'p':
                    if (strlen(argv[i]) > 3)
                    {
                        iPort = atoi(&argv[i][3]);
                    }
                    break;
                case 's':
                    if (strlen(argv[i]) > 3)
                    {
                        strcpy(szServer, &argv[i][3]);
                    }
                    break;
                case 'n':
                    if (strlen(argv[i]) > 3)
                    {
                        dwCount = atol(&argv[i][3]);
                    }
                    break;
                case 'o':
                    bSendOnly = TRUE;
                    break;
                default:
                    usage();
                    break;
            }
        }
    }
}




int main(int argc, char **argv)
{
    WSADATA wsd;
    SOCKET sClient;
    char szBuffer[DEFAULT_BUFFER];
    int ret, i;

    struct sockaddr_in server;
    struct hostent *host = NULL;

    ValidateArgs(argc, argv);
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        printf("Failed to load winsock library!\n");
        return 1;
    }
    strcpy(szMessage, DEFAULT_MESSAGE);
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sClient == INVALID_SOCKET)
    {
        printf("socket() failed : %d\n", WSAGetLastError());
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(iPort);
    server.sin_addr.s_addr = inet_addr(szServer);

    if ( server.sin_addr.s_addr == INADDR_NONE)
    {
        host = gethostbyname(szServer);
        if (host == NULL)
        {
            printf("Unable to resolve server : %s\n", szServer);
            return 1;
        }

        CopyMemory(&server.sin_addr, host->h_addr_list[0], host->h_length);
    }

    if (connect(sClient, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("connect() failed : %d\n", WSAGetLastError());
        return 1;
    }

    for (i=0; i<dwCount; i++)
    {
        ret = send(sClient, szMessage, strlen(szMessage), 0);
        if (ret == 0)
        {
            break;
        }
        else if (ret == SOCKET_ERROR)
        {
            printf("send() failed: %d\n", WSAGetLastError());
            break;
        }
        printf("Send %d bytes\n", ret);
        if (!bSendOnly)
        {
            ret = recv(sClient, szBuffer, DEFAULT_BUFFER, 0);
            if (ret == 0)
            {
                break;
            }
            else if (ret == SOCKET_ERROR)
            {
                printf("recv() failed: %d\n", WSAGetLastError());
                break;
            }
            szBuffer[ret] = '\0';
            printf("RECV [%d bytes]: %s\n", ret, szBuffer);
        }
    }
    closesocket(sClient);

    WSACleanup();
    return 0;
}
