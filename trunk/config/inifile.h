#ifndef __COMM_BASE_INIFILE_H__
#define __COMM_BASE_INIFILE_H__

#include <memory.h>


namespace comm
{
namespace base
{


typedef unsigned char BYTE;

class CIniFile
{
private:
    char    *m_pszContent;      /* �����ļ���ԭʼ���� */
    char    *m_pszShadow;       /* �����ļ�������ȫ��ת����Сд */
    size_t  m_nSize;            /* �����ļ����ݵĳ��ȣ�����������NULL */
    short   m_bIsOpen;          /* �����ļ��Ƿ�򿪳ɹ��ı�־ */

public:
    char    *m_pszFilename;     /* �����Ҫ��ȡ�������ļ��� */
    CIniFile();  
    ~CIniFile();

    /*************************************************
      Function:     IsOpen
      Description:
            ���ض�ȡ�����ļ��Ƿ�ɹ��ı�־
      Calls:
      Called By:    CConfigFile::GetItemValue,
                    CConfigFile::uT_main
      Input:
      Output:
      Return:       ��������ļ���ȡ�ɹ�������true�����򷵻�false
      Others:
    *************************************************/
    unsigned int IsOpen();



    /*************************************************
      Function:     GetItemValue�ַ���
      Description:
            ���ڴ滺�������ҵ�KeyName����ֵ������ָ���Ŀռ䡣
            �������ֵ���ڿռ�Ĵ�С������ַ������н�β����
        ���ڻ����������һ���ֽڼ���NULL��
            ������������������ַ��Ǻ��ֱ���ʱ�����Զ���������������
      Calls:        CConfigFile::IsOpen,
                    CConfigFile::LocateKey,
                    CConfigFile::LocateSection
      Called By:    CConfigFile::uT_main
      Input:
            pszSectionName  - ��NULL��β���ַ���ָ�룬ָʾ����Key��Ƭ��
            pszKeyName      - ��NULL��β���ַ���ָ�룬ָʾ��Ҫ����ֵ��Key
            nSize           - ָ�����ջ������Ĵ�С
      Output:
            pszReturnedString - ָ�����ڽ��ս���Ļ�������ַ
      Return:       ���ػ������е���Ч�ַ��������������ַ�����β��NULL
      Others:       ����������UNICODE�汾
    *************************************************/
    unsigned int GetItemValue( const char *pszSectionName, 
                        const char *pszKeyName, 
                        char *pszReturnedString, 
                        unsigned int nSize );



    /*************************************************
      Function:     SetItemValue����
      Description:
            ��ָ�����������ͼ�ֵ���������ͬʱ�����ڴ�������ļ�
      Calls:        CConfigFile::SetItemValue�ַ���
      Called By:    CConfigFile::uT_main
      Input:
            pszSectionName  - ��NULL��β���ַ���ָ�룬ָʾ����Key��Ƭ��
            pszKeyName      - ��NULL��β���ַ���ָ�룬ָʾ��Ҫ���õ�Key
            ulKeyValue      - �������ͣ�ָʾ��Ҫ���õ�ֵ
      Output:
      Return:       �����Ƿ�ɹ��ı�־���ɹ�������true�����򷵻�false
      Others:       ����������UNICODE�汾
    *************************************************/
    unsigned int SetItemValue( const char *pszSectionName, 
                        const char *pszKeyName, 
                        const char *pszKeyValue );




     /*************************************************
      Function:     GetItemValue����
      Description:
            ���ڴ���ȡָ�����������͵ļ�ֵ
      Calls:        CConfigFile::GetItemValue�ַ���
      Called By:    CConfigFile::uT_main
      Input:
            pszSectionName  - ��NULL��β���ַ���ָ�룬ָʾ����Key��Ƭ��
            pszKeyName      - ��NULL��β���ַ���ָ�룬ָʾ��Ҫ����ֵ��Key
      Output:
            ulReturnedValue - ָ�����ڽ��ս���Ļ�������ַ
      Return:       �ɹ�����true, ʧ�ܷ���false
      Others:       ����������UNICODE�汾
    *************************************************/
    unsigned int GetItemValue( const char *pszSectionName, 
                        const char *pszKeyName, 
                        int &lReturnedValue );


