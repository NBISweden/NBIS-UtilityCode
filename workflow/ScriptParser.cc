#include "workflow/ScriptParser.h"
#include "base/StringUtil.h"



void NewLines(string &s)
{
  int i;

  for (i=0; i<(int)s.size()-1; i++) {
    if (s[i] == '\\' && s[i+1] == 'n') {
      s[i] = ' ';
      s[i+1] = '\n';
    }
  }

}

void Table::Prepend(const string & what, const string & to, const string & sep)
{

  int root = -1;
  int leaf = -1;

  int i;
  for (i=0; i<m_columns.isize(); i++) {
    if (m_columns[i].Label() == what) {
      root = i;
    }
    if (m_columns[i].Label() == to) {
      leaf = i;
    }
  }

  for (i=0; i<m_columns[0].isize(); i++) {
    string a = (m_columns[root])[i];
    a += sep;
    a += (m_columns[leaf])[i];
    (m_columns[leaf])[i] = a;
  }
}

void Table::Write(const string & fileName)
{
  FILE * p = fopen(fileName.c_str(), "w");

  int i, j;

  for (i=0; i<m_columns.isize(); i++) {
    fprintf(p, "%s\t", m_columns[i].Label().c_str());
  }
  fprintf(p, "\n");

  for (j=0; j<m_columns[0].isize(); j++) {
    //cout << "j=" << j << endl;
    for (i=0; i<m_columns.isize(); i++) {
      //cout << i << " " << m_columns[i].isize() << endl;
      fprintf(p, "%s\t", (m_columns[i])[j].c_str());
    }
    fprintf(p, "\n");
  }
  
  fclose(p);
}

void Table::AddColumn(const string & label)
{
  int i;

  int index = ColIndex(label);
  if (index >= 0) {
    cout << "WARNING: column " << label << " already exists!!" << endl;
    return;
  }


  TableColumn tmp;
  tmp.Label() = label;
  m_columns.push_back(tmp);
  TableColumn & t = m_columns[m_columns.isize()-1];
  TableColumn & one = m_columns[0];
  //cout << "Size col 0: " << m_columns[0].isize() << endl;
  for (i=0; i<one.isize(); i++) {
    //cout << "pushing" << endl;
    t.push_back(".n/a");
  }
}

void Table::RemoveColumn(const string & label)
{
  int index = ColIndex(label);
  if (index < 0) {
    cout << "WARNING: column " << label << " not found!!" << endl;
    return;
  }
  for (int i=index+1; i<m_columns.isize(); i++)
    m_columns[i-1] = m_columns[i];
  m_columns.resize(m_columns.isize()-1);
}

void Table::FillColumn(const string & label, const svec<string> & c, int from)
{
  int index = ColIndex(label);
  if (index < 0) {
    cout << "WARNING: column " << label << " not found (FillColumn)!!" << endl;
    return;
  }
  int i;
  TableColumn & t = m_columns[index];
  for (i=0; i<c.isize(); i++) {
    t[i+from] = c[i];
  }
}

void Table::SetInColumn(const string & label, int i, const string & v)
{
  int index = ColIndex(label);
  if (index < 0) {
    cout << "WARNING: column " << label << " not found (SetInColumn)!!" << endl;
    return;
  }
 
  TableColumn & t = m_columns[index];
  
  t[i] = v;
  
}



void Table::Read(const string & fileName)
{
  FlatFileParser parser;
  
  parser.Open(fileName);

  parser.ParseLine();

  resize(parser.GetItemCount());

  int i;
  m_index.clear();
  
  for (i=0; i<parser.GetItemCount(); i++)
    m_columns[i].Label() = parser.AsString(i);

  int k = 0;
  while (parser.ParseLine()) {
    if (parser.GetItemCount() == 0)
      continue;
    m_index.push_back(k);
    k++;
    for (i=0; i<parser.GetItemCount(); i++)
      m_columns[i].push_back(parser.AsString(i));
  }

}

void Table::Collapse(const string & key)
{
  int i, j;
  int index = -1;
  Table t2;
  t2.resize(isize());
  t2.Name() = Name();
  for (i=0; i<m_columns.isize(); i++) {
    if (m_columns[i].Label() == key) {
      index = i;
    }
    t2[i].Label() = (*this)[i].Label();
    //t2[i].push_back("");
  }
  if (index == -1) {
    cout << "ERROR Collapse(): column not found in table: " << key << endl;
    return;
  }
  
  Print();

  cout << "Collapsing table" << endl;

  string last;
  int k = -1;
  
  m_index.clear();

  for (j=0; j<(*this)[index].isize(); j++) {
    bool push = false;
    if (((*this)[index])[j] != last) {
      last = ((*this)[index])[j];
      push = true;
      for (i=0; i<t2.isize(); i++) {
	t2[i].push_back("");
      }
      k++;
      m_index.push_back(k);
    }
    for (i=0; i<t2.isize(); i++) {
      if ((t2[i])[k] != ((*this)[i])[j]) {
	if ((t2[i])[k] == "")
	  (t2[i])[k] = ((*this)[i])[j];
	else
	  (t2[i])[k] += " " + ((*this)[i])[j];
      }
    }
  }

  *this = t2;
  Print();
}

void Table::Print() const
{
  int i, j;
  for (i=0; i<m_columns.isize(); i++) {
    cout << m_columns[i].Label();
    for (j=0; j<m_columns[i].isize(); j++) {
      cout << " " << (m_columns[i])[j];
    }
    cout << endl;
  }
}

