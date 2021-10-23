#include "SectionsList.h"

SectionsList::SectionsList() {
	first = last = nullptr;
	currentSection = nullptr;
}

SectionsList::~SectionsList() {
	Section* temp;
	while (first) {
		temp = first;
		first = first->next;
		delete temp;
	}
	first = last = nullptr;
}

Section* SectionsList::getSection(string sectionName) {
	Section* temp = first;
	while (temp) {
		if (temp->name == sectionName)
			return temp;
		temp = temp->next;
	}
	return nullptr;
}

void SectionsList::setCurrentSection(string sectionName) {
	Section* temp = getSection(sectionName);
	currentSection = temp;
}

void SectionsList::addSection(string name) {

	//First sections has Base adress 0
	if (!first) {
		first = new Section(name, 0);
		last = first;
	}

	//Every other section has base adress equal to last sections base address + size
	else {
		int baseAddress = last->baseAddress + last->size;
		last->next = new Section(name, 0);
		last = last->next;
	}
	currentSection = last;
}

void SectionsList::printSectionList() {
	Section* temp = first;

	while (temp) {
		cout << temp->name << " " << temp->baseAddress << " " << temp->size << endl;

		Data* temp2 = temp->data->first;

		while (temp2) {
			cout << temp2->value << " ";
			temp2 = temp2->next;
		}
		cout << endl;
		temp = temp->next;
	}
}


