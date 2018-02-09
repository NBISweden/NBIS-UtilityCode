#ifndef TABLEMANIP_H
#define TABLEMANIP_H

#define FORCE_DEBUG

#include "base/SVector.h"
#include "workflow/ScriptParser.h"

class TableManipulator
{
 public:
  TableManipulator(const string & fileName) {
    m_name = fileName;
    m_lock = m_name + ".lock";
  }

  void AddColumn(const string & label);
  void RemoveColumn(const string & label);
  void FillColumn(const string & label, const svec<string> & c, int from);
  void SetInColumn(const string & label, int i, const string & v);

  
 private:
  void AddLock();
  void RemoveLock();
  void Wait();
  
  string m_name;
  string m_lock;
};





#endif


