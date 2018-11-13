#include <string>

#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/StringUtil.h"

int exec(const string & s)
{
  cout << "Executing: " << s << endl;
  int ret = system(s.c_str());
  return ret;
}

/*
string ExecPath(const string & a)
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
  }*/

void SendMail(const string & msg, const string & mail)
{
  if (mail == "")
    return;
  string cmmd = "echo \"" + msg + "\" | mail -s \"Grapevine status update (automated, do not reply)\" " + mail;
  exec(cmmd);
}


void UpdatePackages(svec<string> & sw)
{
  FILE *p = fopen("grapevine.packages", "r");
  if (p == NULL) {
    cout <<  "\nERROR: Log file grapevine.packages not found!!" << endl;    
  }

  fclose(p);
  FlatFileParser parser;
  
  parser.Open("grapevine.packages");
  string ret = "\n";
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    sw.push_back(parser.Line());
  }
  UniqueSort(sw);

}

string ReadLog(const string & logFile)
{
  FILE *p = fopen(logFile.c_str(), "r");
  if (p == NULL) {
    string r = "\nERROR: Log file grapevine.log not found!!";
    return r;
  }

  fclose(p);
  FlatFileParser parser;
  
  parser.Open(logFile);
  string ret = "\n";
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    ret += "\n" + parser.Line();
  } 
  ret += "\n";
  return ret;
}

int CheckLog(const string & l)
{
  FILE *p = fopen(l.c_str(), "r");
  if (p == NULL) {
    string r = "\nERROR: Log file grapevine.log not found!!";
    return 0x7FFF;
  }

  fclose(p);
  FlatFileParser parser;
  
  parser.Open(l);
  int err = 0;
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    if (parser.AsString(parser.GetItemCount()-1) != "COMPLETED")
      err++;
  } 
 
  return err;
}

bool CopyTableAndHeader(const string & h, const string & dir)
{
  string o = dir + "/header.txt"; 
  FILE * p = fopen(o.c_str(), "w");

  FlatFileParser parser;
  
  parser.Open(h);



  while (parser.ParseLine()) {
    if (parser.GetItemCount() < 3) {
      fprintf(p, "%s\n", parser.Line().c_str());
      continue;
    }
    if (parser.AsString(0) == "@table") {
      string cmmd = "cp " + parser.AsString(2) + " " + dir + "/table.txt";
      exec(cmmd);
      string t = dir + "/table.txt";
      fprintf(p, "@table = %s\n", t.c_str());
    } else {      
      fprintf(p, "%s\n", parser.Line().c_str());
    }
  }


  fclose(p);
  return true;
}

