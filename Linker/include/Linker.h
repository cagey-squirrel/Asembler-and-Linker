#pragma once
#include <iostream>
#include "WordList.h"
#include "SymbolTable.h"
#include "RelocationTable.h"
#include "SectionList.h"
#include "PlaceCommands.h"

using namespace std;

class Linker
{

public:
    string mod;
    WordList* fileList;
    SectionList* sectionList;
    SymbolTable* globalListOfSymbols;
    SectionList* globalSectionList;
    SymbolTable* externSymbols;
    PlaceCommands* placeCommands;
    WordList* imenaSekcija;
    string outputFileName;
    Linker(WordList* fl, string ofl, string mode, PlaceCommands* placeCommands);
    ~Linker();

    void obrada();
   
};

