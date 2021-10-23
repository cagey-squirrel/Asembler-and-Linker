#include <fstream>
#include <iostream>
#include "WordList.h"
#include "Linker.h"

using namespace std;

int main(int argc, char* argv[]) {
    WordList* fileList = new WordList();
    fileList->addWord("linkerinput.txt");
    fileList->addWord("nestopise.txt");
    PlaceCommands* pc = new PlaceCommands();
    pc->addPlaceCommand("text", "1000");
    string mod = "hex";
    string outputFileName = "linkeroutput.txt";

    Linker* linker = new Linker(fileList, outputFileName, mod, pc);

    linker->obrada();

  


}