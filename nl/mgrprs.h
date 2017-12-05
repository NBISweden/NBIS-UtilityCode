//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//

#ifndef _MGRPRS_H_
#define _MGRPRS_H_


#include "util/mutil.h"
//#include <mstrprs.h>

class CMGrammarParser
{
public:
    CMGrammarParser() {}
    virtual ~CMGrammarParser() {}


};

class CMParseStackElement;
typedef TMPtrList<CMParseStackElement> CMParseStackElementList;


class CMParseStackElement
{
public:
	CMParseStackElement() {}
	~CMParseStackElement() 
	{
	}

	const CMString & GetTag() const {return m_tag;}
	const CMString & GetToken() const {return m_token;}

	void SetTag(const CMString & s) {m_tag = s;}
	void SetToken(const CMString & s) {m_token = s;}

	void AddReference(CMParseStackElement * p);
	void RemoveReference(CMParseStackElement * p);


private:
    CMString m_token;
	CMString m_tag;

	CMParseStackElementList m_refs;
};


class CMGrammarNode;
typedef TMPtrList<CMGrammarNode> CMParseTree;

class CMParseStack
{
public:
	CMParseStack();
	~CMParseStack();

	bool FillParseTokens(const CMPtrStringList & list);

	const CMString & GetCurrentToken();
	const CMString & GetParseToken(long i) {return *m_parseTokens(i);}
	bool IsEndOfTokens();
	void Push(CMGrammarNode * pNode, bool incTokens = true);
	void PushSeparator();

	CMGrammarNode * Pop(bool decTokens = true);

	void PushReturnNode(CMGrammarNode * pNode);
	CMGrammarNode * PopReturnNode();

	void Freeze();

	CMParseTree & GetParseTree() {return m_finalParseTree;}

	void SetDebug() {m_bDebugFlag = true;}
    void SetPublicRuleName(const CMString & name) {m_publicRuleName = name;}
    const CMString & GetPublicRuleName() {return m_publicRuleName;}
	void SetExpansionLimit(long v) {m_expLim = v;}

	bool IsExpansionLimit() 
	{
      if (m_freezes >= m_expLim)
		 return true;
	  else
		  return false;
	}

private:
	void DebugPrint();
	long m_expLim;
	long m_freezes;
	CMParseTree m_parseTree;
	CMString m_publicRuleName;
	
	CMParseTree m_finalParseTree;

	CMParseTree m_nodeStack;

	CMPtrStringList m_parseTokens;

	long m_tokenCounter;

	bool m_bDebugFlag;
	//bool m_bPopFlag;
    CMString m_lastPrint;
	//CMParseStackElementList m_parseList;

	//CMParseStackElementList m_leafs;
	//CMParseStackElementList m_roots;

};


#endif //_MGRPRS_H_


