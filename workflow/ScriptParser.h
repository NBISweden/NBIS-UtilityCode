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
  Command() {}

  const string & Raw() const {return m_raw;}
  string & Raw() {return m_raw;}

  const svec<string> & Valid() const {return m_valid;}
  svec<string> & Valid() {return m_valid;}

  const string & Processed() const {return m_processed;}
  string & Processed() {return m_processed;}

  const svec<string> & Out() const {return m_out;}
  svec<string> & Out() {return m_out;}
  
 private:
  string m_raw;
  svec<string> m_valid;
  string m_processed;
  svec<string> m_out;
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
  
 private:
  svec<TableColumn> m_columns;
};



class ScriptParser
{
 public:
  ScriptParser() {}

  int Read(const string & fileName);

  int GetCount() const {
    if (m_table.isize() > 0)
      return m_table[0].isize();
    return 1;
  }

  bool Process(int i=0);

  int isize() const {return m_commands.isize();}
  const string & operator [] (int i) const {return m_commands[i].Processed();}
  
 private:
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
    return -1;;
  }
  
  svec<Variable> m_vars;
  svec<Command> m_commands;
  CMGrammarStack m_grmStack;

  Table m_table;
};



#endif //SCRIPTPARSER_H
