#include "stdafx.h"
#include <iostream>

int val = 1;
int val2 = 2;
int* valPtr = &val;
int& valRef = val;


void printVals() {
	std::cout << "Values:" std::endl;
	std::cout << "val: " << val << " val2: " << val2 << " valPtr: " << *valPtr << " valRef: " << valRef << std::endl;
}

int main()
{
	printVals();
    return 0;
}