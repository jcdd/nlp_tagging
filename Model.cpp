#include "Model.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>
Model::Model(int threadNum, int window, int beam_size){
  m_threadNum = threadNum;
  m_dict = new Dictionary();
  m_sample = new Sample(m_dict, window);
  m_template = new Template();
  m_decode_num = 0;
  m_correct_num = 0;
  m_train_sample_num = 0;
  m_beam_size = beam_size;
  m_window = window;
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

void print_dict(const vector<map<int, float> >& m) {
    std::cout << "jcdd5:" ;
    for (int i = 0; i<m.size();++i) {
        std::cout << i << "~";
       for(map<int,float>::const_iterator iter = m.at(i).begin(); iter != m.at(i).end(); ++iter) {
           std::cout << iter->first << ":" << iter->second<<",";
       }
       std::cout << "|";
    }
    std::cout << std::endl;
}
void Model::initTrainParam() {
  for(int i = 0 ; i < m_threadNum; ++i){
        TrainParam trainParm(m_template, m_dict);
        trainParm.m_ngram = m_template->getNgram();
        trainParm.m_window = m_window;
        trainParm.m_beam_size = m_beam_size;
        trainParm.m_word2tag = m_sample->m_word2tag;
        trainParm.m_tag2tag = m_sample->m_tag2tag;
        trainParm.m_all_tag = m_sample->m_all_tag;
        m_trainParmVec.push_back(trainParm);
   }
}

bool Model::train(const std::string& template_file, const std::string& sample_file,  int epoch,  const std::string& model_file){
    time_t begin_time = time(NULL);
    if (!m_template->loadTemplate(template_file)) {
        std::cout << "[ERROR] loadTemplate fail..." << std::endl;
        return false;
    }
    if (!m_sample->loadSample(sample_file, m_window)) {
        std::cout << "[ERROR] loadSample fail...." << std::endl;
        return false;
    }
    if (!processSampleTemplate()) {
        std::cout << "[ERROR] processSampleTemplate fail..." << std::endl;
        return false;
    }
    m_fid2tag2score.resize(m_template->getFeatureSize());
    m_fid2tag2sum.resize(m_template->getFeatureSize());
    m_fid2tag2update.resize(m_template->getFeatureSize());
    initTrainParam();
    vector<int> indexVec;
    int sample_num = m_sample->getSampleNum();
    indexVec.reserve(sample_num);
    for(int i = 0; i < sample_num; ++i) {
        indexVec.push_back(i);
    }
    time_t train_begin = time(NULL);
    std::cout << "begin to train...." << train_begin << ",sample_num="<<sample_num << ",indexVec="<<indexVec.size() << ",epech=" << epoch <<std::endl;
    for (int i = 0; i < epoch; ++i) {
        random_shuffle(indexVec.begin(), indexVec.end());
        float accuarcy = iterator_train(i, indexVec, m_threadNum);
        std::cout << ".....iterator_train return=" << accuarcy << std::endl;
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

bool Model::processSampleTemplate() {
    for(int sampleIndex = 0; sampleIndex < m_sample->getSampleNum(); ++sampleIndex){
        vector<vector<int> >* train_sample = m_sample->getSample(sampleIndex);
        print_sample(*train_sample,m_dict);
        if (train_sample) {
            if (!m_template->insertSample2FeatureNode(*train_sample)) {
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

void* Model::trainThreadFun(void* fun_param) {
    vector<int> test_tag;
    TrainParam* param = (TrainParam*)(fun_param);
    BeamSearch *bs = new BeamSearch(param->m_beam_size, param->m_window, param->m_ngram, param->m_train_sample, param->m_template, param->m_dict);
    std::cout << "...jcdd1:m_beam_size="<<param->m_beam_size<<",window="<< param->m_window<<",ngram=" << param->m_ngram << "sample_size=" << param->m_train_sample->size() <<",dict_wordcount="<< param->m_dict->wordCount() << ",template_ngram=" << param->m_template->getNgram() << ",template_num=" << param->m_template->getTemplateNum() << ",m_fid2tag2score_size=" << param->m_fid2tag2score->size() << ",fid2tag2score(3)= " << (param->m_fid2tag2score)->at(2)[1] << std::endl;
    param->m_decodeNum = bs->beamSearch(param->m_fid2tag2score, test_tag ,param->m_word2tag, param->m_tag2tag, param->m_all_tag);
    vector<int> gold_tag;
    for(size_t i = 0; i < param->m_train_sample->size() - param->m_window;++i) {
        gold_tag.push_back(param->m_train_sample->at(i).back());
    }
    if (gold_tag.size() != test_tag.size()) {
        std::cout << "[ERROR] gold_tag.size() != test_tag.size()" << std::endl;
        return NULL;
    }
    for(size_t i = 0; i < gold_tag.size(); ++i) {
        if (gold_tag.at(i) == test_tag.at(i)) {
            param->m_correctNum ++;
        }
    }
    for(size_t gold_index = param->m_window; gold_index < gold_tag.size(); ++gold_index) {
        vector<int> gold_features;
        int tmp_gold_tag = gold_tag.at(gold_index);
        param->m_template->generateFeatures(gold_index, *(param->m_train_sample), gold_tag, gold_features);
        for(size_t fIndex = 0; fIndex < gold_features.size(); ++fIndex) {
            map<int, float> &tag2score = param->m_updateWeight.at(gold_features.at(fIndex));
            if (tag2score.find(tmp_gold_tag) != tag2score.end()) {
                tag2score[tmp_gold_tag] += 1;
            }
            else {
                tag2score.insert(make_pair(tmp_gold_tag, 1));
            }
        }
    }
    
    for(size_t test_index = param->m_window; test_index < test_tag.size(); ++test_index) {
        vector<int> test_features;
        int tmp_test_tag = test_tag.at(test_index);
        param->m_template->generateFeatures(test_index, *(param->m_train_sample), test_tag, test_features);
        for(size_t fIndex = 0; fIndex < test_features.size(); ++fIndex) {
            map<int, float> &tag2score = param->m_updateWeight.at(test_features.at(fIndex));
            if (tag2score.find(tmp_test_tag) != tag2score.end()) {
                tag2score[tmp_test_tag] -= 1;
            }
            else {
                tag2score.insert(make_pair(tmp_test_tag, -1));
            }
        }
    }
    std::cout << "..jcdd,glod_tag_size=" << gold_tag.size() << ",test_tag.size=" << test_tag.size() << "! gold_tag=";
    for(size_t i = 0; i < gold_tag.size(); ++i) {
        std::cout << param->m_dict->getWord(gold_tag.at(i)) << "|";
    }
    std::cout << ",test_tag=";
    //param->m_correctVec = pkTestAndGold(test_tag, gold_tag)
    vector<int>::iterator iter = test_tag.begin();
    for(; iter != test_tag.end(); ++iter) {
         std::cout << param->m_dict->getWord(*iter)<<"|";
    }
    std::cout << std::endl;
    print_dict(param->m_updateWeight);
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
    vector<int> train_sample_vec;
    vector<int> train_index_vec;
    
    for (size_t sample_index = 0; sample_index < indexVec.size(); ++sample_index) {
        train_sample_vec.push_back(indexVec.at(sample_index));
        train_index_vec.push_back(epoch_index * indexVec.size() + sample_index+1);
        pthread_t threads[threadNum];
        if (train_sample_vec.size() == threadNum) {
            std::cout << "......epoch_index=" << epoch_index << ",train_sample_vec.size="<<train_sample_vec.size() << ",threadNum=" <<threadNum << std::endl;
            for(int i = 0 ;i < threadNum; ++i) {
                m_trainParmVec.at(i).m_updateWeight.clear();
                m_trainParmVec.at(i).m_updateWeight.resize(m_template->getFeatureSize());
                m_trainParmVec.at(i).m_fid2tag2score = &m_fid2tag2score;
                m_trainParmVec.at(i).m_decodeNum = 0;
                m_trainParmVec.at(i).m_correctNum = 0;
                m_trainParmVec.at(i).m_trainSampleNum = 0;
                m_trainParmVec.at(i).m_trainIndex = train_index_vec.at(i);
                m_trainParmVec.at(i).m_train_sample = m_sample->getSample(indexVec.at(i));
                m_trainParmVec.at(i).m_trainSampleNum = m_sample->getSample(indexVec.at(i))->size() - 2*m_window;
                int rc = pthread_create(&threads[i], NULL, trainThreadFun, (void *)(&m_trainParmVec.at(i)));
                if (rc) {
                   std::cout << "[ERROR] pthread_create fail.rc=" << rc << std::endl;
                   return false;
                }
            }
        }
        for (int i = 0; i < threadNum; ++i) {
            pthread_join(threads[i], NULL); 
        }
        train_sample_vec.clear();
        train_index_vec.clear();
        //sleep(3);
        updateThreadResult(m_trainParmVec,threadNum);
    } 
    return (float)m_correct_num/m_train_sample_num;
}



/*
   *更新多线程的updateWeight
   *累积correctnum
   *累积decodenum
   *累积samplenum
*/
bool Model::updateThreadResult(const vector<TrainParam>& m_trainParmVec, int threadNum) {
    vector<map<int, float> >::iterator iter1;
    map<int, float>::const_iterator iter2;
    for (int i = 0; i < threadNum; ++i) {
        const vector<map<int, float> >& updateWeight = m_trainParmVec.at(i).m_updateWeight;
        print_dict(updateWeight);
        for(size_t j = 0; j < updateWeight.size(); ++j) {
            for (iter2 = updateWeight.at(j).begin(); iter2 != updateWeight.at(j).end(); ++iter2) {
                if (m_fid2tag2score.at(j).find(iter2->first) != m_fid2tag2score.at(j).end()) {
                    m_fid2tag2score[j][iter2->first] += iter2->second;
                    std::cout << "...jcdd66," << j << "->" << m_fid2tag2score[j][iter2->first]<<std::endl;
                    m_fid2tag2sum[j][iter2->first] += (iter2->second) * (m_trainParmVec.at(i).m_trainIndex - m_fid2tag2update[j][iter2->first]);
                    m_fid2tag2update[j][iter2->first] = m_trainParmVec.at(i).m_trainIndex;
                }
                else {
                    m_fid2tag2score[j][iter2->first] = iter2->second;
                    m_fid2tag2sum[j][iter2->first] = 0;
                    m_fid2tag2update[j][iter2->first] = m_trainParmVec.at(i).m_trainIndex;
                }
            }
        }
        m_decode_num += m_trainParmVec.at(i).m_decodeNum;
        m_correct_num += m_trainParmVec.at(i).m_correctNum;
        m_train_sample_num += m_trainParmVec.at(i).m_trainSampleNum;
    }
    std::cout << "...jcdd3:" << m_fid2tag2score.at(2)[1]<< ",m_decode_num=" << m_decode_num <<",m_correct_num=" << m_correct_num<<",m_train_sample_num=" << m_train_sample_num<<std::endl;
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
