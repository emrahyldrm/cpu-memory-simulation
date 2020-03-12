#include <iostream>
#include "CPU.cpp"

using namespace std;

#define FILE_NAME_INDEX 1
#define MODE_INDEX      2

int main(int argc, char *argv[])
{
    if(argc < 3 )
    {
        cout << "Too few arguments" << endl;
        exit(-1);
    }
    string filename(argv[FILE_NAME_INDEX]);
    CPU simulator(filename, atoi(argv[MODE_INDEX]));

    simulator.cpuRun();
    //simulator.printMemory();
    return 0;
}
