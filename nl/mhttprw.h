//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//=  Distribution or use of the source code, data or other 		   =//
//=  information contained herein wihout the author's              =//
//=  permission is strictly prohibited.                            =//
//=                                                                =//
//=  This file and all its contents are confidential.              =//
//==================================================================//

#ifndef MHTTPRW_H_
#define MHTTPRW_H_

#include "util/mutil.h"
//#include "mlists.h"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class CMHTTPReadStream : public IMReadStream
{
public:
    MDLLEXPORT CMHTTPReadStream();
    MDLLEXPORT virtual ~CMHTTPReadStream();


    MDLLEXPORT virtual bool Open(const CMString & name);
    MDLLEXPORT virtual bool Close();
    MDLLEXPORT virtual bool IsOpen();
    MDLLEXPORT virtual bool IsEnd();

    MDLLEXPORT virtual bool ReadLine(CMString & string);
    virtual IMReadStream * CloneAndOpen(const CMString &name);

    long GetBufferLength() {return m_bufferLen;}
    virtual long BytesProcessed() {return m_readPosition;}

protected:
    virtual bool ReadSimpleType(void * pData, long lenInBytes);
    virtual bool ReadBlob(void * pData, long lenInElements, long elSize);
    virtual bool ReadString(CMString & string);
    virtual bool ReadStringLine(CMString & string);


private:

    long m_internetHandle;
    long m_fileHandle;
    CMString m_urlName;
    bool m_bIsOpen;
    bool m_bIsEof;
    CMCharList m_buffer;
    long m_bufferLen;
    long m_readPosition;

};

#endif /* MHTTPRW_H_ */



