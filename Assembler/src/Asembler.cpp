#include "Asembler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include <math.h>
#include <iomanip>

using namespace std;


Asembler::Asembler(string inputFileName, string outputFileName) {
    this->inputFileName = inputFileName;
    this->outputFileName = outputFileName;
    this->lines = new LineList();
    this->symbolTable = new SymbolTable();
    this->sectionList = new SectionsList();
    this->ended = false;
    this->relocationTable = new RelocationTable();
}

Asembler::~Asembler() {
    delete symbolTable;
    delete lines;
    delete sectionList;
    delete relocationTable;
}

void Asembler::firstPass() {

    ifstream myFile(inputFileName);

    if (!myFile.is_open()) {
        cout << "Pogresno ime fajla" << endl;
        exit(-1);
    }

    string line;

    while (getline(myFile, line)) {
        decipher(line);
        if (ended)
            return;

    }

    myFile.close();

    if (!ended) {
        cout << "Program mora da sadrzi direktivu .end" << endl;
        exit(-1);
    }
}

int Asembler::stringToInt(string s) {
    if (s.length() > 2) {
        if (s[0] == '0' and s[1] == 'x') {
            //s.erase(0, 2);
            return stoi(s, nullptr, 16);
        }
        else {
            return stoi(s);
        }
    }
    else {
        return stoi(s);
    }
}

bool Asembler::checkSymbol(string s) {
    if (!isalpha(int(s.front()))) {
        return false;
    }
    for (auto c : s) {
        if (!isalnum(c)) {
            if(c!='_')
                return false;
        }
    }

    return true;
}

bool Asembler::checkRegistar(string s) {
    if (!(s == "r0" || s == "r1" || s == "r2" || s == "r3" || s == "r4" || s == "r5" || s == "r6" || s == "r7")) {
        return false;
    }
    return true;
}

bool Asembler::checkLiteral(string s) {
    if (s.length() >= 2) {
        if (s[0] == '0' and s[1] == 'x') {
            for (int i = 2; i < s.length(); i++)
                if (!(s[i] == '0' || s[i] == '1' || s[i] == '2' || s[i] == '3' || s[i] == '4' || s[i] == '5' || s[i] == '6' || s[i] == '7' || s[i] == '8' || s[i] == '9' ||
                    s[i] == 'A' || s[i] == 'B' || s[i] == 'C' || s[i] == 'D' || s[i] == 'E' || s[i] == 'F')) {
                    return false;
                }

        }
        else {
            for (auto c : s) {
                if (!(isdigit(c))) {
                    return false;
                }
            }
        }
    }
    else {
        for (auto c : s) {
            if (!(isdigit(c))) {
                return false;
            }
        }
    }
    return true;
}


WordList* Asembler::split(string line) {
    WordList* words = new WordList();
    string word = "";

    for (auto c : line) {
        if (c == '#') {
            break;
        }
        else if (c == ' ' || c == '\t' || c == '\n') {
            if (word != "") {
                words->addWord(word);
                word = "";
            }
        }
        else 
            word += c;      
    }
    if (word != "") {
        words->addWord(word);
    }
    return words;
}

