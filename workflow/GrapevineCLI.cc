#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

string exec(const char* cmd) {
  //cout << "EXECUTING: " << cmd << endl;
  char buffer[2048];
  string result = "";
  FILE* pipe = popen(cmd, "r");
  if (!pipe) { 
    cout << "Could not open pipe!!" << endl;
    throw ;
  }
  try {
    while (!feof(pipe)) {
      if (fgets(buffer, sizeof(buffer), pipe) != NULL)
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

  commandArg<string> fileCmmd("-i","project directory");
  commandLineParser P(argc,argv);
  P.SetDescription("Command line interface to the Grapevine Workflow.");
  P.registerArg(fileCmmd);
 
  P.parse();
  
  string pipeIn = P.GetStringValueFor(fileCmmd) + "/runlog/grapevine.pipe";
  string pipeOut = P.GetStringValueFor(fileCmmd) + "/runlog/grapevine.pipe_fb";

  string cmmd = "mkfifo " + pipeOut;
  int ret = system(cmmd.c_str()); 


  cmmd = "echo !terminal hello > " + pipeIn;  
  ret = system(cmmd.c_str());

  cmmd = "cat < " + pipeOut;
  string result = exec(cmmd.c_str());

  cout << "Connected to project " << P.GetStringValueFor(fileCmmd) << endl;
  cout << "Grapevine responds " << result << endl;
  cout << "Ready, type 'exit' to exit" << endl << endl;
  
  string usr;

  char szText[2018];
  
  while (usr != "exit") {
    cout << "> "; 
    cin.getline(szText, sizeof(szText));
    usr = szText;
    cout << usr << endl;
    
    cmmd = "echo !terminal " + usr + " > " + pipeIn;  
    ret = system(cmmd.c_str());

    cmmd = "cat < " + pipeOut;
    result = exec(cmmd.c_str());
    cout << "> " << result << endl;

  }
  
  return 0;
}
