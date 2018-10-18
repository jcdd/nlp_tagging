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
        std::cout << ".....iterator_train ,epoch=" << i << ", return=" << accuarcy << std::endl;
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
        //print_sample(*train_sample,m_dict);
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
    TrainParam* param = (TrainParam*)(fun_param);
    BeamSearch *bs = new BeamSearch(param->m_beam_size, param->m_window, param->m_ngram, param->m_train_sample, param->m_template, param->m_dict);
    //std::cout << "...jcdd1:m_beam_size="<<param->m_beam_size<<",window="<< param->m_window<<",ngram=" << param->m_ngram << ",sample_size=" << param->m_train_sample->size() <<",dict_wordcount="<< param->m_dict->wordCount() << ",template_ngram=" << param->m_template->getNgram() << ",template_num=" << param->m_template->getTemplateNum() << ",m_fid2tag2score_size=" << param->m_fid2tag2score->size() << ",fid2tag2score(3)= " << (param->m_fid2tag2score)->at(2)[1] << std::endl;
    param->m_decodeNum = bs->beamSearch(param->m_fid2tag2score, param->m_test_tag ,param->m_word2tag, param->m_tag2tag, param->m_all_tag);
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
    pthread_t threads[threadNum];
    for (size_t sample_index = 0; sample_index < indexVec.size(); ++sample_index) {
        train_sample_vec.push_back(indexVec.at(sample_index));
        train_index_vec.push_back(epoch_index * indexVec.size() + sample_index+1);
        if (train_sample_vec.size() == threadNum) {
            for(int i = 0 ;i < threadNum; ++i) {
                m_trainParmVec.at(i).m_updateWeight.clear();
                m_trainParmVec.at(i).m_test_tag.clear();
                m_trainParmVec.at(i).m_updateWeight.resize(m_template->getFeatureSize());
                m_trainParmVec.at(i).m_fid2tag2score = &m_fid2tag2score;
                m_trainParmVec.at(i).m_decodeNum = 0;
                m_trainParmVec.at(i).m_correctNum = 0;
                m_trainParmVec.at(i).m_trainSampleNum = 0;
                m_trainParmVec.at(i).m_trainIndex = train_index_vec.at(i);
                m_trainParmVec.at(i).m_train_sample = m_sample->getSample(train_sample_vec.at(i));
                m_trainParmVec.at(i).m_trainSampleNum = m_sample->getSample(train_sample_vec.at(i))->size() - 2*m_window;
                int rc = pthread_create(&threads[i], NULL, trainThreadFun, (void *)(&m_trainParmVec.at(i)));
                if (rc) {
                   std::cout << "[ERROR] pthread_create fail.rc=" << rc << std::endl;
                   return false;
                }
            }
            for (int i = 0; i < threadNum; ++i) {
                pthread_join(threads[i], NULL); 
            }
            //sleep(3);
            updateThreadResult(m_trainParmVec,threadNum);
            train_sample_vec.clear();
            train_index_vec.clear();
        }
    }
    if (!train_sample_vec.empty()) {
        for(int i = 0 ;i < train_sample_vec.size(); ++i) {
            m_trainParmVec.at(i).m_updateWeight.clear();
            m_trainParmVec.at(i).m_test_tag.clear();
            m_trainParmVec.at(i).m_updateWeight.resize(m_template->getFeatureSize());
            m_trainParmVec.at(i).m_fid2tag2score = &m_fid2tag2score;
            m_trainParmVec.at(i).m_decodeNum = 0;
            m_trainParmVec.at(i).m_correctNum = 0;
            m_trainParmVec.at(i).m_trainSampleNum = 0;
            m_trainParmVec.at(i).m_trainIndex = train_index_vec.at(i);
            m_trainParmVec.at(i).m_train_sample = m_sample->getSample(train_sample_vec.at(i));
            m_trainParmVec.at(i).m_trainSampleNum = m_sample->getSample(train_sample_vec.at(i))->size() - 2 * m_window;
            int rc = pthread_create(&threads[i], NULL, trainThreadFun, (void *)(&m_trainParmVec.at(i)));
            if (rc) {
               std::cout << "[ERROR] pthread_create fail.rc=" << rc << std::endl;                  
               return false;
            }
       }
       for(int i = 0 ;i < train_sample_vec.size(); ++i) {
            pthread_join(threads[i], NULL);
        }
        updateThreadResult(m_trainParmVec,train_sample_vec.size());
        train_sample_vec.clear();
        train_index_vec.clear();
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
    for(size_t i = 0 ; i < threadNum; ++i) {
        vector<int> gold_tag;
        const TrainParam& tmp_param = m_trainParmVec.at(i);
        for(size_t i = 0; i < tmp_param.m_train_sample->size() - tmp_param.m_window;++i) {
            gold_tag.push_back(tmp_param.m_train_sample->at(i).back());
        }
        const vector<int>& test_tag = tmp_param.m_test_tag;
        if (gold_tag.size() != test_tag.size()) {
            std::cout << "[ERROR] gold_tag.size() != test_tag.size()" << std::endl;
            return false;
        }
        for(size_t i = tmp_param.m_window; i < gold_tag.size(); ++i) {
            if (gold_tag.at(i) == test_tag.at(i)) {
                m_correct_num ++;
            }
        }
        
        for(size_t gold_index = tmp_param.m_window; gold_index < gold_tag.size(); ++gold_index) {
            vector<int> gold_features;
            int tmp_gold_tag = gold_tag.at(gold_index);
            m_template->generateFeatures(gold_index, *(tmp_param.m_train_sample), gold_tag, gold_features);
            for(size_t fIndex = 0; fIndex < gold_features.size(); ++fIndex) {
                int fid = gold_features.at(fIndex);
                int score = 1;
                if (m_fid2tag2score.at(fid).find(tmp_gold_tag) != m_fid2tag2score.at(fid).end()) {
                    m_fid2tag2score.at(fid)[tmp_gold_tag] += score;
                    m_fid2tag2sum.at(fid)[tmp_gold_tag] += score * (tmp_param.m_trainIndex - m_fid2tag2update.at(fid)[tmp_gold_tag]);
                    m_fid2tag2update.at(fid)[tmp_gold_tag] = tmp_param.m_trainIndex;
                }
                else {
                    m_fid2tag2score.at(fid)[tmp_gold_tag] = score;
                    m_fid2tag2sum.at(fid)[tmp_gold_tag] = 0; 
                    m_fid2tag2update.at(fid)[tmp_gold_tag] = tmp_param.m_trainIndex;
                }
            }
        }
        for(size_t test_index = tmp_param.m_window; test_index < test_tag.size(); ++test_index) {
            vector<int> test_features;
            int tmp_test_tag = test_tag.at(test_index);
            m_template->generateFeatures(test_index, *(tmp_param.m_train_sample), test_tag, test_features);
            for(size_t fIndex = 0; fIndex < test_features.size(); ++fIndex) {
                int fid = test_features.at(fIndex);
                int score = -1;
                if (m_fid2tag2score.at(fid).find(tmp_test_tag) != m_fid2tag2score.at(fid).end()) {
                    m_fid2tag2score.at(fid)[tmp_test_tag] += score;
                    m_fid2tag2sum.at(fid)[tmp_test_tag] += score * (tmp_param.m_trainIndex - m_fid2tag2update.at(fid)[tmp_test_tag]);
                    m_fid2tag2update.at(fid)[tmp_test_tag] = tmp_param.m_trainIndex;
                }
                else {
                    m_fid2tag2score.at(fid)[tmp_test_tag] = score;
                    m_fid2tag2sum.at(fid)[tmp_test_tag] = 0; 
                    m_fid2tag2update.at(fid)[tmp_test_tag] = tmp_param.m_trainIndex;
                }

            }
        }
        m_decode_num += tmp_param.m_decodeNum;
        m_train_sample_num += tmp_param.m_trainSampleNum;
        /* 
        std::cout << "param_" << i << ":m_decode_num=" << m_decode_num << ",m_train_sample_num=" << m_train_sample_num << ",m_m_correct_num=" << m_correct_num << ",test_tag.size=" << test_tag.size() << ",gold_tag_size=" << gold_tag.size() << ",test_tag=";
        for (int n = 0;n < test_tag.size(); ++n) {
            std::cout << m_dict->getWord(test_tag[n]) << "|";
        }
        std::cout << ",gold_tag=";
        for(int n =0; n < gold_tag.size(); ++n) {
            std::cout << m_dict->getWord(gold_tag[n]) << "|";
        }
        std::cout << std::endl;
        */
        
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
                --iter2;
            }
        }
    }
    return true;
}

