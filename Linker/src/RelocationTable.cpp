#include "RelocationTable.h"
#include <iomanip>


RelocationTable::RelocationTable() {
	first = last = nullptr;
}


RelocationTable::~RelocationTable() {
	Relocation* temp;

	while (first) {
		temp = first;
		first = first->next;
		delete temp;
	}

	first = last = nullptr;
}

void RelocationTable::prepraviRelokacije(int pomeraj) {
	Relocation* temp = first;

	while (temp) {
		temp->offset += pomeraj;

		if (temp->type.substr(0,5) == "local"){
			temp->addend += pomeraj;
		}

		temp = temp->next;
	}
}

void RelocationTable::addRelocation(string s, int o, string t, string r, int a) {
	Relocation* temp = new Relocation(s, o, t, r, a);

	if (!first) {
		first = temp;
		last = first;
	}

	else {
		last->next = temp;
		last = last->next;
	}
}


void RelocationTable::printRelocationTable() {
	Relocation* temp = first;
	cout << setw(10) << "Offset" << setw(10) << "Section" << setw(20) << "Type" << setw(20) << "Rbr" << setw(20)
		<< "Addend" << endl;
	while (temp) {
		cout << setw(10) << temp->offset << setw(10) << temp->section << setw(20) << temp->type << setw(20) << temp->rBr
			<< setw(20) << temp->addend;
		cout << endl;
		temp = temp->next;
	}
}