//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//



#ifndef _MGRMNET_H_
#define _MGRMNET_H_

#include "util/mutil.h"
//#include <mstrprs.h>

class CMGrammarNode;
class CMGrammarExtract;

typedef TMPtrList<CMGrammarNode> CMGrammarNodeList;

#define GRMNODE_FLAG_TERMINAL		1
#define GRMNODE_FLAG_END			2
#define GRMNODE_FLAG_START			4

#define GRMNODE_WILDCARD			'%'

class CMParseStack;
class CMStringDictionary;

enum NODE_OPT_FLAG
{
	NODE_OPT_FLAG_FALSE,
	NODE_OPT_FLAG_TRUE,
	NODE_OPT_FLAG_CONTD
};


typedef TMPtrList<CMGrammarNodeList> CMRHSRuleList;

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
class CMGrammarNode
{
public:
	CMGrammarNode() 
	{
	  m_flags = 0;
	  m_bIsActive = true;
	  m_pNextNode = NULL;
	  m_dummyFlag = false;
	  m_bOptional = NODE_OPT_FLAG_FALSE;
	  m_bIsWildCard = false;
	}
	CMGrammarNode(const CMGrammarNode & node) 
	{
	  *this = node;
	}

	virtual ~CMGrammarNode() 
	{
	  m_references.removeNoDeleteAll();
	}

	virtual bool DoesParse(CMParseStack * pStack);
    
	virtual void Expand(CMGrammarExtract & extract, CMParseStack * pStack);
    

	long GetReferenceCount() {return m_references.length();}
	CMGrammarNode * GetNode(long index) {return m_references(index);}
	void ReferenceNode(CMGrammarNode * p) {m_references.add(p);}

	bool IsTerminal()	{return (m_flags & GRMNODE_FLAG_TERMINAL) != 0;}
	bool IsEnd()		{return (m_flags & GRMNODE_FLAG_END) != 0;}
	bool IsStart()		{return (m_flags & GRMNODE_FLAG_START) != 0;}
	bool IsDummy()		{return m_dummyFlag;}
	bool IsWildcard()		{return m_bIsWildCard;}
	bool IsOptional()	
	{
		return m_bOptional == NODE_OPT_FLAG_TRUE;
	}
	bool IsOptionalContd()	
	{
		return m_bOptional == NODE_OPT_FLAG_CONTD;
	}

	void SetDummy()		{m_dummyFlag = true;}

	void SetOptionalContd() {m_bOptional = NODE_OPT_FLAG_CONTD;} 
	void SetOptional(bool b = true) 
	{
	  if (b)
	    m_bOptional = NODE_OPT_FLAG_TRUE;
	  else
	    m_bOptional = NODE_OPT_FLAG_FALSE;
	}

	void SetTerminal()	{m_flags |= GRMNODE_FLAG_TERMINAL;}
	void SetEnd()		{m_flags |= GRMNODE_FLAG_END;}
	void SetStart()		{m_flags |= GRMNODE_FLAG_START;}

	const CMString & GetTag() const {return m_tag;}
	const CMString & GetToken() const {return m_token;}

	void SetTag(const CMString & tag)		{m_tag = tag;}
	void SetToken(const CMString & token)	
	{
		m_token = token;
		// WARNING!!!!!!!!!!!
		// m_token.toLower();
		long len = m_token.length();
		for (int i=0; i<len; i++) {
		  if (((const char*)m_token)[i] == GRMNODE_WILDCARD)
		    m_bIsWildCard = true;
		}
	}

	bool IsActive() {return m_bIsActive;}
	void SetActive(bool b) {m_bIsActive = b;}

	void SetNextNode(CMGrammarNode * p) {m_pNextNode = p;}
	bool IsDelimiter() {return (m_pNextNode != NULL);}

	CMGrammarNode & operator=(const CMGrammarNode & node)
	{
	  m_tag = node.m_tag;
	  m_token = node.m_token;
	  m_flags = node.m_flags;
	  m_bIsActive = node.m_bIsActive;
	  m_dummyFlag = node.m_dummyFlag;
	  m_bOptional = node.m_bOptional;
	  return *this;
	}


    virtual CMRHSRuleList * GetNodeListOrNot() {return NULL;}

private:
	bool WildCardParse(const CMString & test);

	CMGrammarNodeList m_references;
	CMGrammarNode * m_pNextNode;
	CMString m_tag;
	CMString m_token;
	long m_flags;
	bool m_bIsActive;
	bool m_dummyFlag;
	bool m_bIsWildCard;
	NODE_OPT_FLAG m_bOptional;
};


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
class CMGrammarRule : public CMGrammarNode
{
public:
	CMGrammarRule();
	~CMGrammarRule();

    virtual bool DoesParse(CMParseStack * pStack);
    CMGrammarNodeList * AddRHSRule();

	const CMString & GetName() const {return m_ruleName;}
	void SetName(const CMString & s) 
	{
		m_ruleName = s;
		// WARNING!!!!!!!!!!!
		// m_ruleName.toLower();
	}

	bool IsActive() const {return m_bIsActive;}
	void SetActive(bool b = true) {m_bIsActive = b;}

    CMRHSRuleList & GetNodeList() {return m_ruleList;}
	virtual void Expand(CMGrammarExtract & extract, CMParseStack * pStack);

    virtual CMRHSRuleList * GetNodeListOrNot() {return &m_ruleList;}


protected:

private:
	CMString m_ruleName;
	bool m_bIsActive;
	CMRHSRuleList m_ruleList;
};


typedef TMPtrList<CMGrammarRule> CMRuleList;
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
class CMGrammarNetwork
{
public:
	CMGrammarNetwork();
	virtual ~CMGrammarNetwork();

	long GetRuleCount() {return m_rules.length();}
	CMGrammarRule * GetRule(long index) {return m_rules(index);}

    bool DoesParse(CMParseStack * pStack);
	bool Build(CMPtrStringList & tokens, CMStringDictionary & dict, bool & bHasWildcards);

	void Expand(CMGrammarExtract & extract, bool bFull = true);

    void GetWordTransitionList(CMPtrStringList & words1,
                               CMPtrStringList & words2);

private:
    void AddToWordTransitionList(CMPtrStringList & words1,
                                 CMPtrStringList & words2,
								 CMGrammarNode * pFromNode,
								 const CMString & lastToken);

    void ResolveOptionalsAndMultiples();
    void SetPointers();

	CMRuleList m_rules;
	CMRuleList m_publicRules;

}; 



#endif //_MGRMNET_H_


