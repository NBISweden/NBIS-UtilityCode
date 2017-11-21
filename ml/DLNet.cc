#include "ml/DLNet.h"
#include "base/RandomStuff.h"

double DLNeuron::ComputeOutput()
{
  //double d = atan(m_input*m_steep)/3.1415+1.;
  double d = atan(m_input*m_steep);
  //cout << "Compute " << m_input << " " << m_steep << " " << d << endl;
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
      if (nn > 0 && j-i == nn-on)
	from.AddConnect(j, RandomFloat(0.2));
      else
	from.AddConnect(j, RandomFloat(0.2));
    }
  }
  m_outStart = nn;
}

double DLNet::Train(int iter, double move)
{
  m_errInit = OneRun();
  int i;
  double err = -1;
  for (i=0; i<iter; i++) {
    err = TrainOne(move);
  }
  return err;
}


double DLNet::TrainOne(double move)
{
  double baseErr = OneRun();

  if (m_errInit < 0.)
    m_errInit = baseErr;

  //move *= baseErr/m_errInit;
  
  //cout << "--------------------------" << endl;
  //cout << "Base error: " << baseErr << endl; 
  int i, j;

  svec<double> diff;

  int best = -1;
  double maxDE = 0.;

  //++++++++++++++++++++++++++++++++
  move = 0.01;
  
  for (i=m_neurons.isize()-1; i>=0; i--) {
    DLNeuron & n = m_neurons[i];
    for (j=0; j<n.GetConnections(); j++) {
      n.Weight(j) += move*m_adjust;

      double de = baseErr-OneRun();

      //if (i == 0) {
      //cout << "Diff err: " << j << " " << de << " weight " << n.Weight(j) << " diff " << move*m_adjust << endl;
      //}
      if (de*de > maxDE) {
	maxDE = de*de;
	best = diff.isize();
      }
      diff.push_back(de);
      n.Weight(j) -= move*m_adjust;
    }
  }


  for (i=m_neurons.isize()-1; i>=0; i--) {
    DLNeuron & n = m_neurons[i];
    n.Sigma() += move*m_adjust;
    double de = baseErr-OneRun();    
    n.Sigma() -= move*m_adjust;
    diff.push_back(de);
  }

  
  int k = 0;

  //double m = m_adjust*0.99/m_errInit;
  double m = m_adjust;
  
  for (i=m_neurons.isize()-1; i>=0; i--) {
    DLNeuron & n = m_neurons[i];
    for (j=0; j<n.GetConnections(); j++) {
      //      if (k == best) {
	double err = baseErr;
	//do {
	baseErr = err;
	n.Weight(j) += diff[k]*m/move;
	//err = OneRun();
	  //} while (err < baseErr);
	  //n.Weight(j) -= diff[k]*m/move*0.1;
	
	//}
      k++;
    }
  }

  for (i=m_neurons.isize()-1; i>=0; i--) {
    DLNeuron & n = m_neurons[i];
    n.Sigma() += diff[k]*m;    
    k++;
  }


  
  double err = OneRun();

  
  if (err > baseErr)
    m_adjust /= 2;
  
  
  return err;
}

double DLNet::Evaluate(bool bQuiet)
{
  svec<string> labels;
  return Evaluate(labels, bQuiet);
}

double DLNet::Evaluate(const svec<string> & labels, bool bQuiet)
{
  int i, j, l;
  double error = 0.;

  double bestHit = 0.;

  cout << "+++++++++ PRINT ++++++++++++++++" << endl;
  for (i=0; i<m_neurons.isize(); i++) {
    DLNeuron & from = m_neurons[i];
    cout << "Neuron " << i << " out: " << from.ComputeOutput() << endl;
    for (j=0; j<from.GetConnections(); j++) {
      cout << " -> " << from.GetConnect(j) << " weight: " <<  from.GetWeight(j) << endl;
    }
  }
     


  
  cout << "+++++++++ EVALUATE ++++++++++++++++" << endl;
  cout << "sample\tpredict\tout" << endl;
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
      for (j=0; j<from.GetConnections(); j++) {
	DLNeuron & to = m_neurons[from.GetConnect(j)];
	to.AddToInput(from.GetOutput()*from.GetWeight(j));
      }
    }
    
    double hiT = -1.;
    double hiG = 0.;
    int idxT = -1;
    int idxG = -2;
    if (labels.isize() > 0)
      cout << "sample: " << labels[l] << endl;
    for (i=m_outStart; i<m_neurons.isize(); i++) {
      double dist = m_neurons[i].GetOutput()-m_data[l].Out()[i-m_outStart];
      error += (dist*dist);
      cout << l << "\t" << m_neurons[i].GetOutput() << "\t" << m_data[l].Out()[i-m_outStart] << endl;
      if (m_neurons[i].GetOutput() > hiG) {
	idxG = i;
	hiG = m_neurons[i].GetOutput();
      }
      if (m_data[l].Out()[i-m_outStart] > hiT) {
	hiT = m_data[l].Out()[i-m_outStart];
	idxT = i;
      }
    }
    if (idxT == idxG) {
      if (!bQuiet) 
	cout << "test " << labels[l] <<  " -> correct " << m_data[l].Out()[0] << endl;
      bestHit += 1.;
    } else {
      if (!bQuiet) 
	cout << "test " << labels[l] << " -> false " << m_data[l].Out()[0] << endl;
    }
  }

  cout << "Best hit %: " << 100.*bestHit/(double)m_data.isize() << endl;
  return error/(double)m_data.isize();

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
      //cout << "Out: " << from.GetOutput() << endl;
      for (j=0; j<from.GetConnections(); j++) {
	DLNeuron & to = m_neurons[from.GetConnect(j)];
	to.AddToInput(from.GetOutput()*from.GetWeight(j));
	//cout << "New input in " << from.GetConnect(j) << " " << to.GetInput();
	//cout << " after add " << from.GetOutput() << " " << from.GetWeight(j) << endl;
      }
    }
    
    
    for (i=m_outStart; i<m_neurons.isize(); i++) {
      double dist = m_neurons[i].GetOutput()-m_data[l].Out()[i-m_outStart];
      error += (dist*dist);
    }
  }

  
  return error/(double)m_data.isize();
}
