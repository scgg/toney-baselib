 /********************************************
//�ļ���:base_socket.h
//����:sockete������װ��TCP�ͻ���ͷ�����
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/

#ifndef _SOCKET_INTERFACE_H_
#define _SOCKET_INTERFACE_H_

#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stropts.h>
#include <netinet/in.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <string>

using std::string;

namespace comm
{
namespace basesock
{

//��ЧSOCKET
const int  INVALID_SOCKET = -1;

/***************************************
*��������:��װSOCKET�����Ļ���
****************************************/
class CBaseSocket
{
public:
    /****************************
    //���캯��
    *****************************/
    CBaseSocket();


    /****************************
    //��������
    *****************************/
    virtual ~CBaseSocket(){Close();}

    /********************************
    //�����ȼ��Ļص���������
    //����˵��:
    //[in] buf:�������ݻ���
    //[in] len:��ͷ�ĳ���
    //����ֵ:
    //0��Ҫ���������ݲ���ȷ����
    //-1:���ݳ���
    //>0:����ȫ��
    ********************************/
    typedef int (*pfHandleInput)(const char * buf, unsigned int len);
	
private:

    /*�������캯��*/
    CBaseSocket(const CBaseSocket &rhs);

    /*��ֵ����*/
    CBaseSocket &operator=(const CBaseSocket &rhs);	

public:

    /**************************************
    //���ܣ�ȡSOCKET������
    //����˵��:��
    //����ֵ:socket��������			
    ****************************************/
    inline int GetFD()const{return m_iSocket;}

    /******************************************
    //����:��ip��ַת��Ϊ������ʽ
    //����˵��:
    //[in] in:IP��ַ
    //[out] buf:�����ʽ��IP��ַ
    //����ֵ:
    //	buf�ĵ�ַ
    *******************************************/
    static char *ip_ntoa(unsigned int in, char *buf)
    {  
        in_addr _in = { in }; 
        return inet_ntoa_r(_in, buf);
    }

    static char *inet_ntoa_r(unsigned int in, char *buf)
    {
        in_addr _in = {in};
        return inet_ntoa_r(_in, buf);
    }
	
    static char *inet_ntoa_r(struct in_addr in, char *buf)
    {
        register char *p;  
        p = (char *)&in;
        #define UC(b)   (((int)b)&0xff)  
        sprintf(buf, "%d.%d.%d.%d", UC(p[0]), UC(p[1]), UC(p[2]), UC(p[3])); 
        return buf;
    }

    /**************************************
    //����:����
    //����˵��:
    //[in] timeout:��ʱʱ��
    //����ֵ:
    //0:�ɹ�,-1ʧ��
    ***************************************/
    int Reconnect(int timeout = -1);


    /******************************************
    //���ܣ��ж��Ƿ���һ�����õ�SOCKET������
    //����ֵ��	
    //	true:����
    //	false:������
    ***********************************************/
    inline bool IsValid(){return m_iSocket != INVALID_SOCKET;}

    /************************************
    //����:ȡ����������
    //����˵��:��
    //����ֵ:
    //������������
    **************************************/
    inline int Type()const {return m_iType;}

    /**************************************
    //���ܣ�ȡ�ϴβ����Ĵ�����
    //����˵��:��
    //����ֵ��	
    //		�����ϴβ����Ĵ�����
    ***************************************/
    inline int GetLastError()const{return errno;}


    /**************************************
    //���ܣ�ȡ�ϴβ����Ĵ�����Ϣ
    //����˵��:	��
    //����ֵ��	
    //	�����ϴβ����Ĵ�����Ϣ
    ***************************************/
    const char *GetLastErrorMsg()const
    {
        return strerror(errno);
    }



    /**************************************
    //���ܣ������µ�TCP SOCKET������
    //����˵��:��
    //����ֵ��	
    //		INVALID_SOCKET:ʧ��
    //		��INVALID_SOCKET:���ɵ�SOCKET������
    ***************************************/
    int CreateTcp(){return Create(SOCK_STREAM);}

