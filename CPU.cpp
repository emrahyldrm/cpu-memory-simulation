#ifndef SIMULATE_CPP
#define SIMULATE_CPP


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "Memory.cpp"
#include "Instruction.cpp"

#define PC_INDEX            0
#define SP_INDEX            1
#define REAL_MEM_SIZE       512
#define VIRTUAL_MEM_SIZE    32768
#define PAGE_SIZE           64
#define DISK_SIZE           1048576

using namespace std;

class TableEntry
{
public:
    TableEntry()
    {
        referenceBit = 0;
        preAbsBit = 0;
        modifiedBit = 0;
        indexOnHW = 0;
        pageNumber = 0;
        numOfData = 1;

    }

    void printEntry()
    {
        cout << "--------------------------------------------------"<< endl;
        cout << "Page Number        : " << this->pageNumber         << endl;
        cout << "Index On Hawrdware : " << this->indexOnHW          << endl;
        cout << "Present/Absent Bit : " << this->preAbsBit          << endl;
        cout << "Referenced Bit     : " << this->referenceBit       << endl;
        cout << "Modified Bit       : " << this->modifiedBit        << endl;
        cout << "--------------------------------------------------"<< endl;
    }


    bool referenceBit;
    bool preAbsBit;
    bool modifiedBit;
    int numOfData;
    int  indexOnHW;
    int  pageNumber;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPU
{
public:
    CPU(string &filename)
    { /**/ }


    CPU(string &filename, int md=-1)
    {
        parseFile(filename);
        mode=md;
        PC=0;
        halted = false;

        tickNumber = 0;
        cout.setf(std::ios::boolalpha);
    }


    ~CPU()
    {
        //
    }

    int getInstructionSize() { return instructions.size(); }

    bool isHalted() { return this->halted; }

    void parseFile(string &filename)
    {
        ifstream file;
        string temp;
        file.open(filename.c_str());

        if(!file.is_open()) {
            cout << "file is not opened" << endl;
            exit(1);
        }
        int i = 0;
        while(!file.eof())
        {
            getline(file, temp);

            if(temp.length() == 0) continue;

            int pos = temp.find('#');
            if(pos == 0) continue;

            temp = temp.substr(0, pos);
            temp.erase(temp.find_last_not_of("\n\r") + 1);

            //fill memory with first values
            int dataIndex = 0;
            if(temp.compare("Begin Data Section") == 0)
            {
                getline(file, temp);
                int pos = temp.find('#');
                temp = temp.substr(0, pos);

                temp.erase(temp.find_last_not_of("\n\r") + 1);
                do
                {
                    vector<string> res;
                    this->split(res, temp, ' ');



                    if(dataIndex % PAGE_SIZE == 0)
                    {
                        TableEntry entry;
                        if(memory.size() != REAL_MEM_SIZE)
                        {
                            entry.preAbsBit = true;
                            entry.indexOnHW = memory.size();
                            entry.pageNumber = findPageNumber(dataIndex);
                        }
                        else
                        {
                            entry.preAbsBit = false;
                            entry.indexOnHW = disk.size();
                            entry.pageNumber = findPageNumber(dataIndex);
                        }
                        pageTable.push_back(entry);
                    }
                    else
                        pageTable[pageTable.size() - 1].numOfData++;

                    dataIndex++;



                    if(memory.size() != REAL_MEM_SIZE)
                        memory.addIndex(atoi(res[1].c_str()));
                    else
                        disk.addIndex(atoi(res[1].c_str()));


                    getline(file, temp);
                    int pos = temp.find('#');
                    if(pos == 0) continue;
                    temp = temp.substr(0, pos);
                    temp.erase(temp.find_last_not_of("\n\r") + 1);
                }while(temp.compare("End Data Section") != 0);
            }

            while(memory.size() % PAGE_SIZE != 0)
            {
                memory.addIndex(-1);
            }
            while(disk.size() % PAGE_SIZE != 0)
            {
                disk.addIndex(-1);
            }

            if(temp.compare("Begin Instruction Section") == 0)
            {
                getline(file, temp);
                int pos = temp.find('#');
                if(pos == 0) continue;
                temp = temp.substr(0, pos);
                temp.erase(temp.find_last_not_of("\n\r") + 1);
                do
                {
                    vector<string> res;
                    this->split(res, temp, ' ');

                    if(res[1].compare("HLT") == 0)
                        instructions.push_back(Instruction(atoi(res[0].c_str()), res[1]));
                    else
                        instructions.push_back(Instruction(atoi(res[0].c_str()), res[1], atoi(res[2].c_str()), atoi(res[3].c_str())));

                    getline(file, temp);
                    int pos = temp.find('#');
                    temp = temp.substr(0, pos);
                    temp.erase(temp.find_last_not_of("\n\r") + 1);
                }while(temp.compare("End Instruction Section") != 0);
            }
        }

        file.close();
        /*
        for (int i = 0; i < instructions.size(); ++i) {
            cout << instructions[i].print();
        }
*/
    }


    void printMemory()
    {
       /*
        int index = 2059;
        if(pageFault(index)) handlePageFault(index);
        cout << index << " " << memory.getValueByIndex(index);
        */
#if 1
        int size = memory.size() + disk.size() - PAGE_SIZE + pageTable[pageTable.size()-1].numOfData;
        cout << "Memory Begin" << endl;
        for (int i = 0; i < size; ++i)
        {
            int index = i;
            if(pageFault(index))
                handlePageFault(index);

            cout<< "[" << i << ", "<<memory.getValueByIndex(index) << "]\t";
            if(i%10==0){
                cout<<"\n";
            }
        }
        cout << "Memory End" << endl;
        cout << "------------------------"
             << endl << endl;


        for (int i = 0; i < pageTable.size(); ++i) {
            pageTable[i].printEntry();
        }

        cout << "Page Table Size  : " << pageTable.size() << endl;
#endif
    }



    void split(vector<string> &output, string str, char  delimeter)
    {
        stringstream  ss;
        output.clear();

        if(str[str.length()-1] != delimeter) str.push_back(delimeter);
        for (int i = 0; i < str.length(); ++i)
        {
            if(str[i] != delimeter && i != str.length() - 1)
                ss << str[i];
            else if(ss.str().length() > 0)
            {
                output.push_back(ss.str());
                ss.str("");
            }
        }
    }


    bool pageFault(int &index)
    {

        int pageNumber = findPageNumber(index);
        int modulo = index % PAGE_SIZE;

        //suppose that the index is on memory
        int i = pageTable[pageNumber].indexOnHW + modulo;

        if( !pageTable[pageNumber].preAbsBit)
            return true;
        else {
            index = i;
            return false;
        }

    }


    //do page replacement and update the index
    //Second Chance FIFO
    void handlePageFault(int &index)
    {
       // cout << "handle : " << index << endl;
        int pageNumber = index / PAGE_SIZE;
        int modulo = index % PAGE_SIZE;
        bool handleFlag = false;

        //find a page that can be replaced
        for (int i = 0; i < pageTable.size(); ++i) {

            if(pageTable[i].preAbsBit && pageTable[i].referenceBit)
            {
                pageTable[i].referenceBit = false;
            }
            else if(pageTable[i].preAbsBit && !pageTable[i].referenceBit)
            {
                //cout << "first handle" << endl;
                //get value of pages start indexes
                int memStart = pageTable[i].indexOnHW;
                int diskStart = pageTable[pageNumber].indexOnHW;
                int temp = 0;


                //page replace between disk and memory
                for (int j = memStart, k = diskStart, x = 0; x < PAGE_SIZE; ++j, ++k, ++x) {
                        temp = memory.getValueByIndex(j);
                        memory.setValueByIndex(j, disk.getValueByIndex(k));
                        disk.setValueByIndex(k, temp);

                }
                //change start index number
                pageTable[i].indexOnHW = diskStart;
                pageTable[pageNumber].indexOnHW = memStart;

                //reverse preabs bits
                pageTable[i].preAbsBit = false;
                pageTable[pageNumber].preAbsBit = true;

                //update index, convert virtual to real

                index = memStart + modulo;
                handleFlag = true;


                if(this->mode == 3)
                {
                    cout << "Replacement Between Page " << pageNumber << " and Page " << i << endl;
                    for (int j = 0; j < pageTable.size(); ++j) {
                        pageTable[j].printEntry();
                    }
                    cout << endl << "###########################################################################" << endl;
                }

                break;
            }

        }
        int pageIndex = -1;
        for (int l = 1; l <pageTable.size(); ++l) {
            if(pageTable[l].preAbsBit == true)
            {
                pageIndex = l;
                break;
            }
        }
        if(!handleFlag)
        {
            //cout << "second handle" << endl;

            int memStart = pageTable[pageIndex].indexOnHW;
            int diskStart = pageTable[pageNumber].indexOnHW;
            int temp = 0;


            //page replace between disk and memory
            for (int j = memStart, k = diskStart, x = 0; x < PAGE_SIZE; ++j, ++k, ++x) {
                temp = memory.getValueByIndex(j);
                memory.setValueByIndex(j, disk.getValueByIndex(k));
                disk.setValueByIndex(k, temp);

            }
            //change start index number
            pageTable[pageIndex].indexOnHW = diskStart;
            pageTable[pageNumber].indexOnHW = memStart;

            //reverse preabs bits
            pageTable[pageIndex].preAbsBit = false;
            pageTable[pageNumber].preAbsBit = true;

            //update index, convert virtual to real

            index = memStart + modulo;
        }


    }


    void tick()
    {
        tickNumber++;
        int pcindex = PC_INDEX;
        if(pageFault(pcindex))
            handlePageFault(pcindex);



        this->PC = memory.getValueByIndex(pcindex);
        pageTable[findPageNumber(PC_INDEX)].referenceBit = true;

        Instruction currentIns = instructions[this->PC];

       // cout << currentIns.print();

        //if want to set PC, should give target instruction (index - 1)
        if(currentIns.getCommand().compare("SET") == 0)
        {
            int A = currentIns.getB();
            int B = currentIns.getA();
            int oldA = A;
            //set content of adress A with value B
            if(pageFault(A))
                handlePageFault(A);

            memory.setValueByIndex(A, B);

            pageTable[findPageNumber(oldA)].modifiedBit = true;

            if(oldA == PC_INDEX)
                return;

        }
        else if(currentIns.getCommand().compare("CPY") == 0)
        {
            int A1 = currentIns.getA();
            int A2 = currentIns.getB();
            int a1back = A1;
            int a2back = A2;

            if(pageFault(A1)) handlePageFault(A1);

            int A1Value =  memory.getValueByIndex(A1);
            pageTable[findPageNumber(a1back)].referenceBit = true;

            if(pageFault(A2)) handlePageFault(A2);
            //copy content of A1 to content of A2
            memory.setValueByIndex(A2, A1Value);
            pageTable[findPageNumber(a2back)].modifiedBit = true;
        }
        else if(currentIns.getCommand().compare("CPYI") == 0)
        {
           // cout << "cpyi" << endl;
            int A1 = currentIns.getA();
            int A2 = currentIns.getB();
            int a1back = A1;
            int a2back = A2;

            if(pageFault(A1)) handlePageFault(A1);

            int adress = memory.getValueByIndex(A1);
            int adressBack = adress;
            pageTable[findPageNumber(a1back)].referenceBit = true;

            if(pageFault(adress)) handlePageFault(adress);


            int adressContent = memory.getValueByIndex(adress);

            pageTable[findPageNumber(adressBack)].referenceBit = true;

            if(pageFault(A2)) handlePageFault(A2);
            //copy content of adress(content of A1) to content of A2
            memory.setValueByIndex(A2,adressContent);
            pageTable[findPageNumber(a2back)].modifiedBit = true;
        }
        else if(currentIns.getCommand().compare("CPYI2") == 0)
        {
            int A1 = currentIns.getA();
            int A2 = currentIns.getB();
            int a1back = A1;
            int a2back = A2;

            if(pageFault(A2)) handlePageFault(A2);
            int adress = memory.getValueByIndex(A2);
            pageTable[findPageNumber(a2back)].referenceBit = true;
            int adressBack = adress;

            if(pageFault(adress)) handlePageFault(adress);
            if(pageFault(A1)) handlePageFault(A1);

            memory.setValueByIndex(adress, memory.getValueByIndex(A1));
            pageTable[findPageNumber(a1back)].referenceBit = true;
            pageTable[findPageNumber(adressBack)].modifiedBit = true;
        }
        else if(currentIns.getCommand().compare("ADD") == 0)
        {
            int A = currentIns.getB();
            int B = currentIns.getA();
            int aback = A;
            int bback = B;

            if(pageFault(A)) handlePageFault(A);
            int AContent = memory.getValueByIndex(A);
            pageTable[findPageNumber(aback)].referenceBit = true;

            //write ((content of A) + B) to location A
            memory.setValueByIndex(A, AContent + B);
            pageTable[findPageNumber(aback)].modifiedBit = true;
        }
        else if(currentIns.getCommand().compare("ADDI") == 0)
        {
            int A1 = currentIns.getA();
            int A2 = currentIns.getB();
            int a1back = A1;
            int a2back = A2;

            if(pageFault(A2)) handlePageFault(A2);
            int A2Content = memory.getValueByIndex(A2);
            pageTable[findPageNumber(a2back)].referenceBit = true;

            if(pageFault(A1)) handlePageFault(A1);

            //add content of A1 to content of A2 and write to A2
            memory.setValueByIndex(A2, A2Content + memory.getValueByIndex(A1));
            pageTable[findPageNumber(a1back)].referenceBit = true;
            pageTable[findPageNumber(a2back)].modifiedBit = true;
        }
        else if(currentIns.getCommand().compare("SUBI") == 0)
        {
            int A1 = currentIns.getA();
            int A2 = currentIns.getB();
            int a1back = A1;
            int a2back = A2;

            if(pageFault(A1)) handlePageFault(A1);
            int A1Content = memory.getValueByIndex(A1);
            pageTable[findPageNumber(a1back)].referenceBit = true;

            if(pageFault(A2)) handlePageFault(A2);
            int A2Content = memory.getValueByIndex(A2);
            pageTable[findPageNumber(a2back)].referenceBit = true;

            //subtract content of A2 from content of A1 and write to A2
            memory.setValueByIndex(A2, A1Content - A2Content);
            pageTable[findPageNumber(a2back)].modifiedBit = true;
        }
        else if(currentIns.getCommand().compare("JIF") == 0)
        {
            int A = currentIns.getA();
            int C = currentIns.getB();
            int aback = A;

            if(pageFault(A)) handlePageFault(A);
            int AContent = memory.getValueByIndex(A);
            pageTable[findPageNumber(aback)].referenceBit = true;

            //set PC with C if content of A is less or equal to zero
            if(AContent <= 0)
            {
                //this->PC = C;
                int pcindex = PC_INDEX;
                if(pageFault(pcindex))
                    handlePageFault(pcindex);
                memory.setValueByIndex(pcindex, C);
                pageTable[findPageNumber(PC_INDEX)].modifiedBit = true;
                return;
            }
        }
        else if(currentIns.getCommand().compare("HLT") == 0)
        {
            this->halted = true;
            cout << "Halt instruction has came, program halted" << endl;
        }

        //increment the program counter
        //this->PC++;
        pcindex = PC_INDEX;
        if(pageFault(pcindex))
            handlePageFault(pcindex);

        memory.setValueByIndex(pcindex, memory.getValueByIndex(pcindex) + 1);
        pageTable[findPageNumber(PC_INDEX)].referenceBit = true;
        pageTable[findPageNumber(PC_INDEX)].modifiedBit = true;
    }



    int findPageNumber(int index) {
        return index / PAGE_SIZE;
    }

    //run simulation
    void cpuRun()
    {

        cout << "Simulation is in Debug Mode " << mode
             << endl << endl;

        //cout << "Memory States at the Beginning" << endl;
        //printMemory();
        //debug mode 0: run until end of the program and display memory content
        if(this->mode == 0)
        {
            while( !this->isHalted() )
                this->tick();

            this->printMemory();
        }
        else if(this->mode  == 1) //debıg mode:1 one step ->print-> one step -> run..
        {
            while( !this->isHalted() )
            {
                this->tick();
                this->printMemory();
            }

        }
        else if(this->mode == 2) // debug mode:2 keyboard input -> one step -> print ..
        {
            while( !this->isHalted() )
            {
                cout <<  "Press Enter for Next CPU Tick !!" << endl;
                cin.get();
                this->tick();
                cout << instructions[PC].print();
                this->printMemory();

                for (int i = 0; i < pageTable.size(); ++i) {
                    pageTable[i].printEntry();
                }
            }
        }
        else if(this->mode == 3)
        {
            while( !this->isHalted() )
            {
                this->tick();
            }
        }
    }

private:
    vector<Instruction> instructions;
    vector<TableEntry> pageTable;
    Memory memory;
    Memory disk;


    int tickNumber;
    int mode;
    int PC;
    bool halted;
};

#endif

