#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"


class Test
{
public:
  Test() {
    m_text = "initialized";
  }
  void Print() const {
    cout << "PRINT " << m_text << endl;
  }
private:
  string m_text;
};

int main( int argc, char** argv )
{

  if (true) {
  Loop:
    Test t;
    t.Print();
    
  } else {
    goto Loop;
  }
  
  goto Exit;
  
  cout << "Something" << endl;

 Exit:
  return 0;
}