//======================================================
int main(int argc,char** argv)
{

  string exe_path = ExecPath(argv[0]);
  
  cout << endl << "Welcome to GrapevineFlow!" << endl << endl;


  cout << " _____                            _           ______ _               " << endl;
  cout << "|  __ \\                          (_)          |  ___| |              " << endl;
  cout << "| |  \\/_ __ __ _ _ __   _____   ___ _ __   ___| |_  | | _____      __" << endl;
  cout << "| | __| '__/ _` | '_ \\ / _ \\ \\ / / | '_ \\ / _ \\  _| | |/ _ \\ \\ /\\ / /" << endl;
  cout << "| |_\\ \\ | | (_| | |_) |  __/\\ V /| | | | |  __/ |   | | (_) \\ V  V / " << endl;
  cout << " \\____/_|  \\__,_| .__/ \\___| \\_/ |_|_| |_|\\___\\_|   |_|\\___/ \\_/\\_/  " << endl;
  cout << "                | |                                                  " << endl;
  cout << "                |_|                                                  " << endl;

  
                                                                   
                                                                
  cout << endl << endl;

  commandArg<string> sCmd("-i","list of scripts");
  commandArg<string> iCmd("-head","data header file", "");
  commandArg<string> confCmd("-config","configuration file", "");
    commandArg<bool> dryCmd("-dryrun","do not submit to the job queue", false);
  commandArg<string> mailCmd("-m","e-mail address for notifications", "");
  commandArg<string> dirCmd("-o","project directory");
  commandArg<string> subCmd("-s","queueing system (SLURM, local)", "SLURM");
  commandArg<bool> errCmd("-e","continue if there are errors", false);
  commandArg<bool> noLogCmd("-nolog","turn off system logging", false);
  //commandArg<string> oCmd("-o","output directory");
 
  
  commandLineParser P(argc,argv);
  P.SetDescription("Runs a grapevine workflow, specify a list of scripts via the -i option, e.g. -i script1,script2!");
  P.registerArg(sCmd);
  P.registerArg(iCmd);
  P.registerArg(confCmd);
  P.registerArg(dryCmd);
  P.registerArg(mailCmd);
  P.registerArg(dirCmd);
  P.registerArg(subCmd);
  P.registerArg(errCmd);
  P.registerArg(noLogCmd);
  //P.registerArg(oCmd);

  P.parse();

  string config = P.GetStringValueFor(confCmd);
  string header = P.GetStringValueFor(iCmd);
  bool bDry = P.GetBoolValueFor(dryCmd);
  bool noLog = P.GetBoolValueFor(noLogCmd);
  bool lenient = P.GetBoolValueFor(errCmd);
  string mail = P.GetStringValueFor(mailCmd);
  string s = P.GetStringValueFor(sCmd);
  string sub = P.GetStringValueFor(subCmd);
  string projDir = P.GetStringValueFor(dirCmd);

  string logDir = projDir + "/" + "runlog";
  string scriptDir = projDir + "/grapevine_scripts";
 
  int i, j;

  if (config != "")
    config = " -config " + config;
  
  string cmmd = "mkdir " +  projDir;
  exec(cmmd);
 
  cmmd = "mkdir " + logDir;
  exec(cmmd);

  cmmd = "mkdir " + scriptDir;
  exec(cmmd);

  string pipe = logDir + "/grapevine.pipe";
  string logFile = logDir + "/grapevine.log";
  
  if (header == "") {
    header = projDir + "/header.txt";
  } else {    
    if (!CopyTableAndHeader(header, projDir)) {
      cout << "Aborting." << endl;
      return -1;
    }
    header = projDir + "/header.txt";
  }


  
  StringParser pp;
  pp.SetLine(s, ",");

  svec<string> scripts;

  for (i=0; i<pp.GetItemCount(); i++)
    scripts.push_back(pp.AsString(i));
  
   
  svec<string> packages;

  for (i=0; i<scripts.isize(); i++) {
    cout << "Processing script " << scripts[i] << endl;
  
    cmmd = exe_path + "/Grapevine -i " + scripts[i];
    if (header != "")
      cmmd += " -head " + header;
    cmmd += " -o " + scriptDir + "/level_" + Stringify(i);
    string subbase = scriptDir + "/submit_" + Stringify(i);
    cmmd += " -s " + subbase;
    if (!noLog)
      cmmd += " -l " + logDir;
    cmmd += config;
    
    exec(cmmd);

    UpdatePackages(packages);
    int err = 0;
    if (!bDry) {
      if (sub == "SLURM") {
	//cout << "Submitting to SLURM" << endl;
	cmmd = exe_path + "RunGrapevineFlow -q " + sub + " -p " + pipe + " -i " + subbase + ".sbatch";
	exec(cmmd);
	err = CheckLog(logFile);
	SendMail("Scripts batch " + Stringify(i) + " have finished on SLURM." + ReadLog(logFile), mail);
      }
      if (sub == "local") {
	cmmd = exe_path + "RunGrapevineFlow -q " + sub + " -p " + pipe + " -i " + subbase + ".bash";
	exec(cmmd);
	err = CheckLog(logFile);
	SendMail("Scripts batch " + Stringify(i) + " have finished on the local server." + ReadLog(logFile), mail);
      }
	/*
	cout << "Running local" << endl;
	cmmd = subbase + ".bash";
	exec(cmmd);
	FILE * p = fopen("grapevine.log", "w");
	fprintf(p, "Local execution, check log files for status.\n");
	fclose(p);
	SendMail("Scripts batch " + Stringify(i) + " have finished on the local server." + ReadLog(logFile), mail);*/
	
      //}
    }
    if (!lenient && err > 0) {
      SendMail("The Grapevine pipeline termiated because of errors.\n", mail);
      break;
    }
  }

  
  cout << endl << endl;
  
  cout << "Scripts have been written to the directory grapevine_scripts/" << endl;
  cout << endl << endl << endl;
  cout << "================ SOFTWARE REQUIRED FOR THIS WORKFLOW ===================" << endl;
  for (i=0; i<packages.isize(); i++) {
    cout << packages[i] << endl;
  }
  cout << "========================================================================" << endl;
  
  return 0;
  
}

