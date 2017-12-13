//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//==================================================================//

#ifndef _MIGRMIF_H_
#define _MIGRMIF_H_

#include "nl/grmdefs.h"

class CMGrammarStack;

class IMGrammarStack;
class IMGrammarGenerator;
class IMGrammarGeneratorExample;


#ifdef WIN32
#define INLGRMDLLEXPORT  __declspec (dllexport)
#else //WIN32
#define INLGRMDLLEXPORT
#endif //WIN32


//Create and destroy methods!
extern "C" {
INLGRMDLLEXPORT IMGrammarStack * CreateGrammarStack();
INLGRMDLLEXPORT void DestroyGrammarStack(IMGrammarStack * p);

INLGRMDLLEXPORT IMGrammarGenerator * CreateGrammarGenerator();
INLGRMDLLEXPORT void DestroyGrammarGenerator(IMGrammarGenerator * p);

INLGRMDLLEXPORT IMGrammarGeneratorExample * CreateGrammarGeneratorExample();
INLGRMDLLEXPORT void DestroyGrammarGeneratorExample(IMGrammarGeneratorExample * p);
}
//Remember: always give back your objects if you don't need them anymore.
//Note: There is NO Ref-counting here, if you destroy it, it's gone.

enum GRAMMAR_ERROR
{
	GRAMMAR_ERROR_SUCCESS,
	GRAMMAR_ERROR_TIMEOUT,
 	GRAMMAR_ERROR_LOADING_GRAMMAR,
 	GRAMMAR_ERROR_FILE_NOT_FOUND,
 	GRAMMAR_ERROR_URL_NOT_FOUND,
 	GRAMMAR_ERROR_INVALID_SYNTAX,
 	GRAMMAR_ERROR_UNRESOLVED_EXTERNAL,
 	GRAMMAR_ERROR_INVALID_MMPORT,
 	GRAMMAR_ERROR_INVALID_GRAMMAR_HANDLE,
 	GRAMMAR_ERROR_INTERNAL_FATAL
};



/////////////////////////////////////////////////////////////////////////////////////
class IMGrammarStack
{
	friend class CMGrammarGenerator;

public:
	virtual ~IMGrammarStack() {}

	//Two ways to get rid of grammars
	virtual bool DestroyGrammar(GRAMMAR_HANDLE handle) = 0;
	virtual void DestroyAllGrammars() = 0;

	//Activate/deactivate
	virtual bool ActivateGrammar(GRAMMAR_HANDLE handle, bool bActivate = true) = 0;

	//Two convenient ways to load them
	virtual GRAMMAR_HANDLE AddGrammaAndReadFromFile(const char * fileName) = 0;
	virtual GRAMMAR_HANDLE AddGrammaAndReadFromURL(const char * urlName) = 0;

	//Parse and get the result in one single string separated by ';'
    virtual GRAMMAR_HANDLE ParseAndEvaluate(char * result,
					    const char * text,
					    long maxLen,
					    bool bFuzzy = false) = 0;


	virtual GRAMMAR_ERROR GetLastError() = 0;
	//Can be NULL!
	virtual const char * GetLastErrorMessage() = 0;

protected:
	virtual CMGrammarStack * GetGrammarStackImp() = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
class IMGrammarGeneratorExample
{
public:
	virtual ~IMGrammarGeneratorExample() {}
    
	//Plain text
	virtual const char * GetText() const = 0;
	virtual void SetText(const char * text) = 0;

	//What is it supposed to do...?
	virtual void AddSlotValuePair(const char * slot, const char * value, const char * grammarName = "") = 0;

	virtual void RemoveLastSlotValuePair() = 0;

	virtual long GetSlotValuePairCount() const = 0;
	virtual const char * GetSlot(long i) const = 0;
	virtual const char * GetValue(long i) const = 0;
	virtual const char * GetGrammar(long i) const = 0;

private:
};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

class IMGrammarGenerator
{
public:
	virtual ~IMGrammarGenerator() {}

	//Pass in the grammar stack (needed for form level grammars)
	//Note: Do NOT destroy the grammar stack before you're done here!
	virtual void SetGrammarStack(IMGrammarStack * p) = 0;

	//When you're done, save it as a text-format grammar
	//NOTE: the prefix is really a prefix and NOT the entire grammar name!
	virtual bool WriteGrammarToFile(const char * filePathAndName, 
			 					    const char * grammarNamePrefix) = 0;

	//Feed an example
	virtual bool AddExample(const IMGrammarGeneratorExample & example) = 0;

	virtual long GetLastAmbiguityCount() = 0;
	virtual const char * GetLastAmbiguityGrammar(long i) = 0;

	virtual bool UndoLastExample() = 0;
   
	//Simple rule
	virtual bool AddSimpleExample(const char * in, const char * out = "") = 0;
	
	//Get the last rule generated...
	virtual const char * GetLastRule() const = 0;

private:
};




#endif //_MIGRMIF_H_


