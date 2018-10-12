#include"BeamSearch.h"

BeamSearch::BeamSearch(int beam_size, int window):m_beam_size(beam_size), m_window(window) {

}

BeamSearch::~BeamSearch() {

}

int BeamSearch::beamSearch() {
   if(m_window > 0)  {
      State* state = new State;
      state->m_score = 0.0;
      for(int i = 0; i < m_window; ++i) {
          string 
      }
   }
}
