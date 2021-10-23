#include "DataList.h"
#include <sstream>

using namespace std;

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

void DataList::updateData(int startPosition, bool pcRel, bool twoByte, int pomeraj, int trenutnaAdresa) {
	Data* temp = first;
	int index = 0;
	while (temp) {
		if (index == startPosition) {
			break;
		}
		index++;
		temp = temp->next;
	}

	if (!temp) {
		cout << "Pogresan index startPosition " << startPosition << endl;
		exit(-1);
	}

	if (twoByte) {
		string gornjiB = temp->value;
		string donjiB = temp->next->value;
		string twoB = gornjiB + donjiB;

		int vrednost;

		stringstream ss;

		ss << hex << twoB;

		ss >> vrednost;

		vrednost += pomeraj;

		if (pcRel) {
			vrednost -= trenutnaAdresa;
		}

		stringstream ss2;

		ss2 << hex << vrednost;
		twoB = ss2.str();

		if (twoB.length() == 1) {
			twoB = "000" + twoB;
		}
		else if (twoB.length() == 2) {
			twoB = "00" + twoB;
		}
		else if (twoB.length() == 3) {
			twoB = "0" + twoB;
		}

		temp->value = twoB.substr(0, 2);
		temp->next->value = twoB.substr(2, 2);

	}

	else {
		string bajt = temp->value;
		int vrednost;

		stringstream ss;

		ss << hex << twoByte;

		ss >> vrednost;

		vrednost += pomeraj;

		stringstream ss2;

		ss2 << hex << vrednost;
		bajt = ss2.str();

		temp->value = bajt;
	}
}

void DataList::addData(string value, bool twoB) {
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
