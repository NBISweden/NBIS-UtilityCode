//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//

#include "nl/mgrmstck.h"
//#include <mlog.h>
#include "util/mutil.h"


CMGrammarStack::CMGrammarStack()
{
  m_lastHandle = 1;
  m_lastError = GRAMMAR_ERROR_SUCCESS;
}

CMGrammarStack::~CMGrammarStack()
{
}

GRAMMAR_ERROR CMGrammarStack::GetLastError()
{
  return m_lastError;
}

const char * CMGrammarStack::GetLastErrorMessage()
{
  return m_lastErrorMessage;
}

void CMGrammarStack::SetLastGrmError(GRAMMAR_ERROR err, const CMString & errMsg)
{
  m_lastError = err;
  m_lastErrorMessage = errMsg;
  MLog("GRAMMAR ERROR: ", (long)m_lastError);
  MLog("        ERROR: ", m_lastErrorMessage);
}

void CMGrammarStack::SetLastGrmError(GRAMMAR_ERROR err, const CMString & errMsg, const CMString & errArg)
{
  CMString theErr = errMsg;
  theErr += " ";
  theErr += errArg;
  SetLastGrmError(err, theErr);
}

void CMGrammarStack::SetLastGrmError(GRAMMAR_ERROR err, const CMString & errMsg, long errNum)
{
  CMString theErr = errMsg;
  theErr += " ";
  char szNum[64];
  sprintf(szNum, "%ld", errNum);
  theErr += szNum;
  SetLastGrmError(err, theErr);
}

long CMGrammarStack::GetGrammarCount()
{
  return m_grammarList.length();
}

CMGrammar * CMGrammarStack::GetGrammar(long i)
{
  return m_grammarList(i);
}

void CMGrammarStack::AddGrammar(CMGrammar * pGrammar)
{
  m_grammarList.insert(pGrammar, 0);
  pGrammar->SetHandle(m_lastHandle++);
}

CMGrammar * CMGrammarStack::NewGrammar()
{
  CMGrammar * p = new CMGrammar;
  AddGrammar(p);
  return p;
}

bool CMGrammarStack::DestroyGrammar(CMGrammar * pGrammar)
{
  m_grammarList.remove(pGrammar);
  return true;
}

bool CMGrammarStack::DestroyGrammar(GRAMMAR_HANDLE handle)
{
  CMGrammar * p = FindGrammar(handle);
  if (p == NULL) {
	SetLastGrmError(GRAMMAR_ERROR_INVALID_GRAMMAR_HANDLE, "Destroy Grammar: Invalid Grammar Handle-", handle);
	return false;
  }
  DestroyGrammar(p);

  return true;
}

void CMGrammarStack::DestroyAllGrammars()
{
  m_grammarList.removeAll();
}

bool CMGrammarStack::ActivateGrammar(GRAMMAR_HANDLE handle, bool bActivate)
{
  CMGrammar * p = FindGrammar(handle);
  if (p == NULL) {
	SetLastGrmError(GRAMMAR_ERROR_INVALID_GRAMMAR_HANDLE, "Activate Grammar: Invalid Grammar Handle -", handle);
	return false;
  }

  p->SetActive(bActivate);
  //Put it on top of the stack
  if (bActivate) {
	m_grammarList.removeNoDelete(p);
    m_grammarList.insert(p, 0);
  }
  return true;
}

CMGrammar * CMGrammarStack::FindGrammar(GRAMMAR_HANDLE handle)
{
  for (int i=0; i<m_grammarList.length(); i++) {
    if (m_grammarList(i)->GetHandle() == handle)
      return m_grammarList(i);
  }
  return NULL;
}

GRAMMAR_HANDLE CMGrammarStack::AddGrammaAndReadFromFile(const char * fileName)
{
  CMGrammar * pGrammar = NewGrammar();
  MCL_TRY {
    pGrammar->ReadFromFile(fileName);
  }

#ifndef MCL_NO_EXCEPTIONS
  catch(CMException & ex) {
	SetLastGrmError(GRAMMAR_ERROR_LOADING_GRAMMAR, ex.GetErrorText());
    return INVALID_GRAMMAR_HANDLE;
  }
#endif

  return pGrammar->GetHandle();
}

GRAMMAR_HANDLE CMGrammarStack::AddGrammaAndReadFromURL(const char * fileName)
{
  CMGrammar * pGrammar = NewGrammar();
  MCL_TRY {
    pGrammar->ReadFromURL(fileName);
  }

#ifndef MCL_NO_EXCEPTIONS
  catch(CMException & ex) {
	SetLastGrmError(GRAMMAR_ERROR_LOADING_GRAMMAR, ex.GetErrorText());
    return INVALID_GRAMMAR_HANDLE;
  }
#endif

  return pGrammar->GetHandle();
}

GRAMMAR_HANDLE CMGrammarStack::AddGrammaAndReadTextFormat(IMReadStream & stream)
{
  CMGrammar * pGrammar = NewGrammar();
  MCL_TRY {
    pGrammar->ReadTextFormat(stream);
  }

#ifndef MCL_NO_EXCEPTIONS
  catch(CMException & ex) {
	SetLastGrmError(GRAMMAR_ERROR_LOADING_GRAMMAR, ex.GetErrorText());
    return INVALID_GRAMMAR_HANDLE;
  }
#endif

  return pGrammar->GetHandle();
}

