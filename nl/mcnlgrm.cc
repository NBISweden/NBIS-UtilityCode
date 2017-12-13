//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//



#include "nl/mcnlgrm.h"
#include "nl/mgrprs.h"
#include "util/mutil.h"
#include "nl/mhttprw.h"
#include "nl/mgrmtok.h"
#include "nl/mgrmnet.h"
#include "nl/mscrptnt.h"

#include <iostream>

using namespace std;
//#include <mlog.h>

CMGrammar::CMGrammar()
{
  m_pGrammar = NULL;
  m_pParser = NULL;
  m_pInterpreter = new CMScriptInterpreter;
  m_bDebugFlag = false;
  m_handle = -1;
  m_bIsActive = true;
  m_bDontUseDict = false;
}

CMGrammar::~CMGrammar()
{
  //if (m_bDebugFlag)
    MLog("Destroying grammar:", m_name);
  if (m_pGrammar != NULL)
	delete m_pGrammar;

  if (m_pParser != NULL)
	delete m_pParser;

  delete m_pInterpreter;
}

void CMGrammar::SetDebugFlag(bool b)
{
  m_bDebugFlag = b;
}

bool CMGrammar::ReadFromURL(const CMString & urlName)
{
  CMHTTPReadStream readStream;
  readStream.Open(urlName);
  ReadTextFormat(readStream);
  readStream.Close();
  return true;
}

bool CMGrammar::ReadFromFile(const CMString & fileName)
{
  CMAsciiReadFileStream readStream;
  readStream.Open(fileName);
  ReadTextFormat(readStream);
  readStream.Close();
  return true;
}

bool CMGrammar::ReadTextFormat(IMReadStream & stream)
{
  CMGrammarTokenizer tokenizer;
  CMString line;

  CMPtrStringList tokenList;
  CMPtrStringList imports;


  tokenizer.Tokenize(tokenList, imports, stream, m_name);   

  CMString dummyName;
  //while (imports.length()) 
  for (int i=0; i<imports.length(); i+=2) {
    CMString grammarName = *imports(i+1);
    grammarName += ".gram";
    IMReadStream * pClonedStream = stream.CloneAndOpen(grammarName);
    if (pClonedStream == NULL) {
      ThrowException("GRAMMAR ERROR: Could not resolve import: ", *imports(i));
    }
    //if (m_bDebugFlag) 
    MLog("WARNING: Importing Grammar:", grammarName);
    tokenizer.Tokenize(tokenList, imports, *pClonedStream, dummyName, true);
    pClonedStream->Close();
    delete pClonedStream;
  } 
  
  //cout << "Number of raw tokens: " << tokenList.length() << endl;
  //for (int i=0; i<tokenList.length(); i++) {
  //const CMString &theToken = *tokenList(i);
  //cout << (const char*)theToken << endl;
  //}
  
  if (m_pGrammar != NULL)
    delete m_pGrammar;
  
  if (m_pParser != NULL)
    delete m_pParser;
  
  m_bDontUseDict = false;
  m_pGrammar = new CMGrammarNetwork;
  
  bool bHasWildCards = false;
  m_pGrammar->Build(tokenList, m_dict, bHasWildCards);
  if (bHasWildCards)
    m_bDontUseDict = true;
  
  m_pParser = new CMGrammarParser;
  //if (m_bDebugFlag)
  MLog("Grammar loaded:", m_name);
  
  return true;
}

bool CMGrammar::Parse(CMTagList & tags, 
				      CMTokenList & tokens,
				      const CMString & text)
{
  CMString publicRuleName;
  return ParseEx(tags, tokens, publicRuleName, text);
}

void CMGrammar::Expand(CMGrammarExtract & extract, bool bFull)
{
  if (m_pGrammar == NULL)
	return;
  m_pGrammar->Expand(extract, bFull);
}


void CMGrammar::GetWordTransitionList(CMPtrStringList & words1,
									  CMPtrStringList & words2)
{
  if (m_pGrammar == NULL)
	return;
  m_pGrammar->GetWordTransitionList(words1, words2);
}


void CMGrammar::GetWordList(CMPtrStringList & words)
{
  for (int j=0; j<m_dict.GetWordCount(); j++) {
	 words.add(new CMString(m_dict.GetWord(j)));
  }

}

bool CMGrammar::ParseEx(CMTagList & tags, 
			CMTokenList & tokens,
                        CMString & publicRuleName,
			const CMString & preText)

