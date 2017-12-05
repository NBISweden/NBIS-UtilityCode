//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//




#include "nl/mgrprs.h"
#include "nl/mgrmnet.h"
//#include <iostream.h>
#include "util/mutil.h"

CMParseStack::CMParseStack()
{
  m_tokenCounter = 0;
  m_bDebugFlag = 0;
  m_expLim = 1000000;
  m_freezes = 0;
}

CMParseStack::~CMParseStack()
{
  m_parseTree.removeNoDeleteAll();
  m_nodeStack.removeNoDeleteAll();
  m_finalParseTree.removeNoDeleteAll();

}

bool CMParseStack::FillParseTokens(const CMPtrStringList & list)
{
  m_parseTree.removeNoDeleteAll();
  m_parseTokens.removeAll();
  m_nodeStack.removeNoDeleteAll();
  m_finalParseTree.removeNoDeleteAll();
  for (int i=0; i<list.length(); i++) {
    CMString * pString = new CMString;
	*pString = *(list(i));
	m_parseTokens.add(pString);	  
  }
  m_tokenCounter = 0;
  return true;
}

void CMParseStack::PushSeparator()
{
}

void CMParseStack::Push(CMGrammarNode * pNode, bool incTokens)
{
  m_parseTree.add(pNode);
  if (incTokens) {
    m_tokenCounter++;
  }

  //if (m_bDebugFlag)
	//MLog("- Push");
  DebugPrint();
}

void CMParseStack::DebugPrint()
{
  if (!m_bDebugFlag)
	return;
  CMString printIt;
  for (int i=0; i<m_parseTree.length(); i++) {
	if (m_parseTree(i)->GetToken() != "") {
      printIt += " ";
	  if (m_parseTree(i)->IsTerminal()) {
	    printIt += m_parseTree(i)->GetToken();
	  } else {
	    printIt += "<";
	    printIt += m_parseTree(i)->GetToken();
	    printIt += ">";
	  }
	}
  }
  if (m_lastPrint != printIt)
    MLog(printIt);
  m_lastPrint = printIt;
}

CMGrammarNode * CMParseStack::Pop(bool decTokens)
{
  if (m_parseTree.length() == 0)
	ThrowException("CMParseStack:: Invalid Pop");

  CMGrammarNode * p = m_parseTree(m_parseTree.length() - 1);

  m_parseTree.removeNoDelete(m_parseTree.length() - 1);
  if (decTokens) {
    m_tokenCounter--;
  }

  //if (m_bDebugFlag)
	//cout << "- Pop" << endl;
  //DebugPrint();

  return p;
}

const CMString & CMParseStack::GetCurrentToken()
{
  if (m_tokenCounter < m_parseTokens.length()) {
    return *m_parseTokens(m_tokenCounter);
  }
  
  static const CMString nope;
  return nope;
}

bool CMParseStack::IsEndOfTokens()
{
  if (m_tokenCounter >= m_parseTokens.length())
	return true;
  else
	return false;
}

void CMParseStack::PushReturnNode(CMGrammarNode * pNode)
{
  m_nodeStack.add(pNode);
}

void CMParseStack::Freeze()
{
  m_finalParseTree.removeNoDeleteAll();

  m_freezes++;
  for (int i=0; i<m_parseTree.length(); i++) {
    if (!m_parseTree(i)->IsDummy())
      m_finalParseTree.add(m_parseTree(i));
  }

  if (m_bDebugFlag)
	MLog("- FREEZE -");
  DebugPrint();
}

CMGrammarNode * CMParseStack::PopReturnNode()
{
  if (m_nodeStack.length() == 0)
	return NULL;

  CMGrammarNode * p = m_nodeStack(m_nodeStack.length() - 1);

  m_nodeStack.removeNoDelete(m_nodeStack.length() - 1);

  return p;
}



