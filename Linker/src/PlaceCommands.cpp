#include "PlaceCommands.h"

PlaceCommands::PlaceCommands() {
    first = nullptr;
    last = nullptr;

}

PlaceCommands::~PlaceCommands() {
    PlaceCommand* temp;

    while (first) {
        temp = first;
        first = first->next;
        delete temp;
    }

    first = nullptr;
    last = nullptr;

}

void PlaceCommands::addPlaceCommand(string sec, string pos) {

    PlaceCommand* temp = new PlaceCommand(sec, pos);

    if (!first) {
        first = temp;
        last = first;
    }

    else {
        last->next = temp;
        last = last->next;
    }
}