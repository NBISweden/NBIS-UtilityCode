#include "workflow/ScriptParser.h"

void Table::Read(const string & fileName)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  parser.ParseLine();

  resize(parser.GetItemCount());

  int i;

  for (i=0; i<parser.GetItemCount(); i++)
    m_columns[i].Label() = parser.AsString(i);
  
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    for (i=0; i<parser.GetItemCount(); i++)
      m_columns[i].push_back(parser.AsString(i));
  }

}


int ScriptParser::Read(const string & fileName)
{
  FlatFileParser parser;
  
  parser.Open(fileName);


  int i;

  
  while (parser.ParseLine()) {
    //if (parser.GetItemCount() == 0)
    //continue;
    Command tmp;
    
    
    tmp.Raw() = parser.Line();
    bool bPre = false;
    if (parser.GetItemCount() >= 3) {
      if (parser.AsString(0) == "@table") {
	bPre = true;
	cout << "Reading table from " << parser.AsString(2) << endl; 
	m_table.Read(parser.AsString(2));
      } else {
	// TODO: dynamic variable assignment!!!!
	if (parser.AsString(0)[0] == '@') {
	  cout << "Setting variable" << parser.AsString(0) << " to " << parser.AsString(2) << endl;
	  int idx = AddVariable(parser.AsString(0));
	  m_vars[idx].Value() = parser.AsString(2);
	} 
	
      }
      
      if (parser.AsString(0) == ">grammar") {
	bPre = true;
	cout << "Reading grammar from " << parser.AsString(2) << endl;

	try {
	  cout << "Loading grammar: " << parser.AsString(2) << endl;
	  GRAMMAR_HANDLE h = m_grmStack.AddGrammaAndReadFromFile(parser.AsString(2).c_str());	 
	  cout << "Success!" << endl;
	}

	catch(CMException & ex) {
	  
	  cout << "Exception cought during load!!" << endl;
	  ex.Print();
	  return -1;
	}	
      }
    }

    if (!bPre) {
      for (i=0; i<parser.GetItemCount(); i++) {
	const string & s = parser.AsString(i);
	if (s[0] == '#' || (s.length() > 1 && s[0] == '/' && s[1] == '/'))	
	  break;     

	if (s == ">") {
	  i++;
	  for (; i<parser.GetItemCount(); i++) {
	    const string & s1 = parser.AsString(i);
	    if (s1[0] == '#' || (s1.length() > 1 && s1[0] == '/' && s1[1] == '/'))	
	      break;     
	    tmp.Out().push_back(s1);
	    AddVariable(s1);
	  }
	  break;
	}
	
	tmp.Valid().push_back(s);	  
      }
    } else {
      tmp.Raw() = "# REMOVED " + tmp.Raw();
    }
    
    m_commands.push_back(tmp);
  }

  return 0;
}

bool ScriptParser::Process(int index)
{
  int i, j;

  cout << "Processing # " << index << endl;

  for (i=0; i<m_commands.isize(); i++) {
    Command & c = m_commands[i];

    cout << "Line: " << c.Raw() << endl;
    c.Processed() = "";
    if (c.Valid().isize() == 0) {
      c.Processed() = c.Raw();
      continue;
    }
    string line;
    for (j=0; j<c.Valid().isize(); j++) {
      string el = c.Valid()[j];
      cout << "valid " << el << endl;
      if (el[0] == '@') {
	int index = GetVariable(el);
	if (index < 0) {
	  cout << "ERROR line " << i << ": variable " << el << " is undefined!!" << endl;
	  return false;
	}
	el = m_vars[index].Value();
      }
      line += el;
      line += " ";
    }
  
    cout << "Parsing line: " << line << endl; 
    CMPtrStringList result;
    GRAMMAR_HANDLE h = m_grmStack.ParseAndEvaluate(result, line.c_str());
    
    if (h != -1) {
      cout << "Parsed w/ handle " << h << ", length=" << result.length() << endl;
      
      for (int j=0; j<result.length(); j++) {
	string var = ((const char*)*result(j));
	cout << "Returned: " << var << endl;
	StringParser pp;
	pp.SetLine(var, " ");
	if (pp.GetItemCount() < 3)
	  continue;
	if (pp.AsString(0) == "command") {
	  //line = pp.AsString(2);
	  line = "";
	  c.Processed() = "";
	  for (int y=2; y<pp.GetItemCount(); y++) {
	    line += pp.AsString(y);
	    line += " ";
	  }
	  cout << "--> Command: " << line << endl;
	  c.Processed() = line;
	  //continue;
	}
	for (int x=0; x<c.Out().isize(); x++) {
	  string vv = "@";
	  vv += pp.AsString(0);
	  cout << "Checking " << c.Out()[x] << " vs " <<  vv << endl;
	  if (c.Out()[x] == vv) {
	    int index = GetVariable(vv);
	    m_vars[index].Value() = pp.AsString(2);
	    cout << "--> Set variable " << pp.AsString(0) << " to " << pp.AsString(2) << endl;
	  }
	}
      }
      
    } else {
      cout << "No result, routing command through:" << c.Raw() << endl;
      c.Processed() = c.Raw();
    }
  }
  
  return true;
}

