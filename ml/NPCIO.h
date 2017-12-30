#ifndef NPCIO_H
#define NPCIO_H

#include "base/SVector.h"


class NPCIO
{
 public:
  NPCIO() {
    m_neuron = -1;
  }

  void AddElement(const string & name, double val, bool bValid = true);

  void AddElement(const string & name);
  void SetElement(const string & name, double v);
  void SetValid(const string & name, bool b);

  double GetElement(const string & name) const;
  double IsValid(const string & name) const;
  double Distance(const NPCIO & n) const;
  double Distance(const svec<double> & n) const;

  int isize() const {return m_data.isize();}
  void resize(int n) {
    m_data.resize(n, 0.);
    m_valid.resize(n, 1);
  }
  const double & operator[] (int i) const {return m_data[i];}
  double & operator[] (int i) {return m_data[i];}

  const string Name(int i) const {return m_name[i];}
 
  bool IsValid(int i) const {return (m_valid[i] > 0);}
  void SetValid(int i, bool b) {
    if (m_valid.isize() == 0)
      m_valid.resize(m_data.isize(), 1);
    m_valid[i] = 0;
    if (b)
      m_valid[i] = 1;
  }

  void SetAllValid(bool b) {
    for (int i=0; i<isize(); i++)
      SetValid(i, b);
  }
  
  void operator = (const svec<double> & n) {
    m_data = n;
  }
  void operator = (double n) {
    for (int i=0; i<m_data.isize(); i++)
      m_data[i] = n;
  }

  virtual void Print() const;

  int GetNeuron() const {return m_neuron;}
  void SetNeuron(int n) {m_neuron = n;}

  const svec<double> & data() const {return m_data;}

  
 protected:
  int Index(const string & name) const {
    for (int i=0; i<m_name.isize(); i++) {
      if (m_name[i] == name)
	return i;
    }
    return -1;
  }

  svec<double> m_data;
  svec<string> m_name;
  svec<int> m_valid;
  int m_neuron;

};



#endif //NPCIO_H
