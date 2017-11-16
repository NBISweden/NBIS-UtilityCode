#include "src/SlideSearch.h"





bool SlideSearch::Search(svec<SlideMatch> & out, const DNAVector & ref, const DNAVector & seq)
{
  Search(out, ref, seq, 1);
  DNAVector tmp = seq;
  tmp.ReverseComplement();
  Search(out, ref, tmp, -1);
  return true;
}

bool SlideSearch::Search(svec<SlideMatch> & out, const DNAVector & ref, const DNAVector & seq, int ori)
{
  //out.clear();

  int i, j, k;

  for (i=0; i<=ref.isize()-seq.isize(); i++) {
    k = 0;
    bool b = true;
    for (j=0; j<seq.isize(); j++) {
      if (DNA_Equal(ref[i+j], seq[j]) < 0.1)
	k++;
      if (k > m_maxMis) {
	b = false;
	break;
      }
    }
    if (b && k <= m_maxMis) {
      out.push_back(SlideMatch(i, i+seq.isize(), k, ori));
    }
  }
  return true;
}

bool SlideSearch::SearchFlex(svec<SlideMatch> & out, const DNAVector & ref, const DNAVector & seq, int minLen)
{
  int i, j, k;

  

  for (i=0; i<=ref.isize(); i++) {
    for (j=0; j<seq.isize(); j++) {
      int mis = 0;
      for (k=0; k<seq.isize(); k++) {
	int i1 = i+k;
	int j1 = j+k;
	if (i1 >= ref.isize())
	  break;
	if (j1 >= seq.isize())
	  break;
	
	if (ref[i1] != seq[j1])
	  mis++;
	int maxMis = m_maxMis * (k/minLen+1);
	if (mis > m_maxMis) {
	  break;
	}
      }
      if (k > minLen) {
	out.push_back(SlideMatch(i, i+k-1, k-1, 1, j));
      }
    }
  }
  return true;
}


