// TK1_Code.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "motorLib.h"
#include "testClass.h"
#include <string>

using namespace std;
int main()
{
	cout << "entering main" << endl;
	string x;
	// specVolts, freeSpeed, freeCurrent, StallTorque, stallCurrent 
	motor testCim("testCim", 12.0, 556.0, 2.7, 2.425, 133);

	testCim.SetTransSpecs(0.65, 0, 0, 12.5);
	testCim.CalcState_SpeedVolts(100, 10.4, 1000);
	testCim.printData();
	cin >> x;
	return 0;
}