    /**************************************
    //���ܣ������µ�UDP SOCKET������
    //����˵��:	��
    //����ֵ��	
    //		INVALID_SOCKET:ʧ��
    //		��INVALID_SOCKET:���ɵ�SOCKET������
    *****************************************/
    int CreateUdp(){return Create(SOCK_DGRAM);}


    /**************************************
    //���ܣ��������µ�SOCKET������
    //����˵����
    //[in] iSocket:�µ�SOCKET������
    //[in] iType:SOCKET������
    //����ֵ��	
    //	��
    *****************************************/
    void Attach(int iSocket, int iType);


    /**************************************
    //���ܣ�ȡ������
    //����˵����
    //		��	
    //����ֵ��	
    //		��
    *****************************************/
    inline void Dettach()
    {
        m_iSocket = INVALID_SOCKET;
        m_iType = -1;
        memset(&m_PeerAddr, 0, sizeof(m_PeerAddr));
        memset(&m_SockAddr,0, sizeof(m_SockAddr));
    }

    /**************************************
    //����:����Զ˵�ַ
    //����˵��:��
    //����ֵ:
    //�ɹ�:0,ʧ��-1,����������ͨ��GetLastError()��ȡ
    *****************************************/
    int SavePeerAddr();

    /******************************************
    //����:���汾�˵�ַ
    //����˵��:��
    //����ֵ:
    //�ɹ�:0,ʧ��-1,����������ͨ��GetLastError()��ȡ
    *******************************************/
    int SaveSockAddr();

    /*****************************************
    //����:�������˵�ַ
    //����˵��:��
    //����ֵ:
    //�ɹ�:0,ʧ��-1,����������ͨ��GetLastError()��ȡ
    ******************************************/
    int SaveAddr();


    /*****************************************
    //���ܣ����ӵ�SERVER
    //����˵��:
    //[in] szServerIP:������IP
    //[in] uServerPort���������˿�
    //[in] timeout_usec: ���ӳ�ʱ�ĺ�����(1��=1000���룩
                -1������ʽ����
                0:��������ʱ�������أ����ȴ�
                 >0����������ʱ�ȴ�����ʱ

    //����ֵ��	
    //0:�ɹ�,-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    **********************************************/
    int Connect(const char *szServerIP, unsigned short iServerPort, int timeout_usec = -1);



    /**********************************************
    //���ܣ��ر�SOCKET������
    //����˵��:	��
    //����ֵ��	
    //	0:�ɹ�,-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    *************************************************/
    int Close();

    /**********************************************
    //���ܣ��ر�SOCKET������
    //����˵��:��	
    //����ֵ��	
    //0:�ɹ�
    //-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    *************************************************/
    int Shutdown(int how = SHUT_RDWR);

    /**********************************************
    //���ܣ���������
    //����˵��:
    //[out] chBuffer:���ջ�����
    //[in] nSize:�������ĳ���
    //[in] timeout: ��ʱ�ĺ�����(1��=1000���룩
            -1������ʽ����
            0:������ʱ�������أ����ȴ�
            >0��������ʱ�ȴ�����ʱ
    //����ֵ��	
        -1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
        0:�Է��ѹر�
        >0:�յ������ݳ���
    *************************************************/
    int Recv(char *chBuffer, unsigned int nSize, int timeout = -1, int flags = 0);


    /**********************************************
    //���ܣ���������
    //����˵��:
    //[in] chBuff�������͵�����
    //[in] nSize:�������ĳ���
    //[in] timeout: ��ʱ�ĺ�����(1��=1000���룩
        -1������ʽ����
        0:���ɷ�ʱ�������أ����ȴ�
        >0�����ɷ�ʱ�ȴ�����ʱ
    //[in] flag:���ͱ�־	
    //����ֵ��	
        -1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
        >=0:�ѷ��͵����ݳ���
    *************************************************/
    int Send(const void* chBuff, unsigned int nSize, int timeout = -1, int flags = 0);

