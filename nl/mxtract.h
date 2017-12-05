//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//


#ifndef _MXTRACT_H_
#define _MXTRACT_H_

#include "util/mutil.h"
//#include <mstrdct.h>

class CMGrammarExtract
{
public:
	MDLLEXPORT CMGrammarExtract();
	MDLLEXPORT virtual ~CMGrammarExtract();

	MDLLEXPORT void PushToken(const CMString & token);
	MDLLEXPORT void PopToken();
	MDLLEXPORT virtual void Add();

	MDLLEXPORT void PushRule(const CMString & rule);
	MDLLEXPORT void PopRule();
	MDLLEXPORT bool IsRecursion(const CMString & rule);

	MDLLEXPORT virtual long GetExpansionCount();
	MDLLEXPORT virtual const CMString & GetExpansion(long i);

	MDLLEXPORT long SetExpansionCountLimit(long lim);
	MDLLEXPORT bool IsExpansionCountLimit();

protected:

	long m_expLim;
	CMPtrStringList m_searchStack;
	CMPtrStringList m_ruleStack;

	CMPtrStringList m_expansions;
};



class CMGrammarBigramExtract : public CMGrammarExtract
{
public:
	MDLLEXPORT CMGrammarBigramExtract();
	MDLLEXPORT virtual ~CMGrammarBigramExtract();

	MDLLEXPORT void Add();

	MDLLEXPORT virtual long GetExpansionCount();
	MDLLEXPORT virtual const CMString & GetExpansion(long i);

private:
	CMStringDictionary m_dict;
};


#endif //_MXTRACT_H_