    /*************************************************
      Function:     SetItemValue����
      Description:
            ��ָ�����������ͼ�ֵ���������ͬʱ�����ڴ�������ļ�
      Calls:        CConfigFile::SetItemValue�ַ���
      Called By:    CConfigFile::uT_main
      Input:
            pszSectionName  - ��NULL��β���ַ���ָ�룬ָʾ����Key��Ƭ��
            pszKeyName      - ��NULL��β���ַ���ָ�룬ָʾ��Ҫ���õ�Key
            ulKeyValue      - �������ͣ�ָʾ��Ҫ���õ�ֵ
      Output:
      Return:       �����Ƿ�ɹ��ı�־���ɹ�������true�����򷵻�false
      Others:       ����������UNICODE�汾
    *************************************************/
    unsigned int SetItemValue( const char *pszSectionName, 
                        const char *pszKeyName, 
                        int lKeyValue );

    /*************************************************
      Function:     GetItemValue����
      Description:
            ���ڴ���ȡָ�����������͵ļ�ֵ����������ڣ���ʹ��ָ����ȱʡֵ
      Calls:        CConfigFile::GetItemValue�ַ���
      Called By:    CConfigFile::uT_main
      Input:
            pszSectionName  - ��NULL��β���ַ���ָ�룬ָʾ����Key��Ƭ��
            pszKeyName      - ��NULL��β���ַ���ָ�룬ָʾ��Ҫ����ֵ��Key
            lDefaultValue   - ȡֵʧ�ܺ�ʹ�õ�ȱʡֵ
      Output:
            ulReturnedValue - ָ�����ڽ��ս���Ļ�������ַ
      Return:       �ɹ�����true, ʧ�ܷ���false
      Others:       ����������UNICODE�汾
    *************************************************/
    unsigned int GetItemValue( const char *pszSectionName, 
                        const char *pszKeyName, 
                        int &lReturnedValue,
                        int lDefaultValue );


     /*************************************************
      Function:     GetItemValue�ַ���
      Description:
            ���ڴ���ȡָ�����ַ������͵ļ�ֵ����������ڣ���ʹ��ָ����ȱʡֵ
      Calls:        CConfigFile::GetItemValue�ַ���
      Called By:    CConfigFile::uT_main
      Input:
            pszSectionName  - ��NULL��β���ַ���ָ�룬ָʾ����Key��Ƭ��
            pszKeyName      - ��NULL��β���ַ���ָ�룬ָʾ��Ҫ����ֵ��Key
            nSize           - ָ�����ջ������Ĵ�С
            pszDefaultValue - ȡֵʧ�ܺ�ʹ�õ�ȱʡֵ
      Output:
            pszReturnedString - ָ�����ڽ��ս���Ļ�������ַ
      Return:       ���ػ������е���Ч�ַ��������������ַ�����β��NULL
      Others:       ����������UNICODE�汾
    *************************************************/
    unsigned int GetItemValue( const char *pszSectionName, 
                        const char *pszKeyName, 
                        char *pszReturnedString, 
                        unsigned int nSize,
                        const char *pszDefaultValue );

    /*************************************************
      Function:     OpenFile
      Description:
            ��ȡָ���������ļ���
            ����ļ���ȡ�ɹ�������m_bIsOpenΪtrue
      Calls:
      Called By:    CConfigFile::CConfigFile
      Input:
            pszFilename     - ��NULL��β�������ļ���
      Output:
      Return:�ɹ�:0,����ʧ��
      Others:
    *************************************************/
    int  OpenFile(const char *pszFilename);

    void CloseFile();

private:
    
    unsigned int LocateSection(const char *pszSectionName, 
                        char * &pszSectionBegin, 
                        char * &pszSectionEnd);
    unsigned int LocateKeyRange(const char *pszKeyName, 
                        const char *pszSectionBegin, 
                        const char *pszSectionEnd, 
                        char * &pszKeyBegin, 
                        char * &pszKeyEnd);
    unsigned int LocateKeyValue(const char *pszKeyName, 
                        const char *pszSectionBegin, 
                        const char *pszSectionEnd, 
                        char * &pszValueBegin, 
                        char * &pszValueEnd);
    char *LocateStr(    const char *pszCharSet, 
                        const char *pszBegin, 
                        const char *pszEnd );
    char *SearchMarchStr(const char *pszBegin, const char *pszCharSet);

    char *MapToContent(const char *p);
    char *MapToShadow(const char *p);

    void ToLower( char * pszSrc, size_t len);

};

}
}
#endif