    /**********************************************
    //���ܣ�������(һֱ����nBytes�ֽ�Ϊֹ�����߳���Ϊֹ�����߳�ʱΪֹ��
    //����˵��:
    //[out] nread:�������յ����ֽ���
    //[out] chBuffer:�������а������յ�������
    //[out] nBytes:�������ĳ���
    //[in] timeout: ��ʱ�ĺ�����(1��=1000���룩
            -1������ʽ����
            0:������ʱ�������أ����ȴ�
            >0��������ʱ�ȴ�����ʱ
    //[in] flags:��־
    //����ֵ��	
        -1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
        0:�Է��ѹر�
        >0:�յ������ݳ���
    *************************************************/
    int RecvN(unsigned int &nread, void *pBuffer, unsigned int nBytes,int timeout = -1, int flags = 0);

    int ReadN(unsigned int &nread, void *vptr, unsigned int n);

    /**********************************************
    //���ܣ���һ���� 
    //����˵��:
    //[in]  pf:�����ȼ��ĺ���
    			(����ֵ:0��Ҫ���������ݲ���ȷ����
    				-1:���ݳ���
    				>0:����ȫ��)
    		unsigned int nBytes:�������ĳ���		
    		int timeout: ��ʱ�ĺ�����(1��=1000���룩
    		 -1������ʽ����
    		 0:������ʱ�������أ����ȴ�
    		 >0��������ʱ�ȴ�����ʱ
    		int flags:��־
    *���������
    		unsigned int &nRead:�������յ����ֽ���
    		char *pBuffer:�������а������յ�������

    ����ֵ: >0:read ok, pack size is return, actually read size is nread, nread>=����ֵ
    0:closed by peer
    -1:error
    -2:timeout
    -3:data invalid
    *************************************************/
    int RecvPack(pfHandleInput pf, unsigned int &nread, void *pBuffer, unsigned int nBytes,int timeout = -1, int flags = 0);


    /*****************************************************************
    //���ܣ���һ���� 
    //����˵��:
    //[in] pf:�����ȼ��ĺ���
    			(����ֵ:0��Ҫ���������ݲ���ȷ����
    				-1:���ݳ���
    				>0:����ȫ��)
    //[in] hlen:��ͷ�ĳ���
    //[out] nRead:�������յ����ֽ���
    //[out] pBuffer:�������а������յ�������
    //[in] nBytes:�������ĳ���		
    //[in] timeout: ��ʱ�ĺ�����(1��=1000���룩
    		 -1������ʽ����
    		 0:������ʱ�������أ����ȴ�
    		 >0��������ʱ�ȴ�����ʱ
    //[in] flags:��־

    ����ֵ: >0:read ok, pack size is return, actually read size is nread, nread>=����ֵ
    0:closed by peer
    -1:error
    -2:timeout
    -3:data invalid
    ***********************************************************************/
    int RecvPack(pfHandleInput pf, unsigned int hlen, unsigned int &nread, void *pBuffer, unsigned int nBytes,int timeout = -1, int flags = 0);


    /*****************************************************************
    //���ܣ�������(һֱ����nBytes�ֽ�Ϊֹ�����߳���Ϊֹ�����߳�ʱΪֹ��
    //���������
    //[out] nwrite:�����ѷ��͵��ֽ���
    //[in] pBuffer:�����͵Ļ�����
    //[in] nBytes:�������ĳ���
    //[in] timeout: ��ʱ�ĺ�����(1��=1000���룩
    		 -1������ʽ����
    		 0:���ɷ�ʱ�������أ����ȴ�
    		 >0�����ɷ�ʱ�ȴ�����ʱ
    //[in] flags:��־
    //����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�Է��ѹر�
    		>0:�յ������ݳ���
    ***********************************************************************/
    int SendN(unsigned int &nwrite, const void *pBuffer, unsigned int nBytes,int timeout = -1, int flags = 0);

    int WriteN(unsigned int &nwrite, const void *pBuffer, unsigned int nBytes);



