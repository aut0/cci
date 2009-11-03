//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//   Wolfgang Klingauf <klingauf@eis.cs.tu-bs.de>
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
//   This program is free software.
// 
//   If you have no applicable agreement with GreenSocs Ltd, this software
//   is licensed to you, and you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
// 
//   If you have a applicable agreement with GreenSocs Ltd, the terms of that
//   agreement prevail.
// 
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// 
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
//   02110-1301  USA 
// 
// ENDLICENSETEXT

#ifndef __PCIEDEVICE_H__
#define __PCIEDEVICE_H__

#include "demo_globals.h"

// SimpleBus API
#include "greenbus/api/simplebusAPI.h"

// GreenControl and GreenConfig
#include "greencontrol/config.h"

#include "ddr_ram.h"


/// PCIeDevice for demonstration platform
/**
 * Registers
 *
 */
class PCIeDevice
: public sc_core::sc_module
{
  
public:
  /// SimpleBus slave port
  tlm::simplebusSlavePort slave_port;

private:
  /// Base address of this device on the SimpleBus
  gs::gs_param<gs_uint64> base_addr;
  /// Size of the register in this device (implies the high_address = base_address + mem_size)
  gs::gs_param<unsigned int> mem_size;

public:
  SC_HAS_PROCESS(PCIeDevice);
	
  /// Constructor
  PCIeDevice(sc_core::sc_module_name name)
    : sc_core::sc_module(name),
      slave_port("simplebus_slaveport"/*, true *//*Analysis: record transactions*/),
      base_addr("base_addr", 0x100),
      mem_size("mem_size", 0xFF),
      ddr_registers("registers")
  { 
    slave_port.slave_port(ddr_registers);
    slave_port.base_addr = tlm::MAddr(base_addr);
    slave_port.high_addr = tlm::MAddr(base_addr + mem_size);
    ddr_registers.size = (tlm::MBurstLength) mem_size;
    //cout << mem_size.getValue() << endl;
  }
  
private:
  /// registers
  ddr ddr_registers;

};


#endif

