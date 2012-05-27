/********************************************
//�ļ���:base64.h
//����:base64�������
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _BASE64_H_INCLUDED_
#define _BASE64_H_INCLUDED_

#include <string>

namespace comm
{
namespace util
{

/********************************************************
//����:base64����
//����˵��:
//[in] src:Դ����
//[in] src_len:���ݳ���
//[out] dst:���ձ�������ݵĻ�����
//����ֵ:��
*********************************************************/
void base64_encode(const char *src, int src_len, char *dst);

/********************************************************
//����:base64����
//����˵��:
//[in] src:Դ����
//[in] src_len:���ݳ���
//[out] dst:���ս�������ݵĻ�����
//����ֵ:��
*********************************************************/
void base64_decode(const char *src, int src_len, char *dst);

void base64_encode(const char *src, int src_len, std::string &dst);
void base64_decode(const char *src, int src_len, std::string &dst);

inline void base64_encode(const std::string &src, std::string &dst)
{
    base64_encode(src.c_str(), src.size(), dst);
}

inline void base64_decode(const std::string &src, std::string &dst)
{
    base64_decode(src.c_str(), src.size(), dst);
}

}
}


#endif // BASE64_H_INCLUDED
