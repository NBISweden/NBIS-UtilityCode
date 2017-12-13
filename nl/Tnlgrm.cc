#include <iostream>
#include "util/mutil.h"

#include "nl/mgrmstck.h"
#include "nl/mgrmgen.h"
#include "nl/mxtract.h"

#include "nl/recogsetup.h"

#include <stdio.h>

//#include 
//#include <mstrprs.h>
using namespace std;

void LoadBackupDict(CMStringDictionary & backupDictGraphemes, CMStringDictionary & backupDictProns)
{
  CMAsciiReadFileStream dict;
  dict.Open("..\\data\\us_04_01_2000.pron");

  long count = 0;
  while (!dict.IsEnd()) {
    CMString line;
	dict.ReadLine(line);
	CMPtrStringList tokens;
	Tokenize(tokens, line);
	CMString prn;
	for (int i=1; i<tokens.length(); i++) {
	  if (i>1)
        prn += " ";
	  prn += *tokens(i);
	}
	if (prn == "") 
	  continue;
    backupDictGraphemes.AddWordDontCheck(*tokens(0));
    backupDictProns.AddWordDontCheck(prn);
	if (count % 100 == 0) 
	  cout << "Loading dict: " << count << endl;
	count++;
  }

  dict.Close();
}

void AddPronunciations(CMStringDictionary & backupDictGraphemes, 
					   CMStringDictionary & backupDictProns,
					   CMStringDictionary & newDict,
					   CMStringDictionary & newProns,
					   const CMString & grapheme)
{
  newDict.AddWordDontCheck(grapheme);
  newProns.AddWordDontCheck("sil");
}


