//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//


#include "nl/mxtract.h"
#include "util/mutil.h"


CMGrammarExtract::CMGrammarExtract()
{
  m_expLim = 100000;
}


CMGrammarExtract::~CMGrammarExtract()
{
  m_searchStack.removeNoDeleteAll();
  m_ruleStack.removeNoDeleteAll();
}

long CMGrammarExtract::SetExpansionCountLimit(long lim)
{
  m_expLim = lim;
  return 0;
}

void CMGrammarExtract::PushToken(const CMString & token)
{
  //Dirty!
  m_searchStack.add((CMString*)&token);
}

void CMGrammarExtract::PopToken()
{
  if (m_searchStack.length() == 0)
	ThrowException("Grammar Extract: Pop Token");
  m_searchStack.removeNoDelete(m_searchStack.length() - 1);
}

void CMGrammarExtract::Add()
{
  CMString * pString = new CMString;
  for (int i=0; i<m_searchStack.length(); i++) {
	*pString += " ";
	*pString += *m_searchStack(i);
  }

  m_expansions.add(pString);
}

bool CMGrammarExtract::IsExpansionCountLimit()
{
  if (m_expansions.length() >= m_expLim)
	return true;
  else
	return false;
}


void CMGrammarExtract::PushRule(const CMString & rule)
{
  //Dirty!
  m_ruleStack.add((CMString*)&rule);
}

void CMGrammarExtract::PopRule()
{
  if (m_ruleStack.length() == 0)
	ThrowException("Grammar Extract: Pop Rule");
  m_ruleStack.removeNoDelete(m_ruleStack.length() - 1);
}

bool CMGrammarExtract::IsRecursion(const CMString & rule)
{
  long refCount = 0;
  for (int i=0; i<m_ruleStack.length(); i++) {
	if (&rule == m_ruleStack(i)) {
      if (refCount == 1)
	    return true;
	  refCount++;
	}
  }
  return false;
}

long CMGrammarExtract::GetExpansionCount()
{
  return m_expansions.length();
}

const CMString & CMGrammarExtract::GetExpansion(long i)
{
  return *m_expansions(i);
}


//============================================================
//============================================================
//============================================================
CMGrammarBigramExtract::CMGrammarBigramExtract() : CMGrammarExtract()
{
}

CMGrammarBigramExtract::~CMGrammarBigramExtract()
{
}

void CMGrammarBigramExtract::Add()
{
  CMString lastWord = "<START>";
  for (int i=0; i<m_searchStack.length(); i++) {
	const CMString & word = *m_searchStack(i);
	CMString toAdd = lastWord;
	//toAdd += " -> ";
	toAdd += " ";
	toAdd += word;
    m_dict.AddWord(toAdd);
	lastWord = word;
  }

  CMString toAddLast = lastWord;
  //toAddLast += " -> <END>";
  toAddLast += " <END>";
  m_dict.AddWord(toAddLast);

}

long CMGrammarBigramExtract::GetExpansionCount()
{
  return m_dict.GetWordCount();
}

const CMString & CMGrammarBigramExtract::GetExpansion(long i)
{
  return m_dict.GetWord(i);
}


