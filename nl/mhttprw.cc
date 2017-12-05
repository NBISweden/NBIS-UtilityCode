//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//

#include "nl/mhttprw.h"
#ifdef WIN32
#define USE_WININET
#endif


#ifdef SOME_GENERIC_WINDOWS
#ifndef USE_WININET
#define USE_WININET
#endif
#endif

#ifdef OS_POCKETPC
#ifndef USE_WININET
#define USE_WININET
#endif
#endif


#ifdef USE_WININET
#include <windows.h>
#include <wininet.h>

#include "util/mutil.h"

CMHTTPReadStream::CMHTTPReadStream()
{
  m_internetHandle = 0;
  m_fileHandle = 0;
  m_bIsOpen = false;
  m_bIsEof = false;
  m_bufferLen = 0;
  m_readPosition = 0;
}

CMHTTPReadStream::~CMHTTPReadStream()
{
}

IMReadStream * CMHTTPReadStream::CloneAndOpen(const CMString &name)
{
  if (m_urlName == "")
	return NULL;

  CMHTTPReadStream * pNewStream = new CMHTTPReadStream;
  
  CMString newPath;
  CMString temp;
  const char * pTemp = (const char *) m_urlName;
  for (int i=0; i<(long)strlen(m_urlName); i++) {
    temp += pTemp[i];
	if (pTemp[i] == '\\' || pTemp[i] == '/') {
	  newPath = temp;
	}
  }

  newPath += name;

  MCL_TRY {
    pNewStream->Open(newPath);
  }

#ifndef MCL_NO_EXCEPTIONS
  catch (CMException & ) {
    delete pNewStream;
	return NULL;
  }
#endif 

  return pNewStream;
}

