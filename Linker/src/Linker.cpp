#include "Linker.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

Linker::Linker(WordList* fl, string ofl, string mode, PlaceCommands* pc) {
    fileList = fl;
    sectionList = new SectionList();
    globalListOfSymbols = new SymbolTable(true); // true znaci da se dodeljuje automatski rbr
    externSymbols = new SymbolTable();
    imenaSekcija = new WordList();
    globalSectionList = new SectionList();
    placeCommands = pc;
    outputFileName = ofl;
    mod = mode;
}

Linker::~Linker() {
    delete fileList;
    delete globalListOfSymbols;
    delete externSymbols;
    delete globalSectionList;
}

void Linker::obrada() {
    Word* temp = fileList->first;

    int indexFajla = 0;


    while (temp) {
        ifstream myFile(temp->name);

        if (!myFile.is_open()) {
            cout << "Fajl sa imenom " << temp->name << " ne postoji." << endl;
            exit(-1);
        }
        string line;


        //Sekcije
        while (getline(myFile, line)) {
            cout << line << endl;

            if (line == "") {
                break;
            }

            if (line.at(0) == 'S') { // Ovo je sekcija
                WordList words = WordList(line);
                sectionList->addSection(words.getWordByIndex(1), 0, stoi(words.getWordByIndex(3)), indexFajla);
                if (!imenaSekcija->contains(words.getWordByIndex(1))) {
                    imenaSekcija->addWord(words.getWordByIndex(1));
                }
            }

            else {

                WordList words = WordList(line);
                Word* temp2 = words.first;
                while (temp2) {

                    //TODO Ovde ubaciti stvarnu vrednost toga
                    if(temp2->name != "|")
                        sectionList->currentSection->data->addData(temp2->name);
                    temp2 = temp2->next;
                }

            }
        }

        
        //Simboli
        while (getline(myFile, line)) {
            cout << line << endl;

            if (line == "")
                break;

            WordList words = WordList(line);
            
            if (words.getWordByIndex(1) == "extern") {
                
                externSymbols->addSymbol(words.getWordByIndex(0), "extern", 0, "glob", stoi(words.getWordByIndex(4)),
                    0, stoi(words.getWordByIndex(6)), indexFajla); 
            }
            else if (words.getWordByIndex(1) == "ABS") {
                Symbol s = Symbol(words.getWordByIndex(0), "ABS", 0, "loc", 0, 0, stoi(words.getWordByIndex(6)), indexFajla);
                globalListOfSymbols->addSymbol(&s);
            }
            else {
                SymbolTable* st = sectionList->getSection(words.getWordByIndex(1), indexFajla)->symbolTable;
                st->addSymbol(words.getWordByIndex(0), words.getWordByIndex(1), stol(words.getWordByIndex(2)),
                    words.getWordByIndex(3), stoi(words.getWordByIndex(4)), stoi(words.getWordByIndex(5)), stoi(words.getWordByIndex(6)), indexFajla);
            }
            
        }

        //Relokacije
        while (getline(myFile, line)) {
            cout << line << endl;

            if (line == "")
                break;

            WordList words = WordList(line);
            RelocationTable* rt = sectionList->getSection(words.getWordByIndex(0), indexFajla)->relocationTable;
            rt->addRelocation(words.getWordByIndex(0), stoi(words.getWordByIndex(1)), words.getWordByIndex(2), 
                words.getWordByIndex(3), stoi(words.getWordByIndex(4)));
        }

        

        myFile.close();
        indexFajla++;
        temp = temp->next;
    }

    cout << endl << endl << endl;
    cout << "Sekcije na pocetku" << endl;
    sectionList->printSectionList();
    cout << endl << endl << endl;

    


    //Formiranje globalne liste simbola i azuriranje njihovih rednih brojeva u tabelama relokacija
    Section* tempSect = sectionList->first;
    while (tempSect) {
        Symbol* tempSimb = tempSect->symbolTable->first;

        while (tempSimb) {
            int noviRbr = globalListOfSymbols->addSymbol(tempSimb);
            
            //cout << endl << "Obradjuje se simbol " << tempSimb->name << " Ciji je stari rbr = " << tempSimb->rbr << " a novi = " << noviRbr << endl;
            
            Section* tempSect2 = sectionList->first;
            while (tempSect2) { //Prodji kroz sve sekcije
                if (tempSect2->indexFajla == tempSimb->indexFajla) //Ako je to sekcija iz fajla kao i simbol
                    tempSect2->promeniRedniBrojSimbolaRelokacije(to_string(tempSimb->rbr), to_string(noviRbr)); //Azuriraj sve relokacije s rbr tog simbola

                 tempSect2 = tempSect2->next;
            }

            tempSimb = tempSimb->next;
        }

        tempSect = tempSect->next;
    }


    //Obrada externih simbola
    Symbol* tempExtSimb = externSymbols->first;
    while (tempExtSimb) {

        Symbol* globalnaDefinicija = globalListOfSymbols->findGlobalName(tempExtSimb->name); //Trazimo taj simbol globalno definisan

        if (!globalnaDefinicija) { //Ako dati simbol nije definisan
            if (mod == "hex") { // To je greska za hex mod
                cout << "simbol " << tempExtSimb->name << " nije definisan" << endl;
                exit(-1);
            }

            else if (mod == "linkable") {
                //Ovo nije greska za hex mode, ovaj externi simbol dodaje se u tabelu simbola
                globalListOfSymbols->addSymbol(tempExtSimb);
            }
        }

        else {
            int noviRbr = globalnaDefinicija->rbr;
            Section* tempSect2 = sectionList->first;
            while (tempSect2) { //Prodji kroz sve sekcije
                if (tempSect2->indexFajla == tempExtSimb->indexFajla) //Ako je to sekcija iz fajla kao i externi simbol
                    tempSect2->promeniRedniBrojSimbolaRelokacije(to_string(tempExtSimb->rbr), to_string(noviRbr)); //Azuriraj sve relokacije s rbr tog simbola

                tempSect2 = tempSect2->next;
            }
        }

        tempExtSimb = tempExtSimb->next;
    }


    //Spajanje sekcija
    Word* tempImeSekc = imenaSekcija->first;

    while (tempImeSekc) {
        cout << "temp ime sek " << endl;
        Section* tempSek = sectionList->first;
        bool pocetnaSekcija = true;
        int size = 0;
        Section* proslaSekcija = nullptr;

        while (tempSek) {
            cout << "temp sek " << endl;
            if (tempSek->name == tempImeSekc->name) { //Ako su to istoimene sekcije

                if (pocetnaSekcija) { //Ako je ovo prva sekcija tog imena
                    cout << "dodajem pocetnu sekc " << endl;
                    globalSectionList->addSection(tempSek);
                    size += tempSek->size;
                    pocetnaSekcija = false;
                    proslaSekcija = tempSek;
                }

                else {
                    //Prepravi datu sekciju tako sto uvecas offsete globalnih simbola i offsete relokacija

                    //Update: za lokalne simbole u relokacijama moramo da povecamo offset od tabele simbola u 'data' 
                    //jer je pocetak sekcije sada udaljen unazad za size
                    //Ovo radimo tako sto im povecamo addend za size

                    tempSek->prepraviRelokacije(size);
                    globalListOfSymbols->prepraviSimbole(tempSek->name, tempSek->indexFajla ,size, 0);
                    proslaSekcija->nadoveziSekciju(tempSek);

                    //Postavi njenu pocetnu adresu (trebace kod relokacija)
                    tempSek->baseAddress = size;

                    size += tempSek->size;
                }


            }

            tempSek = tempSek->next;
        }
        proslaSekcija = nullptr;
        tempImeSekc = tempImeSekc->next;
    }

    globalSectionList->last->next = nullptr;

    


    //Linkeable: samo ispisemo u fajl globalnu listu sekcija, globalnu tabelu simbola i sve relokacije
    if (mod == "linkable") {
        ofstream myFile(outputFileName);

        if (!myFile) {
            cout << "Greska sa otvaranjem fajla za upis" << endl;
            exit(-1);
        }


        Section* tempOutSect = globalSectionList->first;

        while (tempOutSect) {
            myFile << "Sekcija: " << tempOutSect->name << " " << temp - tempOutSect->baseAddress << " " <<
                tempOutSect->size << endl;

            Data* dd = tempOutSect->data->first;

            while (dd) {
                myFile << dd->value << " | ";

                dd = dd->next;
            }
            myFile << endl;
            tempOutSect = tempOutSect->next;
        }

        myFile << endl;

        Symbol* ss = globalListOfSymbols->first;
        while (ss) {
            myFile << ss->name << " " << ss->section << " " << ss->offset << " " << ss->glob << " " << ss->rbr << " "
                << ss->size << " " << ss->value << endl;

            ss = ss->next;
        }

        myFile << endl;

        Section* sss = globalSectionList->first;

        while (sss) {
            Relocation* rr = sss->relocationTable->first;

            while (rr) {
                myFile << rr->section << " " << rr->offset << " " << rr->type << " " << rr->rBr << " " << rr->addend << endl;

                rr = rr->next;
            }

            sss = sss->next;
        }

        myFile << endl;

        myFile.close();


    }

    cout << endl << endl << endl;
    cout << "svi neexterni simboli: " << endl;
    globalListOfSymbols->printSymbolTable();

    cout << endl << endl << endl;
    cout << endl << endl << endl;
    cout << "Glob lista sekcija " << endl;
    globalSectionList->printSectionList();

    if (mod == "hex") {
        PlaceCommand* pc = placeCommands->first;

        int pomeraj = stoi(pc->position);

        while (pc) {

            if (!imenaSekcija->contains(pc->section)) { // Zanemari ako nemas datu sekciju
                continue;
            }

            Section* sec = globalSectionList->getSection(pc->section);
            sec->baseAddress += pomeraj; //Azuriramo sekcije 

            //Relokacije se ne azuriraju jer sadrze offset od pocetka sekcija a to se ne menja
            //sec->prepraviRelokacije(pomeraj); //Azuriramo relokacije

            globalListOfSymbols->prepraviGlobalneSimbole(sec->name, pomeraj); // Azuriramo globalne simbole

            pc = pc->next;
        }

        //Izvrsavanje prepravki

        Section* se = globalSectionList->first;
        while (se) {
            Relocation* re = se->relocationTable->first;

            while (re) {

                //Za globalne uzimamo adresu glob simbola
                //Za loc uzimamo adresu sekcije
                //sabiramo to, ono sto vec pise u data i addend 
                //Ako je relokacija pc relativna oduzimamo tekucu lokaciju 
                //Ako relokacija radi na 2B onda prvo uzmemo vrednost sa oba 2B a na kraju je i upisemo na oba 2B

                int baseAddress = 0;

                if (re->type.substr(0, 6) == "global") {
                    Symbol* symm = globalListOfSymbols->findSymbolByRbr(stoi(re->rBr));
                    baseAddress = symm->offset;
                }

                else if (re->type.substr(0, 5) == "local") {
                    Section* secc = globalSectionList->getSection(re->rBr); // U rbr se nalazi ime sekcije za lok simb
                    baseAddress = secc->baseAddress;
                }

                //U svakom slucaju dodajemo addend (onde gde nam ne treba je svakako 0)
                baseAddress += re->addend;

                cout << "sprovodim " << re->section << " " << re->offset << " " << re->type << " " << re->type << " " << re->addend << endl;

                if (re->type == "globalPCrel2" || re->type == "localPCrel2") {
                    //Oduzima se tekuca vrednost pc a to je (adresa sekcije gde je relokacija + offset od te sekcije - 3
                    // - 3 ide jer smo dodali 3 jer je payload na cetvrtom i petom bajtu
                    se->data->updateData(re->offset, true, true, baseAddress, se->baseAddress + re->offset - 3);
                }

                else if (re->type == "globalDirect2" || re->type == "localDirect2") {
                    se->data->updateData(re->offset, false, true, baseAddress, 0);
                }

                else if (re->type == "globalDirect" || re->type == "localDirect") {
                    se->data->updateData(re->offset, false, false, baseAddress, 0);
                }



                re = re->next;
            }


            se = se->next;
        }

    }



    cout << endl << endl << endl;
    cout << "svi neexterni simboli: " << endl;
    globalListOfSymbols->printSymbolTable();

    cout << endl << endl << endl;
    cout << endl << endl << endl;
    cout << "Glob lista sekcija " << endl;
    globalSectionList->printSectionList();
}
