#pragma once
#include <string>

using namespace std;

struct PlaceCommand {
    string section;
    string position;

    PlaceCommand* next;

    PlaceCommand(string sec, string pos) { section = sec; position = pos; next = nullptr; }
};
class PlaceCommands
{
public:
    PlaceCommand* first, * last;

    void addPlaceCommand(string sec, string pos);
    PlaceCommands();
    ~PlaceCommands();
};

