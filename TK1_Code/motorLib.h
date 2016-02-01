#pragma once
#ifndef MOTORLIB_H
#define MOTORLIB_H
#include <string>

class motor
{
public:

	void printData();

	// Define structures and variables
	struct TransSpecs
	{
		float efficiency;
		float lossTorque;
		float momentOfInertia;
		float gearReduction;
	};

	struct MotorSpecs
	{
		float specVolts;
		float freeSpeed;
		float freeAmps;
		float stallTorque;
		float stallCurrent;
	};

	struct State
	{
		float volts;
		float torqueOut;
		float Amps;
		float speedOut;
		float powerIn;
		float powerOut;
		float efficiency;
	};

	TransSpecs BaseTransSpecs;
	MotorSpecs BaseMotorSpecs;
	State currState;
	State baseState; // specs of motor disregarding transmission data

	// Motor state calculation functions
	MotorSpecs AdjustSpecs(MotorSpecs baseSpecs, float volts);
	MotorSpecs AdjBaseSpecsForTransData(MotorSpecs baseSpecs, TransSpecs trans);

	float CalcLossTorque(float currSpeed, float prevSpeed, TransSpecs trans, int msecTime);
	
	// Adjusted motor calculations - use these in code!
	void CalcState_SpeedVolts(float speed, float volts, int msecTime);
	void CalcState_TorqueVolts(float torque, float volts, int msecTime);
	void CalcState_AmpsVolts(float amps, float volts, int msecTime);
	void CalcState_PowInVolts(float powIn, float volts, int msecTime);
	void CalcState_SpeedTorque(float speed, float torque, int msecTime);


	// Set specs
	motor(std::string name, float specVolts, float freeSpeed, float freeAmps, float stallTorque, float stallAmps);
	void SetTransSpecs(float efficiency, float lossTorque, float MomInertia, float gearReduction);
	void SetMotorSpecs(float specVolts, float freeSpeed, float freeAmps, float stallTorque, float stallAmps);

	// Get Transmission Specs
	float GetTransEfficiency();
	float GetTransLossTorque();
	float GetTransMomInertia();
	float GetTransReduction();

	// Get Motor Specs
	float GetMotorSpecVolts();
	float GetMotorFreeSpeed();
	float GetMotorFreeAmps();
	float GetMotorStallTorque();
	float GetMotorStallCurrent();

	// Get State data
	float GetStateVolts();
	float GetStateTorqueOut();
	float GetStateCurrent();
	float GetStateSpeedOut();
	float GetStatePowerIn();
	float GetStatePowerOut();
	float GetStateEfficiencyTotal();

private:

	int lastUpdateMsec = 0;
	std::string motName;

	// Base motor calculations. These are intermediate methods.
	State CalcState_SpeedVolts_baseMot(float speed, float volts, MotorSpecs specs);
	State CalcState_TorqueVolts_baseMot(float torque, float volts, MotorSpecs specs);
	State CalcState_AmpsVolts_baseMot(float amps, float volts, MotorSpecs specs);
	State CalcState_PowInVolts_baseMot(float powIn, float volts, MotorSpecs specs);
	State CalcState_SpeedTorque_baseMot(float speed, float torque, MotorSpecs specs);

};

#endif