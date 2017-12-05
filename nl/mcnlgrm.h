//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//==================================================================//



#ifndef _MCNLGRM_H_
#define _MCNLGRM_H_


//#include "mstd.h"
#include "util/mutil.h"
//#include "mstrprs.h"
#include "nl/grmdefs.h"
//#include "mstrdct.h"


typedef CMPtrStringList CMTagList;
typedef CMPtrStringList CMTokenList;

class CMGrammarTokenizer;
class CMGrammarNetwork;
class CMGrammarParser;
class CMScriptInterpreter;

class CMGrammarExtract;
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
class CMGrammar
{
public:
	MDLLEXPORT CMGrammar();
	MDLLEXPORT virtual ~CMGrammar();

	MDLLEXPORT virtual bool ReadFromFile(const CMString & fileName);
	MDLLEXPORT virtual bool ReadFromURL(const CMString & urlName);
	MDLLEXPORT virtual bool ReadTextFormat(IMReadStream & stream);

    MDLLEXPORT virtual bool Parse(CMTagList & tags, 
			  		              CMTokenList & tokens,
					              const CMString & text);

    MDLLEXPORT virtual bool ParseEx(CMTagList & tags, 
			  		                CMTokenList & tokens,
                                    CMString & publicRuleName,
					                const CMString & text);

	MDLLEXPORT virtual void SetDebugFlag(bool b = true);

	MDLLEXPORT GRAMMAR_HANDLE GetHandle() {return m_handle;}
	MDLLEXPORT void SetHandle(GRAMMAR_HANDLE h) {m_handle = h;}

	MDLLEXPORT bool IsActive() {return m_bIsActive;}
	MDLLEXPORT void SetActive(bool b) {m_bIsActive = b;}

	MDLLEXPORT const CMString & GetName() {return m_name;}

	MDLLEXPORT void Expand(CMGrammarExtract & extract, bool bFull = true);

	MDLLEXPORT void GetWordList(CMPtrStringList & words);

	MDLLEXPORT void GetWordTransitionList(CMPtrStringList & words1,
                                          CMPtrStringList & words2);

private:

	CMGrammarNetwork * m_pGrammar;
	CMGrammarParser * m_pParser;
	CMScriptInterpreter * m_pInterpreter;
	bool m_bDebugFlag;
	bool m_bIsActive;
	GRAMMAR_HANDLE m_handle;
	CMString m_name;

	bool m_bDontUseDict;
	CMStringDictionary m_dict;
};

#endif //_MCNLGRM_H_



