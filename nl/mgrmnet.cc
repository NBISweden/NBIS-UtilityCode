//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//=                                                                =//
//==================================================================//


//#define MLIST_BOUNDSCHECK

#include "nl/mgrmnet.h"
#include "nl/mgrmtok.h"
//#include <iostream.h>
#include "nl/mgrprs.h"
#include "nl/mxtract.h"
#include "util/mutil.h"
#include <iostream>

using namespace std;

bool CMGrammarNode::WildCardParse(const CMString & test)
{
  if (!m_bIsWildCard)
    return false;
  long len = m_token.length();
  long testLen = test.length();
  
  int i=0, j=0;
  int cnt = 0;
  const char * pToken = (const char*)m_token;
  const char * pTest =  (const char*)test;
  while (i<len && j<testLen) {
    if (pToken[i] == pTest[j] || pToken[i] == GRMNODE_WILDCARD) {
      i++;
      cnt++;
      j++;
      continue;
    }
    if (i > 0 && pToken[i-1] == GRMNODE_WILDCARD) {
      //i--;
      j++;
      cnt++;
    } else {
      return false;
    }
  } 

  return true;
}

void CMGrammarNode::Expand(CMGrammarExtract & extract, CMParseStack * pStack)
{
  //MGG: try this...
  if (extract.IsExpansionCountLimit())
	return;

  if (IsEnd()) {
    CMGrammarNode * pStackNode = pStack->PopReturnNode();
    if (pStackNode != NULL) {
      pStackNode->Expand(extract, pStack);
      pStack->PushReturnNode(pStackNode);
	} else {
	  extract.Add();
	}
	return;
  }

  if (IsTerminal()) {
	if (!IsDummy())
	  extract.PushToken(GetToken());

    pStack->Push(this, false);

	if (m_pNextNode != NULL)
	  pStack->PushReturnNode(m_pNextNode);

	for (int i=0; i<GetReferenceCount(); i++) {
	  CMGrammarNode * pNode = GetNode(i);
  	  pNode->Expand(extract, pStack);
	}

	pStack->Pop(false);
	if (m_pNextNode != NULL)
	  pStack->PopReturnNode();

	if (!IsDummy())
      extract.PopToken();
	
  } else {
    //Not a terminal...
	if (extract.IsRecursion(GetToken()))
	  return;

    pStack->Push(this, false);
    extract.PushRule(GetToken());
	if (m_pNextNode != NULL)
	  pStack->PushReturnNode(m_pNextNode);

	for (int i=0; i<GetReferenceCount(); i++) {
	  CMGrammarNode * pNode = GetNode(i);
	  pNode->Expand(extract, pStack);
	}
	pStack->Pop(false);

	if (m_pNextNode != NULL)
	  pStack->PopReturnNode();

	//pStack->Pop(false);
    extract.PopRule();
  }

}


bool CMGrammarNode::DoesParse(CMParseStack * pStack)
{


  const CMString & token = pStack->GetCurrentToken();

  if (IsEnd()) { 
    if (pStack->IsEndOfTokens()) {
      CMGrammarNode * pStackNode = pStack->PopReturnNode();
      if (pStackNode != NULL) {
        bool bSuccess = pStackNode->DoesParse(pStack);
        pStack->PushReturnNode(pStackNode);
        return bSuccess;
      } else {
        pStack->Freeze();
	return true;
      }
    } else {
      CMGrammarNode * pStackNode = pStack->PopReturnNode();
      if (pStackNode == NULL) {
	return false;
      } else {
        bool bSuccess = pStackNode->DoesParse(pStack);
        pStack->PushReturnNode(pStackNode);
        return bSuccess;
      }
    }
  }
  
  if (IsTerminal()) {
    if (pStack->IsEndOfTokens())
      return false;
    
    if (IsDummy() || GetToken() == token || WildCardParse(token)) {
      //OPTIONAL HACK
      //if (IsDummy() || GetToken() == token || IsOptional() || IsOptionalContd()) {
      //bool bPush = false;
      //if (!IsOptionalContd() && (GetToken() == token || IsDummy())) {
      pStack->Push(this);
      //bPush = true;
      // }
      
      if (m_pNextNode != NULL)
	pStack->PushReturnNode(m_pNextNode);
      
      for (int i=0; i<GetReferenceCount(); i++) {
	CMGrammarNode * pNode = GetNode(i);
	
	
	if (pNode->DoesParse(pStack)) {
	  pStack->Pop();
	  if (m_pNextNode != NULL)
	    pStack->PopReturnNode();
	  return true;
	}
      }
      //Nope....
      //if (bPush)
      pStack->Pop();
      
      if (m_pNextNode != NULL)
	pStack->PopReturnNode();
      
      return false;
    }
    
    return false;
  } else {
    //Not a terminal...
    pStack->Push(this, false);
    
    if (m_pNextNode != NULL)
      pStack->PushReturnNode(m_pNextNode);
    
    for (int i=0; i<GetReferenceCount(); i++) {
      CMGrammarNode * pNode = GetNode(i);
      if (pNode->DoesParse(pStack)) {
	pStack->Pop(false);
	return true;
      }
    }
    
    if (m_pNextNode != NULL)
      pStack->PopReturnNode();
    
    pStack->Pop(false);
    
  }

  return false;

}



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
CMGrammarNetwork::CMGrammarNetwork()
{
}

