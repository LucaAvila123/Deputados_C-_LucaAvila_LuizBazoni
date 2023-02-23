#ifndef CSVREADER_H
#define CSVREADER_H

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <locale>

using namespace std;

class csvReader{
    ifstream inFile; // arquivo de entrada
    map<string, int> headerIndexes;
    char delimiter;
    string currentLine;
    locale place;

public:
    // tentar fazer de um jeito que eu não me orgulho
    csvReader(string filePath, char delimiter);
    vector<string> nextValues(string* selectedItems, int total); 
    bool hasNextValues(){
        return !inFile.eof();
    }
    ~csvReader() { inFile.close(); } 
};

#endif