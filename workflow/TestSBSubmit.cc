#include <string>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>

#include "base/CommandLineParser.h"
#include "base/FileParser.h"

#include "workflow/ScriptParser.h"

string exec(const char* cmd) {
  char buffer[2048];
  string result = "";
  FILE* pipe = popen(cmd, "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
    while (!feof(pipe)) {
      if (fgets(buffer, 128, pipe) != NULL)
	result += buffer;
    }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  return result;
}

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","sbatch script");
  commandLineParser P(argc,argv);
  P.SetDescription("Sends stuff to SLURM.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);

  int i, j;

  string cmd = "sbatch " + fileName;
  
  string ret = exec(cmd.c_str());
  StringParser p;
  p.SetLine(ret);
  if (p.GetItemCount() != 4) {
    cout << "ERROR, Slurm returned " << ret << endl;
    return -1;
  }
  string procID = p.AsString(3);
  cout << "Submitted " << cmd << "Process ID: " << procID << endl;

  cmd = "squeue | grep " + procID;

  string status = "";
  
  while (true) {
    ret = exec(cmd.c_str());
    if (ret == "")
      break;
    p.SetLine(ret);
    string s = p.AsString(4);
    string t = p.AsString(5);
    if (s != status) {
      status = s;
      cout << "Program " << procID << " has status " << s << endl;
    }
    usleep(1000000);
  }
  cout << "Process is finished. " << endl;

  
  return 0;
}