void Asembler::decipher(string line) {

    WordList* words = split(line);

    if (!words->first) { //No words in line or all commented
        return;
    }
    
    lines->addLine(words);

    Word* curr = words->first;

    while(curr) {

        
        string word = curr->word;

        //Obrada labele
        if (word.back() == ':') {

            if (curr != words->first) { //Labela nije na pocetku
                cout << "Labela mora biti na pocetku linije" << endl;
                exit(-1);
            }
            else {

             
                word.pop_back(); //Brisemo ":" sa kraja labele

                if (!checkSymbol(word)) {
                    cout << "Ime labele mora biti simbol (pocinje slovom i sadrzi slova i brojeve)" << endl;
                    exit(-1);
                }

                if (symbolTable->findName(word)) {
                    cout << "Simbol sa datim imenom vec postoji" << endl;
                    exit(-1);
                }

                if (!sectionList->currentSection) {
                    cout << "Simbol " << word << " definisan van sekcije" << endl;
                    exit(-1);
                }

                symbolTable->addSymbol(word, sectionList->currentSection->name, sectionList->currentSection->size, "loc", 0);

            }
        }

        //Obrada direktiva  

        //global
        else if (word.front() == '.') {
            if (word == ".global") {

                if (curr == words->last) {
                    cout << ".global mora da stoji uz bar jedan simbol" << endl;
                    exit(-1);
                }


                curr = curr->next;

                while (curr) {
                    //Obrada te reci
                    if (curr->next and !(curr->word.back() == ',')) {
                        cout << "Argumenti moraju da budu razdvojeni zapetom" << endl;
                        exit(-1);
                    }

                    if (curr->word.back() == ',') {
                        curr->word.pop_back();
                    }

                   if (!checkSymbol(curr->word)) {
                   cout << "Ime globalnog simbola mora da pocinje slovom i sadrzi slova i brojeve" << endl;
                   exit(-1);
                   }

                    //U prvom prolazu se ne radi nista
                    //cout << curr->word << " je global" << endl;
                    curr = curr->next;
                }
            }


            //extern
            else if (word == ".extern") {
                if (curr == words->last) {
                    cout << ".extern mora da stoji uz bar jedan simbol" << endl;
                    exit(-1);
                }


                curr = curr->next;

                while (curr) {
                    //Obrada te reci
                    //U prvom prolazu se ne radi nista
                    if (curr->next and !(curr->word.back() == ',')) {
                        cout << "Argumenti moraju da budu razdvojeni zapetom" << endl;
                        exit(-1);
                    }

                    if (curr->word.back() == ',') {
                        curr->word.pop_back();
                    }
                    if (!checkSymbol(curr->word)) {
                        cout << "Ime externog simbola mora da pocinje slovom i sadrzi slova i brojeve" << endl;
                        exit(-1);
                    }
                    if (symbolTable->findName(curr->word)) {
                        cout << "Simbol datog imena " << curr->word << " je vec definisan" << endl;
                        exit(-1);
                    }
                    symbolTable->addSymbol(curr->word, "extern" , 0, "glob", 0);

                    cout << curr->word << " je extern" << endl;
                    curr = curr->next;
                }
            
            }



            //section
            else if (word == ".section") {
                if (curr == words->last) {
                    cout << "Uz .section mora da stoji i ime sekcije (posle znaka razmaka)" << endl;
                    exit(-1);
                }
                curr = curr->next;

                if (!checkSymbol(curr->word)) {
                    cout << "Ime sekcije mora da pocinje slovom i sadrzi slova i brojeve" << endl;
                    exit(-1);
                }

                if (curr->next) {
                    cout << "Uz section sme da stoji samo ime" << endl;
                    exit(-1);
                }

                //Obrada sekcije:
                if (sectionList->getSection(curr->word)) { // Sekcija vec postoji
                    sectionList->setCurrentSection(curr->word);
                }
                else { // Dodajemo sekciju (automarski se azurira tekuca sekcija)
                    sectionList->addSection(curr->word);
                }
            }

            else if (word == ".word") {
                if (curr == words->last) {
                    cout << ".word mora da stoji uz bar jedan simbol" << endl;
                    exit(-1);
                }

                if (!sectionList->currentSection) {
                    cout << ".word definisan van sekcije" << endl;
                    exit(-1);
                }

                curr = curr->next;

                while (curr) {
                    if (curr->next and !(curr->word.back() == ',')) {
                        cout << "Argumenti moraju da budu razdvojeni zapetom" << endl;
                        exit(-1);
                    }
                    if (curr->word.back() == ',') {
                        curr->word.pop_back();
                    }
                    if (!checkSymbol(curr->word) and !checkLiteral(curr->word)) {
                        cout << "Uz .word mora da ide simbol ili literal" << endl;
                        exit(-1);
                    }
                    //In first pass for every simbol we just add size of 2 bytes
                    sectionList->currentSection->addSize(2);                   
                    //cout << curr->word << " je word" << endl;
                    curr = curr->next;
                }
            }


            //Skip
            else if (word == ".skip") {
                if (curr == words->last) {
                    cout << "Uz .skip mora da stoji i literal (posle znaka razmaka)" << endl;
                    exit(-1);
                }
                curr = curr->next;

                if (curr->next) {
                    cout << "Uz .skip sme da stoji samo jedan literal" << endl;
                    exit(-1);
                }

                if (!sectionList->currentSection) {
                    cout << ".skip definisan van sekcije" << endl;
                    exit(-1);
                }

                if (!checkLiteral(curr->word)) {
                    cout << "Literal mora biti dekadan ili heksa" << endl;
                    exit(-1);
                }

                sectionList->currentSection->addSize(stringToInt(curr->word));


            }

            else if (word == ".equ") {
                if (curr == words->last) {
                    cout << "Uz .equ mora da stoji novi simbol (posle znaka razmaka)" << endl;
                    exit(-1);
                }


                curr = curr->next;

                if (curr->word.back() == ',') {
                    curr->word.pop_back();
                }

                if (!checkSymbol(curr->word)) {
                    cout << "Ime equ simbola mora da pocinje slovom i sadrzi slova i brojeve" << endl;
                    exit(-1);
                }

                string symbolName = curr->word;

                if (curr == words->last) {
                    cout << "Uz .equ mora da stoji novi simbol i literal (posle znaka razmaka)" << endl;
                    exit(-1);
                }

                curr = curr->next;

                if (!(curr == words->last)) {
                    cout << "Uz .equ mora da stoji novi simbol i literal (posle znaka razmaka) i nista vise" << endl;
                    exit(-1);
                }

                if (!checkLiteral(curr->word)) {
                    cout << "Literal u equ mora da bude dekadan ili heksadecimalan" << endl;
                    exit(-1);
                }

                int value = stringToInt(curr->word);

                symbolTable->addSymbol(symbolName, "ABS", 0, "loc", 0, value);
                
            }

            else if (word == ".end") {
                if (! (curr == words->last)) {
                    cout << ".end mora da stoji samostalno" << endl;
                    exit(-1);
                }
                ended = true;
                return;
            }

            else {
                //cout << word << " je rec " << endl;
                cout << word << " je nepostojeca direktiva" << endl;
                exit(-1);

            }


        }


        //Obrada naredbi    
        else {

            //Halt
            if (word == "halt" || word == "iret" || word == "ret") {
                if (!(curr == words->last)) {
                    cout << word << " mora da stoji samostalno" << endl;
                    exit(-1);
                }
                if (!sectionList->currentSection) {
                    cout << word << " se poziva van sekcije" << endl;
                    exit(-1);
                }


                sectionList->currentSection->addSize(1);

            }


            //Int
            else if (word == "int") {

                if (curr == words->last) {
                    cout << "int mora da ima jedan argument" << endl;
                    exit(-1);
                }

                if (!checkRegistar(curr->next->word)) {
                    cout << "Registar mora biti u obliku r[0-8]" << endl;
                    exit(-1);
                }

                if (!(curr->next == words->last)) {
                    cout << "int mora da ima tacno jedan argument" << endl;
                    exit(-1);
                }

                if (!sectionList->currentSection) {
                    cout << "halt se poziva van sekcije" << endl;
                    exit(-1);
                }

                sectionList->currentSection->addSize(2);
                return;
            }

            //Jmp
            else if (word == "call" || word == "jmp" || word == "jeq" || word == "jne" || word == "jgt") {
                if (curr == words->last) {
                    cout << word << " mora da ima jedan argument" << endl;
                    exit(-1);
                }

                if (!(curr->next == words->last)) {
                    cout << word << " mora da ima tacno jedan argument" << endl;
                    exit(-1);
                }

                if (!sectionList->currentSection) {
                    cout << word << " se poziva van sekcije" << endl;
                    exit(-1);
                }

                string operand = curr->next->word;
                if (operand.at(0) == '*') {                   
                    if (checkRegistar(operand.substr(1, operand.length() - 1))) {
                        sectionList->currentSection->addSize(3);
                        return;
                    }

                    if (operand.at(1) == '(') {
                        if (checkRegistar(operand.substr(2, operand.length() - 3))) {
                            sectionList->currentSection->addSize(3);
                            return;
                        }
                    }
                }

                sectionList->currentSection->addSize(5); // else

                return;
            }

            



            else if (word == "push" || word == "pop") {
                if (curr == words->last) {
                    cout << word << " mora da ima jedan argument" << endl;
                    exit(-1);
                }

                if (!checkRegistar(curr->next->word)) {
                    cout << "Registar mora biti u obliku r[0-8]" << endl;
                    exit(-1);
                }

                if (!(curr->next == words->last)) {
                    cout << word << " mora da ima tacno jedan argument" << endl;
                    exit(-1);
                }

                if (!sectionList->currentSection) {
                    cout << word << " se poziva van sekcije" << endl;
                    exit(-1);
                }

                sectionList->currentSection->addSize(3);
                return;
            }

            

            
            
            else if (word == "xchg" || word == "add" || word == "sub" || word == "mul" || word == "div" || word == "cmp" || word == "not" || word == "and"
                || word == "or" || word == "xor" || word == "test" || word == "shl" || word == "shr") {

                if (curr == words->last) {
                    cout << word << " mora da ima 2 argumenta" << endl;
                    exit(-1);
                }

                if (curr->next == words->last) {
                    cout << word << " mora da ima 2 argumenta" << endl;
                    exit(-1);
                }

                if (curr->next->word.back() != ',') {
                    cout << "Argumenti za " << word << " moraju biti navedeni u obliku \"arg1, arg2\"" << endl;
                    exit(-1);
                }

                curr->next->word.pop_back();

                if (!checkRegistar(curr->next->word)) {
                    cout << "Registar mora biti u obliku r[0-9]" << endl;
                    exit(-1);
                }
                if (!(curr->next->next == words->last)) {
                    cout << word << " mora da ima 2 argumenta" << endl;
                    exit(-1);
                }

                if (!checkRegistar(curr->next->next->word)) {
                    cout << "Registar mora biti u obliku r[0-9]" << endl;
                    exit(-1);
                }

                if (!sectionList->currentSection) {
                    cout << word << " se poziva van sekcije" << endl;
                    exit(-1);
                }

                sectionList->currentSection->addSize(2);
                return;
                }
            



            else if (word == "ldr" || word == "str") {
                if (curr == words->last) {
                    cout << word << " mora da ima 2 argumenta" << endl;
                    exit(-1);
                }

                if (curr->next == words->last) {
                    cout << word << " mora da ima 2 argumenta" << endl;
                    exit(-1);
                }

                if (!curr->next->word.back() == ',') {
                    cout << "Format za ldr je ldr rx, operand" << endl;
                    exit(-1);
                }

                curr->next->word.pop_back(); //Brisemo ','

                if (!checkRegistar(curr->next->word)) {
                    cout << "Registar mora biti u obliku r[0-9]" << endl;
                    exit(-1);
                }

                if (!(curr->next->next == words->last)) {
                    cout << word << " mora da ima 2 argumenta" << endl;
                    exit(-1);
                }


                if (!sectionList->currentSection) {
                    cout << word << " se poziva van sekcije" << endl;
                    exit(-1);
                }

                string operand = curr->next->next->word;

                if (checkRegistar(operand)) {
                    sectionList->currentSection->addSize(3);
                    return;
                }

                if (operand.at(0) == '(') {
                    if (checkRegistar(operand.substr(1, operand.length() - 2))) {
                        sectionList->currentSection->addSize(3);
                        return;
                    }
                }
               
                sectionList->currentSection->addSize(5); // else
                              
                return;

                
            }

            else {
                cout << word << " ne postoji kao direktiva ili instrukcija" << endl;
                exit(-1);
                //cout << word << " je neispravno unesena naredba" << endl;
            }



        }
        if(curr)
            curr = curr->next;
    }
}