CMGrammarNetwork::~CMGrammarNetwork()
{
  m_publicRules.removeNoDeleteAll();
}

void CMGrammarNetwork::Expand(CMGrammarExtract & extract, bool bFull)
{
  CMParseStack stack;
  if (!bFull)
	extract.SetExpansionCountLimit(20);
  for (int i=0; i<m_publicRules.length(); i++) {
	m_publicRules(i)->Expand(extract, &stack);
  }
}

bool CMGrammarNetwork::Build(CMPtrStringList & tokens, CMStringDictionary & dict, bool & bHasWildcards)
{
  int i;

  
  CMGrammarNodeList lhsNodes;

  CMPtrStringList formattedTokens;

  CMString newToken;

  //First, let's resolve open/close issues
  bool bOpen = false;
  for (i=0; i<tokens.length(); i++) {
    const CMString & token = *(tokens(i));
	if (bOpen) {
	  if (token == NONTERM_CLOSE
				//|| token == TAG_CLOSE
				|| token == QUOTE_CLOSE
				|| token == WEIGHT_CLOSE ) {
        bOpen = false;
        CMString * pNewToken = new CMString;
	    *pNewToken = newToken;
	    formattedTokens.add(pNewToken);
        if (token != QUOTE_CLOSE) {
          pNewToken = new CMString;
	      *pNewToken = token;
	      formattedTokens.add(pNewToken);
		}
		continue;
	  }
	} else {
      //Just add it
	  if (token != QUOTE_OPEN) {
        CMString * pNewToken = new CMString;
	    *pNewToken = token;
	    formattedTokens.add(pNewToken);
	  }
	
	}

	newToken += token;
    
	if (!bOpen &&  (token == NONTERM_OPEN
			//|| token == TAG_OPEN
			|| token == QUOTE_OPEN
			|| token == WEIGHT_OPEN) ) {
      bOpen = true;
      newToken = "";
	}
  }


  //cout << "Number of tokens: " << formattedTokens.length() << endl;
  //for (i=0; i<formattedTokens.length(); i++) {
    //const CMString &theToken = *formattedTokens(i);
	//cout << (const char*)theToken << endl;
  //}


  //Next, build the list of LHS non-terminals...
  bool bLHS = true;
  bool bRHS = false;
  bool bRHSNonTerm = false;
  bool bTag = false;
  bool bOptional = false;
  //bool bOptionalDelayed = false;
  bool bOptionalContd = false;
  CMString nonTermToken;

  CMGrammarNodeList * pCurrentRHSRule = NULL;
  CMGrammarRule * pCurrentRule = NULL;
  CMGrammarNode * pCurrentNode = NULL;

  for (i=0; i<formattedTokens.length(); i++) {
    const CMString & token = *(formattedTokens(i));
	
	if (bLHS && token == NONTERM_CLOSE) {
      bLHS = false;

      //Found a left hand side non-terminal!!
	  pCurrentRule = new CMGrammarRule;
	  m_rules.add(pCurrentRule);

	  if (i>=3 && *formattedTokens(i-3) == PUBLIC_RULE)
	    m_publicRules.add(pCurrentRule);

	  pCurrentRule->SetName(nonTermToken);

      pCurrentRHSRule = pCurrentRule->AddRHSRule();

	  continue;
	}

	if (!bLHS && token == EQUAL) {
	  bRHS = true;
	  continue;
	}


	if (token == OPTIONAL_OPEN) {
      bOptional = true;
	  continue;
	}

	if (token == OPTIONAL_CLOSE) {
      bOptional = false;
	  bOptionalContd = false;
	  //bOptionalDelayed = false;
	  continue;
	}

	if (bRHS && token == NONTERM_OPEN) {
	  bRHSNonTerm = true;
      nonTermToken = "";
	  continue;
	}

	if (bRHS && token == TAG_OPEN) {
      bTag = true;
      nonTermToken = "";
	  continue;
	}

	if (bRHS && token == NONTERM_CLOSE) {
	  bRHSNonTerm = false;
      pCurrentNode = new CMGrammarNode;
   //pCurrentNode->SetTerminal();
      pCurrentNode->SetToken(nonTermToken);
  	  pCurrentNode->SetOptional(bOptional);
	  if (bOptionalContd)
        pCurrentNode->SetOptionalContd();
      bOptionalContd = bOptional;
      pCurrentRHSRule->add(pCurrentNode);
      nonTermToken = "";
	  continue;
	}

	if (bRHS && token == TAG_CLOSE) {
	  pCurrentNode->SetTag(nonTermToken);
      bTag = false;
	  continue;
	}

	if (bRHS && token == RULE_OR) {
      pCurrentRHSRule = pCurrentRule->AddRHSRule();
	  continue;
	}


	if (bLHS && token == NONTERM_OPEN) {
      nonTermToken = "";
	  continue;
	}

	if (!bLHS && token == RULE_END) {
      bRHS = false;
	  bLHS = true;
	  continue;
	}

	if (!bRHSNonTerm && bRHS) {
	  if (bTag) {
		pCurrentNode->SetTag(token);
	  } else {
        pCurrentNode = new CMGrammarNode;
	    pCurrentNode->SetTerminal();
	    pCurrentNode->SetToken(token);
		dict.AddWord(pCurrentNode->GetToken());
		if (pCurrentNode->IsWildcard())
		  bHasWildcards = true;

		pCurrentNode->SetOptional(bOptional);
	    if (bOptionalContd)
          pCurrentNode->SetOptionalContd();
        bOptionalContd = bOptional;
        pCurrentRHSRule->add(pCurrentNode);
	    nonTermToken = "";
	  }
	}
	nonTermToken += token;
  }


  /*bool bTerminalFlag;
  //Let's go for the terminals now...
  for (i=0; i<formattedTokens.length(); i++) {
    const CMString & token = *(formattedTokens(i));
    if (token == NONTERM_OPEN) {
	  bTerminalFlag = false;
	}
	if (token == OPTIONAL_OPEN)

	if (token == TAG_OPEN)

	if (token == BRACKET_OPEN)
	if (token == BRACKET_CLOSE)

	if (token == EQUAL)
	if (token == QUOTE_OPEN)

	if (token == RULE_END)

	if (token == ONE_OR_MORE)
	if (token == ZERO_OR_MORE)

	if (token == WEIGHT_OPEN)
  
  
  } */
  ResolveOptionalsAndMultiples();

  SetPointers();

  return true;
}

