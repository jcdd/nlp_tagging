#ifndef MODEL_H_
#define MODEL_H_

#include "Dictionary.h"
#include "Template.h"
#include "Sample.h"
#include "BeamSearch.h"


struct TrainParam{
  public:
     TrainParam(Template* temp, Dictionary* dict) {
         m_template = temp;
         m_dict = dict;
     }
     vector<map<int, float> > m_updateWeight;
     int m_decodeNum;
     int m_correctNum;
     int m_trainSampleNum;
     int64_t m_trainIndex;
     int m_window;
     int m_beam_size;
     Template* m_template;
     vector<vector<int> > *m_train_sample;
     Dictionary* m_dict;
     int m_ngram;
     vector<map<int,float> >* m_fid2tag2score;
     map<int, set<int> > m_word2tag;
     map<int, set<int> > m_tag2tag;
     set<int> m_all_tag;
 };

class Model{
  public:
    Model(int threadNum, int window, int beamsize);
    ~Model();
    bool train(const std::string& template_file, const std::string& sample_file, int epoch, const std::string& model_file);
    bool processSampleTemplate();
    float iterator_train(int index, const vector<int>& indexVec, int threadNum);
    bool updateThreadResult(const vector<TrainParam>& trainParamVec, int threadNum);
    bool avgWeight(int64_t num);
    bool saveModel(const std::string& file);
    void initTrainParam();
    static void* trainThreadFun(void* fun_param);


  private:
    Dictionary* m_dict;
    Sample* m_sample;
    Template* m_template;

  private:
    vector<map<int, float> > m_fid2tag2score;
    vector<map<int, float> > m_fid2tag2sum;
    vector<map<int, int64_t> > m_fid2tag2update;
    int64_t m_decode_num;
    int64_t m_correct_num;
    int64_t m_train_sample_num;
    int m_window;
    int m_beam_size;
    int m_threadNum;
    vector<TrainParam> m_trainParmVec;
};

#endif
