#include "workflow/TableManip.h"
#include "base/FileParser.h"
#include <unistd.h>
#include <stdlib.h>

void TableManipulator::AddToColumn(const string & label, const string & item)
{
  MakeIfNotExist();
  AddLock();
  Table t;
  t.Read(m_name);
  t.AddToColumn(label, item);
  t.Write(m_name);
  RemoveLock();

}


void TableManipulator::AddColumn(const string & label)
{
  MakeIfNotExist();
  AddLock();
  Table t;
  t.Read(m_name);
  t.AddColumn(label);
  t.Write(m_name);
  RemoveLock();
}


void TableManipulator::RemoveColumn(const string & label)
{
  MakeIfNotExist();
  AddLock();
  Table t;
  t.Read(m_name);
  t.RemoveColumn(label);
  t.Write(m_name);
  RemoveLock();
}

void TableManipulator::RemoveHeaders()
{
  MakeIfNotExist();
  AddLock();
  Table t;
  t.Read(m_name);
  t.RemoveHeaders();
  t.Write(m_name);
  RemoveLock();
  
}

void TableManipulator::FillColumn(const string & label, const svec<string> & c, int from)
{
  MakeIfNotExist();
  AddLock();
  Table t;
  t.Read(m_name);
  t.FillColumn(label, c, from);
  t.Write(m_name);
  RemoveLock();
}

void TableManipulator::SetInColumn(const string & label, int i, const string & v)
{
  MakeIfNotExist();
  AddLock();
  Table t;
  t.Read(m_name);
  t.SetInColumn(label, i, v);
  t.Write(m_name);
  RemoveLock();
}

void TableManipulator::AddLock()
{
  MakeIfNotExist();
  Wait();
  FILE * p = fopen(m_lock.c_str(), "w");
  fprintf(p, "locked\n");
  fclose(p);
}
  
void TableManipulator::RemoveLock()
{
  string cmmd = "rm " + m_lock;
  int r = system(cmmd.c_str());
}

void TableManipulator::Wait()
{
  FILE * p = fopen(m_lock.c_str(), "r");
  int n = 0;
  while (p != NULL) {
    fclose(p);
    usleep(1000000);
    p = fopen(m_lock.c_str(), "r");
    n++;
    if (n > 60) {
      cout << "WARNING: file lock broken!!!" << endl;
      break;
    }
  }
}

void TableManipulator::MakeIfNotExist()
{
  FILE * p = fopen(m_name.c_str(), "r");
  if (p == NULL) {
    p = fopen(m_name.c_str(), "w");
  }
  fclose(p);
}
