#ifndef BEAMSEARCH_H_
#define BEAMSEARCH_H_

struct State{
   float m_score;
   vector<int> m_pre_tag;
};

class StateStack{
  public:
    StateStack(){}
    ~StateStack(){}
    int addState(State* state, int ngram) {
       for(size_t i = 0; i < Size(); ++i) {
           if (needMerge(state, m_state_vec.at(i), ngram)) {
               if (state->m_score > m_state_vec.at(i)->m_score) {
                   delete m_state_vec.at(i);
                   m_state_vec.at(i) = statel;
               }
               else {
                   delete state;
               }
               return Size();
           }
       }
       m_state_vec.push_back(state);
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
        sort(m_pre_tag.begin(), m_pre_tag.end(), CompareFun);
        if (Size() > beam_size) {
            for(int i = beam_size; i < Size();++i) {
                delete m_pre_tag.at(i);
            }
            m_pre_tag.resize(beam_size);
        }
    }

    size_t Size() {
        return m_state_vec.size();
    }

  private:
    vector<*State> m_state_vec;
}

class BeamSearch{
  public:
    BeamSearch() {}
    ~BeamSearch() {}
    int beamSearch();

  private:
    int m_beam_size;

};

#endif
