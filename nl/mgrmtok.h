//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//


#ifndef _MGRMTOK_H_
#define _MGRMTOK_H_

//Keyword definitions

#define NONTERM_OPEN	"<"
#define NONTERM_CLOSE	">"

#define OPTIONAL_OPEN	"["
#define OPTIONAL_CLOSE	"]"

#define TAG_OPEN		"{"
#define TAG_CLOSE		"}"

#define BRACKET_OPEN	"("
#define BRACKET_CLOSE	")"

#define RULE_OR			"|"
#define EQUAL			"="
#define QUOTE_OPEN		"\""
#define QUOTE_CLOSE		"\""

#define RULE_END		";"

#define ONE_OR_MORE		"+"
#define ZERO_OR_MORE	"*"

#define WEIGHT_OPEN		"/"
#define WEIGHT_CLOSE	"/"

//Comments are multiple characters
#define COMMENT_OPEN	"/*"
#define COMMENT_CLOSE	"*/"
#define COMMENT_EOL		"//"


#define IMPORT_GRAMMAR "import"
#define INCLUDE_OPEN	'<'
#define INCLUDE_CLOSE	'>'

#define PUBLIC_RULE "public"

#include "util/mutil.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class CMGrammarTokenizer
{
public:
	CMGrammarTokenizer();
	virtual ~CMGrammarTokenizer();

	bool Tokenize(CMPtrStringList & result, CMPtrStringList & imports, IMReadStream &, CMString & grmName, bool bSuppressPublicRules = false);

private:
    bool CheckForImport(CMPtrStringList & imports, const CMString & line);
    bool CheckForFirstToken(const CMString & text, const CMString & token);
    bool CheckForToken(CMPtrStringList & result, const CMString & token, 
  					   const CMString & checkToken, long & i, char * pTemp);

    bool IsEndOfComment(const CMString & token);
    bool IsCPPStyleComment(CMString & token);

    bool CheckAndSeeIfItsAComment(CMPtrStringList & result, const CMString & token);

	bool IsGrammarFormatTag(CMString & format, const CMString & line);
};


#endif //_MGRMTOK_H_


