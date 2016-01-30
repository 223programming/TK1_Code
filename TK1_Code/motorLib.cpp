#include "stdafx.h"
#include "motorLib.h"
#include <iostream>

using namespace std;

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

	return motor::MotorSpecs
	{
		stateScalar * baseSpecs.freeAmps,
		stateScalar * baseSpecs.freeSpeed,
		volts, // return the input voltage as the new spec voltage for the modified specs
		stateScalar * baseSpecs.stallCurrent,
		stateScalar * baseSpecs.stallTorque,
	};
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_TorqueVolts_baseMot(float torque, float volts)
{
	// Adjust specs based on voltage
	AdjMotSpecs = AdjustSpecs(BaseMotorSpecs, volts);

	// Calculate motor base state based on torque and voltage
	baseState.speedOut = AdjMotSpecs.freeSpeed * (1 - (torque / AdjMotSpecs.stallTorque));
	baseState.Amps = AdjMotSpecs.freeAmps + ((torque / AdjMotSpecs.stallTorque)*(AdjMotSpecs.stallCurrent - AdjMotSpecs.freeAmps));
	baseState.powerOut = baseState.speedOut * torque;
	baseState.powerIn = volts * baseState.Amps;
	baseState.efficiency = baseState.powerOut / baseState.powerIn;
	baseState.torqueOut = torque;
	baseState.volts = volts;

	return baseState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_SpeedVolts_baseMot(float speed, float volts)
{
	// Adjust specs based on voltage
	AdjMotSpecs = AdjustSpecs(BaseMotorSpecs, volts);

	// Calculate motor base state based on speed and voltage
	float torque = AdjMotSpecs.stallTorque * (1 - (speed / AdjMotSpecs.freeSpeed));
	return CalcState_TorqueVolts_baseMot(torque, volts);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_AmpsVolts_baseMot(float amps, float volts)
{
	// Adjust specs based on voltage
	AdjMotSpecs = AdjustSpecs(BaseMotorSpecs, volts);
	float torque = (amps - AdjMotSpecs.freeAmps) / (AdjMotSpecs.stallCurrent - AdjMotSpecs.freeAmps) * AdjMotSpecs.stallTorque;
	return motor::CalcState_TorqueVolts_baseMot(torque, volts);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_PowInVolts_baseMot(float PowIn, float volts)
{
	// Calculate current draw
	float amps = PowIn / volts;
	return motor::CalcState_AmpsVolts_baseMot(amps, volts);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
motor::State motor::CalcState_SpeedTorque_baseMot(float speed, float torque)
{
	float volts = BaseMotorSpecs.specVolts * ((torque / BaseMotorSpecs.stallTorque) + (speed / BaseMotorSpecs.freeSpeed));
	return CalcState_TorqueVolts_baseMot(torque, volts);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extrapolated calculations. These take the transmission mechanism data into account. ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void motor::CalcState_SpeedVolts(float speed, float volts, int msecTime)
{
	float speed_mot = BaseTransSpecs.gearReduction * speed;
	intrmdState = CalcState_SpeedVolts_baseMot(speed_mot, volts);


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float motor::CalcLossTorque(State currState, State prevState, TransSpecs trans, int deltaMsec)
{

}