    /*****************************************************************
    //���ܣ��󶨱��ص�ַ
    //����˵��:
    //[in] pszBindIP ���󶨵�IP
    //[in] iBindPort���󶨵Ķ˿�
    //����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɹ�
    ***********************************************************************/
    int Bind(const char *pszBindIP, const unsigned short iBindPort);

    /*****************************************************************
    //���ܣ�ȡSOCKETѡ��
    //����˵��:
    //[in] level:����
    //[in] optname:ѡ������
    //[out] optval:ѡ��ֵ
    //[out] optlen:ѡ��ֵ�ĳ���
    //����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɹ�
    ***********************************************************************/
    int GetSockOpt(int level, int optname, void *optval, socklen_t *optlen);

    /*****************************************************************
    //���ܣ�����SOCKETѡ��
    //����˵��:
    //	[in]  level:����
    //	[in] optname:ѡ������
    //	[in] optval:ѡ��ֵ
    //	[in] optlen:ѡ��ֵ�ĳ���
    //����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɹ�
    ***********************************************************************/
    int SetSockOpt(int level, int optname, const void *optval, socklen_t optlen);



    /*****************************************************************
    *���ܣ�
    		ȡSOCKET�ı��˵�ַ
    *���������
    		socklen_t *namelen:��ʼ��Ϊname�ṹ��Ĵ�С
    *���������
    		struct sockaddr *name�����صı��˵�ַ
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɹ�
    ***********************************************************************/
    int GetSockName(struct sockaddr *name, socklen_t *namelen);


    /*****************************************************************
    *���ܣ�
    		ȡ���˵�IP��ַ
    *���������
    		��
    *���������
    		sring &strHost�����صı���IP��ַ
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɹ�
    ***********************************************************************/
    int GetSockIP(string &strHost)
    {
        if(SaveSockAddr() != 0)
        {
            return -1;
        }
        char buf[32] = {0};
        strHost = inet_ntoa_r(m_SockAddr.sin_addr, buf);
        return 0;
    }
    
    /***********************************************************************
    //����:��ȡ����IP��ַ
    //����˵��:
    //[in] ip:����IP��ַ�Ļ�����
    //[in] len:�������ĳ���
    //����ֵ:0,�ɹ���-1ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    ***********************************************************************/
    int GetSockIP(char * ip, int len)
    {
        string s;

        GetSockIP(s);

        strncpy(ip, s.c_str(), len);

        return 0;
    }

    /***********************************************************************
    *���ܣ�ȡ���˵Ķ� ��
    *���������
        ��
    *���������
        ��
    *����ֵ��	
        -1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
        >=0:���˵Ķ˿ں�
    ***********************************************************************/
    int GetSockPort()
    {
        if( 0 != SaveSockAddr())
        {
            return -1;
        }
        return ntohs(m_SockAddr.sin_port);
    }



    /***********************************************************************
    *���ܣ�
    		ȡSOCKET�ĶԶ˵�ַ
    *���������
    		socklen_t *namelen:��ʼ��Ϊname�ṹ��Ĵ�С
    *���������
    		struct sockaddr *name�����صı��˵�ַ
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɹ�
    ***********************************************************************/
    int GetPeerName(struct sockaddr *name, socklen_t *namelen);

    /***********************************************************************
    *���ܣ�
    		ȡ�Զ˵�IP��ַ
    *���������
    		��
    *���������
    		sring &strHost�����صĶԶ�IP��ַ
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɹ�
    ***********************************************************************/
    int GetPeerIP(string &strHost)
    {
        if( SavePeerAddr() != 0)
        {
            return -1;
        }
        char buf[32] = {0};
        strHost = inet_ntoa_r(m_PeerAddr.sin_addr, buf);
        return 0;
    }

    int GetPeerIP(char * ip, int len)
    {
        string s;

        GetPeerIP(s);

        strncpy(ip, s.c_str(), len);

        return 0;
    }

