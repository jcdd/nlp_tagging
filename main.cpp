#include"Dictionary.h"
#include "util.h"
#include "Sample.h"
#include "Template.h"
#include "Model.h"
#include<iostream>

int main() {
    std::cout << "hello world" << std::endl;
    /*
    Dictionary *dict = new Dictionary;
    Sample sample(dict);
   //sample.loadSample("data/chn-seg-file.txt",2);

    Template temp;
    temp.loadTemplate("data/template.txt");
    */
    int threadNum = 10;
    int epoch = 50;
    int window = 2;
    int beam_size = 5;

    Model *model = new Model(threadNum, window, beam_size);

    model->train("data/template.txt", "data/chn-seg-file.txt_1k", epoch, "xx");
    delete model;
    return 1;
}
