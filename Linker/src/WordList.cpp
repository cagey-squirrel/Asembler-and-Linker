#include "WordList.h"



string WordList::getWordByIndex(int index) {
    int i = 0;
    Word* temp = first;

    while (temp) {
        if (i == index) {
            return temp->name;
        }
        i++;
        temp = temp->next;
    }

    cout << "Pogresan index " << index << endl;
    exit(-1);
}


void WordList::addWord(string fn) {
    if (!first) {
        first = new Word(fn);
        last = first;
    }

    else {
        last->next = new Word(fn);
        last = last->next;
    }
}

bool WordList::contains(string n) {
    Word* temp = first;

    while (temp) {
        if (temp->name == n) {
            return true;
        }

        temp = temp->next;
    }

    return false;
}

void WordList::printWordList() {
    Word* temp = first;

    while (temp) {
        cout << temp->name << " ";

        temp = temp->next;
    }

    cout << endl;
}

WordList::WordList() {

}

WordList::WordList(string line) {
    string word = "";

    for (auto c : line) {
        if (c == ' ' || c == '\t' || c == '\n') {
            if (word != "") {
                addWord(word);
                word = "";
            }
        }

        else {
            word += c;
        }
    }
    if (word != "") {
        addWord(word);
    }

}

WordList::~WordList() {
    Word* temp;
    while (first) {
        temp = first;
        first = first->next;
        delete temp;
    }
    first = nullptr;
    last = nullptr;
}