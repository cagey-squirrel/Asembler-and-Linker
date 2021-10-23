#pragma once
#include <string>

using namespace std;

struct Symbol {
	string name;
	string section;
	long offset;
	string glob;
	int rbr;
	int size;
	int value;
	int indexFajla;

	Symbol* next;

	Symbol(string n, string s, long o, string g, int rb, int si, int v, int i) { name = n; section = s; offset = o; glob = g; rbr = rb; size = si; value = v; indexFajla = i;  next = nullptr; };
};
class SymbolTable
{
public:
	int rb;
	Symbol* first, * last;
	SymbolTable(bool numbered = false);
	~SymbolTable();
	void printSymbolTable();
	void prepraviSimbole(string imeSekcije, int indexFajla, int pomeraj, int baseAddress);
	void prepraviGlobalneSimbole(string imeSekcije, int pomeraj);
	void addSymbol(string name, string section, long offset, string glob, int rbr, int size, int value, int indexFajla);
	int addSymbol(Symbol* s);
	Symbol* findGlobalName(string name);
	Symbol* findSymbolByRbr(int rbr);

};

