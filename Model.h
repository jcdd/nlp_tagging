#ifndef MODEL_H_
#define MODEL_H_

#include "Dictionary.h"
#include "Template.h"
#include "Sample.h"


struct TrainParam{
     vector<map<int, float> > m_updateWeight;
     int m_decodeNum;
     int m_correctNum;
     int m_trainSampleNum;
     int64_t m_trainIndex;

 };

class Model{
  public:
    Model();
    ~Model();
    bool train(const std::string& template_file, const std::string& sample_file, int window, int epoch, int threadNum, const std::string& model_file);
    bool processSampleTemplate(Sample* sample, Template* tem);
    float iterator_train(int index, const vector<int>& indexVec, int threadNum);
    bool updateThreadResult(const vector<TrainParam*>& trainParamVec, int threadNum);
    bool avgWeight(int64_t num);
    bool saveModel(const std::string& file);


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
};

#endif
