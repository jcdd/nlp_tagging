#include "Template.h"
#include <fstream>

Template::Template():m_ngram(0),
       m_window_size(0),
       m_max_col(0), 
       m_dynamic_template_size(0),
       m_static_template_size(0),
       m_feature_size(0)
{
    m_template_vec = new vector<FeatureTemplate>;
    m_trie_root = new FeatureNode;
    m_feature_vec = new vector<vector<int> >;
}

Template::~Template() {
   if (NULL != m_template_vec) {
       delete m_template_vec;
   }
   if (NULL != m_feature_vec) {
       delete m_feature_vec;
   }
   clearNode(m_trie_root);
}

void Template::clearNode(FeatureNode* root) {
    for(unordered_map<int, FeatureNode*>::iterator iter = root->m_children_map.begin(); iter != root->m_children_map.end(); ++iter) {
        if (NULL != iter->second) {
            clearNode(iter->second);
            delete iter->second;
        }
    }
    root->m_children_map.clear();
}
/*
   %x[0,5]%x[0,6]
   %x[-1,0]%x[0,5]
   %x[0,0]%x[0,5]
   %x[1,0]%x[0,5]

   #Bigram
   %y[-1]
*/
bool Template::loadTemplate(const std::string &file) {
  ifstream fin(file);
  if (!fin.is_open()) {
     std::cout << "[ERROR] in Template::loadTemplate,file=" << file<<", is_not_open_succ!!" << std::endl;
     return false;
  }
  string line;
  while(getline(fin, line)) {
    String::Trim(line);
    addTemplate(line);
  }
  fin.close();
  if (m_ngram > m_window_size) {
      m_window_size = m_ngram;
  }
  std::cout << "[INFO] in loadTemplate over,m_ngram=" << m_ngram << ",m_window_size=" << m_window_size << ",m_max_col=" << m_max_col << ",m_dynamic_template_size=" << m_dynamic_template_size << ",m_static_template_size=" << m_static_template_size << ",m_temple_vec.size()=" << getTemplateNum() << std::endl;
   return true;
}

int Template::getTemplateNum() {
    return m_template_vec->size();
}

bool Template::addTemplate(const std::string& line) {
  if (line.empty())  {
      return false;
  }
  if (line.substr(0,1) == "#") {
      return false;
  }
  FeatureTemplate one_template;
  one_template.m_index = m_template_vec->size();
  size_t beginIndex;
  size_t endIndex;
  beginIndex = line.find("%x[", 0);
  while(beginIndex != string::npos) {
    endIndex = line.find(",",beginIndex);
    if (endIndex != string::npos) {
        int row = String::Str2Int(line.substr(beginIndex + 3, endIndex - beginIndex - 3));
        beginIndex = endIndex;
        endIndex = line.find("]", beginIndex);
        if (endIndex != std::string::npos) {
            int col = String::Str2Int(line.substr(beginIndex + 1, endIndex - beginIndex - 1));
            if (col < 0) {
                std::cout << "[ERROR] in addtemplate,col<0,col=" << col << std::endl;
                return false;
            }
            std::cout << "line=" << line<<",row=" <<row << ",col=" << col << std::endl;
            one_template.x.push_back(make_pair(row, col));
            if (abs(row) > m_window_size) {
                m_window_size = abs(row);
            }
            if (col > m_max_col) {
                m_max_col = col;
            }
            beginIndex = line.find("%x[", endIndex);
        }
        else{
           std::cout << "[ERROR] in add_template,error template=" << line << std::endl;
           return false;
        }
     }
  }

  beginIndex = line.find("%y[",0);
  while(beginIndex != std::string::npos) {
     endIndex = line.find("]", beginIndex);
     if (endIndex != std::string::npos) {
         int row = String::Str2Int(line.substr(beginIndex+3, endIndex - beginIndex - 3));
         one_template.y.push_back(row);
         if (abs(row) > m_ngram) {
            m_ngram = abs(row);
         }
         std::cout << "line=" << line << ",row=" << row << ",m_ngram=" << m_ngram << std::endl;
         beginIndex = line.find("%y[", endIndex);
     }
     else {
         std::cout << "[ERROR], in addtemplate,error template=" << line << std::endl;
         return false;
     }
  }
  one_template.m_length = (int)one_template.x.size() + (int)one_template.y.size();
  if (!one_template.y.empty()) {
     ++m_dynamic_template_size;
  }
  else {
     ++m_static_template_size;
  }
  m_template_vec->push_back(one_template);
  return true;
}

int Template::insertNode(const vector<int>& featureContext) {
    FeatureNode* curr = m_trie_root;
    vector<int>::const_iterator iter = featureContext.begin();
    unordered_map<int, FeatureNode*>::iterator map_iter;
    for (;iter != featureContext.end(); ++iter) {
        map_iter = curr->m_children_map.find(*iter);
        if (map_iter != curr->m_children_map.end()) {
            curr = map_iter->second;
        }
        else {
            FeatureNode* tmp = new FeatureNode;
            curr->m_children_map.insert(make_pair(*iter, tmp));
            curr = tmp;
        }
    }
    if (curr->m_feature_id == -1) {
        curr->m_feature_id = m_feature_size++;
        m_feature2freq_vec.resize(m_feature_size);
        m_feature2freq_vec.at(curr->m_feature_id) = 1;
    }
    else {
        m_feature2freq_vec.at(curr->m_feature_id) += 1;
    }
    return curr->m_feature_id;
}

