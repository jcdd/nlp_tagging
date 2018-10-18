#include"Sample.h"
#include "util.h"

Sample::Sample(Dictionary* dict, int window) {
    m_columu = 0;
    m_row = 0;
    m_sample_vec = new vector<vector<vector<int> > >;
    m_dict = dict;
    m_window = window;
}

Sample::~Sample(){
    if (NULL != m_sample_vec) {
        delete m_sample_vec;
    }
    m_dict = nullptr;
}

Dictionary* Sample::getDict() {
    return m_dict;
}

bool Sample::loadSample(const std::string& file, int window) {
    ifstream fin(file);
    if (!fin.is_open()) {
        std::cout << "[error] in Sample::loadSample file=" << file << ",is_not_open" << std::endl;
        return false;
    }
    std::string line;
    std::vector<int> elementVec;
    std::vector<vector<int> > sentenceVec;
    for (int i = 0 ; i < window; ++i) {
        sentenceVec.push_back(elementVec);
    }
    std::vector<std::string>::iterator iter;
    int tmp_count = 0;
    while(getline(fin, line)) {
        if (tmp_count++ == 0 && line.substr(0,2) == "##") {
            continue;
        }
        String::Trim(line);
        if (line.empty()) {
            //每句话的分隔符，空行
            if ((int)sentenceVec.size() == window) {
                continue;
            }
            for (int i = 0; i < window; ++i) {
                //B_2,B_1
                sentenceVec.at(i).resize(m_columu, m_dict->getInsId("B_" + String::ToString(window - i)));
                elementVec.clear();
                sentenceVec.push_back(elementVec);
                sentenceVec.back().resize(m_columu, m_dict->getInsId("E_"+String::ToString(i+1)));
            }
            m_sample_vec->push_back(sentenceVec);
            sentenceVec.clear();
            for(int i = 0; i < window; ++i) {
                sentenceVec.push_back(elementVec);
            }
            if (m_sample_vec->size() % 5000 == 0) {
                std::cout << "[INFO] in loadsample m_sample_vec.size()=" << m_sample_vec->size() << ",m_columu=" << m_columu<< std::endl;
            }
            continue;     
        }
        vector<std::string> wordVec;
        String::Split(line, "\t", wordVec);
        if (m_columu == 0) {
            m_columu = wordVec.size();
        }
        if (wordVec.size() != m_columu) {
             std::cout << "[error] in loadsample,wordVec.size()!= m_columu,line=" << line << ",m_columu=" << m_columu << std::endl;
             return false;
        }
        for (iter = wordVec.begin(); iter != wordVec.end(); ++iter) {
            elementVec.push_back(m_dict->getInsId(*iter));
        }
        sentenceVec.push_back(elementVec);
        elementVec.clear();
    }
    if (sentenceVec.size() > window) {
        for(int i = 0 ; i < window ; ++i) {
            //B_2,B_1
            sentenceVec.at(i).resize(m_columu, m_dict->getInsId("B_" + String::ToString(window - i)));
            elementVec.clear();
            sentenceVec.push_back(elementVec);
            sentenceVec.back().resize(m_columu, m_dict->getInsId("E_"+String::ToString(i+1)));
        }
        m_sample_vec->push_back(sentenceVec);
        sentenceVec.clear();
    }
    fin.close();
    generateTagDict();
    std::cout << "[INFO] in loadsmaple,over load_sample,m_columu=" << m_columu << ",smaple_size=" << m_sample_vec->size() << ",dict->wordcount=" << m_dict->wordCount() << ",m_tag2tag.size=" << m_tag2tag.size()<<",m_word2tag.size=" << m_word2tag.size()<<",all_tag_size="<<m_all_tag.size() <<std::endl;
    return true;
}

int Sample::getColumu() {
    return m_columu;
}

int Sample::getRow() {
    return m_row;
}

vector<vector<int> >* Sample::getSample(int index) {
    if (index >= 0 && index < m_sample_vec->size()) {
        return &m_sample_vec->at(index);
    }
    else {
        return NULL;
    }
}

int Sample::getSampleNum() {
   return (int)m_sample_vec->size();
}

void Sample::generateTagDict() {
   for (size_t i = 0; i < m_sample_vec->size(); ++i) {
       const vector<vector<int> >& train_sample = m_sample_vec->at(i);
       for (size_t j = m_window; j < train_sample.size() - m_window; ++j) {
          int curr_tag = train_sample.at(j).back();
          m_all_tag.insert(curr_tag);
          int pre_tag = train_sample.at(j-1).back();
          if (m_tag2tag.find(pre_tag) != m_tag2tag.end()) {
              m_tag2tag[pre_tag].insert(curr_tag);
          }
          else {
              set<int> tmp_set;
              tmp_set.insert(curr_tag);
              m_tag2tag[pre_tag] = tmp_set;
          }
          int curr_word = train_sample.at(j).front();
          if (m_word2tag.find(curr_word) != m_word2tag.end()) {
              m_word2tag[curr_word].insert(curr_tag);
          }
          else {
              set<int> tmp_set;
              tmp_set.insert(curr_tag);
              m_word2tag[curr_word] = tmp_set;
          }
       }
   }
}
