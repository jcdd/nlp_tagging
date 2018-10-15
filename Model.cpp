#include "Model.h"
#include <time.h>
#include <pthread.h>
Model::Model(){
  m_dict = new Dictionary();
  m_sample = new Sample(m_dict);
  m_template = new Template();
  m_decode_num = 0;
  m_correct_num = 0;
  m_train_sample_num = 0;
  m_beam_size = 0;
  m_window = 0;
}

Model::~Model(){
  if (NULL != m_dict) {
     delete m_dict;
  }
  if(NULL != m_sample) {
     delete m_sample;
  }
  if (NULL != m_template) {
     delete m_template;
  }
}

bool Model::train(const std::string& template_file, const std::string& sample_file, int window, int epoch, int threadNum, const std::string& model_file, int beam_size){
    m_window = window;
    m_beam_size = beam_size;
    time_t begin_time = time(NULL);
    if (!m_template->loadTemplate(template_file)) {
        std::cout << "[ERROR] loadTemplate fail..." << std::endl;
        return false;
    }
    if (!m_sample->loadSample(sample_file, window)) {
        std::cout << "[ERROR] loadSample fail...." << std::endl;
        return false;
    }
    if (!processSampleTemplate(m_sample, m_template)) {
        std::cout << "[ERROR] processSampleTemplate fail..." << std::endl;
        return false;
    }
    vector<int> indexVec;
    int sample_num = m_sample->getSampleNum();
    indexVec.reserve(sample_num);
    for(int i = 0; i < sample_num; ++i) {
        indexVec.push_back(i);
    }
    time_t train_begin = time(NULL);
    std::cout << "begin to train...." << train_begin << ",sample_num="<<sample_num << ",indexVec="<<indexVec.size()<<std::endl;
    for (int i = 0; i < epoch; ++i) {
        random_shuffle(indexVec.begin(), indexVec.end());
        float accuarcy = iterator_train(i, indexVec, threadNum);
        if (fabs(accuarcy - 1.0) < 0.00001) {
            break;
        }
    }
    avgWeight(epoch * sample_num);
    if (!saveModel(model_file)) {
        std::cout << "[ERROR] savemodel fail..." << std::endl;
    }
    time_t train_end = time(NULL);
    std::cout << "end to train.....task_time=" << train_end - train_begin<<std::endl;
    return true;
}

void print_sample(vector<vector<int> >& v, Dictionary* dict) {
    for(int i = 0; i < v.size(); ++i) {
        std::cout << "hello....." << dict->getWord(v[i][0]) <<"|"<< dict->getWord(v[i][1]) << "|" << "|" << dict->getWord(v[i][v[i].size()-2]) << "|" << dict->getWord(v[i][v[i].size()-1]) << std::endl;
    }   
}

bool Model::processSampleTemplate(Sample* sample, Template* temp) {
    for(int sampleIndex = 0; sampleIndex < sample->getSampleNum(); ++sampleIndex){
        vector<vector<int> >* train_sample = sample->getSample(sampleIndex);
        print_sample(*train_sample,m_dict);
        if (train_sample) {
            if (!temp->insertSample2FeatureNode(*train_sample)) {
                std::cout << "[ERROR] in Model::processSampleTemplate,insertSample2FeatureNode fail.." <<std::endl;
            }
        }
        else {
             std::cout << "[ERROR] in Model::processSampleTemplate,train_sample is null" << std::endl;
             return false;
        }
    }
    return true;
}

void* trainThreadFun(void* fun_param) {
    vector<int> test_tag;
    TrainParam* param = *(TrainParam**)(fun_param);
    std::cout << "......sample=" << param->m_train_sample->size() << std::endl;
    BeamSearch *bs = new BeamSearch(param->m_beam_size, param->m_window, param->m_ngram, param->m_train_sample, param->m_template, param->m_dict);
    std::cout << "...wc1:m_beam_size="<<param->m_beam_size<<",window="<< param->m_window<<",ngram=" << param->m_ngram << "sample_size=" << param->m_train_sample->size() <<",dict_wordcount="<< param->m_dict->wordCount() << std::endl;
    param->m_decodeNum = bs->beamSearch(param->m_fid2tag2score, test_tag);
    //param->m_correctVec = pkTestAndGold(test_tag, gold_tag)
    std::cout << "thread...." << ",m_trainIndex=" << param->m_trainIndex<<",m_decodeNum="<<param->m_decodeNum<<",m_correctNum="<< param->m_correctNum <<std::endl;
    if (NULL != bs) {
        delete bs;
    }
    return NULL;
}

