#define FORCE_DEBUG

#include <string>
#include "base/CommandLineParser.h"
#include "base/FileParser.h"
#include "base/ThreadHandler.h"
#include <math.h>

class SharedData
{
public:
  SharedData() {
    m_data.resize(24, 0);
  }

  void Set(int i, int val) {
    m_mutex.Lock();
    if (m_data[i] == 0)
      m_data[i] = val;
    m_mutex.Unlock();
  }

  int Size() const {return m_data.isize();}
  int Get(int i) const {
    return m_data[i];
  }

  void Print() {
    for (int i=0; i<m_data.isize(); i++) 
      cout << i << "\t" << m_data[i] << endl;
  }

private:
  svec<int> m_data;
  ThreadMutex m_mutex;
};


class MyThread : public IOneThread
{
public:
  MyThread() {
    m_pData = NULL;
    m_id = -1;
  }

  MyThread(SharedData * p, int id) {
    m_pData = p;
    m_id = id;
  }

  void SetData(SharedData * p, int id) {
    m_pData = p;
    m_id = id;
  }
protected:

  virtual bool OnDie() {
    cout << "Killed!!" << endl;
    return true;
  }

  virtual bool OnDo(const string & msg) {
    cout << "Doing stupid things w/ " << msg << endl;
    svec<string> dummy;
    double v  = 0.;
    for (int i=0; i<1000; i++) {
      //cout << i << endl;
      for (int x=0; x<m_pData->Size(); x++) {
	cout << "me " << m_id << " sees " << x << " as " << m_pData->Get(x) << endl;
	if (m_pData->Get(x) == 0) {
	  cout << "Setting " << x << " to " << m_id << " " << m_pData << endl;
	  m_pData->Set(x, m_id);
	  break;
	  
	}
      }
      //dummy.push_back(msg);
      for (int j=0; j<250000; j++) {
	double f = exp(-(double)i);
	f = exp(-f);
	f = exp(-f);
	v += f;
      }
    }
    cout << "Done!" << endl;
    return true;
  }

  virtual bool OnInitialize(const string & msg) {
    cout << "Initializing!" << endl;
    svec<int> dummy;
    for (int i=0; i<500000; i++) {
      dummy.push_back(i);
    }
    cout << "Done init!" << endl;
    return true;
  }
private:
  SharedData * m_pData;
  int m_id;
};



int main( int argc, char** argv )
{

  commandArg<int> nCmmd("-n","number of threads");
  commandLineParser P(argc,argv);
  P.SetDescription("Testing the thread handler.");
  P.registerArg(nCmmd);
  
  P.parse();
  
  int n = P.GetIntValueFor(nCmmd);
  
  int i;
  ThreadHandler th;

  SharedData data;
  cout << &data << endl;
  //data.Print();
  for (i=0; i<n; i++) {
    char tmp[256];
    sprintf(tmp, "%d", i);
    string init = "init_";
    init += tmp;
    cout << init << endl;
    th.AddThread(new MyThread(&data, i+1), init);    
  }
  for (i=0; i<n; i++) {
    char tmp[256];
    sprintf(tmp, "%d", i);
    string init = "feed_";
    init += tmp;
    cout << init << endl;
    th.Feed(i, init);
  }

  while (!th.AllDone()) {
    usleep(10000);
  }
 

  return 0;
}



