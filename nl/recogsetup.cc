#include "nl/mgrmstck.h"
#include "nl/mgrmgen.h"
#include "nl/mxtract.h"


#include "util/mutil.h"
//#include <mrwfile.h>
//#include <mstrprs.h>
//#include <mfhdr.h>

#include "nl/recogsetup.h"
#include <iostream>

using namespace std;


CBackupDict::CBackupDict()
{
}

CBackupDict::~CBackupDict()
{
}


void CBackupDict::LoadFromTextFile(const CMString & fileName)
{  
  CMAsciiReadFileStream dict;
  dict.Open(fileName);

  long count = 0;
  while (!dict.IsEnd()) {
    CMString line;
	dict.ReadLine(line);
	/*if (count % 2 == 1) {
	  count++;
	  continue;
	}*/

	CMPtrStringList tokens;
	Tokenize(tokens, line);
	CMString prn;
	for (int i=1; i<tokens.length()/*-1*/; i++) {
	  if (i>1)
        prn += " ";
	  prn += *tokens(i);
	}
	if (prn == "") 
	  continue;
	tokens(0)->toLower();
    m_backupGraphemes.AddWordDontCheck(*tokens(0));
    m_backupProns.AddWordDontCheck(prn);
	if (count % 100 == 0) 
	  cout << "Loading dict: " << count << endl;
	count++;
  }

  dict.Close();
}

void CBackupDict::Load(const CMString & fileName)
{
  CMReadFileStream stream;
  stream.Open(fileName);

  CMFileHeader header;
  header.Read(stream);

  m_backupGraphemes.Read(stream);
  m_backupProns.Read(stream);

  stream.Close();
}

void CBackupDict::Save(const CMString & fileName)
{
  CMWriteFileStream stream;
  stream.Open(fileName);

  CMFileHeader header;
  header.Write(stream);

  m_backupGraphemes.Write(stream);
  m_backupProns.Write(stream);

  stream.Close();
}



bool CBackupDict::AddPronunciations(CMStringDictionary & newDict,
					                CMStringDictionary & newProns,
					                const CMString & grapheme)
{
  CMInt32List words;
  m_backupGraphemes.GetDictIDList(words, grapheme);
  if (words.length() == 0)
	return false;

  for (int i=0; i<words.length(); i++) {
    newDict.AddWordDontCheck(grapheme);
    newProns.AddWordDontCheck(m_backupProns.GetWord(words(i)));
  }
  return true;
}


//======================================================================
//======================================================================
//======================================================================
CRecognizerSetup::CRecognizerSetup()
{
  m_dataFileName = "dictdata.h";
  m_dictFileName = "dictdata.prons";
}

CRecognizerSetup::~CRecognizerSetup()
{
}


void CRecognizerSetup::AddBackupDict(const CMString & fileName)
{
  CBackupDict * pDict = new CBackupDict;
  m_backupDicts.add(pDict);
  pDict->Load(fileName);
}

void CRecognizerSetup::AddBackupDictAscii(const CMString & fileName)
{
  CBackupDict * pDict = new CBackupDict;
  m_backupDicts.add(pDict);
  pDict->LoadFromTextFile(fileName);
}


void CRecognizerSetup::SetDataFileName(const CMString & fileName)
{
  m_dataFileName = fileName;
}

void CRecognizerSetup::SetDictFileName(const CMString & fileName)
{
  m_dictFileName = fileName;
}


bool CRecognizerSetup::DumpOutFiles(CMGrammarBigramExtract & bigrams, CMPtrStringList & dict)
{
  CMStringDictionary strDict;
  CMStringDictionary bigramDict;

  CMStringDictionary strDictProns;


  int i;
  int j, k;

  //Sort the dictionary
  CMStringList dictOut;
  CMStringList prnsOut;

  for (i=0; i<dict.length(); i++) {

	for (j=0; j<m_backupDicts.length(); j++) {
      if (m_backupDicts(j)->AddPronunciations(strDict, strDictProns, *dict(i)))
		break;
	}
  }

  dictOut.reshape(strDict.GetWordCount());
  prnsOut.reshape(strDict.GetWordCount());
  for (i=0; i<strDict.GetWordCount(); i++) {
	long mappedID = strDict.IndexToIndex(i);
	dictOut(i) = strDict.GetWord(mappedID);
	prnsOut(i) = strDictProns.GetWord(mappedID);
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

  pOut = fopen(m_dataFileName, "w");
  fprintf(pOut, "#ifndef _DICTDATA_H_\n");

  fprintf(pOut, "#define _DICTDATA_H_\n");
  fprintf(pOut, "static long gNWords = %ld;\n", dictOut.length()-1); //Minus silence
  fprintf(pOut, "#define WORDID_MULT 0x10000\n\n");


  fprintf(pOut, "const char * const gDict_Words[] = {\n");
  CMAsciiWriteFileStream prns;
  prns.Open(m_dictFileName);

  CMInt32List unigrams;
  unigrams.reshape(dictOut.length()+1);
  for (i=0; i<dictOut.length(); i++) {
	CMString entry = dictOut(i);
	entry += " ";
	entry += prnsOut(i);
	prns.WriteLine(entry);
	fprintf(pOut, "  \"%s\",\n", (const char*)dictOut(i));
    unigrams(i) = 900;
  }

  prns.Close();

  fprintf(pOut, "  \"<silence>\",\n");
  unigrams(dictOut.length()) = 900;

  fprintf(pOut, "};\n\n");
  long realBigramCount = 0;

  fprintf(pOut, "const long gDictBigrams[] = {\n");

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
  return true;
}







