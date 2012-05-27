

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#include "base_socket.h"

namespace comm
{
namespace basesock
{

inline int DiffMillSec(const struct timeval &t1, const struct timeval &t2)
{
    return (t1.tv_sec * 1000 - t2.tv_sec * 1000) + (t1.tv_usec / 1000 - t2.tv_usec/1000); 
}

inline int DiffMillSec(const struct timeval &t)
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return DiffMillSec(tv, t);
}

inline void MkTimeval(struct timeval &tv, int millisec)
{
    if(millisec > 0)
    {
        tv.tv_sec = millisec / 1000;
        tv.tv_usec = (millisec % 1000)*1000;
    }
    else
    {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
    }
}

/*
*���SOCKET�����Ļ���
*/

CBaseSocket::CBaseSocket()
{
    m_iSocket = 	INVALID_SOCKET;
    m_iType = -1;
    memset(&m_PeerAddr,0,sizeof(m_PeerAddr));
    memset(&m_SockAddr,0,sizeof(m_SockAddr));
}
	





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
int CBaseSocket::Create(int nSocketType)
{
    m_iType = nSocketType;
    if( (m_iType != SOCK_STREAM) && (m_iType != SOCK_DGRAM))
    {
        return INVALID_SOCKET;
    }
    m_iSocket = socket(AF_INET, m_iType, 0);
    return m_iSocket;
}



/*
*���ܣ�
		�������µ�SOCKET������
*���������
		int iSocket:�µ�SOCKET������
*���������
		��
*����ֵ��	
		��
*/
void CBaseSocket::Attach(int iSocket, int iType)
{
    if(IsValid())
    {
        Close();
    }
    m_iSocket = iSocket;	
    m_iType = iType;
    memset(&m_PeerAddr, 0, sizeof(m_PeerAddr));
    memset(&m_SockAddr, 0, sizeof(m_SockAddr));
    SaveAddr();
}

/*
*����Զ˵�ַ
*/
int CBaseSocket::SavePeerAddr()
{
    socklen_t len = sizeof(m_PeerAddr);
    int n =  getpeername(m_iSocket, (struct sockaddr *)&m_PeerAddr, &len);
    char buf[32] = {0};
    inet_ntoa_r(m_PeerAddr.sin_addr, buf);
    return n;
}

/*
*���汾�˵�ַ
*/
int CBaseSocket::SaveSockAddr()
{
    socklen_t len = sizeof(m_SockAddr);
    int n =  getsockname(m_iSocket, (struct sockaddr *)&m_SockAddr, &len);
    char buf[32] = {0};
    inet_ntoa_r(m_SockAddr.sin_addr, buf);
    return n;
}

/*
*�������˵�ַ
*/
int CBaseSocket::SaveAddr()
{
    SaveSockAddr();
    return SavePeerAddr();
}





/*
*���ܣ�
		�ر�SOCKET������
*���������
		��
*���������
		��
*����ֵ��	
		0:�ɹ�
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
*/
int CBaseSocket::Close()
{
    if(IsValid())
    {
        close(m_iSocket);
        m_iSocket = INVALID_SOCKET;
    }
    return 0;
}

/*
*���ܣ�
		�ر�SOCKET������
*���������
		��
*���������
		��
*����ֵ��	
		0:�ɹ�
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
*/
int CBaseSocket::Shutdown(int how /* = SHUT_RDWR */)
{
    if(IsValid())
    {
        shutdown(m_iSocket, how);
    }
    return 0;
}


/*
*���ܣ�
		������
*���������
		unsigned int nSize:�������ĳ���
		int timeout: ��ʱ�ĺ�����(1��=1000���룩
		 -1������ʽ����
		 0:������ʱ�������أ����ȴ�
		 >0��������ʱ�ȴ�����ʱ
*���������
		char *chBuffer:������
*����ֵ��	
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�Է��ѹر�
		>0:�յ������ݳ���
*/
int CBaseSocket::Recv(char *chBuffer, unsigned int nSize, int timeout /*= -1*/, int flags /*= 0*/)
{
    if(timeout < 0)
    {
        return recv(m_iSocket, chBuffer, nSize, flags);
    }
    int n = WaitRead( timeout );
    if(n != 0)
    {
        return n;
    }
    return recv(m_iSocket, chBuffer, nSize, flags);
}


/*
*���ܣ�
		������
*���������
		const void *chBuff�������͵�����
		unsigned int nSize:�������ĳ���
		int timeout: ��ʱ�ĺ�����(1��=1000���룩
		 -1������ʽ����
		 0:���ɷ�ʱ�������أ����ȴ�
		 >0�����ɷ�ʱ�ȴ�����ʱ
		int flag:���ͱ�־
*���������
		��
*����ֵ��	
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		>=0:�ѷ��͵����ݳ���
*/
int CBaseSocket::Send(const void* chBuff, unsigned int nSize, int timeout /*= -1*/, int flags /*= 0*/)
{
    if(timeout < 0)
    {
        return send(m_iSocket, chBuff, nSize, flags);
    }
    int n = WaitWrite( timeout );
    if(n != 0)
    {
        return n;
    }
    return send(m_iSocket, chBuff, nSize, flags);
}


/*
*���ܣ�
		�󶨱��ص�ַ
*���������
		const char *pszBindIP, const ���󶨵�IP
		unsigned short iBindPort	���󶨵Ķ˿�
*���������
		��
*����ֵ��	
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�ɹ�
*/
int CBaseSocket::Bind(const char *pszBindIP, const unsigned short iBindPort)
{
    struct sockaddr_in inaddr;	
    bzero (&inaddr, sizeof (struct sockaddr_in));
    inaddr.sin_family = AF_INET;		//ipv4Э����	
    inaddr.sin_port = htons (iBindPort);	
    if(pszBindIP== 0 || pszBindIP[0] == 0 || strcmp(pszBindIP, "0.0.0.0") == 0)
    {
        inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        if (inet_pton(AF_INET, pszBindIP, &inaddr.sin_addr) <= 0) //���ʮ����ip��ַתΪ�����Ƶ�ַ	
        {		
            return -1;	
        }
    }
    int n = bind(m_iSocket, (struct sockaddr*)&inaddr, sizeof(struct sockaddr));
    if( n == 0)
    {
        SaveSockAddr();
    }
    return n;
}


/*
*���ܣ�
		ȡSOCKETѡ��
*���������
		int level:����
		int optname:ѡ������
*���������
		void *optval:ѡ��ֵ
		socklen_t *optlen:ѡ��ֵ�ĳ���
*����ֵ��	
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�ɹ�
*/
int CBaseSocket::GetSockOpt(int level, int optname, void *optval, socklen_t *optlen)
{
    return getsockopt(m_iSocket, level, optname, optval, optlen);
}
/*
*���ܣ�
		����SOCKETѡ��
*���������
		int level:����
		int optname:ѡ������
		void *optval:ѡ��ֵ
		socklen_t *optlen:ѡ��ֵ�ĳ���

*���������
		��
*����ֵ��	
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�ɹ�
*/
int CBaseSocket::SetSockOpt(int level, int optname, const void *optval, socklen_t optlen)
{
    return setsockopt(m_iSocket, level, optname, optval, optlen);
}



/*
*���ܣ�
		ȡSOCKET�ı��˵�ַ
*���������
		socklen_t *namelen:��ʼ��Ϊname�ṹ��Ĵ�С
*���������
		struct sockaddr *name�����صı��˵�ַ
*����ֵ��	
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�ɹ�
*/
int CBaseSocket::GetSockName(struct sockaddr *name, socklen_t *namelen)
{
    if(0 != SaveSockAddr())
    {
        return -1;
    }
    memcpy(name, &m_SockAddr, sizeof(m_SockAddr));
    *namelen = sizeof(m_SockAddr); 
    return 0;
}



/*
*���ܣ�
		ȡSOCKET�ĶԶ˵�ַ
*���������
		socklen_t *namelen:��ʼ��Ϊname�ṹ��Ĵ�С
*���������
		struct sockaddr *name�����صı��˵�ַ
*����ֵ��	
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�ɹ�
*/
int CBaseSocket::GetPeerName(struct sockaddr *name, socklen_t *namelen)
{
    if(0 != SavePeerAddr())
    {
        return -1;
    }
    memcpy(name, &m_PeerAddr, sizeof(m_PeerAddr));
    *namelen = sizeof(m_PeerAddr); 
    return 0;

}

/*
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
*/
int CBaseSocket::SetNonBlockOption(bool flag /*= true*/)
{
    int save_mode;
    save_mode = fcntl( m_iSocket, F_GETFL, 0 );
    if (flag)
    { // set nonblock
        save_mode |= O_NONBLOCK;
    }
    else 
    { // set block
        save_mode &= (~O_NONBLOCK);
    }
    
    fcntl( m_iSocket, F_SETFL, save_mode );
    return 0;
}


int CBaseSocket::Reconnect(int timeout /*=-1*/)
{
    Close();
    if(m_iType == SOCK_STREAM)
    {
        CreateTcp();
    }
    else
    {
        CreateUdp();
    }
    if(!IsValid())
    {
        return -1;
    }

    if( 0 != Connect(&m_PeerAddr, timeout))
    {
        //Close();
        return -1;
    }
    return 0;
}






/*
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
            -2:timeout
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�ɶ�
*/
int CBaseSocket::WaitRead(int timeout /*= -1*/)
{
    if(m_iSocket < 0)
    {
        return -1;
    }
    struct timeval tv;
    struct timeval*ptv = &tv;
    if(timeout < 0)
    {
        ptv = NULL;
    }
    else
    {
        MkTimeval(tv, timeout);
    }
    
    fd_set recv_fds;
    int iNum= 0;

    FD_ZERO( &recv_fds );
    FD_SET( m_iSocket, &recv_fds );

    iNum= select( m_iSocket+1, &recv_fds, NULL, NULL, ptv );
    if(iNum == 1)
    {
        return 0;
    }
    if( iNum == 0)
    {
        return -2;
    }
    return -1;
}


/*
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
            -2:timeout
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:��д
*/
int CBaseSocket::WaitWrite(int timeout /*= -1*/)
{
    if(m_iSocket < 0)
    {
        return -1;
    }
    struct timeval tv;
    struct timeval*ptv = &tv;
    if(timeout < 0)
    {
        ptv = NULL;
    }
    else
    {
        MkTimeval(tv, timeout);
    }

    fd_set wfds;
    int iNum= 0;

    FD_ZERO( &wfds );
    FD_SET( m_iSocket, &wfds );
    iNum= select( m_iSocket+1, NULL, &wfds, NULL, ptv );
    if(iNum == 1)
    {
        return 0;
    }
    
    if( iNum == 0)
    {
        return -2;
    }
    return -1;
}



/*
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
            -2:timeout
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		1:��д
		2:�ɶ�
		3:����д���ֿɶ�
*/
int CBaseSocket::WaitRdWr(int timeout /*= -1*/)
{
    if(m_iSocket < 0)
    {
        return -1;
    }
    struct timeval tv;
    struct timeval *ptv = &tv;
    if(timeout < 0)
    {
        ptv = NULL;
    }
    else
    {
        MkTimeval(tv, timeout);
    }

    fd_set recv_fds;
    int iNum= 0;
    fd_set wfds;

    FD_ZERO( &recv_fds );
    FD_SET( m_iSocket, &recv_fds );
    FD_ZERO( &wfds );
    FD_SET( m_iSocket, &wfds );

    iNum= select( m_iSocket+1, &recv_fds, &wfds, NULL, ptv );
    if(iNum == 1)
    {
        if(FD_ISSET(m_iSocket, &recv_fds))
        {
            return 2;
        }
        return 1;
    }
    
    if(iNum == 2)
    {
        return 3;
    }
    
    if(iNum == 0)
    {
        return -2;
    }
    return -1;
}





/*
*���ܣ�
		������(һֱ����nBytes�ֽ�Ϊֹ�����߳���Ϊֹ�����߳�ʱΪֹ��
*���������
		const void *pBuffer:�����͵Ļ�����
		unsigned int &nBytes:�������ĳ���
		int timeout: ��ʱ�ĺ�����(1��=1000���룩
		 -1������ʽ����
		 0:���ɷ�ʱ�������أ����ȴ�
		 >0�����ɷ�ʱ�ȴ�����ʱ
		int flags:��־
*���������
		unsigned int &nBytes:�����ѷ��͵��ֽ���
*����ֵ��
            -2:timeout
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�Է��ѹر�
		>0:�յ������ݳ���
*/
int CBaseSocket::SendN(unsigned int &nwrite, const void *pBuffer, unsigned int nBytes,int timeout /*= -1*/, int flags /*= 0*/)
{
    if(m_iSocket < 0)
    {
        return -1;
    }
    nwrite = 0;
    if(timeout < 0)
    {
        return WriteN(nwrite, pBuffer, nBytes);
    }

    struct timeval start;
    struct timezone tz;
    gettimeofday(&start, &tz);
    unsigned int left = nBytes;
    int n = 0;
    char *ptr = (char *)pBuffer;
    while(1)
    {
        n = WaitWrite(timeout);
        if(n < 0)	//error
        {
            return n;
        }
        n = send(m_iSocket, ptr, left, flags);
        if( n < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            return n;
        }
        
        if(n == 0)
        {
            return 0;
        }
        
        left -= n;
        ptr += n;
        nwrite += n;
        if(left <= 0)
        {
            break;
        }

        timeout -= DiffMillSec(start);
        if(timeout <= 0)	//timeout
        {
            return -2;
        }
    }
    return nwrite;
}


int CBaseSocket::WriteN(unsigned int &nwrite, const void *pBuffer, unsigned int n)
{
    if(m_iSocket < 0)
    {
        return -1;
    }
    size_t	nleft;
    char	*ptr;

    ptr = (char *)pBuffer;
    nleft = n;
    int ret = 0;
    nwrite = 0;
    while (nleft > 0) 
    {
        if ( (ret = write(m_iSocket, ptr, nleft)) < 0) 
        {
            if (errno == EINTR)
                ret = 0;		/* and call read() again */
            else
                return(-1);
        } 
        else if (ret == 0)
        {
            return 0;				/* EOF */
        }
        
        nleft -= ret;
        ptr   += ret;
        nwrite += ret;
    }
    return nwrite;	
}


/*
*���ܣ�
		������(һֱ����nBytes�ֽ�Ϊֹ�����߳���Ϊֹ�����߳�ʱΪֹ��
*���������
		unsigned int &nBytes:�������ĳ���
		int timeout: ��ʱ�ĺ�����(1��=1000���룩
		 -1������ʽ����
		 0:������ʱ�������أ����ȴ�
		 >0��������ʱ�ȴ�����ʱ
		int flags:��־
*���������
		char *chBuffer:�������а������յ�������
*����ֵ��	
		-2:timeout
		-1��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
		0:�Է��ѹر�
		>0:��������������
*/
int CBaseSocket::RecvN(unsigned int &nread, void *pBuffer, unsigned int nBytes,int timeout /*= -1*/, int flags /*= 0*/)
{
    if(m_iSocket < 0)
    {
        return -1;
    }
    nread = 0;
    if( timeout < 0)
    {
        return ReadN(nread, pBuffer, nBytes);		
    }
    struct timeval start;
    struct timezone tz;
    gettimeofday(&start, &tz);
    unsigned int left = nBytes;
    int n = 0;
    char *ptr = (char *)pBuffer;
    while(1)
    {
        n = WaitRead(timeout);
        if(n < 0)	//error
        {
            //if(errno == ETIMEO)
            return n;	//timeout
        }

        n = recv(m_iSocket, ptr, left, flags);
        if( n < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            return -1;		//error
        }
        
        if(n == 0)
        {
            return 0;		//closed by peer
        }
        
        left -= n;
        ptr += n;
        nread += n;
        if(left <= 0)
        {
            break;	//ok
        }

        timeout -= DiffMillSec(start);
        if(timeout <= 0)	//timeout
        {
            return -2;
        }
    }
    
    if(nread == nBytes)
    {
        return nread;
    }
    return -2;	//timeout
}

int CBaseSocket::ReadN(unsigned int &nread,void *vptr, unsigned int n)
{
    if(m_iSocket < 0)
    {
        return -1;
    }
    size_t	nleft;
    char	*ptr;

    ptr = (char *)vptr;
    nleft = n;
    int ret = 0;
    nread = 0;
    while (nleft > 0) 
    {
        if ( (ret = read(m_iSocket, ptr, nleft)) < 0) 
        {
            if (errno == EINTR)
                ret = 0;		/* and call read() again */
            else
                return(-1);
        } 
        else if (ret == 0)
        {
            return 0;				/* EOF */
        }
        nleft -= ret;
        ptr   += ret;
        nread += ret;
    }
    return nread;	
}



/*
*���ܣ�
		��һ���� 
*���������
		pfHandleInput pf:�����ȼ��ĺ���
			(����ֵ:0��Ҫ���������ݲ���ȷ����
				-1:���ݳ���
				>0:����ȫ��)
		unsigned int hlen:��ͷ�ĳ���
		unsigned int nBytes:�������ĳ���		
		int timeout: ��ʱ�ĺ�����(1��=1000���룩
		 -1������ʽ����
		 0:������ʱ�������أ����ȴ�
		 >0��������ʱ�ȴ�����ʱ
		int flags:��־
*���������
		unsigned int &nRead:�������յ����ֽ���
		char *pBuffer:�������а������յ�������

����ֵ: >0:read ok, pack size is return, actually read size is nread, nread=����ֵ
0:closed by peer
-1:error
-2:timeout
-3:data invalid
*/
int CBaseSocket::RecvPack(pfHandleInput pf, unsigned int hlen, unsigned int &nread, void *pBuffer, unsigned int nBytes,int timeout , int flags )
{
    nread = 0;
    struct timeval start;
    struct timezone tz;
    if(timeout > 0)
    {
        gettimeofday(&start, &tz);
    }

    char *ptr = (char *)pBuffer;
    int ret = RecvN(nread, ptr, hlen, timeout, flags);
    if(nread != hlen)
    {
	return ret;
    }

    if(timeout > 0)
    {
	timeout -= DiffMillSec(start);
	if(timeout < 0)
	{
		return -2;	//timeout
	}
    }

   int pack_len = pf(ptr, hlen);
   if(pack_len <= 0)
   {
   	return -3;	//data error.
   }

    if((unsigned int )pack_len == hlen)
    {
        return pack_len;	//����Ϊ��
    }

    if((unsigned int)pack_len > nBytes)
    {
        return -4;	//too big.
    }
	
    unsigned int nbody = 0;
    ret = RecvN(nbody, ptr + hlen, pack_len - hlen, timeout, flags); 
    nread += nbody;
    if(nread == (unsigned int)pack_len)
    {
	return nread;
    }
    return ret;
}


int CBaseSocket::Connect(struct sockaddr_in *addr, int timeout_usec /*=-1*/)
{
    char buf[32] = {0};
    inet_ntoa_r(addr->sin_addr, buf);
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int retval = 0;
    if (timeout_usec < 0) // ����
    {
        int n = connect(m_iSocket, (struct sockaddr *)addr, addr_len);
        if(n != 0)
        {
            perror("connect:");
            return -1;
        }

        //...........
        SaveAddr();
        return 0;
    }
    SetNonBlockOption(true);
    retval= connect(m_iSocket, (struct sockaddr *) addr, addr_len);
    if(retval < 0 && errno != EINPROGRESS)
    {
        Close();
        return -1;
    }


    if(retval == 0)
    {
    	SaveAddr();
        SetNonBlockOption(false);
        return 0;
    }

    if(timeout_usec == 0)   //timeout
    {
    	Close();
        return -1;
    }
    fd_set rset, wset;
    FD_ZERO(&rset);
    FD_SET(m_iSocket, &rset);
    FD_ZERO(&wset);
    FD_SET(m_iSocket, &wset);
    struct timeval timeout;
    MkTimeval(timeout, timeout_usec);
    int n = select(m_iSocket + 1, &rset, &wset, NULL, &timeout);
    if( n == 0) //timeout
    {
    	Close();
        return -1;
    }

    int error = 0;
    if(FD_ISSET(m_iSocket, &rset) || FD_ISSET(m_iSocket, &wset))
    {
        socklen_t len = sizeof(error);
        if(getsockopt(m_iSocket, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            Close();
            return -1;
        }

        SetNonBlockOption(false);

        if(error)
        {
        	    Close();
            return -1;
        }
        SaveAddr();
        return 0;
    }
        //not ready...
    Close();
    return -1;
}

/*
*���ܣ�
		���ӵ�SERVER
*���������
*		const char *szServerIP:������IP
		unsigned short uServerPort���������˿�
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
int CBaseSocket::Connect(const char *pszHost, unsigned short nPort, int timeout_usec /*= -1*/)
{
    sockaddr_in	addr = { 0 };
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(pszHost);
    addr.sin_port        = htons((ushort)nPort);
    // Host name isn't an IP Address?
    if (addr.sin_addr.s_addr == INADDR_NONE)
    {
        // Resolve host name.
        hostent* pHost = gethostbyname(pszHost);

        if (pHost == NULL)
        {
            return -1;
        }

        memcpy(&addr.sin_addr, pHost->h_addr_list[0], pHost->h_length);
    }

    return Connect(&addr, timeout_usec);
}


/*
*���ܣ�
		���ӵ�SERVER
*���������
*		const char uServerIP:������IP(�����ֽ���)
		unsigned short uServerPort���������˿�(�����ֽ���)
		int timeout_usec: ���ӳ�ʱ�ĺ�����(1��=1000���룩
			   -1������ʽ����
			   0:��������ʱ�������أ����ȴ�
			   >0����������ʱ�ȴ�����ʱ

*���������
		��
*����ֵ��	
		0:�ɹ�
		-1:error, -2:timeout��ʧ�ܣ�����������ͨ��GetLastError()��ȡ
*/
int CBaseSocket::Connect(unsigned int uServerIP, unsigned short iServerPort, int timeout_usec /*= -1*/)
{
    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = uServerIP;
    addr.sin_port = htons(iServerPort);
    return Connect(&addr, timeout_usec);
}




/*
*	-1:recv error
	-2:recv timeout
	-3:recv invalid data
	-4:send error
	-5:send timeout		
	0:closed
	>0:ok,the pack len is returned.
*/
int CBaseSocket::SendRecv(
		const char *req,
		unsigned int req_len, 
		pfHandleInput pf,
		unsigned int hlen,    		
		char *rsp, 
		unsigned int rsp_len,  
		int timeout,
		int flags)
{
    unsigned int nwrite = 0;
    int ret = SendN(nwrite, req, req_len, timeout);

    if(nwrite != req_len)
    {
        if(ret == 0)
        {
            return 0;
        }
        
        if(ret == -2)
        {
            return -5;
        }
        return -4;
    }
	
    return RecvPack(pf, hlen, rsp_len, rsp, rsp_len, timeout);
}



//example:
#ifdef _TEST_SOCKET_

int HandleInput(const char *buf, unsigned int len)
{
    return *buf;
}

void Client(const char *host, unsigned int port)
{
	CTcpClient client(host, port);
	if(client.IsValid())
	{
		dbg_out("client is ok,connect server ok.\n");
	}
	else
	{
		dbg_out("connect server error.\n");
		return;
	}

	int ret = 0;
	unsigned int req_len = 6;
	char req[32] = {0};
	req[0] = req_len;
	strcpy(req + 1,"12345");
	char rsp[32];
	unsigned int rsp_len = 31;
	while(1)
	{
		ret = client.SendRecvWithRetry(req, req_len, HandleInput, 
			1, rsp, rsp_len);
		if(ret <= 0)
		{
			printf("send recv error,ret = %d\n",ret);
			break;
		}
		rsp[ret] = 0;
		printf("recv packet, len = %d, content = %s\n", rsp[0], rsp + 1);
		sleep(10);
	}
}

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("usage: %s host port\n", argv[0]);
		return 0;
	}
	signal(SIGPIPE, SIG_IGN);

	Client(argv[1], atoi(argv[2]));
	
	return 0;
}

#endif


}
}

