#include "Library_App.h"

void Braude_Library();

int main() 
{
    Braude_Library();
    cout << endl << "Leaks: " << _CrtDumpMemoryLeaks() << endl;
    return 0;
}

void Braude_Library()
{
    srand(time(0));
    Library_App app;
    /* default Ctor doing serializaion, Dtor doing deserialization
       for different data path, insert the path as argument in the Library_App object*/
}