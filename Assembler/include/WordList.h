#pragma once
#include <iostream>
using namespace std;

struct Word {
    string name;
    Word* next = nullptr;

    Word(string fn) { name = fn; next = nullptr; }
};
class WordList
{
public:
    Word* first = nullptr, * last = nullptr;
    ~WordList();
    WordList();
    WordList(string line);
    string getWordByIndex(int index);
    void addWord(string name);
    bool contains(string name);
    void printWordList();
};

