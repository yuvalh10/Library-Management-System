#ifndef _READFROMFILE_H
#define _READFROMFILE_H

#include "Includes.h"

template <class T, template <class Key, class Val, class... Args> class Container = unordered_map>
Container<string, T> ReadFromFile(const string& name, const string& path) // reads all the data from all the files
{
    Container<string, T> DS;
    string filename = path + name;
    ifstream file(filename);

    if (!file) {
        // if the file doesn't exist, create it.
        ofstream file(filename);
        file.close();
    }
    else {
        // if the file exists, read its content
        try {
            string line;
            if (getline(file, line)) {} // discard the first (header) line
            while (getline(file, line)) 
            {
                if (line == ",,") { break; } // no more data to read
                try 
                {
                    T object = T::fromCSV(line, path);
                    DS.emplace(object.GetID(), object); // store object in container
                }
                catch (const exception& e) {
                    cerr << "Error parsing line: " << line << " - " << e.what() << endl;
                }
            }
            file.close();
        }
        catch (const exception& e) {
            cerr << "Error opening file: " << filename << " - " << e.what() << endl;
        }
    }
    return DS;
}

#endif // _READFROMFILE_H