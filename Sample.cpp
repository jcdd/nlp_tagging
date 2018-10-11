#include"Sample.h"
#include "util.h"

Sample::Sample(Dictionary* dict) {
    m_columu = 0;
    m_row = 0;
    m_sample_vec = new vector<vector<vector<int> > >;
    m_dict = dict;
}

Sample::~Sample(){
    if (NULL != m_sample_vec) {
        delete m_sample_vec;
    }
    m_dict = nullptr;
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
    while(getline(fin, line)) {
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
            if (m_sample_vec->size() % 100 == 0) {
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
    std::cout << "[INFO] in loadsmaple,over load_sample,m_columu=" << m_columu << ",smaple_size=" << m_sample_vec->size() << ",dict->wordcount=" << m_dict->wordCount() <<std::endl;
    return true;
}

int Sample::getColumu() {
    return m_columu;
}

int Sample::getRow() {
    return m_row;
}

