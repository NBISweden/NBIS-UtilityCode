//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//



#include "nl/mscrptnt.h"
#include "util/mutil.h"
#include "nl/moscalls.h"
#include <stdlib.h>
//#include <mlog.h>
//#include <mascrw.h>

#define SCRPT_THIS_VAR	"this"
#define SCRPT_VALUE_VAR	"$value"
#define SCRPT_DOLLAR_VAR	"$"


#define TMP_EXT_FILE	"grmscrpt.tmp"

CMScriptInterpreter::CMScriptInterpreter()
{
}

CMScriptInterpreter::~CMScriptInterpreter()
{
}

bool CMScriptInterpreter::IsTerminal(const CMString & exp)
{
  if (strlen(exp) < 1)
	return true;
  if (((const char*)exp)[0] == '$') // && ((const char*)exp)[strlen(exp)-1] == '>')
	return false;

  return true;
}

bool CMScriptInterpreter::Interprete(CMPtrStringList & result, const CMPtrStringList & script,
									 const CMPtrStringList & preParseTokens)
{
  m_variables.removeAll();
  m_tempVariables.removeAll();

  CMScriptVariable & dollarVar = FindVariable("", SCRPT_DOLLAR_VAR);

  //m_lastReferenced.StringVal() = "";

  CMPtrStringList statements;
  CMPtrStringList parseTokens;
  PreParse(statements, script, parseTokens, preParseTokens);

  int i;
  for (i=0; i<statements.length(); i++) {
	const CMString & exp = *statements(i);
	const CMString & token = *parseTokens(i);
	CMString nonTermVarName;
	if (IsTerminal(token)) {
	  dollarVar.StringVal() = token;
	  bool bExit = false;
	  for (int j=i; j<parseTokens.length(); j++) {
	  //for (int j=i; j<m; j++) {
		if (!IsTerminal(*parseTokens(j))) {
	      nonTermVarName = *parseTokens(j);
		  bExit = true;
          if (*statements(j) != "")
			break;
		  //break;
		} else {
		  if (bExit)
			break;
		}
	  }
	} else {
	  //int x = 0;
	}
    if (i+1<statements.length() && !IsTerminal(*parseTokens(i+1)))
	  nonTermVarName = *parseTokens(i+1);
	else
      nonTermVarName = ""; 
    ExecuteStatement(exp, nonTermVarName);
  }

  result.removeAll();

  for (i=0; i<m_variables.length(); i++) {
	if (m_variables(i)->VarName() == SCRPT_THIS_VAR) {
	  CMString * pString = new CMString;
	  *pString = m_variables(i)->MemberName();
	  result.add(pString);

	  pString = new CMString;
	  *pString = m_variables(i)->StringVal();
	  result.add(pString);
	}
  }

  return true;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
bool CMScriptInterpreter::HandleSystemCall(CMScriptVariable & dest, CMString & result, const CMString & systemCallWithArguments)
{
  CMString function;
  CMPtrStringList arguments;
  if (!ResolveFunctionAndArguments(function, arguments, systemCallWithArguments))
    return false;

  int i;
  //--------------------------------------------------------------------------------------------
  if (function == "ReadFile") {
	if (arguments.length() == 0) {
	  MLog("Grammar Script Interpreter: Too few arguments", systemCallWithArguments);
	  return false;
	} 

    CMAsciiReadFileStream res;
    MCL_TRY {
      res.Open(*arguments(0));
	} 

#ifndef MCL_NO_EXCEPTIONS
    catch(CMException & ) {
	  MLog("Grammar Script Interpreter: Could not execute (File I/O Error)", systemCallWithArguments);
      return false;
	} 
#endif 

    while (!res.IsEnd()) {
      CMString line;
	  res.ReadLine(line);
	  if (result != "")
	    result += " ";
	  result += line;
	}
	res.Close();
    return true;
  }
  //--------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------
  if (function == "WriteFile") {
	if (arguments.length() < 2) {
	  MLog("Grammar Script Interpreter: Too few arguments", systemCallWithArguments);
	  return false;
	} 
    CMAsciiWriteFileStream res;
    MCL_TRY {
      res.Open(*arguments(0));
	} 

#ifndef MCL_NO_EXCEPTIONS
    catch(CMException & ) {
	  MLog("Grammar Script Interpreter: Could not execute (File I/O Error)", systemCallWithArguments);
      return false;
	} 
#endif

	for (i = 1; i<arguments.length(); i++)
	  res.WriteLine(*arguments(i));
	res.Close();
    return true;
  }
  //--------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------
//MDLLEXPORT bool CMGetSystemTime(CMString & result, long plusminusHours = 0, long plusminusMinutes = 0, bool bEuropean = false);
//MDLLEXPORT long CMGetSystemTimeHour(bool bEuropean = false);
//MDLLEXPORT long CMGetSystemTimeMinutes();
//MDLLEXPORT long CMGetSystemTimeSeconds();
//MDLLEXPORT long CMGetSystemTimeMilliSeconds();
//MDLLEXPORT void CMGetSystemTimeAmPm(CMString & result, long plusminus = 0);


//---------- Date functions ---------------
//MDLLEXPORT bool CMGetSystemDate(CMString & result, long plusminusDays = 0, long plusminusMonths = 0);

//MDLLEXPORT long CMGetSystemDateYear();
//MDLLEXPORT long CMGetSystemDateMonth();
//MDLLEXPORT long CMGetSystemDateDay();
//MDLLEXPORT void CMGetSystemDateDOW(CMString & dow, long plusminus = 0);
  if (function == "GetSystemTime") {
	if (arguments.length() == 2) {
      MGetSystemTime(result, atol(*arguments(0)), atol(*arguments(1)));
	  dest.StringVal() = result; 
      return true;
	} 
	if (arguments.length() == 1) {
      MGetSystemTime(result, atol(*arguments(0)));
	  dest.StringVal() = result; 
      return true;
	} 
	if (arguments.length() == 0) {
      MGetSystemTime(result);
	  dest.StringVal() = result; 
      return true;
	} 
  }
  //--------------------------------------------------------------------------------------------
  if (function == "GetSystemDate") {
	if (arguments.length() == 2) {
      MGetSystemDate(result, atol(*arguments(0)), atol(*arguments(1)));
	  dest.StringVal() = result; 
      return true;
	} 
	if (arguments.length() == 1) {
      MGetSystemDate(result, atol(*arguments(0)));
	  dest.StringVal() = result; 
      return true;
	} 
	if (arguments.length() == 0) {
      MGetSystemDate(result);
	  dest.StringVal() = result; 
      return true;
	} 
  }
  //--------------------------------------------------------------------------------------------
  
  if (function == "GetSystemDayOfWeek") {

    //MGetSystemDateDOW(result, atol(*arguments(0)), atol(*arguments(1)));
	dest.IntVal() = MGetSystemDateDOW(); 
    return true;
  } 



  return true;
}

bool CMScriptInterpreter::HandleExternalCall(CMString & result, const CMString & externalCallWithArguments)
{
  CMString function;
  CMPtrStringList arguments;
  if (!ResolveFunctionAndArguments(function, arguments, externalCallWithArguments))
    return false;

  CMString cmdLine = function;
  int i;
  for (i=0; i<arguments.length(); i++) {
    cmdLine += " ";
    cmdLine += *arguments(i);  
  }

  cmdLine += " > ";
  cmdLine += TMP_EXT_FILE;
//  system(cmdLine);

  result = "";

  CMAsciiReadFileStream res;
  MCL_TRY {
    res.Open(TMP_EXT_FILE);
  }

#ifndef MCL_NO_EXCEPTIONS
  catch(CMException & ) {
	MLog("Grammar Script Interpreter: Could not get result after external call", externalCallWithArguments);
    return false;
  }
#endif

  while (!res.IsEnd()) {
    CMString line;
	res.ReadLine(line);
	if (result != "")
	  result += " ";
	result += line;
  }
  
  res.Close();
  return true;
}

bool CMScriptInterpreter::ResolveFunctionAndArguments(CMString & function, CMPtrStringList & arguments, const CMString & toParse)
{
  CMTokenizer tokenizer;
  tokenizer.AddDelimiter("(", "");
  tokenizer.AddDelimiter(")", "");
  tokenizer.AddDelimiter(",", ",");
  tokenizer.AddDelimiter("\"", "");
  
  CMPtrStringList tokens;
  tokenizer.Tokenize(tokens, toParse);
  if (tokens.length() < 1) {
	MLog("Grammar Script Interpreter: Invalid function:", toParse);
	return false;
  }

  function = *tokens(0);
  //CMString arg;
  CMTokenizer subTokenizer;
  subTokenizer.AddDelimiter(".", ".");

  for (int i=1; i<tokens.length(); i++) {
	if (*tokens(i) != ",") { // || i+1 == tokens.length()) {
      CMPtrStringList subTokens;
      subTokenizer.Tokenize(subTokens, *tokens(i));
	  if (subTokens.length() == 3 && *subTokens(1) == ".") {
        CMScriptVariable & var = FindVariable(*subTokens(0), *subTokens(2));
	    arguments.add(var.StringVal());
	  } else {
	    arguments.add(new CMString(*tokens(i)));
	  }
		//arg = "";
	  continue;
	}
	//if (arg != "")
	//  arg += " ";
	//arg += *tokens(i);
  }

  return true;
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
bool CMScriptInterpreter::ExecuteStatement(const CMString & statement, const CMString & ntVarName)
{
  if (statement == "") {
    if (ntVarName != "") {
      CMScriptVariable & toSave = FindVariable(SCRPT_THIS_VAR, ntVarName);
      CMScriptVariable & toCopy = FindVariable(SCRPT_THIS_VAR, "$value");
      toSave = toCopy;
    }
    return true;
  }
  
  CMPtrStringList tokens;
  TokenizeStatement(tokens, statement);
  long i=0; 
  const CMString & var = *tokens(i);
  i++;
  i++;
  const CMString & member = *tokens(i);
  i++;
  
  CMScriptVariable & dest = FindVariable(var, member);
  
  //const CMString & assignment = *tokens(i);
  i++;
  //Assume it's always '='...
  
  //Build list of local variables...
  CMVariableList locals;
  CMPtrStringList ops;
  int j;
  for (j=i; j<tokens.length(); j++) {
    const CMString & workToken = *tokens(j);
    if (workToken == "+" || workToken == "-" || workToken == "*" || workToken == "/") {
      ops.add(new CMString(workToken));
    } else {
      CMScriptVariable * pVar = new CMScriptVariable;
      if (AssignTemp(pVar, workToken)) {
      } else {
	if (j < tokens.length() - 2 && *tokens(j+1) == ".") {
	  *pVar = FindVariable(*tokens(j), *tokens(j+2));
	  j += 2;
	} else {
	  *pVar = FindVariable("", *tokens(j));
	}
      }
      
      locals.add(pVar);
    }
  }
  
  //Multiplications/divisions first
  int k = 0;
  for (j=0; j<ops.length(); j++) {
    if (*ops(j) == "*") {
      *locals(k) *= *locals(k+1);
      locals.remove(k+1);
      ops.remove(j);
      j--;
      continue;
    }
    if (*ops(j) == "/") {
      *locals(k) /= *locals(k+1);
      locals.remove(k+1);
      ops.remove(j);
      j--;
      continue;
    }
    
    k++;
  }
  
  //plus and minus
  k = 0;
  for (j=0; j<ops.length(); j++) {
    if (*ops(j) == "+") {
      *locals(k) += *locals(k+1);
      locals.remove(k+1);
      ops.remove(j);
      j--;
      continue;
    }
    if (*ops(j) == "-") {
      *locals(k) -= *locals(k+1);
      locals.remove(k+1);
      ops.remove(j);
      j--;
      continue;
    }
    
    k++;
  }
  
  //Done!
  dest = *locals(0);

  if (dest.MemberName() == SCRPT_VALUE_VAR && dest.VarName() == SCRPT_THIS_VAR) {
	if (ntVarName != "") {
      CMScriptVariable & toSave = FindVariable(SCRPT_THIS_VAR, ntVarName);
      CMScriptVariable & dollarSave = FindVariable("", SCRPT_DOLLAR_VAR);
      toSave = dest;
	  dollarSave = toSave;
	}
  }


  return true;
}
//--------------------------------------------------------------------------------------

bool CMScriptInterpreter::AssignTemp(CMScriptVariable * pVar, const CMString & token)
{
  //Check for NUMERICALS and quoted STRINGS here...
  if (token(0) == '"' && token(strlen(token)-1) == '"') {
    char szTemp[1024];
    strcpy(szTemp, &((const char*)token)[1]);
    szTemp[strlen(token)-2] = 0;
    pVar->StringVal() = szTemp;
    return true;
  }
  
  for (int i=0; i<(int)strlen(token); i++) {
    if (token(i) > '9' || token(i) < '0')
      return false;
  }
  
  pVar->IntVal() = atoi(token);
  pVar->SetInt();
  pVar->ConvertToString();

  return true;
}


void CMScriptInterpreter::PreParse(CMPtrStringList & statements, const CMPtrStringList & script,
								   CMPtrStringList & postTokens, const CMPtrStringList & preTokens)
{
  for (int i=0; i<script.length(); i++) {
    CMPtrStringList tokens;
    CMString s = *script(i);
    s += ";";
    Tokenize(tokens, s, ';');
    for (int j=0; j<tokens.length(); j++) {
      if (*tokens(j) != ";") {
	statements.add(new CMString(*tokens(j)));
	if (j == 0)
	  postTokens.add(new CMString(*preTokens(i)));
	else
	  postTokens.add("");
      }
    }
    if (tokens.length() == 0) {
      statements.add(new CMString);
      postTokens.add(new CMString(*preTokens(i)));
    }
	  
  }
}

bool CMScriptInterpreter::TokenizeStatement(CMPtrStringList & result, const CMString & source)
{
  const MCL_TCHAR * pszBuffer = source;

  MCL_TCHAR szTemp[2048];
  //Clear out any previous results in the tokenizer
  result.removeAll();
  __mccl_strcpy(szTemp, _TMCL(""));
  
  int k = 0;
  
  long len = (long)__mccl_strlen(pszBuffer);
  
  bool bQuoteFlag = false;
  
  //Hack to preserve tags
  char cQuoteOpen = '"';
  char cQuoteClose = '"';
  
  for (int i=0; i<=len; i++) {
    if (!bQuoteFlag && pszBuffer[i] == cQuoteOpen) {
      bQuoteFlag = true;
	} else {
      if (bQuoteFlag && pszBuffer[i] == cQuoteClose)
	bQuoteFlag = false;
    }
    
    if (!bQuoteFlag && (pszBuffer[i] == ' ' 
			|| pszBuffer[i] == '.'
			|| pszBuffer[i] == '='
			|| pszBuffer[i] == '+'
			|| pszBuffer[i] == '-'
			|| pszBuffer[i] == '*'
			|| pszBuffer[i] == '/'
			|| i == len)) {
      k = 0;
      if (__mccl_strcmp(szTemp, _TMCL("")) != 0 && __mccl_strcmp(szTemp, _TMCL(" ")) != 0) {
	CMString * pNewString = new CMString;
	*pNewString = szTemp;
	result.add(pNewString);
        __mccl_strcpy(szTemp, _TMCL(""));
      }
      if (i < len && pszBuffer[i] != ' ') {
	CMString * pNewString = new CMString;
	szTemp[0] = pszBuffer[i];
	szTemp[1] = 0;
	*pNewString = szTemp;
	result.add(pNewString);
        __mccl_strcpy(szTemp, _TMCL(""));
	
	//if (result.length() >= limit)
	//break;
      }
    } else {
      szTemp[k] = pszBuffer[i];
      szTemp[k + 1] = 0;
      k++;
    }
  }

  return true;
}


CMScriptVariable & CMScriptInterpreter::AddVariable(const CMString & var, const CMString & member)
{
  CMScriptVariable * pVar = new CMScriptVariable;

  pVar->MemberName() = member;
  pVar->VarName() = var;

  m_variables.add(pVar);
  return *pVar;
}

CMScriptVariable & CMScriptInterpreter::FindVariable(const CMString & var, const CMString & member)
{
  if (var == "systemcall" || var == "externcall") {
    CMScriptVariable & fnVar = AddVariable(var, member);
    CMString result;
    if (var == "systemcall")
      HandleSystemCall(fnVar, result, member);
    if (var == "externcall") {
      HandleExternalCall(result, member);
      fnVar.StringVal() = result;
    }
    return fnVar;
    
  } else {
    for (int i=0; i<m_variables.length(); i++) {
      if (m_variables(i)->VarName() == var && m_variables(i)->MemberName() == member)
	return *m_variables(i);
    }
  } 
  
  CMScriptVariable & retVar = AddVariable(var, member); 
  return retVar;
}




