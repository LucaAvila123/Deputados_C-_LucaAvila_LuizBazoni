#include "csvReader.h"
#include <vector>
#include <sstream>

using namespace std;

string iso_8859_1_to_utf8(string &str)
{
  // adaptado de: https://stackoverflow.com/a/39884120 :-)
  string strOut;
  string::iterator it;
  for (it = str.begin(); it != str.end(); ++it)
  {
    uint8_t ch = *it;
    if (ch < 0x80)
    {
      // já está na faixa ASCII (bit mais significativo 0), só copiar para a saída
      strOut.push_back(ch);
    }
    else
    {
      // está na faixa ASCII-estendido, escrever 2 bytes de acordo com UTF-8
      // o primeiro byte codifica os 2 bits mais significativos do byte original (ISO-8859-1)
      strOut.push_back(0b11000000 | (ch >> 6));
      // o segundo byte codifica os 6 bits menos significativos do byte original (ISO-8859-1)
      strOut.push_back(0b10000000 | (ch & 0b00111111));
    }
  }
  return strOut;
}

csvReader::csvReader(string filePath, char delimiter){
    this->delimiter = delimiter;
    inFile.open(filePath); 
    inFile.imbue(locale("pt_BR.ISO8859-1"));

    if(!this->inFile){
        cout << "Arquivo inválido" << endl;
        abort();
    }

    int headerIndex = 0;
   
    string token;

    //currentLine esta em iso_8859_1
    getline(inFile, currentLine);

    //tokens sao salvos em utf8 (isso porque a comparacao vem de fora da classe)
    string currentLine_utf8 = iso_8859_1_to_utf8(currentLine);
    stringstream linhaDividida(currentLine_utf8);

    while (getline(linhaDividida, token, ';')) {
        headerIndexes[token] = headerIndex;
        headerIndex++;
    }

    getline(inFile, currentLine);
}

// diferentemente de Java, um string* não vai ter um parâmetro size que sirva bem
vector<string> csvReader::nextValues(string* selectedItems, int total){

    vector<string> lineValues;
    vector<string> selectedValuesList;
    string token;

    //tokens sao salvos em utf8
    string currentline_utf8 = iso_8859_1_to_utf8(currentLine);
    stringstream linhaDividida(currentline_utf8);
    
    while(getline(linhaDividida, token, ';')){
        lineValues.push_back(token);
    }

    for(int i = 0; i < total; i++){
        //cout << selectedItems[i] << endl;
        int itemIndex = this->headerIndexes[selectedItems[i]];
        string selectedValue = lineValues[itemIndex];
        selectedValuesList.push_back(selectedValue);
    }

    //current line esta codificada em iso889_1
    getline(inFile, currentLine);

    return selectedValuesList;
}