bool Model::saveModel(const std::string& file) {
    ofstream fout(file);
    if (!fout.is_open()) {
        std::cout << "[ERROR] in savemodel,file=" << file << std::endl;
        return false;
    }
    fout << "#text_model"<<std::endl;
    fout << "#template=" << m_template->getTemplateNum() << std::endl;
    fout << m_template->getTemplateNum() << std::endl;
    for (int tindex = 0; tindex < m_template->getTemplateNum(); ++tindex) {
        for(int xindex = 0; xindex < m_template->getTemplate(tindex).x.size(); ++xindex) {
            fout<<"%x[" << m_template->getTemplate(tindex).x.at(xindex).first << "," << m_template->getTemplate(tindex).x.at(xindex).second <<"]";
        }
        for(int yindex = 0; yindex < m_template->getTemplate(tindex).y.size(); ++yindex) {
            fout<<"%y[" << m_template->getTemplate(tindex).y.at(yindex) <<"]";
        }
        fout<< std::endl;
    }
    fout << "#dict=" << m_dict->wordCount() << std::endl;
    m_dict->saveDict(fout);
    fout << "#all_tags=" << m_sample->m_all_tag.size() << std::endl;
    fout << m_sample->m_all_tag.size() << std::endl;
    for (set<int>::iterator iter =m_sample->m_all_tag.begin(); iter != m_sample->m_all_tag.end(); ++iter) {
        fout << *iter << std::endl;
    }
    fout << "#tag2tag=" << m_sample->m_tag2tag.size() << std::endl;
    fout << m_sample->m_tag2tag.size() << std::endl;
    map<int, set<int> >::iterator m_iter = m_sample->m_tag2tag.begin();
    set<int>::iterator s_iter;
    for(; m_iter != m_sample->m_tag2tag.end(); ++m_iter) {
        fout << m_iter->first;
        for(s_iter = m_iter->second.begin(); s_iter != m_iter->second.end(); ++s_iter) {
            fout << "\t" << *s_iter;
        }
        fout << std::endl;
    }
    fout << "#word2tag=" << m_sample->m_word2tag.size() << std::endl;
    fout << m_sample->m_word2tag.size() << std::endl;
    for(m_iter = m_sample->m_word2tag.begin(); m_iter != m_sample->m_word2tag.end(); ++m_iter) {
        fout << m_iter->first ;
        for(s_iter = m_iter->second.begin(); s_iter != m_iter->second.end(); ++s_iter) {
            fout << "\t" << *s_iter;
        }
        fout << std::endl;
    }
    fout << "#features=" << m_template->m_feature_vec->size()<<std::endl;
    fout << m_template->m_feature_vec->size()<<std::endl;
    for(size_t i = 0; i < m_template->m_feature_vec->size(); ++i) {
        for(size_t j = 0; j < m_template->m_feature_vec->at(i).size(); ++j) {
            fout << m_template->m_feature_vec->at(i).at(j) << "\t";
        }
        fout  << std::endl;
    }
    fout << "#fid2tag2score=" << m_fid2tag2score.size() << std::endl;
    fout << m_fid2tag2score.size() << std::endl;
    for(size_t i = 0; i < m_fid2tag2score.size(); ++i) {
        for(map<int,float>::iterator iter = m_fid2tag2score.at(i).begin(); iter != m_fid2tag2score.at(i).end(); ++iter) {
            fout << i << "\t" << iter->first<<"\t" << iter->second << std::endl;
        }
    }
    fout << "#window"<<std::endl;
    fout << m_window << std::endl;
    fout << "#beam_size" << std::endl;
    fout<<m_beam_size<<std::endl;
    fout << "#ngram" << std::endl;
    fout << m_template->m_ngram << std::endl;

    fout.close();
    return true;
}

