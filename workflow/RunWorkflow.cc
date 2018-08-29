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

string ReadLog()
{
  FILE *p = fopen("grapevine.log", "r");
  if (p == NULL) {
    string r = "\nERROR: Log file grapevine.log not found!!";
    return r;
  }

  fclose(p);
  FlatFileParser parser;
  
  parser.Open("grapevine.log");
  string ret = "\n";
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    ret += "\n" + parser.Line();
  } 
  ret += "\n";
  return ret;
}

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
  commandArg<bool> dryCmd("-dryrun","do not submit to the job queue", false);
  commandArg<string> mailCmd("-m","e-mail address for notifications", "");
  commandArg<string> dirCmd("-d","scripts directory", "grapevine_scripts");
  commandArg<string> subCmd("-s","queueing system (SLURM, local)", "SLURM");
  commandArg<bool> noLogCmd("-nolog","turn off system logging", false);
  //commandArg<string> oCmd("-o","output directory");
 
  
  commandLineParser P(argc,argv);
  P.SetDescription("Runs a grapevine workflow, specify a list of scripts via the -i option, e.g. -i script1,script2!");
  P.registerArg(sCmd);
  P.registerArg(iCmd);
  P.registerArg(dryCmd);
  P.registerArg(mailCmd);
  P.registerArg(dirCmd);
  P.registerArg(subCmd);
  P.registerArg(noLogCmd);
  //P.registerArg(oCmd);

  P.parse();

  string header = P.GetStringValueFor(iCmd);
  bool bDry = P.GetBoolValueFor(dryCmd);
  bool noLog = P.GetBoolValueFor(noLogCmd);
  string mail = P.GetStringValueFor(mailCmd);
  string s = P.GetStringValueFor(sCmd);
  string sub = P.GetStringValueFor(subCmd);
  string sdir = P.GetStringValueFor(dirCmd);
  string logDir = sdir + "/" + "runlog";
  
  int i, j;

  string cmmd = "mkdir " +  sdir;
  exec(cmmd);

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
    cmmd += " -o " + sdir + "/level_" + Stringify(i);
    string subbase = sdir + "/submit_" + Stringify(i);
    cmmd += " -s " + subbase;
    if (!noLog)
      cmmd += " -l " + logDir;
    exec(cmmd);

    UpdatePackages(packages);

    if (!bDry) {
      if (sub == "SLURM") {
	cout << "Submitting to SLURM" << endl;
	cmmd = exe_path + "RunGrapevineFlow " + subbase + ".sbatch";
	exec(cmmd);
	SendMail("Scripts batch " + Stringify(i) + " have finished on SLURM." + ReadLog(), mail);
      }
      if (sub == "local") {
	cout << "Running local" << endl;
	cmmd = subbase + ".bash";
	exec(cmmd);
	FILE * p = fopen("grapevine.log", "w");
	fprintf(p, "Local execution, check log files for status.\n");
	fclose(p);
	SendMail("Scripts batch " + Stringify(i) + " have finished on the local server." + ReadLog(), mail);
      }
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

