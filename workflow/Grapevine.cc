#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"

#include "workflow/ScriptParser.h"
#include "base/StringUtil.h"



void WrapSingle(string & line, const string & log, const string & pipe)
{
  StringParser pp;
  pp.SetLine(line);

  // Let's not make any decisions early on here and defer this to later instead.
  
  if (pp.GetItemCount() == 0)
    return;
  if (pp.AsString(0)[0] == '#')
    return;
   
  if (pp.AsString(0) == "module")
    return;
  if (pp.AsString(0) == "export")
    return;
  if (pp.AsString(0) == "set")
    return;
  if (pp.AsString(0) == "MakeDir")
    return;
  if (pp.AsString(0) == "rm")
    return;
  if (pp.AsString(0) == "TryRemove")
    return;
  if (pp.AsString(0) == "wait")
    return;
 
  if (pp.AsString(0) == "EditTable") {
    string send = "echo \"" + line + "\" >> " + pipe;
    line = send;
    return;
  }
  
  if (pp.AsString(0) == ">local") {
    string send = "echo \"";
    for (int i=1; i<pp.GetItemCount(); i++)
      send += " " + pp.AsString(i);
    send += "\" > " + pipe;
    line = send;
    return;
  }
  
  string out = "ExecuteTracked -o " + log + " " + line;
  line = out;
  
}

string Wrap(const string & line, const string & log, const string & pipe)
{
  StringParser pp;
  char delim[16];
  delim[0] = 10;
  delim[1] = 0;
  pp.SetLine(line.c_str(), delim);
  //cout << "WRAP: raw ";

  /*
  int i = 0;
  while (line[i] != 0) {
    if (line[i] >= ' ')
      cout << line[i];
    else {
      int n = line[i];
      cout << endl << n << endl;
    }
    i++;
  }
  cout << endl;
  printf("WRAP printf %s\n", line.c_str());
  */
  
  //cout << "WRAP: Lines " << pp.GetItemCount() << endl;
  string out;
  for (int i=0; i<pp.GetItemCount(); i++) {
    string in = pp.AsString(i);
    //cout << "WRAP Before " << in << endl;
    WrapSingle(in, log, pipe);
    //cout << "WRAP After " << in << endl;
    out += in;
    if (i<pp.GetItemCount()-1)
      out += delim;
  } 
  return out;
}

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","script file");
  commandArg<string> headCmmd("-head","header file", "");
  commandArg<string> outCmmd("-o","output script file");
  commandArg<string> subCmmd("-s","submit script prefix", "submit");
  commandArg<string> grCmmd("-g","grammar directory", "");
  commandArg<string> logCmmd("-l","log directory", "");
  //commandArg<bool> helpCmmd("-h","help", false);
  commandLineParser P(argc,argv);
  P.SetDescription("Natural language-like workflow manager.");
  P.registerArg(fileCmmd);
  P.registerArg(headCmmd);
  P.registerArg(outCmmd);
  P.registerArg(subCmmd);
  P.registerArg(grCmmd);
  P.registerArg(logCmmd);
  // P.registerArg(helpCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  string headName = P.GetStringValueFor(headCmmd);
  string outName = P.GetStringValueFor(outCmmd);
  string subName = P.GetStringValueFor(subCmmd);
  string grName = P.GetStringValueFor(grCmmd);
  // bool bHelp = P.GetBoolValueFor(helpCmmd);
  string logDir = P.GetStringValueFor(logCmmd);

  if (logDir != "") {
    string md = "mkdir " + logDir;
    int r = system(md.c_str());
  }

  string pipe = logDir + "/grapevine.pipe"; 
  
  int i, j;


  if (grName == "")
    grName = ExecPath(argv[0]);
  
  ScriptParser p;

  p.SetGrammarPath(grName);

  cout << "Using grammar path " << grName << endl;
  
  p.Read(fileName);
 
  if (headName != "")
    p.Read(headName);
  
  //p.UnwrapLoops();
  
  //p.Prepend("folder", "sample", "/");

  
  svec<string> script;
  
  for (j=0; j<p.GetCount(); j++) {
    p.Process(j);


    string name = outName;
    if (p.GetCount() > 1) {
      name += ".";
      name += Stringify(j);
    }

    string logFile;
    if (logDir != "") {
      StringParser pp;
      pp.SetLine(name, "/");
      logFile = logDir + "/" + pp.AsString(pp.GetItemCount()-1) + ".log";
      string rmcmmd = "rm " + logFile;
      int rx = system(rmcmmd.c_str());
    }
    
    script.push_back(name);
    FILE * pOut = fopen(name.c_str(), "w");
    if (pOut == NULL) {
      cout << "ERROR: can't write to " << name << endl;
      return -1;
    }

    cout << "*****************************************************" << endl;
    cout << "***************** Generating script " << j << endl;
    for (i=0; i<p.isize(); i++) {
      //if (!p.IsSilent(i)) {
      string wrap = p[i];
      if (logDir != "") {
	wrap = Wrap(wrap.c_str(), logFile, pipe);
      }
      fprintf(pOut, "%s\n", wrap.c_str());
      //fprintf(stdout, "CHECK  %s\n", wrap.c_str());
      //cout << "FINAL " << p[i] << endl;
      //} else {
      //cout << "SILENT: " << p[i] << endl;
      //}
    }
    
    fclose(pOut);

    string me = "chmod +x " + name;
    int r = system(me.c_str());
    

    cout << "***************** DONE " << endl;
    cout << "*****************************************************" << endl;
  }
  

  cout << "Writing submit scripts..." << endl;


  string s = subName + ".sbatch";
  string b = subName + ".bsub";
  string q = subName + ".qsub";
  string a = subName + ".bash";
  FILE * pS = fopen(s.c_str(), "w"); 
  FILE * pB = fopen(b.c_str(), "w"); 
  FILE * pQ = fopen(q.c_str(), "w"); 
  FILE * pA = fopen(a.c_str(), "w"); 

  for (i=0; i<script.isize(); i++) {
    fprintf(pS, "sbatch %s\n", script[i].c_str());
    fprintf(pB, "bsub %s\n", script[i].c_str());
    fprintf(pQ, "qsub %s\n", script[i].c_str());
    fprintf(pA, "%s > log%d.out &\n", script[i].c_str(), i);
    fprintf(pA, "sleep 1\n");
  }
  
  fprintf(pA, "wait\n");
  

  fclose(pS);
  fclose(pB);
  fclose(pQ);
  fclose(pA);

  string sx = "chmod +x " + s;
  string bx = "chmod +x " + b;
  string qx = "chmod +x " + q;
  string ax = "chmod +x " + a;
  //cout << "Run " << 
  int rrr = system(sx.c_str());
  rrr = system(bx.c_str());
  rrr = system(qx.c_str());
  rrr = system(ax.c_str());
  
  cout << "Done" << endl;
  
  const svec<SoftwarePackage> & sw = p.GetPackages();
  cout << endl;
  FILE * package = fopen("grapevine.packages", "w");
  
  cout << "============= REQUIRED SOFTWARE PACKAGES ==============" << endl;
  for (i=0; i<sw.isize(); i++) {
    cout << sw[i].Name() << " " << sw[i].Version() << endl;
    fprintf(package, "%s %s\n", sw[i].Name().c_str(), sw[i].Version().c_str());
  }
  cout << "============= REQUIRED SOFTWARE PACKAGES ==============" << endl;
  fclose(package);
  return 0;
}
