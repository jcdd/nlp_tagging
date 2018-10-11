#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include<iostream>
#include<string>
#include<map>
#include<set>
#include <unordered_map>
#include<vector>
#include <stdlib.h>
#include"util.h"

using namespace std;

enum FeatureType{
   static_feature,
   dynamic_feature,
   all_feature
};

struct FeatureTemplate {
    int m_index;
    int m_length;
    vector<pair<int,int> > x;
    vector<int> y;
};

struct FeatureNode{
    FeatureNode() {
        m_feature_id = -1;
    }
    int m_feature_id;
    unordered_map<int , FeatureNode*> m_children_map;
};

class Template{
  public:
    Template();
    ~Template();
    void clearNode(FeatureNode* root);
    bool loadTemplate(const std::string& file);
    bool addTemplate(const std::string& line);
    int getTemplateNum();
    bool generateStaticFeatures(int index, const vector<vector<int> > &testSample, vector<int>& featureVec);
    int getFeatureId(const vector<int>& feature);
    int insertNode(const vector<int>& featureContext);
    bool insertSample2FeatureNode(const std::vector<std::vector<int> >& sample) ;
    bool generateDynamicFeatures(int index, const vector<vector<int> > &testSample,const vector<int>& tagVec, vector<int>& featureVec);
    bool getOneFeature(int index, const vector<vector<int> >& testSample, vector<int>& featureVec, FeatureType f_type, const vector<int>& tagVec);

  private:
    int m_ngram;
    int m_window_size;
    int m_max_col;
    int m_dynamic_template_size;
    int m_static_template_size;
    int m_feature_size;
    vector<FeatureTemplate> *m_template_vec;
    FeatureNode* m_trie_root;
    vector<int> m_feature2freq_vec;
    vector<vector<int> > *m_feature_vec;
    set<int> m_tag_set;
};


#endif