{
  if (m_pGrammar == NULL)
    return false;
  if (!m_bIsActive)
    return false;
  
  CMString text = preText;
  // WARNING: remove lower casing of input!!
  //text.toLower();
  
  CMParseStack stack;
  
  if (m_bDebugFlag)
    stack.SetDebug();

  CMPtrStringList parseTokens;
  Tokenize(parseTokens, text);
  //cout << "TEXT " << (const char *) text << endl;
  
  //Quick check if all words are indeed in the grammar
  //m_bDontUseDict = true;
  if (!m_bDontUseDict) {
    for (int j=0; j<parseTokens.length(); j++) {
      if (m_dict.GetDictID(*parseTokens(j)) == INVALID_STRING_DICT_ID)
	return false;
    }
  }
  
  stack.FillParseTokens(parseTokens);
  
	
  bool bParsed = m_pGrammar->DoesParse(&stack);


  CMPtrStringList script;
  
  if (m_bDebugFlag)
    MLog("DEBUG (GRAMMAR): Parse Tree...", m_name);
  
  bool bScriptFlag = false;

 


  
  //cout << "Parsing...... " << (const char *)text << endl;
  if (bParsed) {
    //cout << "Parse SUCCESSFUL!!! " << endl;
    CMParseTree & parseTree = stack.GetParseTree();
    CMUInt32List mapToParseTokens;
    mapToParseTokens.reshape(parseTree.length());
    int kk = 0;
    for (int i=0; i<parseTree.length(); i++) {
      if (parseTree(i)->IsTerminal()) {
	mapToParseTokens[i] = kk;
	kk++;
      } else {
	mapToParseTokens[i] = -1;
      }

      /*
      cout << "token: " << parseTree(i)->GetToken() << endl;
      if (parseTree(i)->IsTerminal())
	cout << "Term" << endl;
      else
      cout << "No term" << endl; */
    }



    CMString accumulateToken;
    for (int i=0; i<parseTree.length(); i++) {
      //cout << "Parse tree size:" << parseTree.length() << endl;
      if (parseTree(i)->GetTag() != "")
        bScriptFlag = true;
      
      script.add(new CMString(parseTree(i)->GetTag()));
      //Add to the token list
      CMString tokenToAdd;
      CMString tokenToPrint;
      if (parseTree(i)->IsTerminal()) {
	if (parseTree(i)->IsWildcard()) {
	  //cout << "Wildcard" << endl;
	  if (parseTree(i)->IsDelimiter()) {
	    //cout << "Delimiter" << endl;
	    tokenToAdd = stack.GetParseToken(i);
	    tokenToPrint = stack.GetParseToken(i);
	    //cout << "DEBUG (1) " << tokenToAdd << " " << tokenToPrint << endl;
	    accumulateToken = "";
	  } else {
	    //cout << "NO Delimiter " << parseTokens.length() << " " << i << endl;
	    //cout << parseTree(i)->GetToken() << endl;
	    if (accumulateToken != "")
	      accumulateToken += " ";
	    // if (i < parseTokens.length())
	    accumulateToken += stack.GetParseToken(mapToParseTokens[i]);

	    tokenToAdd = accumulateToken;
	    tokenToPrint = accumulateToken;
	    //cout << "DEBUG (2) " << tokenToAdd << " " << tokenToPrint << endl;
	    //tokenToAdd += stack.GetParseToken(i);
	    //tokenToPrint += stack.GetParseToken(i);
	    //accumulateToken += tokenToAdd;
	  }
	} else {
	  if (parseTree(i)->IsDelimiter()) {
	    tokenToAdd = parseTree(i)->GetToken();
	    tokenToPrint = parseTree(i)->GetToken();
	    accumulateToken = "";
	  } else {
	    if (accumulateToken != "")
	      accumulateToken += " ";
	    accumulateToken += parseTree(i)->GetToken();
            tokenToAdd = accumulateToken;
	    tokenToPrint = accumulateToken;
	    //tokenToAdd += parseTree(i)->GetToken();
	    //tokenToPrint += parseTree(i)->GetToken();
	    //if (accumulateToken != "")
	    //accumulateToken += " ";
	    //accumulateToken += tokenToAdd;
	  }
	} 
      } else {
	accumulateToken = ""; 
	tokenToAdd = "$";
	tokenToAdd += parseTree(i)->GetToken();
	tokenToPrint = "<";
	tokenToPrint += parseTree(i)->GetToken();
	tokenToPrint += ">";
	//tokenToAdd += ">";
      }
      
      tokens.add(new CMString(tokenToAdd));
      
      //if (m_bDebugFlag) {
	CMString loggg = tokenToPrint;
	loggg += " -- {";
	loggg += parseTree(i)->GetTag();
	loggg += " }";

	//cout << (const char*)loggg << endl;
	
        //MLog(loggg);
	// cout << tokenToPrint << "  -- " << "{ " << (const char*)parseTree(i)->GetTag() << " }" << endl;
	//}
    }
  } else {
    //cout << "..........Parse FAILED!!! " << endl;
  }
  
  if (m_bDebugFlag)
    MLog("");
  
  if (bParsed) {
    if (bScriptFlag) {
      CMPtrStringList result;
      m_pInterpreter->Interprete(result, script, tokens);
      
      //cout << "Interpreted Script...... "  << endl;
      for (int j=0; j<result.length(); j++) {
	//cout << (const char*)(*result(j)) << endl;
	tags.add(new CMString(*result(j)));
      } 
    } else {
      CMString fakeResult = "$value";
      tags.add(new CMString(fakeResult));
      tags.add(new CMString(text));
    }
    
    publicRuleName = stack.GetPublicRuleName();
  }
  
  return bParsed;
}