void SortAndDumpOut(CMGrammarBigramExtract & bigrams, CMPtrStringList & dict)
{
  CMStringDictionary strDict;
  CMStringDictionary bigramDict;

  CMStringDictionary backupDictGraphemes;
  CMStringDictionary backupDictProns;

  CMStringDictionary strDictProns;

  LoadBackupDict(backupDictGraphemes, backupDictProns);


  int i;
  //Sort the dictionary
  CMStringList dictOut;
  for (i=0; i<dict.length(); i++) {
    //long newID = strDict.AddWord(*dict(i));
	//long mappedID = strDict.IndexToIndex(newID);
    AddPronunciations(backupDictGraphemes, 
	  		          backupDictProns,
					  strDict,
					  strDictProns,
					  *dict(i));
  }
  dictOut.reshape(strDict.GetWordCount());
  for (i=0; i<strDict.GetWordCount(); i++) {
	long mappedID = strDict.IndexToIndex(i);
	dictOut(i) = strDict.GetWord(mappedID);
  }


  //Sort the bigrams
  CMStringList bigramOut;
  bigramOut.reshape(bigrams.GetExpansionCount());
  for (i=0; i<bigrams.GetExpansionCount(); i++) {
    const CMString & exp = bigrams.GetExpansion(i);
    long newID = bigramDict.AddWord(exp);
	long mappedID = bigramDict.IndexToIndex(newID);
	bigramOut(mappedID) = exp;
  }
  for (i=0; i<bigramDict.GetWordCount(); i++) {
	long mappedID = bigramDict.IndexToIndex(i);
	bigramOut(i) = bigramDict.GetWord(mappedID);
  }


  FILE * pOut;

  pOut = fopen("dictdata.h", "w");
  fprintf(pOut, "#ifndef _DICTDATA_H_\n");

  fprintf(pOut, "#define _DICTDATA_H_\n");
  fprintf(pOut, "static long gNWords = %ld;\n", dictOut.length()-1); //Minus silence
  fprintf(pOut, "#define WORDID_MULT 0x10000\n\n");


  fprintf(pOut, "const char * const gDict_Words[] = {\n");

  CMInt32List unigrams;
  unigrams.reshape(dictOut.length()+1);
  for (i=0; i<dictOut.length(); i++) {
    fprintf(pOut, "  \"%s\",\n", (const char*)dictOut(i) /*, i*/);
    unigrams(i) = 900;
  }

  fprintf(pOut, "  \"<silence>\",\n");
  unigrams(dictOut.length()) = 900;

  fprintf(pOut, "};\n\n");
  long realBigramCount = 0;

  fprintf(pOut, "const long gDictBigrams[] = {\n");

  int j, k;
  CMInt32List realBigrams1;
  CMInt32List realBigrams2;
  for (i=0; i<bigramOut.length(); i++) {
    const CMString & exp = bigramOut(i);
	CMPtrStringList tokens;
	Tokenize(tokens, exp);
	if (tokens.length() != 3) {
	  cout << "Screeeaaaaam!!!!!" << endl;
	  continue;
	}
	const CMString & first = *tokens(0);
	const CMString & second = *tokens(2);
	CMInt32List firstIDs;
	CMInt32List secondIDs;
	CMInt32List trueFirstIDs;
	CMInt32List trueSecondIDs;

	strDict.GetDictIDList(firstIDs, first);
	strDict.GetDictIDList(secondIDs, second);
	//long firstID = strDict.GetDictID(first);
	//long secondID = strDict.GetDictID(second);
	if (secondIDs.length() == 0)
	  continue;

	trueFirstIDs.reshape(firstIDs.length());
	trueSecondIDs.reshape(secondIDs.length());

	for (k=0; k<secondIDs.length(); k++) {
	  for (j=0; j<strDict.GetWordCount(); j++) {
	    if (strDict.IndexToIndex(j) == secondIDs(k)) {
	      trueSecondIDs(k) = j;
		} 
	  }
	}
	if (firstIDs.length() == 0) { 
	  for (k=0; k<trueSecondIDs.length(); k++) {
	    unigrams(trueSecondIDs(k)) = 0;
	  }
	  continue;
	}

	for (k=0; k<firstIDs.length(); k++) {
	  for (j=0; j<strDict.GetWordCount(); j++) {
	    if (strDict.IndexToIndex(j) == firstIDs(k)) {
	      trueFirstIDs(k) = j;
		}
	  }
	}

    for (j=0; j<trueFirstIDs.length(); j++) {
      for (k=0; k<trueSecondIDs.length(); k++) {
		if (realBigramCount >= realBigrams1.length()) {
		  realBigrams1.reshape(realBigramCount + 512);
		  realBigrams2.reshape(realBigramCount + 512);
		} 
  	    realBigrams1(realBigramCount) = trueFirstIDs(j);
		realBigrams2(realBigramCount) = trueSecondIDs(k);
        realBigramCount++;
	  }
	}
  }

  bool bBubble = true;
  do {
    bBubble = true;
    for (j=1; j<realBigramCount; j++) {
	  if (realBigrams1(j-1) > realBigrams1(j)) {
		long tmp1 = realBigrams1(j-1);
		long tmp2 = realBigrams2(j-1);
		realBigrams1(j-1) = realBigrams1(j);
		realBigrams2(j-1) = realBigrams2(j);
		realBigrams1(j) = tmp1;
		realBigrams2(j) = tmp2;
		bBubble = false;
	  }
	}
  } while (!bBubble);

  do {
    bBubble = true;
	for (j=1; j<realBigramCount; j++) {
	  if (realBigrams1(j-1) == realBigrams1(j) && realBigrams2(j-1) > realBigrams2(j)) {
		long tmp2 = realBigrams2(j-1);
		realBigrams2(j-1) = realBigrams2(j);
		realBigrams2(j) = tmp2;
		bBubble = false;
	  }
	}
  } while (!bBubble);


  for (j=0; j<realBigramCount; j++) {
    fprintf(pOut, "%ld * WORDID_MULT + %ld,\n", realBigrams1(j), realBigrams2(j));
  }

  fprintf(pOut, "};\n\nstatic long gNBigrams = %ld;\n\n", realBigramCount);

  fprintf(pOut, "const long gDictUnigrams[] = {\n");
  for (i=0; i<unigrams.length(); i++) {
	fprintf(pOut, "  %ld,\n", unigrams(i));  
  }
  fprintf(pOut, "};\n\n");


  fprintf(pOut, "#endif //_DICTDATA_H_\n");
  
  fclose(pOut);





}



