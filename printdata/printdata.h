/**
 * 按十六进制打印字符串
 */

#ifndef _PRINTDATA_H
#define _PRINTDATA_H


#ifdef	__cplusplus
extern "C" {
#endif

/**
 * 以十六进制打印
 */
extern void PrintRawData (char *data, int len);

/**
 * 以二进制打印
 */
extern void PrintBinaryData(char *data, int len);

#ifdef	__cplusplus
}
#endif


#endif