const CMString & CMGrammarStack::GetPublicRuleName(CMTagList & tags, 
		                                           CMTokenList & tokens)
{
  return m_ruleName;
}

bool CMGrammarStack::GetDollarValue(CMTagList & tags, 
		                            CMTokenList & tokens,
					                CMString & dollarValue)
{
  int i;
  for (i=0; i<tags.length(); i+=2) {
    const CMString & theTag = *tags(i);
    if (theTag == "$value") {
  	  dollarValue = *tags(i+1);
      return true;
	}
  }

  return false;
}

bool CMGrammarStack::FormatResult(CMTagList & tags, 
			                      CMTokenList & tokens,
								  CMPtrStringList & result)
{		                      
  CMString dollar_value;
  CMString dollar;
  bool bRealVars = false;
  int i;
  for (i=0; i<tags.length(); i+=2) {
    const CMString & theTag = *tags(i);
    if (theTag == "$value") {
  	  dollar_value = *tags(i+1);
      continue;
	}
	if (theTag == "$") {
	  dollar = *tags(i+1);
      continue;
	}
	if (((const char*)theTag)[0] != '$') {
	  //Ted doesn't want the "this.";
	  CMString addResult; //= "this."; 
	  addResult += theTag;
	  addResult += " = ";
	  addResult += *tags(i+1);
	  result.add(new CMString(addResult));
	  bRealVars = true;
	}
  }
  if (!bRealVars) {
	if (dollar_value != "") {
	  CMString theDollar = "$value = ";
	  theDollar += dollar_value;
      result.add(new CMString(theDollar));
	}
    return false;
  } else {
    return true;
  }
}

GRAMMAR_HANDLE CMGrammarStack::ParseAndEvaluate(char * resultChar,
						const char * text,
						long maxLen,
						bool bFuzzy)
{
  CMString result;
  CMPtrStringList list;  
  GRAMMAR_HANDLE h = ParseAndEvaluate(list, text, bFuzzy);

  if (h != -1) {
    for (int i=0; i<list.length(); i++) {
      result += *list(i);
      result += ";";
    }
  }
  if (result.length() < maxLen)
    strcpy(resultChar, result);
  return h;
}

void CMGrammarStack::GetWordList(CMPtrStringList & words)
{
  for (int i=0; i<m_grammarList.length(); i++) {
	if (m_grammarList(i)->IsActive())
	  m_grammarList(i)->GetWordList(words);
  }
}


void CMGrammarStack::GetWordTransitionList(CMPtrStringList & words1,
                                           CMPtrStringList & words2)
{
  for (int i=0; i<m_grammarList.length(); i++) {
	if (m_grammarList(i)->IsActive())
	  m_grammarList(i)->GetWordTransitionList(words1, words2);
  }
}

void CMGrammarStack::Expand(CMGrammarExtract & extract)
{
  for (int i=0; i<m_grammarList.length(); i++) {
	if (m_grammarList(i)->IsActive())
	  m_grammarList(i)->Expand(extract);
  }
}

GRAMMAR_HANDLE CMGrammarStack::ParseAndEvaluate(CMPtrStringList & result,
						const CMString & text,
						bool bFuzzy)
{
  CMTagList tags;
  CMTokenList tokens;
  int i, j, k;

  //Do exact match...
  GRAMMAR_HANDLE h = Parse(tags, tokens, text);
  if (h != -1) {
    FormatResult(tags, tokens, result);
    return h;
  }

  if (!bFuzzy)
    return -1;
  
  CMPtrStringList theWords;
  Tokenize(theWords, text);

  CMString subSet;

  GRAMMAR_HANDLE globalHandle = -1;

  //ATTN: This is SLOW!!
  for (i=0; i<theWords.length(); i++) {
    for (k=theWords.length(); k>i; k--) {
      subSet = "";
      for (j=i; j<k; j++) {
        subSet += *(theWords(j));
        if (j < k-1)
	  subSet += " ";
      }
      tags.removeAll();
      tokens.removeAll();
      //Now parse against all we have...
      h = Parse(tags, tokens, subSet);
      if (h != -1) {
	globalHandle = h; 
	CMPtrStringList subResult;
        if (FormatResult(tags, tokens, subResult)) {
	  for (int l=0; l<subResult.length(); l++) {
	    result.add(new CMString(*subResult(l)));
	  }
	  i = k-1;
	  break;
	}
      }
    }
  }

  return globalHandle;
}

GRAMMAR_HANDLE CMGrammarStack::Parse(CMTagList & tags, 
				     CMTokenList & tokens,
				     const CMString & text)
{
  CMString ruleName;
  for (int i=0; i<m_grammarList.length(); i++) {
    if (m_grammarList(i)->ParseEx(tags, tokens, ruleName, text)) {
      m_ruleName = ruleName;
      return m_grammarList(i)->GetHandle();
    }
  }
  return -1;
}
