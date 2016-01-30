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
	motor test;
	float fReturn = test.GetStateEfficiencyTotal();
	cout << fReturn;
	cout << test.GetStateEfficiencyTotal();

	// test.currState.Amps;

	cin >> x;
	return 0;
}