float Model::iterator_train(int epoch_index, const vector<int> &indexVec, int threadNum) {
    //多线程train
        // beamsearch:  return correct_num ,求testtag
        //correctSampleWeight glod+1,test-1
    vector<TrainParam*> trainParmVec;
    for(int i = 0 ; i < threadNum; ++i){
        TrainParam* trainParm = new TrainParam;
        trainParmVec.push_back(trainParm);
    }
    vector<int> train_sample_vec;
    vector<int> train_index_vec;
    for (size_t sample_index = 0; sample_index < indexVec.size(); ++sample_index) {
        train_sample_vec.push_back(indexVec.at(sample_index));
        train_index_vec.push_back(epoch_index * indexVec.size() + sample_index+1);
        if (train_sample_vec.size() == threadNum) {
            std::cout << "......epoch_index=" << epoch_index << ",train_sample_vec.size="<<train_sample_vec.size() << ",threadNum=" <<threadNum << std::endl;
            for(int i = 0 ;i < threadNum; ++i) {
                pthread_t threads[threadNum];
                trainParmVec.at(i)->m_updateWeight.clear();
                trainParmVec.at(i)->m_fid2tag2score = &m_fid2tag2score;
                trainParmVec.at(i)->m_template =m_template;
                trainParmVec.at(i)->m_window = m_window;
                trainParmVec.at(i)->m_dict = m_sample->getDict();
                std::cout << ".....wc=" << m_sample->getDict()->wordCount()<<std::endl;
                trainParmVec.at(i)->m_ngram = m_template->getNgram();
                trainParmVec.at(i)->m_beam_size = m_beam_size;
                trainParmVec.at(i)->m_decodeNum = 0;
                trainParmVec.at(i)->m_correctNum = 0;
                trainParmVec.at(i)->m_trainSampleNum = 0;
                trainParmVec.at(i)->m_trainIndex = train_index_vec.at(i);
                trainParmVec.at(i)->m_train_sample = m_sample->getSample(indexVec.at(i));
                int rc = pthread_create(&threads[i], NULL, trainThreadFun, (void *)(&trainParmVec.at(i)));
                if (rc) {
                   std::cout << "[ERROR] pthread_create fail.rc=" << rc << std::endl;
                   return false;
                }
            }
            train_sample_vec.clear();
            train_index_vec.clear();
        }
        updateThreadResult(trainParmVec,threadNum);
    } 
    for(size_t i = 0; i < trainParmVec.size(); ++i) {
        if(NULL != trainParmVec[i]) {
            delete trainParmVec[i];
        }
    }
    return 0.01;
}


/*
   *更新多线程的updateWeight
   *累积correctnum
   *累积decodenum
   *累积samplenum
*/
bool Model::updateThreadResult(const vector<TrainParam*>& trainParmVec, int threadNum) {
    vector<map<int, float> >::iterator iter1;
    map<int, float>::iterator iter2;
    for (int i = 0; i < threadNum; ++i) {
        vector<map<int, float> >& updateWeight = trainParmVec.at(i)->m_updateWeight;
        for(size_t j = 0; j < updateWeight.size(); ++j) {
            for (iter2 = updateWeight.at(j).begin(); iter2 != updateWeight.at(j).end(); ++iter2) {
                if (m_fid2tag2score.at(j).find(iter2->first) != m_fid2tag2score.at(j).end()) {
                    m_fid2tag2score[j][iter2->first] += iter2->second;
                    m_fid2tag2sum[j][iter2->first] += (iter2->second) * (trainParmVec.at(i)->m_trainIndex - m_fid2tag2update[j][iter2->first]);
                    m_fid2tag2update[j][iter2->first] = trainParmVec.at(i)->m_trainIndex;
                }
                else {
                    m_fid2tag2score[j][iter2->first] = iter2->second;
                    m_fid2tag2sum[j][iter2->first] = 0;
                    m_fid2tag2update[j][iter2->first] = trainParmVec.at(i)->m_trainIndex;
                }
            }
        }
        m_decode_num += trainParmVec.at(i)->m_decodeNum;
        m_correct_num += trainParmVec.at(i)->m_correctNum;
        m_train_sample_num += trainParmVec.at(i)->m_trainSampleNum;
    }
    return true;
}

bool Model::avgWeight(int64_t sample_total) {
    for(size_t i = 0; i < m_fid2tag2score.size(); ++i) { 
        for(map<int, float>::iterator iter2 = m_fid2tag2score.at(i).begin(); iter2 != m_fid2tag2score.at(i).end(); ++iter2) {
            m_fid2tag2sum.at(i)[iter2->first] += iter2->second * (sample_total - m_fid2tag2update.at(i)[iter2->first] + 1);
            iter2->second = m_fid2tag2sum.at(i)[iter2->first]/sample_total;
            if (fabs(iter2->second) < 0.00001) {
                m_fid2tag2score.at(i).erase(iter2);
            }
        }
    }
    return true;
}

bool Model::saveModel(const std::string& file) {
    return true;
}
