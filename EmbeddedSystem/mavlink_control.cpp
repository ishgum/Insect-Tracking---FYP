/****************************************************************************
 *
 *   Copyright (c) 2014 MAVlink Development Team. All rights reserved.
 *   Author: Trent Lukaczyk, <aerialhedgehog@gmail.com>
 *           Jaycee Lock,    <jaycee.lock@gmail.com>
 *           Lorenz Meier,   <lm@inf.ethz.ch>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mavlink_control.cpp
 *
 * @brief An example offboard control process via mavlink
 *
 * This process connects an external MAVLink UART device to send an receive data
 *
 * @author Trent Lukaczyk, <aerialhedgehog@gmail.com>
 * @author Jaycee Lock,    <jaycee.lock@gmail.com>
 * @author Lorenz Meier,   <lm@inf.ethz.ch>
 *
 */



// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include "mavlink_control.h"


// ------------------------------------------------------------------------------
//   Constructor
// ------------------------------------------------------------------------------

UAVControl::UAVControl (char* uart_name, int baudrate) : serial_port(uart_name, baudrate), api(&serial_port)
{
	uavControl = false;
	initialised = false;

}



// ------------------------------------------------------------------------------
//   Destructor
// ------------------------------------------------------------------------------

UAVControl::~UAVControl()
{

	wprintw(output.outputStream, "Closing Mavlink Connection\n");

	wrefresh(output.outputStream);
	// autopilot interface
	try {
		api.handle_quit(0);		// Also disables offboard control
	}
	catch (int error){}


	// serial port
	try {
		serial_port.handle_quit(0);
	}
	catch (int error){}

}

void UAVControl::init (void) 
{


	// --------------------------------------------------------------------------
	//   PORT and THREAD STARTUP
	// --------------------------------------------------------------------------
	//Serial_Port serial_port(uart_name, baudrate);
	//Autopilot_Interface api(&serial_port);

	/*
	 * Start the port and autopilot_interface
	 * This is where the port is opened, and read and write threads are started.
	 */


	try { serial_port.start(); }
	catch (int i) { throw("Serial Connection\n"); }
	

	try { api.start(); }
	catch (int i) { throw("Auto-pilot start up\n"); }

	wrefresh(output.outputStream);
	api.disable_offboard_control();
	uavControl = false;
	
	// initialize command data strtuctures
	mavlink_set_position_target_local_ned_t sp;
	mavlink_set_position_target_local_ned_t ip = api.initial_position;
	
	wprintw(output.outputStream, "\nInitial POSITION XYZ = [ % .4f , % .4f , % .4f ] \n\n", ip.x, ip.y, ip.z);
	
	initialised = true;

	// autopilot_interface.h provides some helper functions to build the command

}




// ------------------------------------------------------------------------------
//   Methods
// ------------------------------------------------------------------------------


void UAVControl::enableControl(void)
{
	// --------------------------------------------------------------------------
	//   START OFFBOARD MODE
	// --------------------------------------------------------------------------

	mavlink_local_position_ned_t pos = api.current_messages.local_position_ned;
	if ((pos.z - ip.z) < 10) { wprintw(output.outputStream, "Error: UAV too low"); }
	else {
		api.enable_offboard_control();
		uavControl = true;
		usleep(100); // give some time to let it sink in
	}

	// now the autopilot is accepting setpoint commands
}


void UAVControl::disableControl(void)
{
	// --------------------------------------------------------------------------
	//   STOP OFFBOARD MODE
	// --------------------------------------------------------------------------

	api.disable_offboard_control();
	uavControl = false;
	usleep(100); // give some time to let it sink in

	// now the autopilot is accepting setpoint commands
}


void UAVControl::toggleControl(void)
{
	// --------------------------------------------------------------------------
	//   TOGGLE OFFBOARD MODE
	// --------------------------------------------------------------------------

	if (uavControl) api.disable_offboard_control();
	else api.enable_offboard_control();
	uavControl = !uavControl;
	usleep(100); // give some time to let it sink in

	
}

bool UAVControl::getControlStatus(void)
{
	return uavControl;
}



void UAVControl::printControl (void)
{
	if (uavControl)	wprintw(output.outputStream, "\nUAV Control ON\n");
	else wprintw(output.outputStream, "\nUAV Control OFF\n");
}


void UAVControl::updateVelocity(float x, float y, float z)
{


	set_velocity( x       , // [m/s]
			y       , // [m/s]
			z       , // [m/s]
			sp        );

	set_yaw( 0 , // [rad]
			 sp     );

	// SEND THE COMMAND
	api.update_setpoint(sp);
	// NOW pixhawk will try to move

	return;
}


/*
void UAVControl::updateVelocityTest(float angle)
{


	x
	// Example 1 - Set Velocity
	set_velocity( x       , // [m/s]
			y       , // [m/s]
			z       , // [m/s]
			sp        );

	// Example 2 - Set Position
//	 set_position( ip.x - 5.0 , // [m]
//			 	   ip.y - 5.0 , // [m]
//				   ip.z       , // [m]
//				   sp         );


	// Example 1.2 - Append Yaw Command
	set_yaw( ip.yaw , // [rad]
			 sp     );

	// SEND THE COMMAND
	api.update_setpoint(sp);
	// NOW pixhawk will try to move

	return;
}
*/


void UAVControl::printPosition (void) 
{

	// Wait for 8 seconds, check position
	mavlink_local_position_ned_t pos = api.current_messages.local_position_ned;
	wprintw(output.outputStream, "CURRENT POSITION XYZ = [ % .4f , % .4f , % .4f ]", pos.x, pos.y, pos.z);

	//wprintw(output.outputStream, "\n");

	return;

}


bool UAVControl::isInit()
{
	return initialised;
}


void UAVControl::printParameters(int y, int x)
{
	outputMap["UAV Data"] = 0;

	outputMap["UAV Connected:"] = 2;
	outputMap["X Position:"] = 3;
	outputMap["Y Position:"] = 4;
	outputMap["Z Position:"] = 5;


	printDataWindow(outputMap, y, x);

}

void UAVControl::printOutput(void) {

	mavlink_local_position_ned_t pos = api.current_messages.local_position_ned;

	werase(output.uavData);
	mvwprintw(output.uavData, outputMap["UAV Connected:"], 0, "%d\n", initialised);
	mvwprintw(output.uavData, outputMap["X Position:"], 0, "%.4f\n", pos.x);
	mvwprintw(output.uavData, outputMap["Y Position:"], 0, "%.4f\n", pos.y);
	mvwprintw(output.uavData, outputMap["Z Position:"], 0, "%.4f\n", pos.z);


}



