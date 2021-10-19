(C) Copyright 2014 Rafael Marsolla - rafamarsolla at gmail dot com

If you are looking for an LGPL or MIT licenses, please contact us.

============================ GPL LICENSE ============================ 

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Open C37.118
==========

IEEE C37.118-2011 Synchrophasor Protocol Implementation

The OpenC37.118-2011 library was developed as a way of providing a means of communication Synchrophasor implementing the Protocol as per IEEE C37.118-2011.

Fully developed in C++, allows you to simply communicating via TCP/IP using the frames described the normative standard.

Any application developed and supported by this language may make use of the features implemented.

Through the methods available, it is possible to encapsulate phasors data, analog and digital measurements, or even information related to product functionality.

The library is divided into six classes:

* 1 main class responsible for encapsulating common to all types of frame data.

* 1 class mapping the structure of PMU (PMU Station) , serving as a reference for storing characteristic data of the equipment.

* The other 4 classes describe the 4 types of frames described in the protocol, they are:

 ConfigFrame( 1 and 2 / / Setup 3 - To Do )
 HeaderFrame( )
 DataFrame( )
 CommandFrame( ) ;

The entire operation of the equipment is made using the CommandFrame( ), which contains the command to be executed by the machine. (Ex. Send Data ON / Send Data OFF).

All communication between devices starts with a configuration request command which reports through the FrameConfig( ) the current mode setting / available equipment.

Config 1 = Available
Config 2 = Current
Config 3 = To Do - Not available yet.

Incoming or outgoing setup, this will tell you how the equipment is set up and what types of data to be transmitted.

Subsequently, the unit waits to receive a command from the beginning of data transmission as CommandFrame( ) sent.

DataFrame( ), is responsible for encapsulating the measurements and transmit them.

The HeaderFrame( ) describes specific information about equipment, usually customized by the manufacturer/user (eg: Serial Number, SW Version).

For each of the frames sent, it is necessary to fill out the data directly in the structures (BUFFER) before any transmission.

The pack( ) method is responsible for encapsulating a buffer of parameters in a specific frame as defined in the standard.

When receiving a packet C37.118, you should use the unpack( ) method on the type of frame received.

The information contained in this package are then copied to a buffer used by the application.

Summary :

TX : Create the object, populate the fields of the object, encapsulate with pack( ) and pass.
RX : Create the object, receive the package, descanpsule with unpack( ) and read the values of the object's fields.
In the examples folder it is possible to see the implementation of a PMU and PDC using the methods implemented and tested in a real time closed loop.
