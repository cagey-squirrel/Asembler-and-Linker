#include "SectionList.h"

SectionList::SectionList() {
	first = last = nullptr;
	currentSection = nullptr;
}

SectionList::~SectionList() {
	Section* temp;
	while (first) {
		temp = first;
		first = first->next;
		delete temp;
	}
	first = last = nullptr;
}

Section* SectionList::getSection(string sectionName, int index) {
	Section* temp = first;
	while (temp) {
		if (temp->name == sectionName && temp->indexFajla == index)
			return temp;
		temp = temp->next;
	}
	return nullptr;
}

Section* SectionList::getSection(string sectionName) {
	Section* temp = first;
	while (temp) {
		if (temp->name == sectionName)
			return temp;
		temp = temp->next;
	}
	return nullptr;
}





void SectionList::addSection(string name, int baseAddress, int size, int indexFajla) {

	//First sections has Base adress 0
	if (!first) {
		first = new Section(name, 0, size, indexFajla);
		last = first;
	}

	//Every other section has base adress equal to last sections base address + size
	else {
		int baseAddress = last->baseAddress + last->size;
		last->next = new Section(name, 0, size, indexFajla);
		last = last->next;
	}
	currentSection = last;
}

void SectionList::addSection(Section* s) {
	//Section * s = new Section(s->name, s->baseAddress, s->)
	if (!first) {
		first = s;
		last = first;
	}

	
	else {
		last->next = s;
		last = last->next;
	}
	currentSection = last;
}

void SectionList::printSectionList() {
	Section* temp = first;

	while (temp) {
		cout << temp->name << " " << temp->baseAddress << " " << temp->size << " " << temp->indexFajla << endl;

		Data* temp2 = temp->data->first;

		while (temp2) {
			cout << temp2->value << " ";
			temp2 = temp2->next;
		}
		cout << endl;

		cout << endl;
		cout << "Tabela simbola: " << endl;
		temp->symbolTable->printSymbolTable();
		cout << endl << endl << endl;
		cout << "Tabela relokacija" << endl;
		temp->relocationTable->printRelocationTable();
		cout << endl << endl << endl << endl << endl << endl;
		temp = temp->next;
	}


}


