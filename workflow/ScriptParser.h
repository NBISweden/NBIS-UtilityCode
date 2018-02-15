#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

#define FORCE_DEBUG

#include "base/FileParser.h"
#include "nl/mgrmstck.h"
#include "nl/mgrmgen.h"
#include "nl/mxtract.h"


class Command
{
 public:
  Command() {m_bg = false;}

  const string & Raw() const {return m_raw;}
  string & Raw() {return m_raw;}

  const svec<string> & Valid() const {return m_valid;}
  svec<string> & Valid() {return m_valid;}

  const string & Processed() const {return m_processed;}
  string & Processed() {return m_processed;}

  const svec<string> & Out() const {return m_out;}
  svec<string> & Out() {return m_out;}

  bool IsBG() const {return m_bg;}
  void SetBG(bool b) {m_bg = b;}
  
 private:
  string m_raw;
  svec<string> m_valid;
  string m_processed;
  svec<string> m_out;
  bool m_bg;
};


class Variable
{
 public:
  Variable() {}

  const string & Name() const {return m_name;}
  string & Name() {return m_name;}
  
  const string & Value() const {return m_value;}
  string & Value() {return m_value;}
 
 private:
  string m_name;
  string m_value;
};


class TableColumn
{
 public:
  TableColumn() {}

  int isize() const {return m_values.isize();}
  string & operator[] (int i) {return m_values[i];}
  const string & operator[] (int i) const {return m_values[i];}
  void push_back(const string & s) {m_values.push_back(s);}
  
  const string & Label() const {return m_label;}
  string & Label() {return m_label;}

  
 private:
  string m_label;
  svec<string> m_values;

};


class Table
{
 public:
  Table() {}

  TableColumn & operator[] (int i) {return m_columns[i];}
  const TableColumn & operator[] (int i) const {return m_columns[i];}
  int isize() const {return m_columns.isize();}
  void resize(int n) {m_columns.resize(n);}

  void Read(const string & fileName);
  void Write(const string & fileName);

  const string & Name() const {return m_name;}
  string & Name() {return m_name;}

  bool Get(string &ret, const string & label, int index) const;
  void Prepend(const string & what, const string & to, const string & sep = "");
  void Collapse(const string & key);
  void Print() const;
  void AddColumn(const string & label);
  void RemoveColumn(const string & label);
  void FillColumn(const string & label, const svec<string> & c, int from);
  void SetInColumn(const string & label, int i, const string & v);
  int Index(int i) const {
    if (m_index.isize() == 0)
      return 0;
    return m_index[i];
  }

  const string & FileName() const {return m_fileName;}
  
 private:
  int ColIndex(const string & s) const {
    int i;
    for (i=0; i<m_columns.isize(); i++) {
      if (m_columns[i].Label() == s)
	return i;
    }
    return -1;
  }
  
  svec<TableColumn> m_columns;
  string m_name;
  svec<int> m_index;
  string m_fileName;
};


//===========================================
class ScriptParser
{
 public:
  ScriptParser() {
    m_curr = 0;
  }

  int Read(const string & fileName);

  int GetCount() const {
    if (m_table.isize() > 0)
      return m_table[0].isize();
    return 1;
  }

  bool Process(int i);

  int isize() const {return m_commands.isize();}
  const string & operator [] (int i) const {return m_commands[i].Processed();}

  void Prepend(const string & what, const string & to, const string & sep = "") {
    m_table.Prepend(what, to, sep);
  }

  void SetGrammarPath(const string & g) {
    m_gramPath = g;
  }
  
 private:

  void CollapseTable(const string & key) {
    m_table.Collapse(key);
  }
  
  int AddVariable(const string & s) {
    int i;
    for (i=0; i<m_vars.isize(); i++) {
      if (m_vars[i].Name() == s)
	return i;
    }
    Variable tmp;
    tmp.Name() = s;
    m_vars.push_back(tmp);
    return m_vars.isize()-1;
  }

  int GetVariable(const string & s) {
    int i;
    for (i=0; i<m_vars.isize(); i++) {
      if (m_vars[i].Name() == s)
	return i;
    }    
    
    return -1;
  }

  bool CheckForErrors(const string & in);
  
  void AddTableVars(int index);

  bool VariableAssign(const Command & c);

  
  svec<Variable> m_vars;
  svec<Command> m_commands;
  CMGrammarStack m_grmStack;
  int m_curr;
  Table m_table;
  svec<string> m_dict;
  string m_gramPath;
  string m_collapse;
};



#endif //SCRIPTPARSER_H