    /***********************************************************************
    *���ܣ�
    		ȡ�Զ˵Ķ� ��
    *���������
    		��
    *���������
    		��
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		>��0:�Զ˵Ķ˿ں�
    ***********************************************************************/
    int GetPeerPort()
    {
        if( 0 != SaveSockAddr())
        {
            return -1;
        }
        return ntohs(m_PeerAddr.sin_port);
    }

    /***********************************************************************
    *���ܣ�
    		���÷�����ѡ��
    *���������
    		bool flag:�Ƿ�Ϊ������
    			true:��
    			false:����
    *���������
    		��
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɹ�
    ***********************************************************************/
    int SetNonBlockOption(bool flag = true);


    bool SetReuseAddr(int on = 1)
    {
        return setsockopt(m_iSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(int)) != -1;
    }


    /***********************************************************************
    *���ܣ�
    		�ȴ��ɶ�
    *���������
    		int timeout: ��ʱ�ĺ�����(1��=1000���룩
    		 -1:һֱ�ȴ�
    		 0:���ȴ�
    		 >0�����ȴ�timeout����
    *���������
    		��
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:�ɶ�
    ***********************************************************************/
    int WaitRead(int timeout = -1);

    /***********************************************************************
    *���ܣ�
    		�ȴ���д
    *���������
    		int timeout: ��ʱ�ĺ�����(1��=1000���룩
    		 -1:һֱ�ȴ�
    		 0:���ȴ�
    		 >0�����ȴ�timeout����
    *���������
    		��
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		0:��д
    ***********************************************************************/
    int WaitWrite(int timeout = -1);
	
    /***********************************************************************
    *���ܣ�
    		�ȴ���д��ɶ�
    *���������
    		int timeout: ��ʱ�ĺ�����(1��=1000���룩
    		 -1:һֱ�ȴ�
    		 0:���ȴ�
    		 >0�����ȴ�timeout����
    *���������
    		��
    *����ֵ��	
    		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    		1:��д
    		2:�ɶ�
    		3:����д���ֿɶ�
    ***********************************************************************/
    int WaitRdWr(int timeout = -1);	

    int SendRecvWithRetry(
        const char *req,
        unsigned int req_len, 
        pfHandleInput pf,
        unsigned int hlen,    		
        char *rsp, 
        unsigned int rsp_len,  
        int timeout = -1,
        int flags = 0)
    {
        int n = SendRecv(req, req_len, pf, hlen, rsp, rsp_len, timeout, flags) ;
        if( n <= 0)
        {
            if( Reconnect(timeout) != 0)
            {
                return n;
            }
            n = SendRecv(req, req_len, pf, hlen, rsp, rsp_len, timeout, flags); 
        }
        return n;
    }

    /*
    *	-1:recv error
    	-2:recv timeout
    	-3:recv invalid data
    	-4:send error
    	-5:send timeout		
    	0:closed
    	>0:ok, ret the pack len.
    */
    int SendRecv(
    	const char *req,
    		unsigned int req_len, 
    	pfHandleInput pf,
    	unsigned int hlen,
    		char *rsp, 
    		unsigned int rsp_len,
    		int timeout = -1,
    		int flags = 0);
	
	
protected:
    /*
    *���ܣ�
        �����µ�SOCKET������
    *���������
        int nSocketType:SOCKET���ͣ�SOCK_DGRAM:UDP,SOCK_STREAM:TCP)
    *���������
    		��
    *����ֵ��	
            INVALID_SOCKET:ʧ��
            ��INVALID_SOCKET:���ɵ�SOCKET������
    */
    int Create(int nSocketType);
    int Connect(struct sockaddr_in *addr, int timeout_usec =-1);


    /*
    *���ܣ�
        ���ӵ�SERVER
    *���������
    *   const struct sockaddr *serv_addr:��������ַ
        socklen_t addrlen����ַ�ṹ�ĳ���
        int timeout_usec: ���ӳ�ʱ�ĺ�����(1��=1000���룩
            -1������ʽ����
            0:��������ʱ�������أ����ȴ�
            >0����������ʱ�ȴ�����ʱ

    *���������
            ��
    *����ֵ��	
        0:�ɹ�
        -1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
    */

