#include"BeamSearch.h"

BeamSearch::BeamSearch(int beam_size, int window, int ngram, vector<vector<int> >* sample, Template* temp, Dictionary* dict) {
    m_beam_size = beam_size;
    m_window = window;
    m_state_vec = new vector<StateStack*>;
    m_sample = sample;
    m_sample_length = m_sample->size();
    m_state_vec->resize(m_sample_length);
    m_template = temp;
    m_static_feature = new vector<vector<int> >;
    m_static_feature->resize(m_sample_length);
    m_dict = dict;
    m_ngram = ngram;
}

BeamSearch::~BeamSearch() {
  for (size_t i = 0; i < m_state_vec->size(); ++i) {
     if (NULL != m_state_vec->at(i)) {
         delete m_state_vec->at(i);
     }
  }
  delete m_state_vec;
  if (NULL!=m_static_feature) {
     delete m_static_feature;
  }
}

int BeamSearch::beamSearch(vector<map<int, float> >* fid2tag2score, vector<int>& testTagVec) {
   if(m_window > 0)  {
      State* state = new State;
      state->m_score = 0.0;
      for(int i = 0; i < m_window; ++i) {
          string tmp_tag = "B_" + String::ToString(m_window - i);
          state->m_pre_tag.push_back(m_dict->getId(tmp_tag));
      }
      m_state_vec->at(m_window-1)->addState(state, m_ngram);
   }
   int sample_index = m_window;
   
   for (; sample_index < m_sample_length - m_window; ++sample_index) {
      //GenerateStaticFeatures
      m_template->generateStaticFeatures(sample_index, *m_sample, m_static_feature->at(sample_index));
      for (int state_index = 0;  state_index < m_state_vec->at(sample_index-1)->Size(); ++state_index) {
          AppendState(m_state_vec->at(sample_index - 1)->At(state_index), fid2tag2score);
      }
   }
   m_state_vec->at(sample_index-1)->Sort();
   testTagVec = m_state_vec->at(sample_index-1)->At(0)->m_pre_tag;
   //testTagVec.pop_back();
   return (int)testTagVec.size() - m_window;
}

void BeamSearch::AppendState(State* state, vector<map<int, float> >* fid2tag2score) {
    set<int> tagSet;
    int tIndex = (int)state->m_pre_tag.size();
    //to do.....
    //getNextTagSet(state->m_pre_tag.back(), m_sample->at(tIndex).at(0), tagSet);
    for(set<int>::iterator iter = tagSet.begin(); iter != tagSet.end(); ++iter) {
       State *new_state = new State;
       new_state->m_score = state->m_score + getScore(m_static_feature->at(tIndex), *iter, fid2tag2score);
       new_state->m_pre_tag = state->m_pre_tag;
       new_state->m_pre_tag.push_back(*iter);
       //generateDynamicFeatures
       vector<int> dynamic_feature;
       m_template->generateDynamicFeatures(tIndex, *m_sample, new_state->m_pre_tag, dynamic_feature);
       new_state->m_score += getScore(dynamic_feature, *iter, fid2tag2score);
       if (m_state_vec->at(tIndex)->addState(new_state, m_ngram) > m_beam_size) {
           m_state_vec->at(tIndex)->Resize(m_beam_size);
       }
    }
}

float BeamSearch::getScore(const vector<int>& features, int tagId, const vector<map<int, float> >* fid2tag2score) {
    float score = 0.0;
    map<int, float>::const_iterator iter;
    for(size_t i = 0; i < features.size(); ++i) {
        iter = fid2tag2score->at(features.at(i)).find(tagId);
        if (iter != fid2tag2score->at(features.at(i)).end()) {
            score += iter->second;
        }
    }
    return score;
}