string Asembler::intToBin(string number, bool twoByteInt) {
    int num = stoi(number);

    if (twoByteInt) {
        string bin =  bitset<16>(num).to_string();
        return bin;
    }

    else {
        string bin = bitset<4>(num).to_string();
        return bin;
    }
}

int Asembler::binToInt(string bin) {
    return stoi(bin, 0, 2);
}

void Asembler::writeToFile() {

    //fstream mojFajl;
    //mojFajl.open("output.txt", ios::out);

    string root = "C:\\Users\\pc\\source\\repos\\SSProjekat Linker\\SSProjekat Linker\\";
    root += outputFileName;
        
    

    ofstream mojFajl(root);


    if (!mojFajl) {
        cout << "Greska sa otvaranjem fajla za pisanje" << endl;
        exit(-1);
    }

    Section* s = sectionList->first;
    while (s) {
        mojFajl << setw(20) << "Ime sekcije " << setw(20) << "pocetna adresa " << setw(10) << " size" << endl;
        mojFajl << "Sekcija: " << setw(10) << s->name << setw(19) << " " << s->baseAddress << setw(10) << " " << s->size << endl;
        //cout << "size = " << s->size << " and position = " << s->position << endl;
        Data* d = s->data->first;
        while (d) {
            /*
            if (!d->twoByte) {
                mojFajl << bitset<8>(d->value).to_string() << " | ";
            }
            else {
                mojFajl << bitset<16>(d->value).to_string().substr(0,8) << " | ";
                mojFajl << bitset<16>(d->value).to_string().substr(8, 8) << " | ";
            }
            */
            stringstream ss;
            ss << hex << d->value;
            string hexStr = ss.str();
            if (!d->twoByte) {
                if (hexStr.length() == 1) {
                    hexStr = "0" + hexStr;
                }
                mojFajl << hexStr << " | ";
            }
            else {
                if (hexStr.length() == 1) {
                    mojFajl << "00" << " | ";
                    mojFajl << "0" + hexStr << " | ";
                }

                else if (hexStr.length() == 2) {
                    mojFajl << "00" << " | ";
                    mojFajl << hexStr << " | ";
                }

                else if (hexStr.length() == 3) {
                    mojFajl << "0" + hexStr.substr(0,1) << " | ";
                    mojFajl << hexStr.substr(1,2) << " | ";
                }

                else if (hexStr.length() == 4) {
                    mojFajl << hexStr.substr(0, 2) << " | ";
                    mojFajl << hexStr.substr(2, 2) << " | ";
                }
            }
            
            d = d->next;
        }
        mojFajl << endl;
        s = s->next;
    }

    mojFajl << endl;

    Symbol* t = symbolTable->first;
    mojFajl << setw(10) << "Ime" << setw(10) << "Sekcija" << setw(10) << "Offset" << setw(10) << "g/l" << setw(10) << "Rbr" << setw(10) << "Size" << setw(10) << "Value" << endl;
    while (t) {
        mojFajl << setw(10) << t->name << setw(10) << t->section << setw(10) << t->offset << setw(10) << t->glob << setw(10) << t->rbr << setw(10) << t->size << setw(10)  << t->value <<endl;
        t = t->next;
    }

    mojFajl << endl;

    Relocation* r = relocationTable->first;
    mojFajl << setw(10) << "Sekcija" << setw(10) << "Offset" << setw(20) << "Tip" << setw(20) << "rbr" <<  setw(20) << "addend" << endl;
    while (r) {
        mojFajl << setw(10) << r->section << setw(10) << r->offset << setw(20) << r->type << setw(20) << r->rBr << setw(20) << r->addend << endl;
        r = r->next;
    }

    mojFajl << endl;

    mojFajl.close();
}



