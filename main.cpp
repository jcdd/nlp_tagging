#include"Dictionary.h"
#include "util.h"
#include "Sample.h"
#include "Template.h"
#include<iostream>

int main() {
    std::cout << "hello world" << std::endl;
    Dictionary *dict = new Dictionary;
    Sample sample(dict);
   //sample.loadSample("data/chn-seg-file.txt",2);

    Template temp;
    temp.loadTemplate("data/template.txt");

    return 1;
}
