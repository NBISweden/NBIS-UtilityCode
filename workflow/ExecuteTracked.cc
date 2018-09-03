#include <string>
#include <stdio.h>

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "util/SysTime.h"
#include <time.h>

class Elapsed
{
public:
  Elapsed() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);    
  }

  double InSeconds() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    double result = (stop.tv_sec - start.tv_sec) * 1e6 + (stop.tv_nsec - start.tv_nsec) / 1e6;    // in seconds
    return result;
  }
  
private:
  struct timespec start, stop;
};


string GetID(const string & date)
{
  StringParser p;
  p.SetLine(date, ":");
  int n = p.AsInt(p.GetItemCount()-1);
  int m = n | ((2 ^ 16)-1);
  string s = Stringify(m);
  s = "ID=" + s;
  //cout << s << endl;
  return s;
}

int main( int argc, char** argv )
{

  
  if (argc < 4) {
    cout << "Usage: ExecuteTracked -o <logfile> <command>..." << endl;
    return -1;    
  }
  
  string o = argv[1];
  if (o != "-o") {
    cout << "Usage: ExecuteTracked -o <logfile> <command>..." << endl;
    return -1;
  }
  
  string log = argv[2];

  FILE *p = fopen(log.c_str(), "a");
  if (p == NULL) {
    cout << "ERROR opening log file: " << log << endl;
    return -2;
  }
  
  Elapsed timer;
  int i;
  string l = GetTimeStatic();
  string pid = GetID(l);
  l += " " + pid + " START ";
  string cmmd;
  for (i=3; i<argc; i++) {
    l += " ";
    l += argv[i];
    cmmd += argv[i];
    cmmd += " ";
  }
  fprintf(p, "%s\n", l.c_str());
  fflush(p);
  l = "";

  int ret = system(cmmd.c_str());
  
  fprintf(p, "ELAPSED %s %f\n", pid.c_str(), timer.InSeconds());
  
  if (ret != 0) {
    l = GetTimeStatic() + " ERROR " + pid + " program returned ";
    fprintf(p, "%s %d and died.\n", l.c_str(), ret);
    fclose(p);
    return ret;
  }

  l = GetTimeStatic() + " FINISH " + pid;
  fprintf(p, "%s\n", l.c_str());
  fclose(p);
  return ret;
}
