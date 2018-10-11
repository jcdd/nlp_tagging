#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include<map>
#include<vector>
#include<iostream>

using namespace std;

class Dictionary {
  public:
    Dictionary();
    ~Dictionary();
    int getId(const std::string& word);
    int getInsId(const std::string& word);
    bool getWord(int id, std::string& word);
    size_t wordCount();
    int addWord(const std::string& word);

  private:
    map<std::string, int> m_word2id;
    vector<std::string> m_word_vec;

};

#endif