bool Model::loadModel(const std::string& file) {
    ifstream fin(file);
    if(!fin.is_open()) {
        std::cout << "[ERROR] in loadmodel fail,file=" << file << std::endl;
        return false;
    }
    std::cout << "...start to loadmodel..." << std::endl;
    std::string line="";
    getline(fin, line);//text_model
    getline(fin,line);//template
    getline(fin, line);//template_size
    int num = String::Str2Int(line);
    for(int i = 0; i < num; ++i) {
        getline(fin, line);
        m_template->addTemplate(line);
    }
    getline(fin,line);//skip dict
    getline(fin,line);//dict word_count
    num = String::Str2Int(line);
    m_dict->resizeWord(num);
    for(int i =0 ; i < num; ++i) {
        getline(fin, line);
        m_dict->loadWordId(line);
    }
    std::cout << "..start to load_all_tag" << std::endl;
    getline(fin, line);//skip all_tag
    getline(fin, line);//all_tag_size
    num = String::Str2Int(line);
    for(int i = 0; i < num; ++i) {
        getline(fin,line);
        m_sample->m_all_tag.insert(String::Str2Int(line));
    }
    getline(fin, line);//skip tag2tag
    getline(fin, line);//tag2tag_size
    num = String::Str2Int(line);
    for(int i = 0; i < num; ++i) {
        getline(fin,line);
        vector<string> vec;
        String::Split(line, "\t", vec);
        int ktag = String::Str2Int(vec[0]);
        for(int i = 1; i < vec.size(); ++i) {
            int tmp_tag = String::Str2Int(vec[i]);
            m_sample->m_tag2tag[ktag].insert(tmp_tag);
        }
    }
    getline(fin, line);//skip word2tag
    getline(fin, line);//word2tag_size
    num = String::Str2Int(line);
    for(int i = 0; i < num; ++i) {
        getline(fin,line);
        vector<string> vec;
        String::Split(line, "\t", vec);
        int ktag = String::Str2Int(vec[0]);
        for(int i = 1; i < vec.size(); ++i) {
            int tmp_tag = String::Str2Int(vec[i]);
            m_sample->m_word2tag[ktag].insert(tmp_tag);
        }
    }
    getline(fin, line);//skip features
    getline(fin, line);//features_size
    num = String::Str2Int(line);
    m_template->m_feature_vec->resize(num);
    for(int i = 0; i < num; ++i) {
        getline(fin,line);
        vector<string> vec;
        String::Split(line, "\t", vec);
        for(int j = 0; j < vec.size(); ++j) {
             int fid = String::Str2Int(vec[j]);
             m_template->m_feature_vec->at(i).push_back(fid);
        }
    }
    getline(fin, line);//skip fid2tag2score
    getline(fin, line);//fid2tag2score_size
    num = String::Str2Int(line);
    m_fid2tag2score.resize(num);
    for(int i = 0; i < num; ++i) {
        getline(fin,line);
        vector<string> vec;
        String::Split(line, "\t", vec);
        int fid = String::Str2Int(vec[0]);
        int tag = String::Str2Int(vec[1]);
        float score = String::Str2Float(vec[2]);
        m_fid2tag2score.at(fid)[tag] = score;
    }
    getline(fin, line);//skip m_window
    getline(fin, line);
    m_window = String::Str2Int(line);
    getline(fin, line);//skip m_beam_size
    getline(fin, line);
    m_beam_size = String::Str2Int(line);
    getline(fin, line);//skip m_ngram
    getline(fin, line);
    m_template->m_ngram = String::Str2Int(line);
    std::cout << "...end loadmodel..." << std::endl;
    return true;
}

bool Model::predict(const std::vector<std::string>& sample, vector<std::strng>& test_tag) {
    vector<vectot<int> > new_sample;
    int col = sample.front().size();
    new_sample.resize(sample.size() + 2*m_window);
    for(int i = 0; i < m_window; ++i) {
        string tmp_tag = "B_" + String::ToString(m_window - i);
        new_sample.at(i).resize(col, m_dict->getId(tmp_tag));
        tmp_tag = "E_" + String::ToString(i);
        new_sample.at(new_sample.size() - m_window + i).resize(col, m_dict->getId(tmp_tag));
    }
    for(int i = m_window; i < new_sample.size() - m_window; ++i) {
        new_sample.at(i).resize(sample.at(i-m_window).size());
        for(int j = 0; j < sample.at(i-m_window).size(); ++j) {
            new_sample.at(i).at(j) = m_dict->getId(sample.at(i-m_window).at(j));
        }
    }
    BeamSearch* bs = new BeamSearch(m_beam_size, m_window, m_template->m_ngram, new_sample, m_template, m_dict);
    bs->beamSearch(&m_fid2tag2score, test_tag, m_sample->m_word2tag, m_sample->m_tag2tag, m_sample->m_all_tag);
    return true;
}