void Asembler::secondPass() {
    Line* currLine = lines->first;

    while (currLine) {
        WordList* words = currLine->wordList;
        secondPassProcess(words);
        currLine = currLine->next;
    }
}


void Asembler::secondPassProcess(WordList* words) {

    Word* curr = words->first;

    while (curr) {
        //Obrada labele
        string word = curr->word;
        if (word.back() == ':') {
            //pass
        }

        //Obrada direktiva  

        //Direktive
        else if (word.front() == '.') {

            //Global
            if (word == ".global") {
                curr = curr->next;

                while (curr) {
                    Symbol* temp = symbolTable->findName(curr->word);
                    if (!temp) {
                        cout << "Globalni simbol " << curr->word << " ne postoji";
                        exit(-1);
                    }
                    temp->changeGlob("glob");
                    curr = curr->next;
                }
            }


            //extern
            else if (word == ".extern") {

                curr = curr->next;

                while (curr) {
                    //TODO
                    curr = curr->next;
                }

            }



            //section
            else if (word == ".section") {
                sectionList->setCurrentSection(curr->next->word);
                return;
            }


            else if (word == ".word") {
                curr = curr->next;

                while (curr) {

                    
                    if (checkSymbol(curr->word)) {

                        Symbol* symbol = symbolTable->findName(curr->word);

                        if (!symbol) { //Ako simbol nije u tabeli dodajemo ga
                            cout << "Simbol " << curr->word << " nije definisan" << endl;
                            exit(-1);
                        }


                        string glob = symbol->glob;
                        if (glob == "glob") { //Ako je simbol globalan koristimo njegov rbr i upisujemo 0 u data a tip je global direct
                            int rbr = symbol->rbr;
                            relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position, "globalDirect2", to_string(rbr), 0);
                            sectionList->currentSection->data->addData(0); // Ovde ce tek biti upisan (dodat) simb
                            sectionList->currentSection->data->addData(0); // Ovde ce tek biti upisan (dodat) simb
                        }
                        else { //For local simbols we use their sections name for rbr
                            string section = symbol->section;
                            relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position, "localDirect2", section, 0);
                            int offset = symbol->offset;
                            sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb
                        }
                        
                    }
                    else if (checkLiteral(curr->word)) {
                        int value = stringToInt(curr->word);
                        sectionList->currentSection->data->addData(value, true);
                    }
                    else {
                        cout << curr->word << " mora biti simbol ili literal" << endl;
                        exit(-1);
                    }
                    sectionList->currentSection->position += 2;

                    curr = curr->next;
                }
            }


            //Skip
            else if (word == ".skip") {
                curr = curr->next;
                int n = stringToInt(curr->word);
                for (int i = 0; i < n; i++)
                    sectionList->currentSection->data->addData(0);
            }

            else if (word == ".equ") {
                return;
            }

            else if (word == ".end") {
                return;
            }




        }


        //Obrada naredbi    
        else {

            //Halt
            if (word == "halt") {
                sectionList->currentSection->data->addData(0); // Code 0000 0000
                sectionList->currentSection->position += 1;
            }


            //Int
            else if (word == "int") {
                //InstrDescr
                sectionList->currentSection->data->addData(16);

                //RegsDescr

                string registar = curr->next->word.erase(0, 1);

                registar = intToBin(registar, false);
                registar += "1111";
                int num = binToInt(registar);
                sectionList->currentSection->data->addData(num);

                sectionList->currentSection->position += 2;
                return;
            }


            //Iret
            else if (word == "iret") {
                sectionList->currentSection->data->addData(32);
                sectionList->currentSection->position += 1;
            }


            
            //Ret
            else if (word == "ret") {
                sectionList->currentSection->data->addData(64);
                sectionList->currentSection->position += 1;
            }

            //Jmp
            else if (word == "call" || word == "jmp" || word == "jeq" || word == "jne" || word == "jgt") {
                int instrDescr = 5;

                //OpCode
                instrDescr += word == "jeq" + 2 * (word == "jne") + 3 * (word == "jgt");

                if (word == "call") {
                    instrDescr = 48; // 0011 0000 = call
                }


                sectionList->currentSection->data->addData(instrDescr);

                //RegDescr



                curr = curr->next;
                string arg = curr->word;

                if (checkLiteral(arg)) { // <literal>
                    int regDescr = 240; // 1111 + 0000
                    int adrMode = 0; // Nema azuriranja i nacin adresiranja je neposredan(0)
                    sectionList->currentSection->data->addData(regDescr);
                    sectionList->currentSection->data->addData(adrMode);

                    //Payload
                    int payload = stringToInt(arg);
                    sectionList->currentSection->data->addData(payload, true); //Dodaje 2B
                    sectionList->currentSection->position += 5;
                    return;
                }

                else if (checkSymbol(arg)) { // <symbol>
                    int regDescr = 240; // 1111 + 0000
                    int adrMode = 0; //Nema azuriranja i nacin adresiranja je neposredno
                    sectionList->currentSection->data->addData(regDescr);
                    sectionList->currentSection->data->addData(adrMode);

                    //Payload
                    Symbol* symbol = symbolTable->findName(arg);
                    if (!symbol) { //Ako simbol jos nije def to je greska
                        cout << "Simbol " << arg << " nije definisan" << endl;
                        exit(-1);
                    }

                    //U svakom slucaju formiramo relokacioni zapis
                    string glob = symbol->glob;
                    if (glob == "glob") { //This is a global symbol so we use its rbr
                        int rbr = symbol->rbr;
                        relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position+3, "globalDirect2", to_string(rbr), 0);
                        sectionList->currentSection->data->addData(0, true); // Ovde ce tek biti upisan (dodat) simb 2B
                    }
                    else { //For local simbols we use their sections name for rbr
                        string section = symbol->section;
                        relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position+3, "localDirect2", section, 0);
                        int offset = symbol->offset; //Offset od pocetka instrukcije - na ovo dodajemo samo offset instrukcije                    }
                        sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb 2B
                    }
                    sectionList->currentSection->position += 5;
                    return;
                }

                else if (arg[0] == '%') { // %<simbol>
                    arg.erase(0, 1); //Deleting %
                    if (!checkSymbol(arg)) {
                        cout << "% mora da stoji uz simbol (bez razmaka)" << endl;
                        exit(-1);
                    }



                    //PCRel adresiranje = reg indirektno sa pomerajem gde je reg = r7

                    int regDescr = 7; //0000 0111 = r7
                    int addrMode = 3; //0000 0011 = regindpom

                    sectionList->currentSection->data->addData(regDescr);
                    sectionList->currentSection->data->addData(addrMode);

                    Symbol* symbol = symbolTable->findName(arg);
                    if (!symbol) { //Ako simbol jos nije def to je greska
                        cout << "Simbol " << arg << " nije definisan" << endl;
                        exit(-1);
                    }

                    //U svakom slucaju formiramo relokacioni zapis
                    string glob = symbol->glob;
                    if (glob == "glob") { //This is a global symbol so we use its rbr
                        int rbr = symbol->rbr;
                        relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "globalPCrel2", to_string(rbr), -5);
                        sectionList->currentSection->data->addData(0, true); // Ovde ce tek biti upisan (dodat) simb 2B
                    }
                    else { //For local simbols we use their sections name for rbr
                        string section = symbol->section;
                        relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "localPCrel2", section, -5);
                        int offset = symbol->offset; //Offset od pocetka instrukcije - na ovo dodajemo samo offset instrukcije                    }
                        sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb 2B
                    }
                    sectionList->currentSection->position += 5;
                    return;
                }

                else if (arg[0] == '*') {

                    arg.erase(0, 1); //Deleting '*'


                    if (checkLiteral(arg)) { // *<literal>

                        int regDescr = 240; // 1111 + 0000
                        int adrMode = 4; // Nema azuriranja i nacin adresiranja je memorijski(0)
                        sectionList->currentSection->data->addData(regDescr);
                        sectionList->currentSection->data->addData(adrMode);

                        //Payload
                        int payload = stringToInt(arg);
                        sectionList->currentSection->data->addData(payload, true); //Dodaje 2B
                        sectionList->currentSection->position += 5;
                        return;
                    }

                    else if (checkRegistar(arg)) { // *<reg>
                        arg.erase(0, 1); //Brisemo 'r'
                        int regDescr = 240; // 1111 0000
                        regDescr += stoi(arg); //Druga cetvroka oznacava registar
                        sectionList->currentSection->data->addData(regDescr);

                        int adrMode = 1; //regdir = 1, a nema azuriranja
                        sectionList->currentSection->data->addData(adrMode);

                        sectionList->currentSection->position += 3;
                        return;
                    }

                    else if (checkSymbol(arg)) { // *<symbol>

                        int regDescr = 240; // 1111 + 0000
                        int adrMode = 4; // Nema azuriranja i nacin adresiranja je memorijski(0)
                        sectionList->currentSection->data->addData(regDescr);
                        sectionList->currentSection->data->addData(adrMode);

                        Symbol* symbol = symbolTable->findName(arg);
                        if (!symbol) { //Ako simbol jos nije def to je greska
                            cout << "Simbol " << arg << " nije definisan" << endl;
                            exit(-1);
                        }

                        //U svakom slucaju formiramo relokacioni zapis
                        string glob = symbol->glob;
                        if (glob == "glob") { //This is a global symbol so we use its rbr
                            int rbr = symbol->rbr;
                            relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "globalDirect2", to_string(rbr), 0);
                            sectionList->currentSection->data->addData(0, true); // Ovde ce tek biti upisan (dodat) simb 2B
                        }
                        else { //For local simbols we use their sections name for rbr
                            string section = symbol->section;
                            relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "localDirect2", section, 0);
                            int offset = symbol->offset; //Offset od pocetka instrukcije - na ovo dodajemo samo offset instrukcije                    }
                            sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb 2B
                        }
                        sectionList->currentSection->position += 5;
                        return;
                    }

                    

                    if (arg[0] == '(') {
                        arg.erase(0, 1); //Deleting '('
                        arg.pop_back(); //Deleting ')'

                        if (checkRegistar(arg)) { // *[<reg>] 
                            arg.erase(0, 1);
                            int regDescr = 240;
                            regDescr += stoi(arg);
                            sectionList->currentSection->data->addData(regDescr);

                            int adrMode = 2; //regind = 2, a nema azuriranja
                            sectionList->currentSection->data->addData(adrMode);

                            sectionList->currentSection->position += 3;
                            return;
                        }

                        if (arg.length() >= 4) { // *[<reg> + <literal/symbol>]

                            if (checkRegistar(arg.substr(0, 2))) {
                                if (arg.substr(2, 1) == "+") {

                                    int regDescr = 240; // 1111 0000
                                    regDescr += stoi(arg.substr(1, 2)); // bez r
                                    sectionList->currentSection->data->addData(regDescr);

                                    int adrMode = 3; // regindpom = 3, bez azuriranja

                                    sectionList->currentSection->data->addData(adrMode);

                                    if (checkLiteral(arg.substr(3))) { // *[<reg> + <literal>]


                                        int literal = stringToInt(arg.substr(3));

                                        sectionList->currentSection->data->addData(literal, true);
                                        sectionList->currentSection->position += 5;
                                        return;
                                    }

                                    else if (checkSymbol(arg.substr(3))) { // *[<reg> + <symbol>]
                                        Symbol* symbol = symbolTable->findName(arg.substr(3));
                                        if (!symbol) { //Ako simbol jos nije def to je greska
                                            cout << "Simbol " << arg.substr(3) << " nije definisan" << endl;
                                            exit(-1);
                                        }

                                        //U svakom slucaju formiramo relokacioni zapis
                                        string glob = symbol->glob;
                                        if (glob == "glob") { //This is a global symbol so we use its rbr
                                            int rbr = symbol->rbr;
                                            relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "globalDirect2", to_string(rbr), 0);
                                            sectionList->currentSection->data->addData(0, true); // Ovde ce tek biti upisan (dodat) simb 2B
                                            sectionList->currentSection->position += 5;
                                        }
                                        else { //For local simbols we use their sections name for rbr
                                            string section = symbol->section;
                                            relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "localDirect2", section, 0);
                                            int offset = symbol->offset; //Offset od pocetka instrukcije - na ovo dodajemo samo offset instrukcije                    }
                                            sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb 2B
                                            sectionList->currentSection->position += 5;
                                        }
                                        return;
                                    }

                                }
                            }


                        }


                    }


                    cout << "Pogresno naveden argument instrukcije skoka" << endl;
                    exit(-1);


                }

            }



            else if (word == "push" || word == "pop") {

                string registar = curr->next->word.erase(0, 1);
                registar = intToBin(registar, false);
                registar += "0110"; // sp = r6

                int regDescr = binToInt(registar);

                if (word == "push") {
                    int instDescr = 176; // 1011 0000 = store
                    sectionList->currentSection->data->addData(instDescr);

                    sectionList->currentSection->data->addData(regDescr);

                    int addrMode = 18; // 0001 0010 -> regind sa predekrementom 2
                    sectionList->currentSection->data->addData(addrMode);
                }

                else { // word == pop
                    int instDescr = 160; // 1010 0000 = load
                    sectionList->currentSection->data->addData(instDescr);

                    sectionList->currentSection->data->addData(regDescr);

                    int addrMode = 66; // 0100 0010 -> regind sa postinkrementom 2
                    sectionList->currentSection->data->addData(addrMode);
                }


                sectionList->currentSection->position += 3;
                return;
            }
             
            else if (word == "xchg") {
                //InstDescr
                sectionList->currentSection->data->addData(96);
            
                //RegDescr
                string codeD = curr->next->word.erase(0, 1);
                codeD = intToBin(codeD, false);
                string codeS = curr->next->next->word.erase(0, 1);
                codeS = intToBin(codeS, false);
            
                int num = binToInt(codeD + codeS);
                sectionList->currentSection->data->addData(num);
                sectionList->currentSection->position += 2;
                return;
            }
            
            
            
            
            
            else if (word == "add" || word == "sub" || word == "mul" || word == "div" || word == "cmp") {
                //InstDescr
                int instrDescr = 112;

            
                //OpCode (add = 0 | sub = 1...)
                instrDescr += (word == "sub") + 2 * (word == "mul") + 3 * (word == "div") + 4 * (word == "cmp");
            
                sectionList->currentSection->data->addData(instrDescr);
            
                //RegDescr
                string codeD = curr->next->word.erase(0, 1);
                codeD = intToBin(codeD, false);
                string codeS = curr->next->next->word.erase(0, 1);
                codeS = intToBin(codeS, false);
            
                int num = binToInt(codeD + codeS);
                sectionList->currentSection->data->addData(num);
                sectionList->currentSection->position += 2;
                return;
            
            }
            
            else if (word == "not" || word == "and" || word == "or" || word == "xor" || word == "test") {
                //InstDescr
                int instrDescr = 128;
            
                //OpCode (not = 0 | and = 1...)
                instrDescr += (word == "and") + 2 * (word == "or") + 3 * (word == "xor") + 4 * (word == "test");
            
                sectionList->currentSection->data->addData(instrDescr);
            
                //RegDescr
                string codeD = curr->next->word.erase(0, 1);
                codeD = intToBin(codeD, false);
                string codeS = curr->next->next->word.erase(0, 1);
                codeS = intToBin(codeS, false);
            
                int num = binToInt(codeD + codeS);
                sectionList->currentSection->data->addData(num);
                sectionList->currentSection->position += 2;
                return;
            }
            
            else if (word == "shl" || word == "shr") {
                //InstDescr
                int instrDescr = 129;
            
                //OpCode (shl = 0 | shr = 1)
                instrDescr += (word == "shr");
            
                sectionList->currentSection->data->addData(instrDescr);
            
                //RegDescr
                string codeD = curr->next->word.erase(0, 1); //Brisemo r
                codeD = intToBin(codeD, false);
                string codeS = curr->next->next->word.erase(0, 1);
                codeS = intToBin(codeS, false);
            
                int num = binToInt(codeD + codeS);
                sectionList->currentSection->data->addData(num);
                sectionList->currentSection->position += 2;
                return;
            }
            
            else if (word == "ldr" or word == "str") {

                int instrDescr = 160 + 16 * (word == "str"); // 1010 000 za ldr i 1011 0000 za str

                sectionList->currentSection->data->addData(instrDescr);
                
                string regDesc = curr->next->word.erase(0,1);
                regDesc = intToBin(regDesc, false);

                

                string operand = curr->next->next->word;

                if (checkLiteral(operand)) {
                    regDesc += "0000";
                    sectionList->currentSection->data->addData(binToInt(regDesc));

                    int addrMode = 4; //Nema azuriranja + memorijsko

                    sectionList->currentSection->data->addData(addrMode);

                    int data = stringToInt(operand);

                    sectionList->currentSection->data->addData(data, true);

                    sectionList->currentSection->position += 5;
                    return;
                }

                if (checkRegistar(operand)) {
                    string dstReg = intToBin(operand.substr(1, 1), false);
                    regDesc += dstReg;

                    int regDescr = binToInt(regDesc);
                    sectionList->currentSection->data->addData(regDescr);

                    int addrMode = 1; //Regdir bez azuriranja
                    sectionList->currentSection->data->addData(addrMode);

                    sectionList->currentSection->position += 3;
                    return;

                }

                if (checkSymbol(operand)) {
                    regDesc += "0000"; // nema src
                    sectionList->currentSection->data->addData(binToInt(regDesc));

                    int addrMode = 4; //Nema azuriranja + memorijsko

                    sectionList->currentSection->data->addData(addrMode);

                    Symbol* symbol = symbolTable->findName(operand);
                    if (!symbol) { //Ako simbol jos nije def to je greska
                        cout << "Simbol " << operand << " nije definisan" << endl;
                        exit(-1);
                    }

                    //U svakom slucaju formiramo relokacioni zapis
                    string glob = symbol->glob;
                    if (glob == "glob") { //This is a global symbol so we use its rbr
                        int rbr = symbol->rbr;
                        relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "globalDirect2", to_string(rbr), 0);
                        sectionList->currentSection->data->addData(0, true); // Ovde ce tek biti upisan (dodat) simb 2B
                    }
                    else { //For local simbols we use their sections name for rbr
                        string section = symbol->section;
                        relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "localDirect2", section, 0);
                        int offset = symbol->offset; //Offset od pocetka instrukcije - na ovo dodajemo samo offset instrukcije                    }
                        sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb 2B
                    }
                    sectionList->currentSection->position += 5;
                    return;
                }

                
                if (operand.at(0) == '$') {
                    operand.erase(0, 1); // delete $
                    
                    if (checkLiteral(operand)) {
                        regDesc += "0000";
                        sectionList->currentSection->data->addData(binToInt(regDesc));

                        int addrMode = 0; //Nema azuriranja + neposredno

                        sectionList->currentSection->data->addData(addrMode);

                        int data = stringToInt(operand);

                        sectionList->currentSection->data->addData(data, true);

                        sectionList->currentSection->position += 5;
                        return;
                    }

                    if (checkSymbol(operand)) {
                        regDesc += "0000"; // nema src
                        sectionList->currentSection->data->addData(binToInt(regDesc));

                        int addrMode = 0; //Nema azuriranja + neposredno

                        sectionList->currentSection->data->addData(addrMode);

                        Symbol* symbol = symbolTable->findName(operand);
                        if (!symbol) { //Ako simbol jos nije def to je greska
                            cout << "Simbol " << operand << " nije definisan" << endl;
                            exit(-1);
                        }

                        //U svakom slucaju formiramo relokacioni zapis
                        string glob = symbol->glob;
                        if (glob == "glob") { //This is a global symbol so we use its rbr
                            int rbr = symbol->rbr;
                            relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "globalDirect2", to_string(rbr), 0);
                            sectionList->currentSection->data->addData(0, true); // Ovde ce tek biti upisan (dodat) simb 2B
                        }
                        else { //For local simbols we use their sections name for rbr
                            string section = symbol->section;
                            relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "localDirect2", section, 0);
                            int offset = symbol->offset; //Offset od pocetka instrukcije - na ovo dodajemo samo offset instrukcije                    }
                            sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb 2B
                        }
                        sectionList->currentSection->position += 5;
                        return;
                    }

                    else {
                        cout << "Argument naveden uz $ mora biti literal ili simbol" << endl;
                        exit(-1);
                    }
                    
                }

                if (operand.at(0) == '%') {

                    operand.erase(0, 1);

                    if (!checkSymbol(operand)) {
                        cout << "Operand uz % mora biti simbol " << endl;
                        exit(-1);
                    }

                    string dstReg = "0111"; // ip = r7
                    regDesc += dstReg;


                    sectionList->currentSection->data->addData(binToInt(regDesc));

                    int addrMode = 3; // 0000 0011 = regindpom bez azuriranja 
                    sectionList->currentSection->data->addData(addrMode);

                    Symbol* symbol = symbolTable->findName(operand);
                    if (!symbol) { //Ako simbol jos nije def to je greska
                        cout << "Simbol " << operand << " nije definisan" << endl;
                        exit(-1);
                    }

                    //U svakom slucaju formiramo relokacioni zapis
                    string glob = symbol->glob;
                    if (glob == "glob") { //This is a global symbol so we use its rbr
                        int rbr = symbol->rbr;
                        relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "globalPCrel2", to_string(rbr), -5);
                        sectionList->currentSection->data->addData(0, true); // Ovde ce tek biti upisan (dodat) simb 2B
                    }
                    else { //For local simbols we use their sections name for rbr
                        string section = symbol->section;
                        relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "localPCrel2", section, -5);
                        int offset = symbol->offset; //Offset od pocetka instrukcije - na ovo dodajemo samo offset instrukcije                    }
                        sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb 2B
                    }

                    sectionList->currentSection->position += 5;
                    return;
                }


                if (operand.at(0) == '(') { //(rx+nesto) ili (rx)

                    

                    if (checkRegistar(operand.substr(1, operand.length()-2))) { // sve osim '(' na pocetku i ')' na kraju
                        operand.pop_back();
                        operand.erase(0, 2);

                        string dstReg = intToBin(operand, false);
                        regDesc += dstReg;

                        int regDescr = binToInt(regDesc);
                        sectionList->currentSection->data->addData(regDescr);

                        int addrMode = 2; //Regind bez azuriranja
                        sectionList->currentSection->data->addData(addrMode);

                        sectionList->currentSection->position += 3;
                        return;
                    }

                    if (operand.length() < 2) {
                        cout << "Greska kod regindpom nacina adresiranja" << endl;
                        exit(-1);
                    }

                   
                    if (operand.at(3) == '+') {
                        if (!checkRegistar(operand.substr(1, 2))) {
                            cout << "Format regindpom = (rx+operand) " <<  endl;
                            exit(-1);
                        }

                        if (checkLiteral(operand.substr(4, operand.length() - 5))) {
                            regDesc += "0000";
                            sectionList->currentSection->data->addData(binToInt(regDesc));

                            int addrMode = 3; //Nema azuriranja + regindpom

                            sectionList->currentSection->data->addData(addrMode);

                            int data = stringToInt(operand.substr(3, operand.length() - 1));

                            sectionList->currentSection->data->addData(data, true);

                            sectionList->currentSection->position += 5;
                            return;
                        }

                        if (checkSymbol(operand.substr(4, operand.length() - 5))) {
                            regDesc += "0000"; // nema src
                            sectionList->currentSection->data->addData(binToInt(regDesc));

                            int addrMode = 3; //Nema azuriranja + regindpom

                            sectionList->currentSection->data->addData(addrMode);

                            Symbol* symbol = symbolTable->findName(operand.substr(4, operand.length() - 5));
                            if (!symbol) { //Ako simbol jos nije def to je greska
                                cout << "Simbol " << operand << " nije definisan" << endl;
                                exit(-1);
                            }

                            //U svakom slucaju formiramo relokacioni zapis
                            string glob = symbol->glob;
                            if (glob == "glob") { //This is a global symbol so we use its rbr
                                int rbr = symbol->rbr;
                                relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "globalDirect2", to_string(rbr), 0);
                                sectionList->currentSection->data->addData(0, true); // Ovde ce tek biti upisan (dodat) simb 2B
                            }
                            else { //For local simbols we use their sections name for rbr
                                string section = symbol->section;
                                relocationTable->addRelocation(sectionList->currentSection->name, sectionList->currentSection->position + 3, "localDirect2", section, 0);
                                int offset = symbol->offset; //Offset od pocetka instrukcije - na ovo dodajemo samo offset instrukcije                    }
                                sectionList->currentSection->data->addData(offset, true); // Ovde ce tek biti upisan (dodat) simb 2B
                            }
                            sectionList->currentSection->position += 5;
                            return;
                        }

                        cout << "Format regindpom = (rx+operand)" << endl;
                        exit(-1);

                    }

                    cout << "Format regindpom = (rx+operand)" << endl;
                    exit(-1);


                }
                return;
            }
            
                        
            else {
                cout << curr->word << " je greska";
                exit(-1);
            }
        }
        if (curr)
            curr = curr->next;
    }
           
       
}

