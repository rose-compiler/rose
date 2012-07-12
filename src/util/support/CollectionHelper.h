#include <iostream>
#include <map>
#include <list>
#include <set>

#include "FileHelper.h"


using namespace std;

class CollectionHelper {
public:

    static void printSet(const set<string>& setToPrint, const string& setString, const string& setElementString) {
        cout << setString << endl;
        for (set<string>::const_iterator setElementPtr = setToPrint.begin(); setElementPtr != setToPrint.end(); setElementPtr++) {
            cout << setElementString << *setElementPtr << endl;
        }
    }

    static void printSet(const set<PreprocessingInfo*>& setToPrint, const string& setString, const string& setElementString) {
        cout << setString << endl;
        for (set<PreprocessingInfo*>::const_iterator setElementPtr = setToPrint.begin(); setElementPtr != setToPrint.end(); setElementPtr++) {
            cout << setElementString << (*setElementPtr) -> getString() << endl;
        }
    }

    static void printList(const list<string>& listToPrint, const string& listString, const string& listElementString) {
        cout << listString << endl;
        for (list<string>::const_iterator listElementPtr = listToPrint.begin(); listElementPtr != listToPrint.end(); listElementPtr++) {
            cout << listElementString << *listElementPtr << endl;
        }
    }

    static void printMapOfLists(const map<string, list<string> >& mapToPrint, const string& mapString, const string& mapKeyString, const string& listElementString) {
        cout << mapString << endl;
        for (map<string, list<string> >::const_iterator mapIterator = mapToPrint.begin(); mapIterator != mapToPrint.end(); mapIterator++) {
            cout << endl << mapKeyString << mapIterator -> first << endl;
            list<string> mappedList = mapIterator -> second;
            for (list<string>::const_iterator listElementPtr = mappedList.begin(); listElementPtr != mappedList.end(); listElementPtr++) {
                cout << listElementString << *listElementPtr << endl;
            }
        }
        cout << endl;
    }

    static void printMapOfSets(const map<string, set<PreprocessingInfo*> >& mapToPrint, const string& mapString, const string& mapKeyString, const string& setElementString) {
        cout << mapString << endl;
        for (map<string, set<PreprocessingInfo*> >::const_iterator mapIterator = mapToPrint.begin(); mapIterator != mapToPrint.end(); mapIterator++) {
            cout << endl << mapKeyString << mapIterator -> first << endl;
            set<PreprocessingInfo*> mappedSet = mapIterator -> second;
            for (set<PreprocessingInfo*>::const_iterator setElementPtr = mappedSet.begin(); setElementPtr != mappedSet.end(); setElementPtr++) {
                cout << setElementString << FileHelper::normalizePath((*setElementPtr) -> get_file_info() -> get_filenameString()) << endl;
            }
        }
        cout << endl;
    }

    static void printMapOfSets(const map<string, set<string> >& mapToPrint, const string& mapString, const string& mapKeyString, const string& setElementString) {
        cout << mapString << endl;
        for (map<string, set<string> >::const_iterator mapIterator = mapToPrint.begin(); mapIterator != mapToPrint.end(); mapIterator++) {
            cout << endl << mapKeyString << mapIterator -> first << endl;
            set<string> mappedSet = mapIterator -> second;
            for (set<string>::const_iterator setElementPtr = mappedSet.begin(); setElementPtr != mappedSet.end(); setElementPtr++) {
                cout << setElementString << *setElementPtr << endl;
            }
        }
        cout << endl;
    }

};