bool CMHTTPReadStream::Open(const CMString & name)
{
  //Open it and suck it all in...

  if (m_internetHandle == 0) {
    m_internetHandle = (long)InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
  }

  if (m_internetHandle == 0) {
	long error = GetLastError();
	MLog("CMHTTPReadStream - could not open URL:", name);
	MLog("GetLastError returned:", error);
    ThrowException("Could not open INTERNET");
	return false;
  }
  
  m_urlName = name;

//#ifdef UNICODE
//  unsigned short nameBuffer[512];
//  for (int i=0; i<strlen(name); i++)
//	nameBuffer[i] = ((const char*)name)[i];
//
//  nameBuffer[i] = 0;

#ifdef UNICODE
  TCHAR theName[1024];
  MultiByteToWideChar(
                      CP_ACP, 
                      MB_PRECOMPOSED, 
                      (const char*)name, 
                      -1, 
                      theName, 
                      sizeof(theName));

  m_fileHandle = (long)InternetOpenUrl((HINTERNET)m_internetHandle, theName, NULL, 1000, 0, 0); 
//  m_fileHandle = (long)InternetOpenUrl((HINTERNET)m_internetHandle, nameBuffer, NULL, 1000, 0, 0);
#else
  //m_fileHandle = (long)InternetOpenUrl((HINTERNET)m_internetHandle, name, NULL, 1000, 0, 0);
  m_fileHandle = (long)InternetOpenUrl((HINTERNET)m_internetHandle, name, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
#endif

  if (m_fileHandle == 0) {
    ThrowException("Could not open URL for read:", m_urlName);
	return false;
  }

  DWORD dwNumberOfBytesRead = 0;
  
  long inc = 32000;
  long pos = 0;
  long bytesRead = 0;
  //Let's be generous...

  long bRet = 0;
  do {
	if (bytesRead + inc > m_buffer.length()) { 
      m_buffer.reshape(m_buffer.length() + inc);
	}
	pos += dwNumberOfBytesRead;
    void * pBufferPtr = (void*)&(m_buffer(pos));

    bRet = InternetReadFile((HINTERNET)m_fileHandle, (void*)pBufferPtr, inc, &dwNumberOfBytesRead);
	if (dwNumberOfBytesRead == 0)
	  break;
	if (bRet == 0) {
	  break;
      InternetCloseHandle((HINTERNET)m_fileHandle);
      ThrowException("Could read from URL:", m_urlName);
	  return false;
	}
	bytesRead += dwNumberOfBytesRead;
  //} while ((long)dwNumberOfBytesRead == inc || bRet);
  } while (bRet);

  m_bufferLen = bytesRead;

  InternetCloseHandle((HINTERNET)m_fileHandle);

  return true;
}

bool CMHTTPReadStream::Close()
{

  InternetCloseHandle((HINTERNET)m_fileHandle);
  InternetCloseHandle((HINTERNET)m_internetHandle);

  m_internetHandle = 0;
  m_fileHandle = 0;
  m_bIsOpen = false;
  m_bIsEof = false;
  m_bufferLen = 0;
  m_readPosition = 0;

  m_buffer.reshape(0);
  return true;
}

bool CMHTTPReadStream::IsOpen()
{
  return m_bIsOpen;
}

bool CMHTTPReadStream::IsEnd()
{
  return m_bIsEof;
}


bool CMHTTPReadStream::ReadSimpleType(void * pData, long lenInBytes)
{
  if (m_bIsEof)
	return false;
  void * pBuffer = (void*)&m_buffer(m_readPosition);
  memcpy(pData, pBuffer, lenInBytes);
  m_readPosition += lenInBytes;
  if (m_readPosition >= m_bufferLen)
	m_bIsEof = true;

  return true;
}

bool CMHTTPReadStream::ReadBlob(void * pData, long lenInElements, long elSize)
{
  if (m_bIsEof)
	return false;
  void * pBuffer = (void*)&m_buffer(m_readPosition);
  memcpy(pData, pBuffer, lenInElements * elSize);
  m_readPosition += lenInElements * elSize;
  if (m_readPosition >= m_bufferLen)
	m_bIsEof = true;

  return true;
}

bool CMHTTPReadStream::ReadString(CMString & string)
{
  if (m_bIsEof)
	return false;
  const char * pBuffer = &m_buffer(m_readPosition);

  long i = 0;
  //discard blanks
  char szBuffer[1024];

  while (pBuffer[i] != ' ') {
    szBuffer[i] = pBuffer[i];
	i++;
  }
  szBuffer[i] = 0;

  string = szBuffer;

  m_readPosition += i;

  if (m_readPosition >= m_bufferLen)
	m_bIsEof = true;

  return true;
}

bool CMHTTPReadStream::ReadLine(CMString & string)
{
  return ReadStringLine(string);
}

bool CMHTTPReadStream::ReadStringLine(CMString & string)
{
  string = "";

  if (m_bIsEof)
	return false;
  const char * pBuffer = &m_buffer(m_readPosition);

  long i = 0;
  //discard blanks
  CMCharList theBuffer;


  char * pWriteBuffer = NULL;

  char szTerm[16];
  strcpy(szTerm, "\n");

  while (pBuffer[i] != szTerm[0] && m_readPosition + i < m_bufferLen) {
	if (i+8 >= theBuffer.length()) {
      theBuffer.reshape(theBuffer.length() + 4096);
      pWriteBuffer = (char*)theBuffer.data();
	}
    pWriteBuffer[i] = pBuffer[i];
	i++;
  }


  if (pWriteBuffer != NULL) {
    pWriteBuffer[i] = 0;

	if (pWriteBuffer[strlen(pWriteBuffer)-1] == 0x0d)
      pWriteBuffer[strlen(pWriteBuffer)-1] = 0;

    string = pWriteBuffer;
  }
  i++;
  if (m_readPosition + i < m_bufferLen && pBuffer[i+1] == 0x0a) {
    i++;
  }

  m_readPosition += i;

  if (m_readPosition >= m_bufferLen)
	m_bIsEof = true;

  return true;
}

#else //WIN32 ------------------------------------------------------------------------------------


CMHTTPReadStream::CMHTTPReadStream()
{
  m_internetHandle = 0;
  m_fileHandle = 0;
  m_bIsOpen = false;
  m_bIsEof = false;
  m_bufferLen = 0;
  m_readPosition = 0;
}

CMHTTPReadStream::~CMHTTPReadStream()
{
}


bool CMHTTPReadStream::Open(const CMString & name)
{
  return false;
}

bool CMHTTPReadStream::Close()
{
  return false;
}
bool CMHTTPReadStream::IsOpen()
{
  return false;
}
bool CMHTTPReadStream::IsEnd()
{
  return false;
}

bool CMHTTPReadStream::ReadLine(CMString & string)
{
  return false;
}
IMReadStream * CMHTTPReadStream::CloneAndOpen(const CMString &name)
{
  return NULL;
}

bool CMHTTPReadStream::ReadSimpleType(void * pData, long lenInBytes)
{
  return false;
}
bool CMHTTPReadStream::ReadBlob(void * pData, long lenInElements, long elSize)
{
  return false;
}
bool CMHTTPReadStream::ReadString(CMString & string)
{
  return false;
}
bool CMHTTPReadStream::ReadStringLine(CMString & string)
{
  return false;
}





#endif //WIN32


