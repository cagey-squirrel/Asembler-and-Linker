#include "WordList.h"


WordList::WordList() {
	first = last = nullptr;
}

void WordList::addWord(string s) {
	if (!first) {
		first = new Word(s);
		last = first;
	}

	else {
		last->next = new Word(s);
		last = last->next;
	}

}

WordList::~WordList() {
	Word* temp;
	while (first) {
		temp = first;
		first = first->next;
		delete temp;
	}
	first = last = nullptr;
}



