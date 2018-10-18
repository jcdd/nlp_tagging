#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include<map>
#include<vector>
#include<iostream>
#include <fstream>
#include "util.h"

using namespace std;

class Dictionary {
  public:
    Dictionary();
    ~Dictionary();
    int getId(const std::string& word);
    int getInsId(const std::string& word);
    bool getWord(int id, std::string& word);
    std::string getWord(int id);
    size_t wordCount();
    int addWord(const std::string& word);
    void saveDict(ofstream& fout) {
        fout << m_word2id.size() << std::endl;
        for (size_t i = 0; i < m_word_vec.size(); ++i) {
            fout << m_word_vec.at(i) << "\t" << i << std::endl;
        }
    }
    bool loadWordId(const std::string& line) {
        std::vector<string> vec;
        String::Split(line, "\t", vec);
        if (vec.size() != 2) {
            return false;
        }
        std::string word = vec[0];
        int id = String::Str2Int(vec[1]);
        m_word_vec.at(id) = word;
        m_word2id[word] = id;
        return true;
    }
    void resizeWord(int num) {
        m_word_vec.resize(num);
    }
  private:
    map<std::string, int> m_word2id;
    vector<std::string> m_word_vec;
};

#endif
