rm a.out
sudo rm -rf /cores/*
g++ -g main.cpp BeamSearch.cpp Dictionary.cpp Sample.cpp util.cpp Template.cpp Model.cpp -lpthread 
./a.out
