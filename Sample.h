#ifndef SAMPLE_H_
#define SAMPLE_H_

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include <set>
#include<map>
#include"Dictionary.h"

using namespace std;

class Sample{
  public:
    Sample(Dictionary* dict, int window);
    ~Sample();
    bool loadSample(const std::string& file, int window);
    int getColumu();
    int getRow();
    vector<vector<int> >* getSample(int index);
    int getSampleNum();
    Dictionary* getDict();
    void generateTagDict();

  private:
    int m_columu;
    int m_row;
    Dictionary* m_dict;
    int m_window;
    vector<vector<vector<int> > >* m_sample_vec;
  public:
    map<int, set<int> > m_tag2tag;
    map<int, set<int> > m_word2tag;
    set<int> m_all_tag;
};

#endif
