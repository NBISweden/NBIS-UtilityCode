//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//



#ifndef _MGRMGEN_H_
#define _MGRMGEN_H_


#include "nl/mgrmstck.h"
#include "nl/mgrmif.h"

/////////////////////////////////////////////////////////////////////////////////////
class CMGrammarGeneratorExample : public IMGrammarGeneratorExample
{
public:
	MDLLEXPORT CMGrammarGeneratorExample();
	virtual MDLLEXPORT ~CMGrammarGeneratorExample();
    
	//Plain text
	MDLLEXPORT virtual const char * GetText() const {return m_text;}
	MDLLEXPORT virtual void SetText(const char * text);

	//What is it supposed to do
	MDLLEXPORT virtual void AddSlotValuePair(const char * slot, const char * value, const char * szGrammarName = "");
	MDLLEXPORT virtual void RemoveLastSlotValuePair();

	//Get the stuff out again
	MDLLEXPORT virtual long GetSlotValuePairCount() const {return m_valueList.length();}
	MDLLEXPORT virtual const char * GetSlot(long i) const ;
	MDLLEXPORT virtual const char * GetValue(long i) const ;
	MDLLEXPORT virtual const char * GetGrammar(long i) const ;

private:
	CMString m_text;
	CMPtrStringList m_slotList;
	CMPtrStringList m_valueList;
	CMPtrStringList m_grammarNameList;
};

/////////////////////////////////////////////////////////////////////////////////////
class CMGrammarGenerator : public IMGrammarGenerator
{
public:
	MDLLEXPORT CMGrammarGenerator();
	virtual MDLLEXPORT ~CMGrammarGenerator();

	//Needs all field-level grammars to do its job; if there's NO stack, then it'll
	//be a "field level" grammar...
	MDLLEXPORT virtual void SetGrammarStack(IMGrammarStack * p);

	MDLLEXPORT CMGrammarStack * GetGrammarStack() {return m_pGrammarStack;}

	//When you're done, save it as a text-format grammar
	MDLLEXPORT virtual bool WriteGrammarToFile(const char * filePathAndName, 
									           const char * grammarNamePrefix);

	//Feed an example
	MDLLEXPORT virtual bool AddExample(const IMGrammarGeneratorExample & example);
	MDLLEXPORT virtual bool UndoLastExample();
	MDLLEXPORT virtual bool AddSimpleExample(const char * in, const char * out = "");
	MDLLEXPORT virtual const char * GetLastRule() const {return *m_rules(m_rules.length()-1);}

	MDLLEXPORT virtual long GetLastAmbiguityCount() {return m_lastAmbiguities.length();}
	MDLLEXPORT virtual const char * GetLastAmbiguityGrammar(long i) {return *m_lastAmbiguities(i);}
private:
    GRAMMAR_HANDLE ParseAll(CMTagList & tags, 
			  	            CMTokenList & tokens,
				            const CMString & text,
							const CMString & grmName = "",
							const CMString & resHint = "");

    void AddToImports(const CMString & import);

	CMPtrStringList m_rules;
	CMPtrStringList m_imports;
	CMGrammarStack * m_pGrammarStack;
	CMPtrStringList m_lastAmbiguities;
	CMString m_lastPublicRule;
};

#endif //_MGRMGEN_H_