void CMGrammarNetwork::ResolveOptionalsAndMultiples()
{
  int j;
  for (int i=0; i<m_rules.length(); i++) {
    //CMRuleList & ruleList = *rules(i);
	CMRHSRuleList & ruleList = (m_rules(i)->GetNodeList());

	for (int l=0; l<ruleList.length(); l++) {
	  CMGrammarNodeList & nodeList = *(ruleList(l));
  	  long firstOptionalIndex = -1;
	  //long lastOptionalIndex = -1;
	  bool bCopyFlag = false;

      for (j=0; j<nodeList.length(); j++) {
		CMGrammarNode * pNode = nodeList(j);

		if (pNode->IsOptional()) {
		  if (firstOptionalIndex != -1) {
            bCopyFlag = true;
		  } else {
		    firstOptionalIndex = j;
		  }
		  //lastOptionalIndex = j;
		  //if (!pNode->IsTerminal())
			//bCopyFlag = true;
		}

		//if (pNode->IsOptionalContd()) {
		//  lastOptionalIndex = j;
		//  if (!pNode->IsTerminal())
		//	bCopyFlag = true;
		//}
		if (!pNode->IsOptional() && !pNode->IsOptionalContd() && firstOptionalIndex != -1)
		  bCopyFlag = true;

		if (bCopyFlag) {
		//if (bCopyFlag) {
          CMGrammarNodeList * pAddedList = new CMGrammarNodeList;
		  *pAddedList = nodeList;
		  for (int k=firstOptionalIndex; k<j; k++)
            pAddedList->remove(firstOptionalIndex);
		  ruleList.add(pAddedList);

		  bCopyFlag = false;
    	  firstOptionalIndex = -1;
	      //lastOptionalIndex = -1;
		}
		if (pNode->IsOptional()) {
  	      firstOptionalIndex = j;
		}
	  }
      //Remaining optionals....
	  if (firstOptionalIndex != -1) {
        CMGrammarNodeList * pAddedList = new CMGrammarNodeList;
	    *pAddedList = nodeList;
	    for (int k=firstOptionalIndex; k<j; k++)
          pAddedList->remove(firstOptionalIndex);
	    ruleList.add(pAddedList);

	    bCopyFlag = false;
        firstOptionalIndex = -1;
	  }

      //Remaining optionals....

	}
  }
}

