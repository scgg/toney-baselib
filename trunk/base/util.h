/********************************************
//�ļ���:util.h
//����:���ߺ���
//����:�Ӻ���
//����ʱ��:2009.08.14
//�޸ļ�¼:

*********************************************/

#ifndef _COMM_UTIL_H_
#define _COMM_UTIL_H_

#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string>

namespace comm
{
namespace util
{

/*
*����:�ж��ļ����Ƿ����
*����:
*[in] pszDir:�ļ��е�·��
*����ֵ:
*���ļ��д��ڷ���true,����false
*/
bool IsDirExist(const char *pszDir);

/*
*����:�ж��ļ��Ƿ����
*����:
*[in] pszName:�ļ���·��
*����ֵ:
*���ļ����ڷ���true,����false
*��ע:�ļ�������ͨ�ļ���FIFO��Ŀ¼��DEVICE��
*/
bool IsFileExist(const char *pszName);

/*
*����:�����ļ���
*����:
*[in] pszName:���������ļ���
*����ֵ:
*���ļ��д��ڷ���true,����false
*/
bool MkDir(const char *pszName, mode_t mode = 0755);


/*
*����:���ַ���ת��ΪСд
*����:
*[in] s:��ת�����ַ���
*����ֵ:
*ת������ַ���ָ��
*/
char *strlower(char *s);


/*
*����:���ַ���ת��Ϊ��д
*����:
*[in] s:��ת�����ַ���
*����ֵ:
*ת������ַ���ָ��
*/
char *strupper(char *s);

}
}

#endif
