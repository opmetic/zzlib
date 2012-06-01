#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define NUM_PIPES 5

DWORD WINAPI PipeInstanceProc(LPVOID lpParameter);

void main(void)
{
    HANDLE ThreadHandle;
    INT i;
    DWORD ThreadId;

    for (i=0; i<NUM_PIPES; i++)
    {
        if ((ThreadHandle = CreateThread(NULL, 0, PipeInstanceProc, 
                        NULL, 0, &ThreadId)) == NULL)
        {
            printf("CreateThread faild with error %d\n", GetLastError());
            return;
        }
        CloseHandle(ThreadHandle);
    }
    printf("Press a key to stop the Server \n");
    _getch();
}

DWORD WINAPI PipeInstanceProc(LPVOID lpParameter)
{
    HANDLE PipeHandle;
    DWORD BytesRead;
    DWORD BytesWritten;
    CHAR Buffer[256];

    if ((PipeHandle = CreateNamedPipe("\\\\.\\PIPE\\jim",
                    PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
                    NUM_PIPES, 0, 0, 1000, NULL)) == INVALID_HANDLE_VALUE)
    {
        printf("CreateNamedPipe failed with error %d\n", GetLastError());
        return 0;
    }

    while(1)
    {
        if (ConnectNamedPipe(PipeHandle, NULL) == 0)
        {
            printf("ConnectNamePipe failed with error %d\n",
                    GetLastError());
            break;
        }

        while (ReadFile(PipeHandle, Buffer, sizeof(Buffer),
                    &BytesRead, NULL) > 0)
        {
            printf("Echo %d bytes to client:%s\n", BytesRead, Buffer);

            if (WriteFile(PipeHandle, Buffer, BytesRead,
                        &BytesWritten, NULL) == 0)
            {
                printf("WriteFile failed with error %d\n",
                        GetLastError());
                break;
            }
        }

        if (DisconnectNamedPipe(PipeHandle) == 0)
        {
            printf("DisconnectNamePipe failed with error %d\n",
                    GetLastError());
            break;
        }
    }
    CloseHandle(PipeHandle);
    return 0;
}
