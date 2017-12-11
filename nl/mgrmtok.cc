//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//



#include "nl/mgrmtok.h"
#include "util/mutil.h"
//#include <iostream.h>
// #include <mexcept.h>

bool LocalTokenize(CMPtrStringList & result, const CMString & source, char delimiter = ' ', long limit = 0x7FFFFFFF);


bool LocalTokenize(CMPtrStringList & result, const CMString & source, char delimiter, long limit)
{
  const MCL_TCHAR * pszBuffer = source;

  MCL_TCHAR szTemp[2048];
  //Clear out any previous results in the tokenizer
  result.removeAll();
  __mccl_strcpy(szTemp, _TMCL(""));

  int k = 0;

  long len = (long)__mccl_strlen(pszBuffer);

  bool bTagFlag = false;

  //Hack to preserve tags
  char cTagOpen = TAG_OPEN[0];
  char cTagClose = TAG_CLOSE[0];

  for (int i=0; i<=len; i++) {
    if (pszBuffer[i] == cTagOpen)
      bTagFlag = true;
    if (pszBuffer[i] == cTagClose)
      bTagFlag = false;
    
    if (!bTagFlag && (pszBuffer[i] == delimiter || pszBuffer[i] == '\t' || i == len)) {
      k = 0;
      if (__mccl_strcmp(szTemp, _TMCL("")) != 0 && __mccl_strcmp(szTemp, _TMCL(" ")) != 0) {
	CMString * pNewString = new CMString;
	*pNewString = szTemp;
	result.add(pNewString);
        __mccl_strcpy(szTemp, _TMCL(""));
	if (result.length() >= limit)
	  break;
      }
    } else {
      szTemp[k] = pszBuffer[i];
      szTemp[k + 1] = 0;
      k++;
    }
  }
  
  return true;
}




CMGrammarTokenizer::CMGrammarTokenizer()
{
}

CMGrammarTokenizer::~CMGrammarTokenizer()
{
}

bool CMGrammarTokenizer::CheckForFirstToken(const CMString & text, const CMString & token)
{
  if (text == "")
	return false;


  char szCheck[16];
  unsigned long len = sizeof(szCheck) - 2;
  if (strlen(token) < len)
	len = strlen(token);

  //cout << len << endl;
  memcpy((void*)szCheck, (void*)(const char*)token, len+1);


  szCheck[len] = 0;

  char szText[16];
  unsigned long copylen = sizeof(szText) - 2;
  if (strlen(text) < copylen)
	copylen = strlen(text);

  //cout << len << endl;
  memcpy((void*)szText, (void*)(const char*)text, copylen+1);

  szText[len] = 0;

  if (strcmp(szText, szCheck) == 0)
    return true;
  else
    return false;
}

bool CMGrammarTokenizer::IsEndOfComment(const CMString & token)
{
  if (strlen(token) < 2)
	return false;

  CMString toTest = &((const char*)token)[strlen(token) - 2];
  if (toTest == COMMENT_CLOSE)
	return true;
  else
    return false;
}


bool CMGrammarTokenizer::CheckForToken(CMPtrStringList & result, const CMString & token, 
									   const CMString & checkToken, long & i, char * pTemp)
{
  //cout << (const char*)checkToken << endl;
  //cout << (const char*)token << " " << i << endl;
  //cout << pTemp << endl;

  const char * pTextPtr = &((const char*)token)[i];
  if (CheckForFirstToken(pTextPtr, checkToken)) {
    //cout << "#1" << endl;
    if (strcmp(pTemp, "") != 0) {
      //cout << "#2" << endl;
      CMString * pNewLastElem = new CMString;
      *pNewLastElem = pTemp;
      pTemp[0] = 0;
      result.add(pNewLastElem);
    }
    
    //cout << "#3" << endl;
    CMString * pNewElem = new CMString;
    *pNewElem = checkToken;
    result.add(pNewElem);
    i += strlen(checkToken) - 1;
    return true;
  }
  return false;
}

