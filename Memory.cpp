#ifndef MEMORY_CPP
#define MEMORY_CPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Memory
{
public:
	Memory(){ }


	//initialize object with spesific memory capacity
	Memory(int memorySize)
	{
		values.resize(memorySize);
	}


	//destructor
	~Memory() { /**/ }


	//increment memory capacity with value
	void addIndex(int value)
	{
		values.push_back(value);
	}


	//get value after checking index boundary
	int getValueByIndex(int index) const
	{
		if(values.size() > index)
			return values[index];
		else {
			cout << "GET - Memory Index Out Of Bound index :" << index << endl;
		}//exception is more convenient here
	}


	//set value after checking index boundary
	void setValueByIndex(int index, int value)
	{
		if(values.size() > index)
			values[index] = value;
		else
			cout << "SET - Memory Index Out Of Bound index : " << index << endl;
	}


	//get current memory capacity
	int size() const { return values.size(); }


private:
	vector<int> values;     //memory vector

};


#endif