void CMGrammarNetwork::SetPointers()
{
  for (int i=0; i<m_rules.length(); i++) {
    //CMRuleList & ruleList = *rules(i);
	CMRHSRuleList & ruleList = (m_rules(i)->GetNodeList());
	for (int l=0; l<ruleList.length(); l++) {
	  CMGrammarNodeList & nodeList = *(ruleList(l));

	  CMGrammarNode * pEndNode = new CMGrammarNode;
	  pEndNode->SetEnd();
	  pEndNode->SetTerminal();
	  nodeList.add(pEndNode);
	  CMGrammarNode * pDummyStartNode = new CMGrammarNode;
	  pDummyStartNode->SetDummy();

	  nodeList.insert(pDummyStartNode, 0);


	  CMGrammarNode * pLastNonOptionalNode = NULL;
	  long lastNonOptionalNodeIndex = 0;
	  bool bWasOptional = false;
	  for (int j=0; j<nodeList.length()-1; j++) {   
	    //const CMString & theToken = nodeList(j)->GetToken();
		if (nodeList(j+1)->IsTerminal()) {
          nodeList(j)->ReferenceNode(nodeList(j+1));

		  //OPTIONAL
		  /*if (nodeList(j)->IsOptional() && !nodeList(j+1)->IsOptionalContd()) {
		    for (int k=lastNonOptionalNodeIndex; k<j; k++) {
			  if (!nodeList(k)->IsOptionalContd())
			    nodeList(k)->ReferenceNode(nodeList(j+1));
			}
		  }*/
            //pLastNonOptionalNode->ReferenceNode(nodeList(j+1));
		  //continue;
		} else {

	      if (!nodeList(j+1)->IsTerminal()) {
		    CMGrammarNode * pRule = NULL;
		    nodeList(j)->SetNextNode(nodeList(j+1));
		    const CMString & searchToken = nodeList(j+1)->GetToken();
		    for (int k=0; k<m_rules.length(); k++) {
		      if (m_rules(k)->GetName() == searchToken) {
			    pRule = m_rules(k);
			    break;
			  }  
			}  
		    if (pRule == NULL) {
		      //cout << "Grammar Error: rule not found - " << (const char*)searchToken << endl;
			  ThrowException("(NLGRAM) Grammar Error: rule not found:", searchToken);
			} 
            nodeList(j)->ReferenceNode(pRule);
			//OPTIONAL
			/*if (nodeList(j)->IsOptional() && !nodeList(j+1)->IsOptionalContd()) {
			  for (int k=lastNonOptionalNodeIndex; k<j; k++) {
				if (!nodeList(k)->IsOptionalContd())
				  nodeList(k)->ReferenceNode(pRule);
			  }
			}*/
		  }
		}

        if (!nodeList(j)->IsOptional()) {
		  //if (bWasOptional) {
            //pLastNonOptionalNode->ReferenceNode(nodeList(j));
			//Optional??
		  //}

		  pLastNonOptionalNode = nodeList(j);

		  lastNonOptionalNodeIndex = j;
		  bWasOptional = false;
		} else {
		  bWasOptional = true;
		}
	  }
	}
  }
}