    int  Connect(unsigned int ip, unsigned short port,int timeout_usec = -1);


    int m_iSocket;	//SOCKET������
    int m_iType; 	//SOCKET����
    string m_strPeerAddr;//�Զ˵�ַ 
    int m_iPeerPort;//�Զ˶˿�

    struct sockaddr_in m_SockAddr;	//���˵�ַ
    struct sockaddr_in m_PeerAddr;	//�Զ˵�ַ

};


class CUdpSocket : public CBaseSocket
{
public:
        CUdpSocket()
        {
            CreateUdp();            
        }

        CUdpSocket(const char *host, unsigned short port, int timeout = -1)
        {
            if( INVALID_SOCKET != CreateUdp())
            {
                if(0 != Connect(host, port, timeout))
                {
                    Close();
                }
            }
        }

        int SendPack(void *pBuffer, unsigned int nBytes,int iFlags /*= 0*/)
        {
            socklen_t addr_len = sizeof(m_PeerAddr);
            int bytes = sendto(m_iSocket, pBuffer, nBytes,iFlags, (struct sockaddr *) &m_PeerAddr, addr_len );
            if(bytes < 0 )
            {
                return errno ? -errno:bytes;
            }
            else
            {
                return bytes;
            }
        }

        int RecvPack(void * pBuffer,unsigned int nBytes,int iFlags /*= 0*/)
        {            
            socklen_t addr_len = sizeof(m_PeerAddr);
            int bytes =::recvfrom(m_iSocket, pBuffer, nBytes, iFlags, (struct sockaddr *) &m_PeerAddr, &addr_len ) ;
            if(bytes < 0)
            {
                return errno ? -errno : bytes;
            }
            else
            {
                return bytes;
            }
        }
    
    /*
    *���ܣ�
        �����ݣ���Ҫ����UDP��
    *���������
        void *pBuffer:���ջ�����
        unsigned int nBytes:���ջ������Ĵ�С
        int iFlags:��־
    *���������
        void *pBuffer:���ջ�����
        struct sockaddr *pFromAddr:�Է��ĵ�ַ 
        socklen_t* iAddrLen����ַ�ĳ���
    *����ֵ��	
        -1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
        0:�Է��ѹر�
        >0:�յ������ݳ���
    */
    int Recvfrom(void *pBuffer, unsigned int nBytes, struct sockaddr *pFromAddr, socklen_t* iAddrLen, int iFlags /*= 0*/)
    {
        return ::recvfrom(m_iSocket, pBuffer, nBytes,iFlags, pFromAddr, iAddrLen);
    }

    /*
    *���ܣ�
        �����ݣ���Ҫ����UDP��
    *���������
        void *pBuffer�����ͻ�����
        unsigned int nBytes:���ͻ������Ĵ�С
        int iFlags:��־
        struct sockaddr *pFromAddr:�Է��ĵ�ַ 
        socklen_t iAddrLen����ַ�ĳ���

    *���������
        ��
    *����ֵ��	
        -1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
        >=0:�ѷ��͵����ݳ���
    */
    int Sendto(void *pBuffer, unsigned int nBytes, struct sockaddr *pToAddr, socklen_t iAddrLen,int iFlags /*= 0*/ )
    {
        return sendto(m_iSocket, pBuffer, nBytes,iFlags, pToAddr, iAddrLen);
    }


};


class CTcpClient : public CBaseSocket
{
public:
    //client side
    CTcpClient(const char *host, unsigned short port, int timeout = -1)
    {
        if( INVALID_SOCKET != CreateTcp())
        {
            if(0 != Connect(host, port, timeout))
            {
                Close();
            }
        }

    }

    //server side
    CTcpClient(int fd) 
    {
        Attach(fd, SOCK_STREAM);
    }


};

}
}


#endif // _CSOCKET_H_


