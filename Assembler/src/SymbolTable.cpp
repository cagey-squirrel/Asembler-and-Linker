#include "SymbolTable.h"
#include <iomanip>
#include <iostream>
using namespace std;

SymbolTable::SymbolTable() {
	first = last = nullptr;
	number = 0;
}

SymbolTable::~SymbolTable() {
	Symbol* temp;
	while (first) {
		temp = first;
		first = first->next;
		delete temp;
	}
	first = last = nullptr;
}

Symbol* SymbolTable::findName(string n) {
	Symbol* temp = first;
	while (temp) {
		if (temp->name == n)
			return temp;
		temp = temp->next;
	}
	return nullptr;
}

void SymbolTable::addSymbol(string name, string section, long offset, string glob, int size, int value) {
	Symbol* sim = new Symbol(name, section, offset, glob, size, value);
	if (!first) {
		first = sim;
		last = first;
	}

	else {
		last->next = sim;
		last = last->next;
	}
}



void SymbolTable::printSymbolTable() {
	Symbol* temp = first;
	cout << setw(10) << "Name" << setw(10) << "Offset" << setw(10) << "Section" << setw(10) << "Size" << setw(10) << "Glob" << setw(10)
		<< "Value" << setw(10) << "Rbr" << endl;
	while (temp) {
		cout << setw(10) << temp->name << setw(10) << temp->offset << setw(10) << temp->section << setw(10) << temp->size
			<< setw(10) << temp->glob << setw(10) << temp->value << setw(10) << temp->rbr;
		cout << endl;
		temp = temp->next;
	}
}
