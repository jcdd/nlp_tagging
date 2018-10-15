#include "Dictionary.h"

Dictionary::Dictionary() {}

Dictionary::~Dictionary() {}

int Dictionary::getId(const std::string& word) {
    std::cout << ".....debug..." << word << "," << m_word2id.size() << std::endl;
    std::map<std::string, int>::iterator iter = m_word2id.find(word);
    if (iter != m_word2id.end()) {
        return iter->second;
    }
    else {
        return -1;
    }
}

int Dictionary::getInsId(const std::string& word) {
    std::map<std::string, int>::iterator iter = m_word2id.find(word);
    if (iter != m_word2id.end()) {
        return iter->second;
    }
    else {
        int index = (int)(m_word_vec.size());
        m_word2id.insert(make_pair(word, index));
        m_word_vec.push_back(word);
        return index;
    }
}

std::string Dictionary::getWord(int id) {
    if (id >= m_word_vec.size()) {
        std::cout << "[error] in Dictionary::getWord,id=" << id << ",m_word_vec.size=" << m_word_vec.size() << std::endl;
        return "";
    }
    return m_word_vec.at(id);
}

bool Dictionary::getWord(int id, std::string& word) {
    if (id >= m_word_vec.size()) {
        std::cout << "[error] in Dictionary::getWord,id=" << id << ",m_word_vec.size=" << m_word_vec.size() << std::endl;
        return false;
    }
    word = m_word_vec.at(id);
    return true;
}

size_t Dictionary::wordCount() {
    return m_word_vec.size();
}

int Dictionary::addWord(const std::string& word) {
    std::map<std::string, int>::iterator iter = m_word2id.find(word);
    if (iter != m_word2id.end()) {
        return iter->second;
    }
    else{
        m_word_vec.push_back(word);
        int index = (int)(m_word_vec.size() - 1);
        m_word2id.insert(make_pair(word, index));
        return index;
    }
}

