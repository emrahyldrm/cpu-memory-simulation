#ifndef INSTRUCTION_CPP
#define INSTRUCTION_CPP

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sstream>

using namespace std;


class Instruction
{
public:
    Instruction(int i, string &s, int a=-1, int b=-1)
    {
        index=i;
        command=s;
        A=a;
        B=b;
    }


    //destructor
    ~Instruction() { /**/ }


    //get instruction command part
    string getCommand()	{ return command; }


    //get instruction index
    int getIndex() const { return index; }


    //get instruction value A
    int getA() const { return A; }


    //get instruction value B
    int getB() const { return B; }


    string print()
    {
        stringstream ss;
        ss << this->index << " - " << this->command << " " << this->A << " " << this->B << endl;
        return ss.str();
    }

private:
    string command;		//like "ADD" ,"SET" ...
    int index;
    int A;
    int B;

};

/*
ostream& operator<<(ostream& out, const Instruction& ins)
{
    out << ins.index << " - " << ins.command << " " << ins.A << " " << ins.B << endl;
    return out;
}
 */
#endif