int main( int argc, char** argv )
{
  if (argc == 1) {
    cout << "Usage: tnlgrm grammarfilename1 [grammarfilename2...]" << endl;
    return 0;
  }

  CMGrammarStack grmStack;

  CMGrammarGenerator grmGen;
  grmGen.SetGrammarStack(&grmStack);

  //CMGrammar grm;
  
  try {
    for (int j=1; j<argc; j++) {
      cout << "Loading grammar: " << argv[j] << endl;
      GRAMMAR_HANDLE h = grmStack.AddGrammaAndReadFromFile(argv[j]);
      //grmStack.FindGrammar(h)->SetDebugFlag();
    }
    cout << "Complete!" << endl;
  }
  
  catch(CMException & ex) {
    
    cout << "Exception cought during load!!" << endl;
    ex.Print();
    return 0;
  }
  
  //Expand.....
  /*
    FILE * pWords = fopen("words.txt", "w");
  CMPtrStringList words;
  grmStack.GetWordList(words);
  int kk;
  for (kk=0; kk<words.length(); kk++) {
    const CMString & word = *words(kk);
	fprintf(pWords, "%s\n", (const char*)word);
    cout << (const char*)word << endl;
  }
  fclose(pWords);

  CMGrammarBigramExtract extract;
  grmStack.Expand(extract);
  FILE * pTrans = fopen("trans.txt", "w");
  for (kk=0; kk<extract.GetExpansionCount(); kk++) {
    const CMString & exp = extract.GetExpansion(kk);
	fprintf(pTrans, "%s\n", (const char*)exp);
    cout << (const char*)exp << endl;
  }
  fclose(pTrans);

  //SortAndDumpOut(extract, words);

  //CBackupDict backupDict;
  //backupDict.LoadFromTextFile("..\\data\\vst.dct.mono");
  //backupDict.Save("..\\data\\vst.backup");

  CRecognizerSetup  recogSetup;
  recogSetup.AddBackupDict("..\\data\\vst.backup");
  recogSetup.DumpOutFiles(extract, words);
  */
  int i;


  CMPtrStringList words;
  grmStack.GetWordList(words);
 
  for (i=0; i<words.length(); i++) {
    // cout << *words(i) << endl;
  }

  cout << "Useful hints:" << endl;
  cout << "    To exit, type 'exit'" << endl;
  cout << "    The grammar parser is case sensitive!!" << endl;

  CMTagList tags;
  CMTokenList tokens;


  char szText[512];

  do {
    cout << "Test Multiple Grammars: ";
    cin.getline(szText, 512);
    if (strcmp(szText, "exit") == 0)
      break;
    
    CMPtrStringList result;
    //GRAMMAR_HANDLE h = grmStack.ParseAndEvaluate(result, szText);
    GRAMMAR_HANDLE h = grmStack.ParseAndEvaluate(result, szText);
    
    if (h != -1) {
      cout << "Parsed w/ handle " << h << endl;
      for (int j=0; j<result.length(); j++) {
	cout << ((const char*)*result(j)) << endl;
      }	
    } else {
      cout << "Nope, didn't parse... " << endl;
    }
    //ParseAndPrint(parser, szText);
  } while (1 == 1);
  

  
  do {
    cout << "Test Grammar: ";
    cin.getline(szText, 512);
    if (strcmp(szText, "exit") == 0)
      break;
    tags.removeAll();
    tokens.removeAll();
    if (grmStack.Parse(tags, tokens, szText) != -1) {
      //tokens.
      cout << "REWRITE RESULT:" << endl;
      for (i=0; i<tags.length(); i+=2) {
	const char * pTag = (const char*)*tags(i);
	if (pTag[0] != '$' || (*tags(i)) == "$value")
	  cout << *tags(i) << " = " << *tags(i+1) << endl;
      }
      
      cout << endl;
      cout << "DEBUG INFO: Internal (all) variables..." << endl;
      for (i=0; i<tags.length(); i+=2) {
	cout << *tags(i) << " = " << *tags(i+1) << endl;
      }
      cout << endl;
      
    } else {
      cout << "Nope, didn't parse... " << endl;
    }
    //ParseAndPrint(parser, szText);
  } while (1 == 1);
  

  
  

  return 0;

}

