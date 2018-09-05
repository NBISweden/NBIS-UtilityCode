#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

#include "workflow/TableManip.h"
#include "base/StringUtil.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "util/SysTime.h"


int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","table file");
  commandArg<string> actCmmd("-a","action (add_column remove_column set_in_column set_in_column_from_file fill_column_from_file add_to_column remove_headers)");
  commandArg<string> parCmmd("-p","parameter", "");
  commandArg<string> labCmmd("-l","label", "");
  commandArg<int> idxCmmd("-idx","index", -1);
  commandArg<int> colCmmd("-col","column to read", 0);
  
  commandLineParser P(argc,argv);
  P.SetDescription("Table editor.");
  P.registerArg(fileCmmd);
  P.registerArg(actCmmd);
  P.registerArg(parCmmd);
  P.registerArg(labCmmd);
  P.registerArg(idxCmmd);
  P.registerArg(colCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string actName = P.GetStringValueFor(actCmmd);
  string parName = P.GetStringValueFor(parCmmd);
  int idx = P.GetIntValueFor(idxCmmd);
  int col = P.GetIntValueFor(colCmmd);
  string labName = P.GetStringValueFor(labCmmd);
  
  int i, j;

  TableManipulator tm(fileName);

  /*
  int max = 10;
  if (idx >= 0) {
    cout << "Waiting my turn: " << idx << endl;
    int s = -1;

#ifndef __APPLE__

    do {
      usleep(10000);
      struct timespec tv;
      clock_gettime( CLOCK_REALTIME, &tv);
      s = tv.tv_sec % max;
      //s /= 10;
      //cout << s << endl;
      //s = s % 60;
      //cout << s << endl;
    } while (s != (idx % max));    
#endif  
  }*/

  cout << "Edit table " << fileName << " at " << GetTimeStatic() << endl;
  
  if (actName == "add_column") {
    tm.AddColumn(labName);
    cout << "Complete" << endl;
    return 0;
  }
  if (actName == "remove_column") {
    tm.RemoveColumn(labName);
    cout << "Complete" << endl;
    return 0;
  }
  if (actName == "set_in_column") {
    tm.SetInColumn(labName, idx, parName);
    cout << "Complete" << endl;
    return 0;
  }
  if (actName == "add_to_column") {
    tm.AddToColumn(labName, parName);
    cout << "Complete" << endl;
    return 0;
  }
  if (actName == "remove_headers") {
    tm.RemoveHeaders();
    cout << "Complete" << endl;
    return 0;
  }
  if (actName == "set_in_column_from_file") {
    if (idx < 0) {
      cout << "ERROR: No index given!!" << endl;
      return -1;
    }
    FlatFileParser parser;
    parser.Open(parName);
    parser.ParseLine();
    tm.SetInColumn(labName, idx, parser.AsString(0));
    cout << "Complete" << endl;
    return 0;        
  }
  
  if (actName == "fill_column_from_file") {
    if (idx < 0) {
      cout << "ERROR: No index given!!" << endl;
      return -1;
    }
    FlatFileParser parser;  
    parser.Open(parName);
    svec<string> s;
    
    while (parser.ParseLine()) {
      if (parser.GetItemCount() == 0)
	continue;
      s.push_back(parser.AsString(col));
    }
    tm.FillColumn(labName, s, idx);
  }

  cout << "Complete" << endl;

  return 0;
}
