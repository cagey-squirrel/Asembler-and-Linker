#include "DataList.h"

DataList::DataList() {
	first = last = nullptr;
}

DataList::~DataList() {
	Data* temp;
	while (first) {
		temp = first;
		first = first->next;
		delete temp;
	}
	first = last = nullptr;
}

void DataList::addData(int value, bool twoB) {
	Data* temp = new Data(value, twoB);

	if (!first) {
		first = temp;
		last = first;
	}

	else {
		last->next = temp;
		last = last->next;
	}
}
