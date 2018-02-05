//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//

#ifndef _MSCRPTNT_H_
#define  _MSCRPTNT_H_

#include "util/mutil.h"
// #include <mlists.h>
#include <stdio.h>
#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class CMScriptVariable
{
 public:
  CMScriptVariable() 
    {
      m_longVal = 0;
      m_intUsed = false;
    }
  
  CMScriptVariable(const CMScriptVariable & val) 
    {
      m_longVal = 0;
      m_intUsed = false;
      *this = val;
    }
  
  CMScriptVariable(const CMString & s) 
    {
      m_longVal = 0;
      m_intUsed = false;
      m_stringVal = s;
    }
  
  CMScriptVariable(long v) 
    {
      m_longVal = v;
      m_intUsed = true;
      ConvertToString();
    }
  
  ~CMScriptVariable() {}
  
  CMString & VarName() {return m_name;}
  CMString & MemberName() {return m_member;}
  
  const CMString & VarName() const {return m_name;}
  const CMString & MemberName() const {return m_member;}
  
  
  const CMString & StringVal() const {return m_stringVal;}
  CMString & StringVal() {return m_stringVal;}
  
  const long & IntVal() const {return m_longVal;}
  long & IntVal() {return m_longVal;}
  
  bool operator==(const CMScriptVariable & val) 
  {
    if (m_stringVal != val.StringVal())
      return false;
    if (m_longVal != val.IntVal())
      return false;
    return true;
  }
  
  bool operator!=(const CMScriptVariable & val) 
  {
    return !operator==(val);
  }
  
  CMScriptVariable & operator = (const CMScriptVariable & val)
    {
      m_stringVal = val.StringVal();
      m_longVal = val.IntVal();
      m_intUsed = val.m_intUsed;
      return *this;
    }
  
  CMScriptVariable & operator += (const CMScriptVariable & val)
    {
      if (val.m_intUsed && !(m_stringVal != "" && !m_intUsed)) {
	m_intUsed = true;
	m_longVal += val.IntVal();
	ConvertToString();
      } else {
	m_stringVal += val.StringVal();
	m_longVal = 0;
	m_intUsed = false;
      }
      
      return *this;
    }
  
  CMScriptVariable & operator -= (const CMScriptVariable & val)
    {
      if (val.m_intUsed && !(m_stringVal != "" && !m_intUsed)) {
	m_longVal -= val.IntVal();
	m_intUsed = true;
	ConvertToString();
      } else {
	if (val.StringVal().len() >= m_stringVal.len()) {
	  std::cout << "WARNING in CMScriptVariable::operator -= : " << val.StringVal();
	  std::cout << " longer than " << m_stringVal << endl;
	  return *this;
	}
	int i, j;
	//cout << "Subtract " << val.StringVal() << " from " << m_stringVal << endl;
	for (j=0; j<=m_stringVal.len()-val.StringVal().len(); j++) {
	  bool good = true;
	  for (i=m_stringVal.len()-val.StringVal().len()-j; i<m_stringVal.len()-j; i++) {
	    if (val.StringVal()[i-(m_stringVal.len()-val.StringVal().len()-j)] != m_stringVal[i]) {
	      good = false;
	      break;
	    }
	  }
	  if (good) {
	    for (i=m_stringVal.len()-val.StringVal().len()-j; i<=m_stringVal.len()-val.StringVal().len(); i++) {
	      //cout << " -> copy " << m_stringVal[i+val.StringVal().len()] << " to " << 
	      m_stringVal[i] = m_stringVal[i+val.StringVal().len()];
	    }
	    CMString tmp = (const char*) m_stringVal;
	    m_stringVal = tmp;
	  }
	}
	m_longVal = 0;
	m_intUsed = false;	
      }
      return *this;
    }
  
  CMScriptVariable & operator *= (const CMScriptVariable & val)
    {
      m_longVal *= val.IntVal();
      m_intUsed = true;
      ConvertToString();
      return *this;
    }
  
  CMScriptVariable & operator /= (const CMScriptVariable & val)
    {
      m_longVal /= val.IntVal();
      m_intUsed = true;
      ConvertToString();
      return *this;
    }
  
  
  operator const CMString&() const {return m_stringVal;}
  operator const long&() const {return m_longVal;}
  
  void ConvertToString() 
  {
    char szText[64];
    sprintf(szText, "%ld", m_longVal);
    m_stringVal = szText;
  }
  
  void SetInt() {m_intUsed = true;}
  
 private:
  
  CMString m_stringVal;
  long m_longVal;
  bool m_intUsed;
  
  CMString m_name;
  CMString m_member;
  
};


typedef TMPtrList<CMScriptVariable> CMVariableList;

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
class CMScriptInterpreter
{
public:
	CMScriptInterpreter();
	virtual ~CMScriptInterpreter();

	bool Interprete(CMPtrStringList & result, const CMPtrStringList & script,
					const CMPtrStringList & parseTokens);

private:
	bool ExecuteStatement(const CMString & statement, const CMString & ntVarName);
	void PreParse(CMPtrStringList & statements, const CMPtrStringList & script,
				  CMPtrStringList & postTokens, const CMPtrStringList & preTokens);

    bool TokenizeStatement(CMPtrStringList & result, const CMString & source);
    bool AssignTemp(CMScriptVariable * pVar, const CMString & token);
    bool IsTerminal(const CMString & exp);

	CMScriptVariable & AddVariable(const CMString & var, const CMString & member);
	CMScriptVariable & FindVariable(const CMString & var, const CMString & member);


	CMVariableList m_variables;
	CMVariableList m_tempVariables;

	CMScriptVariable m_lastReferenced;

	bool HandleSystemCall(CMScriptVariable & dest, CMString & result, const CMString & systemCallWithArguments);
	bool HandleExternalCall(CMString & result, const CMString & externalCallWithArguments);
	bool ResolveFunctionAndArguments(CMString & funcion, CMPtrStringList & arguments, const CMString & toParse);
};

#endif // _MSCRPTNT_H_


