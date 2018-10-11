#ifndef SAMPLE_H_
#define SAMPLE_H_

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include"Dictionary.h"

using namespace std;

class Sample{
  public:
    Sample(Dictionary* dict);
    ~Sample();
    bool loadSample(const std::string& file, int window);
    int getColumu();
    int getRow();

  private:
    int m_columu;
    int m_row;
    Dictionary* m_dict;
    vector<vector<vector<int> > >* m_sample_vec;
};

#endif
