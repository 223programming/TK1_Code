#include "stdafx.h"
#include "motorLib.h"
#include <iostream>
#include <string>

using namespace std;

motor::motor(string name, float specVolts, float freeSpeed, float freeAmps, float stallTorque, float stallAmps)
{
	motName = name;
	cout << "Motor object created" << endl << endl;
	SetMotorSpecs(specVolts, freeSpeed, freeAmps, stallTorque, stallAmps);

	currState = { 0, 0, 0, 0, 0, 0, 0 };
	baseState = currState;
}

void motor::printData()
{
	cout << "The Transmission data for motor " << motName << " is:" << endl;
	cout << "Efficiency: " << BaseTransSpecs.efficiency << endl;
	cout << "Gear Reduction: " << BaseTransSpecs.gearReduction << endl;
	cout << "Static loss torque: " << BaseTransSpecs.lossTorque << endl;
	cout << "Moment of inertia: " << BaseTransSpecs.momentOfInertia << endl << endl << endl;

	cout << "The Base motor specs for motor " << motName << " are:" << endl;
	cout << "Free Current: " << BaseMotorSpecs.freeAmps << endl;
	cout << "Free Speed: " << BaseMotorSpecs.freeSpeed << endl;
	cout << "Spec Voltage: " << BaseMotorSpecs.specVolts << endl;
	cout << "Stall Current: " << BaseMotorSpecs.stallCurrent << endl;
	cout << "Stall Torque: " << BaseMotorSpecs.stallTorque << endl << endl << endl;

	cout << "The state data for motor " << motName << " is:" << endl;
	cout << "Current draw: " << currState.Amps << endl;
	cout << "Efficiency: " << currState.efficiency << endl;
	cout << "Power in: " << currState.powerIn << endl;
	cout << "Power out: " << currState.powerOut << endl;
	cout << "Speed Output: " << currState.speedOut << endl;
	cout << "Torque output: " << currState.torqueOut << endl;
	cout << "Voltage: " << currState.volts << endl << endl;
}
	// Setter methods
void motor::SetTransSpecs(float efficiency, float lossTorque, float momInertia, float gearReduction)
{
	BaseTransSpecs.efficiency = efficiency;
	BaseTransSpecs.lossTorque = lossTorque;
	BaseTransSpecs.momentOfInertia = momInertia;
	BaseTransSpecs.gearReduction = gearReduction;
}

void motor::SetMotorSpecs(float specVolts, float freeSpeed, float freeAmps, float stallTorque, float stallAmps)
{
	BaseMotorSpecs.specVolts = specVolts;
	BaseMotorSpecs.freeSpeed = freeSpeed;
	BaseMotorSpecs.freeAmps = freeAmps;
	BaseMotorSpecs.stallTorque = stallTorque;
	BaseMotorSpecs.stallCurrent = stallAmps;
}

// Transmission getter methods
float motor::GetTransEfficiency()		{	return BaseTransSpecs.efficiency;}
float motor::GetTransLossTorque()		{	return BaseTransSpecs.lossTorque;}
float motor::GetTransMomInertia()		{	return BaseTransSpecs.momentOfInertia;}
float motor::GetTransReduction()		{	return BaseTransSpecs.gearReduction;}

// MotorSpec getter methods
float motor::GetMotorSpecVolts()		{	return BaseMotorSpecs.specVolts;}
float motor::GetMotorFreeSpeed()		{	return BaseMotorSpecs.freeSpeed;}
float motor::GetMotorFreeAmps()			{	return BaseMotorSpecs.freeAmps;}
float motor::GetMotorStallTorque()		{	return BaseMotorSpecs.stallTorque;}
float motor::GetMotorStallCurrent()		{	return BaseMotorSpecs.stallCurrent;}

// State getter methods
float motor::GetStateVolts()			{	return currState.volts;}
float motor::GetStateTorqueOut()		{	return currState.torqueOut;}
float motor::GetStateCurrent()			{	return currState.Amps;}
float motor::GetStateSpeedOut()			{	return currState.speedOut;}
float motor::GetStatePowerIn()			{	return currState.powerIn;}
float motor::GetStatePowerOut()			{	return currState.powerOut;}
float motor::GetStateEfficiencyTotal()	{	return currState.efficiency;}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base Calculation Methods ///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

