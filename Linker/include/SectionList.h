#pragma once
#include <string>
#include "DataList.h"
#include "RelocationTable.h"
#include "SymbolTable.h"

using namespace std;

struct Section {
	string name;
	int baseAddress;
	int size;
	int indexFajla;
	SymbolTable* symbolTable;
	RelocationTable* relocationTable;
	Section* next;
	DataList* data;

	void prepraviRelokacije(int pomeraj) {
		relocationTable->prepraviRelokacije(pomeraj);
	}

	void nadoveziSekciju(Section* s) {
		size += s->size;

		relocationTable->last->next = s->relocationTable->first;
		relocationTable->last = s->relocationTable->last;

		data->last->next = s->data->first;
		data->last = s->data->last;

	}

	Section(string n, int b, int s, int indexF) { 
		name = n; baseAddress = b; size = s; next = nullptr; indexFajla = indexF;
		data = new DataList(); 
		symbolTable = new SymbolTable();
		relocationTable = new RelocationTable();
	}
	~Section() { delete data; delete symbolTable; delete relocationTable; }

	void promeniRedniBrojSimbolaRelokacije(string stariRbr, string noviRbr) {
		Relocation* r = relocationTable->first;

		while (r) {
			if (r->rBr == stariRbr) {
				r->rBr = noviRbr;
			}

			r = r->next;
		}
	}
	void addSize(int s) { size += s; }
};


class SectionList
{
public:
	Section* first, * last;
	Section* currentSection;
	void addSection(string name, int baseAddress, int size, int indexFajla);
	void addSection(Section*);
	void printSectionList();
	Section* getSection(string);
	Section* getSection(string, int);
	SectionList();
	~SectionList();
};

