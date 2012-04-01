/*
 * by xx2012-4-1 8:56:08
 struct _iobuf {
 char *_ptr;
 int   _cnt;
 char *_base;
 int   _flag;
 int   _file;
 int   _charbuf;
 int   _bufsiz;
 char *_tmpfname;
 };
 typedef struct _iobuf FILE;
 */

#include <string.h>
#include <stdio.h>
#include "..\printdata\printdata.h"

#include <stdlib.h>
#include <sys/stat.h>

long FileSize( FILE * fp )
{
    long save_pos;
    long size_of_file;

    save_pos = ftell(fp); /*Save the curent position*/
    fseek(fp, 0L, SEEK_END); /* Jump to the end of the file */
    size_of_file = ftell(fp);
    fseek(fp, save_pos, SEEK_SET); /* Jump back to the original position */

    return size_of_file;

}

int FileSize2(const char * filePath)
{
    struct stat fileStat;
    int fileSize = 0;
    
    if (stat(filePath, &fileStat) != -1) 
    {
        fileSize = fileStat.st_size;
    }
    
    return fileSize;
}

void readFileInfo(FILE *fp)
{

    printf("_ptr=%s\n", fp->_ptr);
    printf("_cnt=%d\n", fp->_cnt);
    printf("_base=%s\n", fp->_base);
    printf("_flag=%d\n", fp->_flag);
    printf("_file=%d\n", fp->_file);
    printf("_charbuf=%d\n", fp->_charbuf);
    printf("_bufsiz=%d\n", fp->_bufsiz);
    printf("_tmpfname=%s\n", fp->_tmpfname);
}
int main(int argc, char **argv)
{
    FILE *fp;
    int buffSize = 0, nFileSize = 0;
    char buff[1024];
    fp = fopen(argv[1], "rb");
    printf("open file:%s\n", argv[1]);
    //fp = fopen("SimpleSection.obj", "rb");
    //fp = fopen("test.txt", "rb");
    if (fp == NULL)
    {
        printf(" open file SimpleSection.obj error\n");
    }
    else
    {
        nFileSize = FileSize(fp);
        printf("file size:[%d] [%d]\n", nFileSize, FileSize2(argv[1]));
        readFileInfo(fp);
        printf("size:%d\n", sizeof(*fp));
        PrintRawData((char *)fp, 32);
        printf("\n-------------------------\n");
        memset(buff, '\0', 1024);
        fread(buff, nFileSize, 1, fp); 
        PrintRawData(buff, nFileSize);
        fclose(fp);
    }
    return 0;
}

