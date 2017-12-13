//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//




#ifndef _MGRMSTCK_H_
#define _MGRMSTCK_H_

//class CMGrammar;
#include "nl/mcnlgrm.h"
#include "nl/mgrmif.h"
#include "util/mutil.h"

typedef TMPtrList<CMGrammar> CMGrammarList;

class CMGrammarStack : public IMGrammarStack
{
	friend class CMGrammarGenerator;
public:
	//c'tor
	MDLLEXPORT CMGrammarStack();
	//d'tor
	MDLLEXPORT virtual ~CMGrammarStack();

	//How many grammars are loaded?
    MDLLEXPORT long GetGrammarCount();
	//Get 'em
	MDLLEXPORT CMGrammar * GetGrammar(long i);

	//Add a grammar that YOU allocated (CMGrammarStack will destroy it for you...)
	MDLLEXPORT void AddGrammar(CMGrammar * pGrammar);
	//A brand new grammar
	MDLLEXPORT CMGrammar * NewGrammar();

	//Three ways to get rid of grammars
	MDLLEXPORT bool DestroyGrammar(CMGrammar * pGrammar);
	virtual MDLLEXPORT bool DestroyGrammar(GRAMMAR_HANDLE handle);
	virtual MDLLEXPORT void DestroyAllGrammars();

	//Activate/deactivate
	virtual MDLLEXPORT bool ActivateGrammar(GRAMMAR_HANDLE handle, bool bActivate = true);

	//Find one by handle
	MDLLEXPORT CMGrammar * FindGrammar(GRAMMAR_HANDLE handle);

	//Three convenient ways to load them
	virtual MDLLEXPORT GRAMMAR_HANDLE AddGrammaAndReadFromFile(const char * fileName);
	virtual MDLLEXPORT GRAMMAR_HANDLE AddGrammaAndReadFromURL(const char * fileName);
	MDLLEXPORT GRAMMAR_HANDLE AddGrammaAndReadTextFormat(IMReadStream & stream);

	//Parse and get the details
	MDLLEXPORT GRAMMAR_HANDLE Parse(CMTagList & tags, 
					CMTokenList & tokens,
					const CMString & text);
	
	//Parse and get the result as action tags
	MDLLEXPORT GRAMMAR_HANDLE ParseAndEvaluate(CMPtrStringList & result,
						   const CMString & text,
						   bool bFuzzy = false);
	
	//Parse and get the result in one single string separated by ';'
	virtual MDLLEXPORT GRAMMAR_HANDLE ParseAndEvaluate(char * result,
							   const char * text, 
							   long maxLen,
							   bool bFuzzy = false);

	virtual GRAMMAR_ERROR GetLastError();
	virtual const char * GetLastErrorMessage();

    MDLLEXPORT void Expand(CMGrammarExtract & extract);

	MDLLEXPORT void GetWordList(CMPtrStringList & words);
	MDLLEXPORT void GetWordTransitionList(CMPtrStringList & words1,
                                          CMPtrStringList & words2);

private:
	virtual CMGrammarStack * GetGrammarStackImp() {return this;}

    bool FormatResult(CMTagList & tags, 
		              CMTokenList & tokens,
					  CMPtrStringList & result);

    bool GetDollarValue(CMTagList & tags, 
		                CMTokenList & tokens,
					    CMString & dollarValue);

    const CMString & GetPublicRuleName(CMTagList & tags, 
		                              CMTokenList & tokens);

    void SetLastGrmError(GRAMMAR_ERROR err, const CMString & errMsg);
    void SetLastGrmError(GRAMMAR_ERROR err, const CMString & errMsg, const CMString & errArg);
    void SetLastGrmError(GRAMMAR_ERROR err, const CMString & errMsg, long errNum);

	CMGrammarList m_grammarList;
	GRAMMAR_HANDLE m_lastHandle;
	CMString m_ruleName;

	GRAMMAR_ERROR m_lastError;
	CMString m_lastErrorMessage;
};

#endif //_MGRMSTCK_H_


