#include "LineList.h"


LineList::LineList() {
	first = last = nullptr;
}

void LineList::addLine(WordList* wordList) {
	if (!first) {
		first = new Line(wordList);
		last = first;
	}

	else {
		last ->next = new Line(wordList);
		last = last->next;
	}
}


LineList::~LineList() {
	Line* temp;
	while (first) {
		temp = first;
		first = first->next;
		delete temp;
	}
	first = last = nullptr;
}