int Template::getFeatureId(const vector<int>& featureContext) {
    FeatureNode* curr = m_trie_root;
    vector<int>::const_iterator iter = featureContext.begin();
    unordered_map<int, FeatureNode*>::iterator map_iter;
    for(; iter != featureContext.end(); ++iter) {
        map_iter = curr->m_children_map.find(*iter);
        if (map_iter != curr->m_children_map.end()) {
            curr = map_iter->second;
        }
        else {
            return -1;
        }       
    }
    return curr->m_feature_id;
}

void print_sample(const std::vector<std::vector<int> >& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << "hello....." << v[i][0] <<"|"<< v[i][1] << "|" << "|" << v[i][v[i].size()-2] << "|" << v[i][v[i].size()-1] << std::endl;
    }
}
bool Template::insertSample2FeatureNode(const std::vector<std::vector<int> >& sample) {
    vector<int> featureContext;
    for(size_t i = m_window_size; i < sample.size() - m_window_size; ++i) {
        for(size_t j = 0; j < m_template_vec->size(); ++j) {
            featureContext.clear();
            FeatureTemplate &tmp = m_template_vec->at(j);
            featureContext.push_back(tmp.m_index);
            for(size_t xIndex = 0; xIndex < tmp.x.size(); ++xIndex) {
                int row = tmp.x.at(xIndex).first;
                int col = tmp.x.at(xIndex).second;
                if (col > m_max_col) {
                    std::cout << "[ERROR] in Template::insertSample2FeatureNode,col=" << col << ",m_max_col=" << m_max_col << std::endl;
                    return false;
                }
                featureContext.push_back(sample.at(i+row).at(col));
            } 
            for(size_t yIndex = 0; yIndex < tmp.y.size(); ++yIndex) {
                int row = tmp.y.at(yIndex);
                featureContext.push_back(sample.at(i+row).back());
            }
            int feature_id = insertNode(featureContext);
            if (feature_id>(int)m_feature_vec->size()) {
                m_feature_vec->push_back(featureContext);
            }
        }
        m_tag_set.insert(sample.at(i).back());
    }
    return true;
}

bool Template::generateStaticFeatures(int index, const vector<vector<int> > &testSample, vector<int>& featureVec) {
    FeatureType f_type = static_feature;
    vector<int> tmp_vec;
    return getOneFeature(index, testSample, featureVec, f_type, tmp_vec);
}


bool Template::generateDynamicFeatures(int index, const vector<vector<int> > &testSample,const vector<int>& tagVec, vector<int>& featureVec) {
    FeatureType f_type = dynamic_feature;
    return getOneFeature(index, testSample, featureVec, f_type, tagVec);
}

bool Template::generateFeatures(int index, const vector<vector<int> > &testSample, const vector<int>& tagVec, vector<int>& featureVec) {
    FeatureType f_type = all_feature;
    return getOneFeature(index, testSample, featureVec, f_type, tagVec);
}    

bool Template::getOneFeature(int index, const vector<vector<int> >& testSample, vector<int>& featureVec, FeatureType f_type, const vector<int>& tagVec) {
    if (index < m_window_size || index >= (int)testSample.size() - m_window_size) {
        std::cout << "[ERROR] in Tempplate::generateStaticFeatures ,index_error, index = " << index << ",testSample.size()=" << testSample.size() << ",m_window_size=" << m_window_size << std::endl;
        return false;
    }
    featureVec.clear();
    if (f_type == static_feature) {
        featureVec.reserve(m_static_template_size);
    }
    else if (f_type == all_feature) {
        featureVec.reserve(m_template_vec->size());        
    }
    else if (f_type == dynamic_feature) {
        featureVec.reserve(m_dynamic_template_size);
    }
    else {
        std::cout << "[ERROR] in Template::getOneFeature,f_type=" << f_type << std::endl;
        return false;
    }
    vector<int> featureContext;
    for(size_t i = 0; i < m_template_vec->size();++i) {
        if (f_type == static_feature) {
            if (!m_template_vec->at(i).y.empty()) {
                continue;
            }
        }
        else if (f_type == dynamic_feature) {
            if(m_template_vec->at(i).y.empty())  {
                continue;
            }
        }
        else if (f_type == all_feature) {
            
        }
        else {
            std::cout << "[ERROR] in Template::getOneFeature_error,f_type=" << f_type << std::endl;
            return false;
        }
        FeatureTemplate &tmp = m_template_vec->at(i);
        featureContext.clear();
        featureContext.reserve(tmp.m_length);
        featureContext.push_back(tmp.m_index);
        for(size_t j = 0; j < tmp.x.size(); ++j) {
            int row = tmp.x.at(j).first;
            int col = tmp.x.at(j).second;
            if (col > (int)testSample.at(index).size() - 1) {
                std::cout << "[ERROR] in generateStaticFeature error,col=" << col << ",sample_size=" << testSample.at(index).size() << std::endl;
                return false;
            }
            featureContext.push_back(testSample.at(index + row).at(col));
        }
        if (f_type == dynamic_feature || f_type == all_feature) {
            for (size_t k = 0; k < tmp.y.size(); ++k) {
                int row = tmp.y.at(k);
                featureContext.push_back(tagVec.at(index + row));
            }
        }
        int feature_id = getFeatureId(featureContext);
        if (feature_id != -1) {
            featureVec.push_back(feature_id);
        }
        else {
            std::string flag = "";
            if (f_type == dynamic_feature) {
                flag = "dynamic_feature";
            }
            else if (f_type== static_feature) {
                flag = "static_feature";
            }
            else if (f_type == all_feature) {
               flag = "all_feature";
            }
            std::cout << "[WARING] in generate "<<flag << ", feature_id=" << feature_id << ":" ;
            for(int i = 0; i < featureContext.size(); ++i) {
                std::cout << featureContext[i]<<"|";
            }
            std::cout << std::endl;
        }
    } 
    return true;
}