motor::MotorSpecs motor::AdjustSpecs(motor::MotorSpecs baseSpecs, float volts)
{
	// scale motor specs for different input voltages
	float stateScalar = volts / baseSpecs.specVolts;
	motor::MotorSpecs specs = BaseMotorSpecs;

	specs.freeAmps *= stateScalar;
	specs.freeSpeed *= stateScalar;
	specs.specVolts *= stateScalar;
	specs.stallCurrent *= stateScalar;
	specs.stallTorque *= stateScalar;

	return specs;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_TorqueVolts_baseMot(float torque, float volts, MotorSpecs specs)
{
	motor::State state;

	// Adjust specs based on voltage
	motor::MotorSpecs VoltAdjMotSpecs = AdjustSpecs(specs, volts);
	// Calculate motor base state based on torque and voltage
	state.speedOut = VoltAdjMotSpecs.freeSpeed * (1 - (torque / VoltAdjMotSpecs.stallTorque));
	state.Amps = VoltAdjMotSpecs.freeAmps + ((torque / VoltAdjMotSpecs.stallTorque)*(VoltAdjMotSpecs.stallCurrent - VoltAdjMotSpecs.freeAmps));
	state.powerOut = state.speedOut * torque;
	state.powerIn = volts * state.Amps;
	state.efficiency = state.powerOut / state.powerIn;
	state.torqueOut = torque;
	state.volts = volts;

	return state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_SpeedVolts_baseMot(float speed, float volts, MotorSpecs specs)
{
	// Adjust specs based on voltage
	motor::MotorSpecs VoltAdjMotSpecs = AdjustSpecs(specs, volts);

	// Calculate motor base state based on speed and voltage
	float torque = VoltAdjMotSpecs.stallTorque * (1 - (speed / VoltAdjMotSpecs.freeSpeed));
	return CalcState_TorqueVolts_baseMot(torque, volts, VoltAdjMotSpecs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_AmpsVolts_baseMot(float amps, float volts, MotorSpecs specs)
{
	// Adjust specs based on voltage
	motor::MotorSpecs VoltAdjMotSpecs = AdjustSpecs(specs, volts);
	float torque = (amps - VoltAdjMotSpecs.freeAmps) / (VoltAdjMotSpecs.stallCurrent - VoltAdjMotSpecs.freeAmps) * VoltAdjMotSpecs.stallTorque;
	return motor::CalcState_TorqueVolts_baseMot(torque, volts, VoltAdjMotSpecs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_PowInVolts_baseMot(float PowIn, float volts, MotorSpecs specs)
{
	// Calculate current draw
	float amps = PowIn / volts;
	return motor::CalcState_AmpsVolts_baseMot(amps, volts, specs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_SpeedTorque_baseMot(float speed, float torque, MotorSpecs specs)
{
	float volts = BaseMotorSpecs.specVolts * ((torque / BaseMotorSpecs.stallTorque) + (speed / BaseMotorSpecs.freeSpeed));
	return CalcState_TorqueVolts_baseMot(torque, volts, specs);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extrapolated calculations. These take the transmission mechanism data into account. ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void motor::CalcState_SpeedVolts(float speed, float volts, int msecTime)
{
	// Calculate motor state before accounting for loss torque
	motor::MotorSpecs TransAdjMotSpecs = AdjBaseSpecsForTransData(BaseMotorSpecs, BaseTransSpecs);
	float lossTorque = CalcLossTorque(speed, currState.speedOut, BaseTransSpecs, msecTime);
	motor::State subState = CalcState_SpeedVolts_baseMot(speed, volts, TransAdjMotSpecs);

	// compensate for torque loses
	subState.torqueOut -= lossTorque;
	subState.powerOut = subState.torqueOut * speed;
	subState.efficiency = subState.powerOut / subState.powerIn;

	currState = subState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void motor::CalcState_TorqueVolts(float torque, float volts, int msecTime)
{
	motor::State subState;
	int iterCt = 5;
	float tLoss = BaseTransSpecs.lossTorque;

	if (torque < 0) // if torque is negative, static losses are also negative
	{
		 tLoss *= -1;
	}

	motor::MotorSpecs TransAdjMotSpecs = AdjBaseSpecsForTransData(BaseMotorSpecs, BaseTransSpecs);

	for (int i = 0; i < iterCt; i++) // This is a simple recursive algorithm to easily approximate the loss torque
	{
		subState = CalcState_TorqueVolts_baseMot(torque + tLoss, volts, TransAdjMotSpecs);
		tLoss = CalcLossTorque(subState.speedOut, currState.speedOut, BaseTransSpecs, msecTime); // update loss torque value, and use it in the state calculation
	}

	currState = subState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void motor::CalcState_AmpsVolts(float amps, float volts, int msecTime)
{
	// Calculate motor state before accounting for loss torque
	motor::MotorSpecs TransAdjMotSpecs = AdjBaseSpecsForTransData(BaseMotorSpecs, BaseTransSpecs);
	motor::State subState = CalcState_AmpsVolts_baseMot(amps, volts, TransAdjMotSpecs);
	float lossTorque = CalcLossTorque(subState.speedOut, currState.speedOut, BaseTransSpecs, msecTime);

	// compensate for torque loses
	subState.torqueOut -= lossTorque;
	subState.powerOut = subState.torqueOut * subState.speedOut;
	subState.efficiency = subState.powerOut / subState.powerIn;

	currState = subState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void motor::CalcState_PowInVolts(float powIn, float volts, int msecTime)
{
	// Calculate motor state before accounting for loss torque
	motor::MotorSpecs TransAdjMotSpecs = AdjBaseSpecsForTransData(BaseMotorSpecs, BaseTransSpecs);
	motor::State subState = CalcState_PowInVolts_baseMot(powIn, volts, TransAdjMotSpecs);
	float lossTorque = CalcLossTorque(subState.speedOut, currState.speedOut, BaseTransSpecs, msecTime);

	// compensate for torque loses
	subState.torqueOut -= lossTorque;
	subState.powerOut = subState.torqueOut * subState.speedOut;
	subState.efficiency = subState.powerOut / subState.powerIn;

	currState = subState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void motor::CalcState_SpeedTorque(float speed, float torque, int msecTime)
{
	// Calculate motor state before accounting for loss torque
	motor::MotorSpecs TransAdjMotSpecs = AdjBaseSpecsForTransData(BaseMotorSpecs, BaseTransSpecs);
	motor::State subState = CalcState_PowInVolts_baseMot(speed, torque, TransAdjMotSpecs);
	float lossTorque = CalcLossTorque(subState.speedOut, currState.speedOut, BaseTransSpecs, msecTime);

	// compensate for torque loses
	subState.torqueOut -= lossTorque;
	subState.powerOut = subState.torqueOut * subState.speedOut;
	subState.efficiency = subState.powerOut / subState.powerIn;

	currState = subState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float motor::CalcLossTorque(float currSpeed, float prevSpeed, TransSpecs trans, int msecTime)
{
	float lossTorque = trans.lossTorque;
	float inertiaTorque = trans.momentOfInertia * ((currSpeed - prevSpeed) / (msecTime - lastUpdateMsec));
	if (currSpeed <= 0)
	{
		lossTorque *= -1;
	}
	return lossTorque + inertiaTorque;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::MotorSpecs motor::AdjBaseSpecsForTransData(MotorSpecs baseSpecs, TransSpecs trans)
{
	motor::MotorSpecs newSpecs = baseSpecs;
	float newFreeSpeed = baseSpecs.freeSpeed / trans.gearReduction;
	float newStallTorque = baseSpecs.stallTorque / trans.gearReduction;
	newSpecs.freeSpeed = newFreeSpeed;
	newSpecs.stallTorque = newStallTorque;
	return newSpecs;
}