bool Table::Get(string &ret, const string & label, int index) const
{
  int i;

  StringParser s;
  s.SetLine(label, ".");

  if (s.GetItemCount() != 2) {
    cout << "ERROR: invalid syntax: " << label << endl;
    return false;
  }
  
  for (i=0; i<m_columns.isize(); i++) {
    if (m_columns[i].Label() == s.AsString(1)) {
      if (index >= 0 && index <m_columns[i].isize()) {
	ret = (m_columns[i])[index];
	return true;
      }
    }
  }
  cout << "ERROR: table entry not found: " << label << endl; 
  return false;
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
	if (m_table.isize() > 0) {
	  cout << "Table loaded already, ignoring request." << endl;
	} else {
	  cout << "Reading table from " << parser.AsString(2) << endl; 
	  int idx = AddVariable("@table");
	  m_vars[idx].Value() = parser.AsString(2);
	  
	  m_table.Read(parser.AsString(2));
	  m_table.Name() = parser.AsString(0);
	  if (parser.GetItemCount() == 5) {
	    if (parser.AsString(3) == "collapse")
	      m_table.Collapse(parser.AsString(4));
	  }
	  if (m_collapse != "") {
	    cout << "Collapsing table using " << m_collapse << endl;
	    m_table.Collapse(m_collapse);
	  }
	}
      } else {
	// TODO: dynamic variable assignment!!!!
	if (parser.AsString(0)[0] == '@') {
	  cout << "Setting variable " << parser.AsString(0) << " to " << parser.AsString(2) << endl;
	  int idx = AddVariable(parser.AsString(0));
	  m_vars[idx].Value() = parser.AsString(2);
	  tmp.Raw() = "# REMOVED " + tmp.Raw();

	} 
	
      }
      
      if (parser.AsString(0) == ">collapse") {
	if (parser.AsString(1) == "table" || parser.AsString(1) == "@table")
	  m_collapse = parser.AsString(2);
	tmp.Raw() = "# REMOVED " + tmp.Raw();
      }
      
      if (parser.AsString(0) == ">grammar") {
	bPre = true;
	cout << "Reading grammar from " << parser.AsString(2) << endl;
	string grmName = parser.AsString(2);
	//if (strstr(grmName.c_str(), "/") == NULL) {
	grmName = m_gramPath + grmName;
	//}
	try {
	  cout << "Loading grammar: " << grmName << endl;
	  GRAMMAR_HANDLE h = m_grmStack.AddGrammaAndReadFromFile(grmName.c_str());	 
	  cout << "Success!" << endl;
	}

	catch(CMException & ex) {
	  
	  cout << "Exception cought during load!!" << endl;
	  ex.Print();
	  return -1;
	}
	CMPtrStringList words;
	m_grmStack.GetWordList(words);
	for (int z=0; z<words.length(); z++)
	  m_dict.push_back((const char *)*words(z));
	UniqueSort(m_dict);

      }
    }

    if (!bPre) {
      for (i=0; i<parser.GetItemCount(); i++) {
	const string & s = parser.AsString(i);
	if (s[0] == '#' || (s.length() > 1 && s[0] == '/' && s[1] == '/'))	
	  break;     
	if (i == parser.GetItemCount() -1 && s == "&") {
	  tmp.SetBG(true);
	  break;
	}
	
	if (s == ">") {
	  i++;
	  for (; i<parser.GetItemCount(); i++) {
	    const string & s1 = parser.AsString(i);
	    if (s1[0] == '#' || (s1.length() > 1 && s1[0] == '/' && s1[1] == '/'))	
	      break;
	    if (s1[0] == '@') {
	      tmp.Out().push_back(s1);
	      AddVariable(s1);
	    } else {
	      if (s1 == "&") {
		tmp.SetBG(true);
	      } else {
		cout << "ERROR line " << m_commands.isize() << ": " << " unrecognized token " << s1 << endl;
		return -1;
	      }
	    }
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
 
bool ScriptParser::CheckForErrors(const string & in)
{
  int i;
  StringParser p;
  p.SetLine(in, " ");
  int n = 0;
  int m = 0;
  for (i=0; i<p.GetItemCount(); i++) {
    if (BinSearch(m_dict, p.AsString(i)) >= 0)
      n++;
    if (p.AsString(i)[0] == '@')
      m++;
  }
  cout << "n=" << n << " m=" << m << endl;
  if (n > 3 || m > 0)
    return true;
  return false;
}



void ScriptParser::AddTableVars(int index)
{
  int i;
  for (i=0; i<m_table.isize(); i++) {
    string v = m_table.Name() + "." + m_table[i].Label();
    int idx = AddVariable(v);
    if (index >=0 && index < m_table[i].isize())
      m_vars[idx].Value() = (m_table[i])[index];
    else
      cout << "ERROR: index " << index << " exceeds table size " << m_table[i].isize() << endl;
  }
}


bool ScriptParser::Process(int index)
{
  int i, j;

  AddTableVars(index);
  
  m_curr = index;
  cout << "Processing # " << index << endl;

  int idx = AddVariable("@index");
  m_vars[idx].Value() = Stringify(m_table.Index(index));

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
	  if (c.IsBG())
	    line += " &";
	  NewLines(line);
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
      bool bErr = CheckForErrors(line);
      c.Processed() = c.Raw();
      if (bErr) {
 	cout << "WARNING line: " << i << " Possible syntax error in command!" << endl;
	c.Processed() =    "##>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n## WARNING!!! This line looks like a command, but wasn't understood:\n";
	c.Processed() += c.Raw();
	c.Processed() += "\n##<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
     } else {
	cout << "No result, routing command through:" << c.Raw() << endl;
      }
    }
  }
  
  return true;
}

