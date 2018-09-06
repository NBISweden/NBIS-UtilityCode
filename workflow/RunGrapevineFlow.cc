#include <string>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/ThreadHandler.h"
#include "base/StringUtil.h"

string exec(const char* cmd);


class PipeThread : public IOneThread
{
public:
  PipeThread(const string & path) {
    m_execPath = path;
  }

  void SetExecPath(const string & path) {
    m_execPath = path;
  }
  
protected:

  virtual bool OnDie() {
    cout << "Killed!!" << endl;
    return true;
  }

  virtual bool OnDo(const string & pipe) {
    cout << "Start listening to: " << pipe << endl;
    int index = 0;
    
    do {
      cout << "Check pseudopipe." << endl;
      //string rr = "cat < " + pipe;
      //string result = exec(rr.c_str());
      //cout << "Pipe returned " << result << endl;
      //StringParser p;
      //char tmp[256];
      //tmp[0] = 10;
      //tmp[1] = 0;
      //p.SetLine(result, tmp);
      sleep(1);
      
      FlatFileParser parser;
  
      parser.Open(pipe);
      svec<string> p;
      int k = 0;
      while (parser.ParseLine()) {
	if (parser.GetItemCount() == 0)
	  continue;
	if (k >= index)
	  p.push_back(parser.Line());
	k++;
      }
      index = k;
	
      cout << "Messages: " << p.isize() << endl;
      for (int i=0; i<p.isize(); i++) {
	if (strstr(p[i].c_str(), "RunGrapevineFlow") != NULL)
	  continue;
	if (p[i] == "exit") {
	  cout << "Got directive exit" << endl;
	  return true;
	}

	string cmmd = p[i];
	StringParser pp;
	pp.SetLine(p[i]);
	if  (pp.AsString(0) == "EditTable")
	  cmmd = m_execPath + "/" + p[i];

	
	if (!Internal(pipe, pp)) {
	  cout << "Executing locally: " << cmmd << endl;
	  int d = system(cmmd.c_str());
	}
      }
    } while (true);
    return true;
  }
  
  virtual bool OnInitialize(const string & msg) {
    //IOneThread::Initialize(msg);
    return true;
  }

  bool Internal(const string & pipe, StringParser & p) {
       
    int i;
    if (p.AsString(0) != "!terminal")
      return false;

    string pipeOut = pipe + "_fb";
    
    string cmmd = "echo Sorry, I do not understand, try again. > "  + pipeOut;
    if (p.AsString(1) == "hello")
      cmmd = "echo Welcome! > " + pipeOut;

    if (p.AsString(1) == "exec" || p.AsString(1) == "execute") {
      string l;
      for (i=2; i<p.GetItemCount(); i++) {
	l += p.AsString(i) + " ";
      }
      cout << "Executing " << l << endl;
      string r; // = exec(l.c_str());
      cout << "Returned " << r << endl;
      cmmd = "echo response\n" + r + " > " + pipeOut;
    }
    
    int ret = system(cmmd.c_str());

    return true;
  }
  
  string m_execPath;
};


//-------------------------------------------

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

  /*
  if (argc < 2 || strcmp(argv[1], "-help") == 0 ||  strcmp(argv[1], "--help") == 0) {
    cout << "Usage: RunGrapevineFlow script1 script2 script3..." << endl;
    return 0;    
    }*/
  commandArg<string> sCmd("-i","list of scripts");
  commandArg<string> qCmd("-q","job scheduling system, can be SLURM or local");
  commandArg<string> pCmd("-p","communication file (also location of log file)", "");
  
  commandLineParser P(argc,argv);
  P.SetDescription("Executes a grapevine workflow locally or using SLURM");

  P.registerArg(sCmd);
  P.registerArg(qCmd);
  P.registerArg(pCmd);

  P.parse();

  string input = P.GetStringValueFor(sCmd);
  string job = P.GetStringValueFor(qCmd);
  string pipe = P.GetStringValueFor(pCmd);

  
  svec<string> scripts;
  
  int i, j;

  StringParser p;
  p.SetLine(input, ",");
  
  for (i=0; i<p.GetItemCount(); i++)
    scripts.push_back(p.AsString(i));

  char logName[1024];
  strcpy(logName, pipe.c_str());
  for (i=strlen(logName)-1; i>=0; i--) {
    if (logName[i] == '/') {
      logName[i] = 0;
      break;
    }
  }
  strcat(logName, "/grapevine.log");

  cout << "Log file name: " << logName << " " << pipe << endl;
  
  FILE * pGrapeLog = fopen(logName, "w");

  // Start listening thread
  ThreadHandler th;

  
  if (pipe != "") {
    string mp = "rm " + pipe;
    exec(mp.c_str());
 
    th.AddThread(new PipeThread(ExecPath(argv[0])));    
    th.Feed(0, pipe);

    cout << "Stuffing pipe" << endl;
    mp = "echo RunGrapevineFlow >> " + pipe;
    int x = system(mp.c_str());
    cout << "done" << endl;
  }

  cout << "Continue" << endl;

  
  if (job == "SLURM") {
    cout << "Submit to SLURM" << endl;
    for (j=0; j<scripts.isize(); j++) {
      
      svec<string> ids, status;
      
      FlatFileParser parser;
      
      parser.Open(scripts[j]);
      
      while (parser.ParseLine()) {
	if (parser.GetItemCount() == 0)
	  continue;
	
	string id;
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
  }

  if (job == "local") {
    cout << "Running locally" << endl;
    for (i=0; i<scripts.isize(); i++) {
      cout << "Execute " << scripts[i] << endl;
      int rrr = system(scripts[i].c_str());
    }    
  }
  cout << "All processes are finished!" << endl;
  //sleep(200);
  
  fclose(pGrapeLog);
  if (pipe != "") {
    cout << "Cleaning up named pipe." << endl;
    string mp = "echo exit >> " + pipe;
    int x = system(mp.c_str());
    while (!th.AllDone()) {
      usleep(10000);
    }
  }

  return 0;
}
