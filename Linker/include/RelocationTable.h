#pragma once
#include <string>
#include <iostream>

using namespace std;


struct Relocation {
	string section;
	int offset;
	string type;
	string rBr;
	int addend;

	Relocation* next;
	Relocation(string s, int o, string t, string r, int a) { section = s; offset = o; type = t; rBr = r; addend = a; next = nullptr; }

};
class RelocationTable
{
public:
	Relocation* first, * last;
	void addRelocation(string s, int o, string t, string r, int a);
	void printRelocationTable();
	void prepraviRelokacije(int pomeraj);
	RelocationTable();
	~RelocationTable();
};

