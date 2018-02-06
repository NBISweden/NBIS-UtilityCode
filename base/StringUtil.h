
#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <string.h>

using namespace std;

string After(string &s, string &t);
string Before(string &s, string &t);
bool Contains(string &s, string &t);

string After(string &s, char *t);
string Before(string &s, char *t);
bool Contains(string &s, char *t);

bool ContainsAt(string &s, string &t, int at);

int PositionAfter(string &in, string& s, int startSearchAt);

inline string ExecPath(const string & a)
{
  char b[512];
  strcpy(b, a.c_str());
  bool bF = false;
  for (int i=(int)a.length()-1; i>=0; i--) {
    if (b[i] == '/') {
      b[i+1] = 0;
      bF = true;
      break;
    }
  }
  if (!bF)
    strcpy(b, "");

  string bb = b;
  return bb;
}

inline string Stringify(int x)
{
  ostringstream out;
  out << x;
  return (out.str());
}


int Tokenize( const string &a_string,
	      vector<char> &separators,
	      vector<string> &tokens );


int Tokenize( const string &a_string,
	      vector<string> &tokens );




#endif
