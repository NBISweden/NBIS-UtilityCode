//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//==================================================================//

#include "nl/mgrmgen.h"
#include "util/mutil.h"
//#include <mlog.h>



CMGrammarGeneratorExample::CMGrammarGeneratorExample()
{
}

CMGrammarGeneratorExample::~CMGrammarGeneratorExample()
{
}

    
void CMGrammarGeneratorExample::SetText(const char * text)
{
  m_text = text;
}

void CMGrammarGeneratorExample::AddSlotValuePair(const char * slot, const char * value, const char * szGrammarName)
{
  m_slotList.add(new CMString(slot));
  m_valueList.add(new CMString(value));
  m_grammarNameList.add(new CMString(szGrammarName));
}

const char * CMGrammarGeneratorExample::GetGrammar(long i) const
{
  return *m_grammarNameList(i);
}

const char * CMGrammarGeneratorExample::GetSlot(long i) const 
{
  return *m_slotList(i);
}

const char * CMGrammarGeneratorExample::GetValue(long i) const 
{
  return *m_valueList(i);
}

void CMGrammarGeneratorExample::RemoveLastSlotValuePair()
{
  m_slotList.remove(m_slotList.length() -1);
  m_valueList.remove(m_valueList.length() -1);
  m_grammarNameList.remove(m_grammarNameList.length() -1);
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CMGrammarGenerator::CMGrammarGenerator()
{
  m_pGrammarStack = NULL;
}

CMGrammarGenerator::~CMGrammarGenerator()
{
}

void CMGrammarGenerator::SetGrammarStack(IMGrammarStack * p)
{
  m_pGrammarStack = p->GetGrammarStackImp();
}

MDLLEXPORT bool CMGrammarGenerator::WriteGrammarToFile(const char * filePathAndName, 
									                   const char * grammarNamePrefix)
{
  CMString fileName;
  CMString grammarName;

  fileName = filePathAndName;

  CMString newPath;
  CMString temp;
  CMString grammarTemp;
  int i;
  const char * pTemp = (const char *) filePathAndName;
  for (i=0; i<(long)strlen(filePathAndName); i++) {
	if (pTemp[i] == '.')
	  grammarName = grammarTemp;

    temp += pTemp[i];
	grammarTemp += pTemp[i];
	if (pTemp[i] == '\\' || pTemp[i] == '/') {
	  newPath = temp;
	  grammarTemp = "";
	}
	  
  }

  if (grammarName == "")
	grammarName = grammarTemp;



  CMAsciiWriteFileStream out;
  out.Open(fileName);

  //Prepare the header...
  out.WriteLine("#JSGF V1.0 ISO8859-1 en");
  out.WriteLine("");
  CMString line = "grammar ";
  line += grammarNamePrefix;

  if (strcmp(grammarNamePrefix, "") != 0 && ((const char*)grammarNamePrefix)[strlen(grammarNamePrefix)-1] != '.')
    line += ".";

  line += grammarName;
  line += ";";
  out.WriteLine(line);
  out.WriteLine("");

  //Import the other grammars that we referenced...
  for (i=0; i<m_imports.length(); i++) {
    const CMString & grmName = *m_imports(i);
	line = "import <";
	line += grmName;
	line += ".*>;";
    out.WriteLine(line);
  }

  out.WriteLine("");

  CMString topLine = "public <";
  topLine += grammarName;
  topLine += "> = ";
  out.WriteLine(topLine);

  for (i=0; i<m_rules.length(); i++) {
	if (i > 0)
	  line = "    |";
	else
	  line = "     ";

	line += *m_rules(i);
    out.WriteLine(line);
  }

  out.WriteLine(";");
  out.Close();
  return true;
}

void CMGrammarGenerator::AddToImports(const CMString & import)
{
  for (int i=0; i<m_imports.length(); i++) {
    if (*m_imports(i) == import)
	  return;
  }
  m_imports.add(new CMString(import));
}

bool CMGrammarGenerator::AddSimpleExample(const char * in, const char * out1)
{
  CMString out = out1;
  CMTokenList tokens;
  Tokenize(tokens, in);
  CMString newRule = in;
  newRule += " {this.$value = \"";

  if (out == "")
	newRule += in;
  else
	newRule += out;

  newRule += "\"}";

  m_rules.add(new CMString(newRule));

  return true;
}

GRAMMAR_HANDLE CMGrammarGenerator::ParseAll(CMTagList & tags, 
			  		                        CMTokenList & tokens,
					                        const CMString & text,
											const CMString & grmName,
											const CMString & resHint)
{
  CMTagList tempTags;
  CMTokenList tempTokens;

  GRAMMAR_HANDLE h = -1;
  CMString lastWinName;
  CMString ruleName;

  for (int i=0; i<m_pGrammarStack->GetGrammarCount(); i++) {
	if (m_pGrammarStack->GetGrammar(i)->ParseEx(tempTags, tempTokens, ruleName, text)) {
	  if (grmName != "" && grmName == m_pGrammarStack->GetGrammar(i)->GetName()) {
        tags = tempTags;
        tokens = tempTokens;
		m_lastPublicRule = ruleName;
	    return m_pGrammarStack->GetGrammar(i)->GetHandle();
	  }

	  if (h == -1) {
		h = m_pGrammarStack->GetGrammar(i)->GetHandle();
        tags = tempTags;
        tokens = tempTokens;
		lastWinName = m_pGrammarStack->GetGrammar(i)->GetName();
		m_lastPublicRule = ruleName;
	  } else {
        //Found it??
		/*if (resHint == *tempTags(1)) {
          tags = tempTags;
          tokens = tempTokens;
		  lastWinName = m_pGrammarStack->GetGrammar(i)->GetName();
		  m_lastPublicRule = ruleName;
		}*/

	    MLog("Ambiguity during grammar generation!!!!");
		m_lastAmbiguities.add(lastWinName);
		m_lastAmbiguities.add(m_pGrammarStack->GetGrammar(i)->GetName());
		//return -2;
	  }
	}
  }

  return h;
}

bool CMGrammarGenerator::UndoLastExample()
{
  m_rules.remove(m_rules.length() -1);
  return true;
}

bool CMGrammarGenerator::AddExample(const IMGrammarGeneratorExample & example)
{

  if (m_pGrammarStack == NULL) {
	MLog("Cannot add rule - no grammars loaded!!");
    return false;
  }

  m_lastAmbiguities.removeAll();

  CMTagList tags;
  CMTokenList tokens;
  CMPtrStringList theWords;

  Tokenize(theWords, example.GetText());

  CMString subSet;

  GRAMMAR_HANDLE h = -1;

  int i, j, k;
  //int lastI = -1;
  long matchCounter = 0;
  CMString newRule;
  //ATTN: This is SLOW!!
  for (i=0; i<theWords.length(); i++) {
	bool bDidParse = false;
	for (k=theWords.length(); k>i; k--) {
      subSet = "";
      for (j=i; j<k; j++) {
        subSet += *(theWords(j));
        if (j < k-1)
		  subSet += " ";
	  }
	  tags.removeAll();
	  tokens.removeAll();
      //Now parse against all we have...
	  long newAmbigCount = m_lastAmbiguities.length();
	  h = ParseAll(tags, tokens, subSet);

	  //if (h == -2) {
	  //  return false;
	  //}

	  if (h != -1) {
		CMString dollarValue;
        if (m_pGrammarStack->GetDollarValue(tags, tokens, dollarValue)) {
          dollarValue.removeLeadingChars(' ');
 	      bDidParse = true;


		  CMString annot;
		  annot = " {";
		  bool bFound = false;
		  for (int l=0; l<example.GetSlotValuePairCount(); l++) {
			if (subSet == example.GetValue(l) || dollarValue == example.GetValue(l)) {

			  if (strcmp(example.GetGrammar(l), "") != 0) {
                m_lastAmbiguities.removeAll();
	            h = ParseAll(tags, tokens, subSet, example.GetGrammar(l));
				if (h == -1) {
		          MLog("Grammar Generator: did not parse against explicit grammar:", example.GetGrammar(l));
				  return false;
				}
			  }

		      annot += example.GetSlot(l);
		      bFound = true;
	          matchCounter++;
		      break;
			}
		  }
          
		  CMString ruleStore = newRule;
		  newRule += " <";
    	  newRule += m_lastPublicRule; //m_pGrammarStack->GetPublicRuleName(tags, tokens);
		  newRule += ">";

		  annot += " = this.$value";
		  annot += "}";

       	  CMGrammar * pGrammar = m_pGrammarStack->FindGrammar(h);

		  AddToImports(pGrammar->GetName());

		  if (!bFound) {
		    MLog("Grammar Generator: value not found:", subSet);
			bDidParse = false;
			newRule = ruleStore;
			//annot = " {this.$value = $}";
		    //newRule += annot;
			//return false;
			while (m_lastAmbiguities.length() > newAmbigCount) {
			  m_lastAmbiguities.remove(m_lastAmbiguities.length()-1);
			  //newAmbigCount--;
			}
			break;
		  } else {
		    newRule += annot;
		  }
		  
		  i = k-1;
		  break;
		}
	  } else {
	  }
	}

	if (!bDidParse) {
      newRule += " ";
      newRule += *theWords(i);
	  //lastI = i;
	}
  }

  if (matchCounter != example.GetSlotValuePairCount()) {
    MLog("Grammar Generator: slot could not be resolved...");
	return false;
  }
  if (newRule != "")
    m_rules.add(new CMString(newRule));

  return true;
}
