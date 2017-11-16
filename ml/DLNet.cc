#include "ml/DLNet.h"

double DLNeuron::ComputeOutput()
{
  double d = atan(m_input*m_steep);
  cout << "Compute " << m_input << " " << m_steep << " " << d << endl;
  m_output = d;
  return d;
}

void DLNet::AddForwardLayer(int n)
{
  int nn = m_neurons.isize();
  int on = m_outStart;

  m_neurons.resize(nn+n);
  int i, j;

  for (i=on; i<nn; i++) {
    DLNeuron & from = m_neurons[i];
    for (j=nn; j<nn+n; j++) {
      DLNeuron & to = m_neurons[j];
      from.AddConnect(j, 0.1);
    }
  }
  m_outStart = nn;
}



void DLNet::Train(double move)
{
  double baseErr = OneRun();
  cout << "--------------------------" << endl;
  cout << "Base error: " << baseErr << endl; 
  int i, j;

  svec<double> diff;
  
  for (i=m_neurons.isize()-1; i>=0; i--) {
    DLNeuron & n = m_neurons[i];
    for (j=0; j<n.GetConnections(); j++) {
      n.Weight(j) += move;

      double de = baseErr-OneRun();
      cout << "Diff err: " << de << endl;
      diff.push_back(de);
      n.Weight(j) -= move;
    }
  }

  int k = 0;

  double m = .01;
  
  for (i=m_neurons.isize()-1; i>=0; i--) {
    DLNeuron & n = m_neurons[i];
    for (j=0; j<n.GetConnections(); j++) {
      n.Weight(j) += diff[k]*m;
      k++;
    }
  } 

}

double DLNet::OneRun()
{
  int i, j, l;
  double error = 0.;

  for (l=0; l<m_data.isize(); l++) {
    ResetInputs();
    
    // Feed inputs
    for (i=0; i<m_data[l].In().isize(); i++) {
      DLNeuron & to = m_neurons[i];
      to.AddToInput(m_data[l].In()[i]);
    }
    
    //Propagate
  
    for (i=0; i<m_neurons.isize(); i++) {
      DLNeuron & from = m_neurons[i];
      from.ComputeOutput();
      cout << "Out: " << from.GetOutput() << endl;
      for (j=0; j<from.GetConnections(); j++) {
	DLNeuron & to = m_neurons[from.GetConnect(j)];
	to.AddToInput(from.GetOutput()*from.GetWeight(j));
	cout << "New input in " << from.GetConnect(j) << " " << to.GetInput();
	cout << " after add " << from.GetOutput() << " " << from.GetWeight(j) << endl;
      }
    }
    
    
    for (i=m_outStart; i<m_neurons.isize(); i++) {
      double dist = m_neurons[i].GetOutput()-m_data[l].Out()[i-m_outStart];
      error += (dist*dist);
    }
  }
  return error/(double)m_data.isize();
}
