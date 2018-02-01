# NBIS-UtilityCode

Welcome to the code base of the National Bioinformatics Infratsructure of Sweden!

To use it:
1. Get a copy of the source code by typing:

git clone https://github.com/NBISweden/NBIS-UtilityCode.git

2. Compile it (we recommend gcc 5.4 and up, but it will also work with earlier versions):

On Uppmax, do:
module load cmake  
module load gcc/5.4.0  

cd NBIS-UtilityCode  
make -j 4

3. Done! Run each executable and it will show you its options and a one line description og what it does.


To contribute to the code base:

1. To add a new executable, simple create a file with the extension .cc, and make sure that the main entry point is written exactly as follows:

int main( int argc, char** argv )

Add in some code, then simply do

make

and it will be added to the makefile systen and compile. Good old "Arachne magic" ;)
Any file that you #include is regarded a dependency, and if there is a .cc file with the same name in the same directory, it will automatically be compiled and linked by the executable.

2. There are a few guidelines that we would like you to follow, and they will make your life - and the life of all users - much easier. 

2.1 Use the commandLineParser for parameter processing. A simple example is this:

int main( int argc, char** argv )
{

  commandArg<string> fileCmmd("-i","input file"); //Need to know what file to process
  commandArg<int> fromCmmd("-from","from where to begin"); //Need to know where to start
  commandArg<double> valCmmd("-scale","scaling factor", 1.); //If not specified otherwise, we assume it's 1

  commandLineParser P(argc,argv);
  
  P.SetDescription("This module performs magic.");
  P.registerArg(fileCmmd);
  P.registerArg(fromCmmd);
  P.registerArg(valCmmd);
 
  P.parse();
  
  string fileName = P.GetStringValueFor(fileCmmd);
  int from = P.GetIntValueFor(fromCmmd);
  double val = P.GetDoubleValueFor(valCmmd);
 
2.2 For file reading, use the FlatFileParser. Simply do something like this:

  FlatFileParser parser;
  parser.Open(fileName);

  while (parser.ParseLine()) {
    if (parser.GetItemCount() < 3)
      continue;
    int a = parser.AsInt(0);
    string b = parser.AsString(1);
    double v = parser.AsFloat(2);
  }

Also, check out the StringParser.


2.3 Factor functionality out of your executable. Other might want to use it. On other words: keep your main routine as slim as possible and implement funtionality in proper headers and C++ files.

2.4 Even though the makefile system is dead easy and allows for creating tons of tools - please consider *where* to add your code.
We will continue to refine the directory structure, but for now, we follow these rules:

2.4.1. If it is of general purpose, e.g. a statistical test, graphing data etc., please put it into the appropriate folder right off of the root. These can be code files as well as main routines.

2.4.2. More application-specific code should go into the applications folder, or in a subfolder therein respectively. 


2.5 Use the coding language of your choice, as long as it is one of the following: C++, C, Java, Lua, or Python. Note that perl is not permitted (for R, matlab, and workflow languages, see below) Furthermore:

2.5.1. C++: just follow the guidelines above.

2.5.2. Java: (a) make a separate folder with all your files per executable; (b) make sure to include the .jar file; (c) include information about how to build the executable, especially if you use eclipse, netbeans etc.

2.5.3. Lua: no specific rules.

2.5.4. Python: no specific rules, even though we would like feedback if there should be any.


2.6 Workflows and pipelines. We also include those and encourage you to commit them to the 'pipelines' folder. The rules here are:

2.6.1. Document how exactly to run the pipepline or workflow, and what other tools have to be installed. 

2.6.2. State any restrictions or requirements, and that includes the computing environment (e.g. SLURM versys LSF etc.)

2.6.3. If possible, provide scripts to set up all dependencies on external modules, such as aligners, etc.


2.7 R, matlab and octave scripts are also welcome. Please add them to the respective R and matlab folders.



