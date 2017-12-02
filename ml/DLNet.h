#ifndef DLNET_H
#define DLNET_H


#include "base/RandomStuff.h"
#include <math.h>

class DLNeuron
{
 public:
  DLNeuron() {
    m_steep = 1.;
    m_input = 0.;
    m_output = 0.;
    m_layer = -1;
  }

  void ResetInput() {m_input = 0.;}

  void AddToInput(double d) {m_input += d;}

  double ComputeOutput();

  int GetConnections() const {return m_connect.isize();}
  int GetConnect(int i) const {return m_connect[i];}
  double GetWeight(int i) const {return m_weight[i];}
  double & Weight(int i) {return m_weight[i];}

  double GetSigma() const {return m_steep;}
  double & Sigma() {return m_steep;}
  
  double GetOutput() const {return m_output;}
  double GetInput() const {return m_input;}

  int GetLayer() const {return m_layer;}
  void SetLayer(int i) {m_layer = i;}

  void AddConnect(int i, double w) {
    m_connect.push_back(i);
    m_weight.push_back(w);
  }

  
 private:
  svec<int> m_connect;
  svec<double> m_weight;
  double m_steep;
  double m_input;
  double m_output;
  int m_layer;

};


class DLIOSingle
{
 public:
  DLIOSingle() {}

  svec<double> & In() {return m_in;}
  const svec<double> & In() const {return m_in;}
  
  svec<double> & Out() {return m_out;}
  const svec<double> & Out() const {return m_out;}

  
 private:
  svec<double> m_in;
  svec<double> m_out;
};



class DLNet
{
 public:
  DLNet() {
    m_outStart = 0;
    m_errInit = -1;
    m_adjust = 1.;
    m_layer = 0;
    m_byLayer = false;
    m_dropRate = 0.;
  }

  void SetTrainByLayer(bool b) {
    m_byLayer = b;
  }

  void AddForwardLayer(int n);
  
  void SupplyInput(const svec<DLIOSingle> & in) {
    m_data = in;
  }
  void AddInput(const DLIOSingle & in) {
    m_data.push_back(in);
  }

  double Train(int iter, double move);
  
  double TrainOne(double move, int layer = -1);

  double Evaluate(const svec<string> & labels);
  double Evaluate();

  void ResetInputs() {
    for (int i=0; i<m_neurons.isize(); i++)
      m_neurons[i].ResetInput();
  }

  void SetDropOutRate(double v) {
    m_dropRate = v;
  }
  
  int GetOutStart() const {return m_outStart;}
  int Neurons() const {return m_neurons.isize();}
  const DLNeuron & GetNeuron(int i) const {return m_neurons[i];}
 private:
  double OneRun();
  
  svec<DLNeuron> m_neurons;
  svec<DLIOSingle> m_data;
  int m_outStart;
  double m_errInit;
  double m_adjust;
  int m_layer;
  bool m_byLayer;
  double m_dropRate;
  svec<int> m_dropout;
};


#endif