bool CMGrammarNetwork::DoesParse(CMParseStack * pStack)
{

  for (int i=0; i<m_publicRules.length(); i++) {
    if (m_publicRules(i)->DoesParse(pStack)) {
      pStack->SetPublicRuleName(m_publicRules(i)->GetName());  
      return true;
    }
  }

  return false;
}

void CMGrammarNetwork::AddToWordTransitionList(CMPtrStringList & words1,
                                               CMPtrStringList & words2,
			             					   CMGrammarNode * pFromNode,
								               const CMString & lastToken)
{
  if (pFromNode->IsDummy()) {
	for (int l=0; l<pFromNode->GetReferenceCount(); l++) {
	  CMGrammarNode * pNode = pFromNode->GetNode(l);
  	  if (!pNode->IsDummy() && pNode->IsTerminal()) {
        words1.add(lastToken);
  	    words2.add(pNode->GetToken());
	  } else {
        AddToWordTransitionList(words1,
                                words2,
 	                   	        pNode,
							    lastToken);
	  }
	}
  }

  if (pFromNode->GetNodeListOrNot() == NULL)
	return;

  CMRHSRuleList & ruleList = *pFromNode->GetNodeListOrNot();


  for (int j=0; j<ruleList.length(); j++) {
    CMGrammarNodeList & nodeList = *(ruleList(j));
    if (nodeList.length() > 0) {
	  CMGrammarNode & node = *nodeList(0);
  	  if (!node.IsDummy() && node.IsTerminal()) {
        words1.add(lastToken);
  	    words2.add(node.GetToken());
	  } else {
        AddToWordTransitionList(words1,
                                words2,
		 	             	    &node,
							    lastToken);
 	  
	  }
	}
  } 


}

void CMGrammarNetwork::GetWordTransitionList(CMPtrStringList & words1,
                                             CMPtrStringList & words2)
{

  for (int i=0; i<m_rules.length(); i++) {
	CMRHSRuleList & ruleList = (m_rules(i)->GetNodeList());
	for (int j=0; j<ruleList.length(); j++) {
	  CMGrammarNodeList & nodeList = *(ruleList(j));
	  CMString token;
	  CMString lastToken;
	  for (int k=0; k<nodeList.length(); k++) {

	    CMGrammarNode & node = *nodeList(k);
		if (!node.IsDummy() && node.IsTerminal()) {
		  token = node.GetToken();
		  if (lastToken != "") {
		    words1.add(lastToken);
			words2.add(token);
		  }
		  lastToken = token;
		}

		for (int l=0; l<node.GetReferenceCount(); l++) {
		  CMGrammarNode * pNode = node.GetNode(l);
          AddToWordTransitionList(words1,
                                  words2,
			                   	  pNode,
							      lastToken);
		}
	  }
	}
  }
}



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

CMGrammarRule::CMGrammarRule()
{
  m_bIsActive = true;
}

CMGrammarRule::~CMGrammarRule()
{
}

void CMGrammarRule::Expand(CMGrammarExtract & extract, CMParseStack * pStack)
{
  for (int i=0; i<m_ruleList.length(); i++) {
    CMGrammarNodeList & list = *m_ruleList(i);
	list(0)->Expand(extract, pStack);
  }
}

bool CMGrammarRule::DoesParse(CMParseStack * pStack)
{
  for (int i=0; i<m_ruleList.length(); i++) {
    CMGrammarNodeList & list = *m_ruleList(i);
    if (list(0)->DoesParse(pStack))
      return true;
  }
  
  return false;
}


CMGrammarNodeList * CMGrammarRule::AddRHSRule()
{
  CMGrammarNodeList * p = new CMGrammarNodeList;
  m_ruleList.add(p);
  return p;
}









