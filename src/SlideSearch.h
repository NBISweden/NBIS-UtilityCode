#ifndef SLIDESEARCH_H_
#define SLIDESEARCH_H_

#include "base/SVector.h"
#include "src/DNAVector.h"

class SlideMatch
{
 public:
  SlideMatch() {
    m_start = -1;
    m_start2 = 0;
    m_end = -1;
    m_mis = -1;
    m_ori = 0;
  }

  SlideMatch(int start, int end, int mis, int ori, int start2 = 0) {
    m_start = start;
    m_end = end;
    m_mis = mis;
    m_ori = ori;
    m_start2 = start2;
  }


  int Start() const {return m_start;}
  int Start2() const {return m_start2;}
  int End() const {return m_end;}
  int Mis() const {return m_mis;}
  int Ori() const {return m_ori;}
  

 private:
  int m_start;
  int m_start2;
  int m_end;
  int m_mis;
  int m_ori;
};



class SlideSearch
{
 public:
  SlideSearch() {
    m_maxMis = 2;
  }
  void SetMis(int i) {m_maxMis = i;}

  bool Search(svec<SlideMatch> & out, const DNAVector & ref, const DNAVector & seq);

  bool Search(svec<SlideMatch> & out, const DNAVector & ref, const DNAVector & seq, int ori);

  bool SearchFlex(svec<SlideMatch> & out, const DNAVector & ref, const DNAVector & seq, int minLen);

 private:
  
  int m_maxMis;
};

#endif 




