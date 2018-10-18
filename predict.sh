rm a.out
sudo rm -rf /cores/*
g++ -g predict.cpp BeamSearch.cpp Dictionary.cpp Sample.cpp util.cpp Template.cpp Model.cpp -lpthread 
./a.out