bool CMGrammarTokenizer::CheckAndSeeIfItsAComment(CMPtrStringList & result, const CMString & token)
{
  

  if (CheckForFirstToken(token, COMMENT_OPEN))
	return true;

  long len = strlen(token);
  long copyLen = 0;
  char * pTemp = new char[len + 1];
  pTemp[0] = 0;

  bool bTagFlag = false;
  for (long i=0; i<len; i++) {
    
    if (CheckForToken(result, token, TAG_OPEN, i, pTemp)) {
      copyLen = 0;
      bTagFlag = true;
      continue;
    }
    if (CheckForToken(result, token, TAG_CLOSE, i, pTemp)) {
      copyLen = 0;
      bTagFlag = false;
      continue;
    }
    
    if (!bTagFlag) {
      if (CheckForToken(result, token, RULE_OR, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, NONTERM_OPEN, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, NONTERM_CLOSE, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, OPTIONAL_OPEN, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, OPTIONAL_CLOSE, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, EQUAL, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, QUOTE_OPEN, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, QUOTE_CLOSE, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, RULE_END, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, ONE_OR_MORE, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, ZERO_OR_MORE, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, WEIGHT_OPEN, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      if (CheckForToken(result, token, WEIGHT_CLOSE, i, pTemp)) {
        copyLen = 0;
	continue;
      } 
      //It's a comment!
      if (CheckForToken(result, token, COMMENT_EOL, i, pTemp)) {
        copyLen = 0;
	break;
      } 
    }
    
    pTemp[copyLen] = ((const char*)token)[i];
    pTemp[copyLen+1] = 0;
    copyLen++;
  }

  if (copyLen > 0) {
    CMString * pNewElem = new CMString;
    *pNewElem = pTemp;
    result.add(pNewElem);
  }

  delete [] pTemp;
  return false;
}



bool CMGrammarTokenizer::CheckForImport(CMPtrStringList & imports, const CMString & line)
{
  


  if (CheckForFirstToken(line, IMPORT_GRAMMAR)) {
    long len = strlen(line);
    long start = strlen(IMPORT_GRAMMAR);
    CMString theImport;
    const char * pText = (const char *)line;
    
    CMString key;
    CMString temp;
    bool bFlag = false;
    for (int i=start; i<len; i++) {
      if (pText[i] == INCLUDE_OPEN) {
	bFlag = true;
	continue;
      }
      if (pText[i] == INCLUDE_CLOSE) {
	for (i++; i<len; i++) {
	  if (!(pText[i] == ' ' || pText[i] == ';'))
	    return false;
	} 
	break;
      }
      if (bFlag) {
	if (pText[i] != ' ') {
	  theImport += pText[i];
	  if (pText[i] != '.')
	    temp += pText[i];
	}
	if (pText[i] == '.') {
	  key = temp;
	  temp = "";
	}
      } else {
	if (pText[i] != ' ')
	  return false;
      }
    }
    imports.add(new CMString(theImport));
    imports.add(new CMString(key));
    return true;
  }
  return false;
}

bool CMGrammarTokenizer::IsCPPStyleComment(CMString & token)
{
  if (CheckForFirstToken(token, COMMENT_EOL))
	return true;
  return false;
}

bool CMGrammarTokenizer::IsGrammarFormatTag(CMString & format, const CMString & line)
{
  CMString cmp = line;
  if (cmp.length() < 5)
	return false;

  if (cmp[(long)0] == '#' && cmp[(long)1] == 'J' && cmp[(long)2] == 'S' && cmp[(long)3] == 'G' && cmp[(long)4] == 'F')
	return true;
  else
	return false;
  
}

bool CMGrammarTokenizer::Tokenize(CMPtrStringList & result, CMPtrStringList & imports, IMReadStream & stream, CMString & grmName, bool bSuppressPublicRules)
{
  bool bCommentFlag = false;
  bool bFormatFlag = false;

  CMString format;

  while (!stream.IsEnd()) {
    CMString line;
    stream.ReadLine(line);
    if (CheckForImport(imports, line)) {
      continue;
    }
    //CheckForName(grmName, line);
    if (!bFormatFlag) {
      bool bIsFormat = IsGrammarFormatTag(format, line);
      if (bIsFormat) {
	bFormatFlag = true;
	continue;
      }
    }
    
    CMPtrStringList preTokens;
    ::LocalTokenize(preTokens, line);
    //Brutal....
    if (preTokens.length() == 2) {
      if (*preTokens(0) == "grammar") {
	grmName = *preTokens(1);
	grmName[(long)(strlen(grmName)-1)] = 0;
	continue;
      }
    }
    
    if (bSuppressPublicRules) {
      if (preTokens.length() > 0) {
	if (*preTokens(0) == PUBLIC_RULE) {
          if (!bFormatFlag)
            ThrowException("Grammar Tokenizer: Missing Format Tag!");
	  preTokens.remove((long)0);
	}
      }
    }
    for (int i=0; i<preTokens.length(); i++) {
      if (IsCPPStyleComment(*preTokens(i))) {
	while (preTokens.length() > i)
	  preTokens.remove(preTokens.length()-1);
	break;
      }
      
      if (bCommentFlag) {
	if (IsEndOfComment(*preTokens(i)))
	  bCommentFlag = false;
      }	else {
	if (CheckAndSeeIfItsAComment(result, *preTokens(i))) {
	  bCommentFlag = true;
	} else {
          if (!bFormatFlag)
            ThrowException("Grammar Tokenizer: Missing Format Tag!");		
	}
      }
    }
  }
  return true;
}


