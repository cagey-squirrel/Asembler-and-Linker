#include "Asembler.h"
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {

    string inputFileName;
    string outputFileName;

    if (argc < 4) {
        cout << "Pogresno uneti elementi komandne linije1" << endl;
        cout << "Sablon argumenata: -o outputFileName inputFileName" << endl;
        exit(-1);
    }
    
    if (string(argv[1]) != "-o") {
        cout << "Pogresno uneti elementi komandne linije2 " << endl;
        cout << "Sablon argumenata: -o outputFileName inputFileName" << endl;
        exit(-1);
    }

    outputFileName = argv[2];
    inputFileName = argv[3];


    Asembler as = Asembler(inputFileName, outputFileName);
    as.firstPass();
    
    as.secondPass();

    as.writeToFile();

    
    /*
    as.symbolTable->printSymbolTable();

    cout << endl << endl;
    
    Section* t = as.sectionList->first;
    while (t) {
        cout << t->name << endl;
        cout << "size = " << t->size << " and position = " << t->position << endl;
        Data* d = t->data->first;
        while (d) {
            cout << d->value << " | " ;
            d = d->next;
        }
        cout << endl << endl;
        t = t->next;
    }


    cout << endl << endl << endl << endl;
    as.relocationTable->printRelocationTable();
    

    
    cout << endl << endl << endl << endl;
    Line* temp = as.lines->first;
    while (temp) {
        Word* t = temp->wordList->first;
        while (t) {
            cout << t->word << " ";
            t = t->next;
        }
        cout << endl;
        temp = temp->next;
    }
    */
    return 0;
}
