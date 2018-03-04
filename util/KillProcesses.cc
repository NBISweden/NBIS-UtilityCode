#include "util/mutil.h"
#include <iostream>
#include <stdlib.h>
#include <cstdio>
using namespace std;

int main(int argc, char * argv[])
{

  CMAsciiReadFileStream in;

  //in.Open(argv[1]);

  CMString line;

  //int col = atol(argv[2]);
  
  CMString s = "ps -A |grep ";
  s += argv[1];
  s += " > kill.tmp"; 
  cout << s << endl;
  int r1 = system(s);
  in.Open("kill.tmp");
    

  int k = 0;
  while (!in.IsEnd()) {
    in.ReadLine(line);
    CMPtrStringList tokens;
    Tokenize(tokens, line);
    CMString kill = "kill ";
    kill += *tokens(0);
    r1 = system(kill);
    cout << kill << endl;
    
  }


  in.Close();


}
