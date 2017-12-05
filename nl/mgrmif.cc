//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//

#include "nl/mgrmif.h"
#include "nl/mgrmstck.h"
#include "nl/mgrmgen.h"


extern "C" {
INLGRMDLLEXPORT IMGrammarStack * CreateGrammarStack()
{
  return new CMGrammarStack;
}

INLGRMDLLEXPORT void DestroyGrammarStack(IMGrammarStack * p)
{
  delete p;
}


INLGRMDLLEXPORT IMGrammarGenerator * CreateGrammarGenerator()
{
  return new CMGrammarGenerator;
}

INLGRMDLLEXPORT void DestroyGrammarGenerator(IMGrammarGenerator * p)
{
  delete p;
}


INLGRMDLLEXPORT IMGrammarGeneratorExample * CreateGrammarGeneratorExample()
{
  return new CMGrammarGeneratorExample;
}

INLGRMDLLEXPORT void DestroyGrammarGeneratorExample(IMGrammarGeneratorExample * p)
{
  delete p;
}
} //extern "C"





