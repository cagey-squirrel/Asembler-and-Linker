#pragma once
#include <iostream>
using namespace std;

struct Data {
	string value;
	bool twoByte;
	Data* next;

	Data(string val, bool twoB) { value = val; twoByte = twoB; next = nullptr; }
};


class DataList
{
public:
	Data* first, * last;
	DataList();
	~DataList();
	void addData(string value, bool twoB = false);
	void updateData(int start_position, bool pcRel, bool twoByte, int pomeraj, int trenutnaAdresa);
};

