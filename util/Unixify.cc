
#include <string>

#include "base/CommandLineParser.h"

int main(int argc,char** argv)
{

  commandArg<string> aStringCmmd("-i","input");
  

  commandLineParser P(argc,argv);
  P.SetDescription("Unixifies stupid Windows text files.");
  P.registerArg(aStringCmmd);

  P.parse();

  string aString = P.GetStringValueFor(aStringCmmd);

  int i;
  FILE * p = fopen(aString.c_str(), "rb");
  char szText[64];
  szText[1] = 0;
 
  //while (fscanf(p, szText, 1, p) != EOF) {
  while (fread((void*)szText, 1, 1, p) == 1) {
    if (feof(p) != 0)
      break;
   
    if (szText[0] < 16 && szText[0] > 9) {      
      cout << endl;
      continue;
    }
    cout << szText;
    if (feof(p) != 0)
      break;
   }

  fclose(p);

}
