#include "SymbolTable.h"
#include <iostream>
#include <iomanip>

using namespace std;


SymbolTable::SymbolTable(bool n) {
	rb = 0;
	first = last = nullptr;
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

void SymbolTable::prepraviGlobalneSimbole(string imeSekcije, int pomeraj) {
	Symbol* temp = first;

	while (temp) {

		if (temp->section == imeSekcije && temp->glob == "glob") {
			temp->offset += pomeraj;
		}

		temp = temp->next;
	}
}

void SymbolTable::prepraviSimbole(string imeSekcije, int indexF, int pomeraj, int baseAddress) {
	Symbol* temp = first;

	while (temp) {
		
		if (temp->section == imeSekcije && temp->indexFajla == indexF && temp->glob == "loc") {
			temp->offset += pomeraj;
		}
		if (temp->section == imeSekcije && temp->indexFajla == indexF && temp->glob == "glob") {
			temp->offset += pomeraj + baseAddress;
		}

		temp = temp->next;
	}
}


void SymbolTable::addSymbol(string name, string section, long offset, string glob, int rbr, int size, int value, int indexFajla) {
	
	Symbol* sim = new Symbol(name, section, offset, glob, rbr, size, value, indexFajla);
	if (!first) {
		first = sim;
		last = first;
	}

	else {
		last->next = sim;
		last = last->next;
	}
}

int SymbolTable::addSymbol(Symbol* s) {
	Symbol* sim = new Symbol(s->name, s->section, s->offset, s->glob, rb, s->size, s->value, s->indexFajla);

	rb++;

	if (!first) {
		first = sim;
		last = first;
	}

	else {
		last->next = sim;
		last = last->next;
	}

	return rb - 1;
}

Symbol* SymbolTable::findGlobalName(string name) {
	Symbol* temp = first;

	while (temp) {
		if (temp->name == name && temp->glob == "glob")
			return temp;

		temp = temp->next;
	}

	return nullptr;
}

Symbol* SymbolTable::findSymbolByRbr(int rbr) {
	Symbol* temp = first;

	while (temp) {
		if (temp->rbr == rbr)
			return temp;

		temp = temp->next;
	}

	return nullptr;
}



