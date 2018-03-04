#include <string>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>

#include "base/CommandLineParser.h"
#include "base/FileParser.h"


string exec(const char* cmd) {
  //cout << "EXECUTING: " << cmd << endl;
  char buffer[2048];
  string result = "";
  FILE* pipe = popen(cmd, "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
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

string SubmitOne(const string & cmd)
{
  cout << "Submitting: " << cmd << endl;
  string ret = exec(cmd.c_str());
  StringParser p;
  p.SetLine(ret);
  if (p.GetItemCount() != 4) {
    cout << "ERROR, Slurm returned " << ret << endl;
    return "-1";
  }
  string procID = p.AsString(3);
  if (procID[procID.size()-1] == '\n')
    procID[procID.size()-1] = 0;
  cout << "Submitted " << cmd << " - process ID: " << procID << endl;
  return procID;
}

bool CheckExit(string & ret, string & succ, const string & id)
{
  string cmd = "sacct --jobs " + id /* + " | grep " + id + " |grep -v \\.ba"*/;
  //cout << "Checking: " << cmd << endl;
  string r = exec(cmd.c_str());
  StringParser p;
  p.SetLine(r);
  //cout << ret << endl;
  if (p.GetItemCount() < 21) {
    ret = "<unknown>";
    succ = "<unknown>";
  } else {
    ret = p.AsString(20);
    succ = p.AsString(19);
  }
  return true;
}

int main( int argc, char** argv )
{

  if (argc < 2 || strcmp(argv[1], "-help") == 0 ||  strcmp(argv[1], "--help") == 0) {
    cout << "Usage: RunGrapevineFlow script1 script2 script3..." << endl;
    return 0;    
  }
  

  svec<string> scripts;
  
  int i, j;

  for (i=1; i<argc; i++)
    scripts.push_back(argv[i]);

  FILE * pGrapeLog = fopen("grapevine.log", "w");
  
  for (j=0; j<scripts.isize(); j++) {
  
    svec<string> ids, status;

    FlatFileParser parser;
  
    parser.Open(scripts[j]);

    while (parser.ParseLine()) {
      if (parser.GetItemCount() == 0)
	continue;
    
      string id = "11794110";
      id = SubmitOne(parser.Line());
      if (id == "-1") {
	cout << "*****************************************" << endl;
	cout << "ERROR during submission!!!" << endl;
	cout << "Check your permissions and accounts!" << endl;
	fprintf(pGrapeLog, "There were ERRORS during job submission, check your permissions and accounts!\n");
	fclose(pGrapeLog);
	return -1;
      }
      ids.push_back(id);
      status.push_back("");
    }
      
    int counter = ids.isize();

    while (counter > 0) {

      for (i=0; i<ids.isize(); i++) {
	if (ids[i] == "")
	  continue;
	
	string cmd = "squeue -j " + ids[i] /*+ " | grep " + ids[i]*/;
	//cout << cmd << endl;
	string ret = exec(cmd.c_str());
	//cout << "RETURNED " << endl << ret << endl;
	StringParser p;
	p.SetLine(ret);
	if (p.GetItemCount() < 9) {	  
	  counter--;
	  string ret1, stat;
	  CheckExit(ret1, stat, ids[i]);
	  cout << "Process " << ids[i] << " finished with ret " << ret1 << " status " << stat << endl;
	  fprintf(pGrapeLog, "Process %s has finished with return code %s status %s\n", ids[i].c_str(), ret1.c_str(), stat.c_str());
	  ids[i] = "";
	  continue;
	}
	string s = p.AsString(12);
	string t = p.AsString(13);
	if (s != status[i]) {
	  status[i] = s;
	  cout << "Program " << ids[i] << " has status " << s << endl;
	}
      }
      usleep(1000000);
    }
    cout << "Script " << scripts[j] << " is complete." << endl;
  }
  cout << "Process is finished. " << endl;

  fclose(pGrapeLog);

  return 0;
}
