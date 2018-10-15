#ifndef BEAMSEARCH_H_
#define BEAMSEARCH_H_
#include <vector>
#include <map>
#include <set>
#include "Sample.h"
#include "Template.h"
#include "Dictionary.h"

using namespace std;
struct State{
   float m_score;
   vector<int> m_pre_tag;
};

struct CompareFun{
   bool operator() (const State* a, const State* b) {
       return a->m_score > b->m_score;
   }
};

class StateStack{
  public:
    StateStack(){m_state_vec = new vector<State*>;}
    ~StateStack(){
       for (size_t i =0 ; i < Size(); ++i) {
           if (NULL != m_state_vec->at(i)) {
               delete m_state_vec->at(i);
           }
       }
       if (NULL != m_state_vec) {
           delete m_state_vec;
       }
    }
    int addState(State* state, int ngram) {
       for(size_t i = 0; i < Size(); ++i) {
           if (needMerge(state, m_state_vec->at(i), ngram)) {
               if (state->m_score > m_state_vec->at(i)->m_score) {
                   delete m_state_vec->at(i);
                   m_state_vec->at(i) = state;
               }
               else {
                   delete state;
               }
               return Size();
           }
       }
       m_state_vec->push_back(state);
       return Size();
    }

    bool needMerge(State* state1, State* state2, int ngram) {
        for(size_t index = state1->m_pre_tag.size() - ngram; index < state1->m_pre_tag.size(); ++index) {
           if (state1->m_pre_tag.at(index) != state2->m_pre_tag.at(index)) {
               return false;
           }
        }
        return true;
    }
    
    void Resize(int beam_size) {
        sort(m_state_vec->begin(), m_state_vec->end(), CompareFun());
        if (Size() > beam_size) {
            for(int i = beam_size; i < Size();++i) {
                delete m_state_vec->at(i);
            }
            m_state_vec->resize(beam_size);
        }
    }

    void Sort() {
        sort(m_state_vec->begin(), m_state_vec->end(), CompareFun());
    }
 
    size_t Size() {
        return m_state_vec->size();
    }

    State* At(int index) {
       if (index >= 0 && index < m_state_vec->size()) {
           return m_state_vec->at(index);
       }
       return NULL;
    }
   
  private:
    vector<State*>* m_state_vec;
};

class BeamSearch{
  public:
    BeamSearch(int beam_size, int window,int ngram,vector<vector<int> >* sample, Template* temp, Dictionary* dict);
    ~BeamSearch();
    int beamSearch(vector<map<int, float> >* fid2tag2score, vector<int>& testTag);
    void AppendState(State* state, vector<map<int, float> >* fid2tag2score);
    float getScore(const vector<int>& features, int tagId, const vector<map<int, float> >* fid2tag2score);
    
  private:
    int m_beam_size;
    int m_window;
    int m_sample_length;
    vector<StateStack>* m_state_vec;
    vector<vector<int> >*  m_sample;
    Template* m_template;
    Dictionary* m_dict;
    vector<vector<int> >* m_static_feature;
    int m_ngram;
};

#endif
