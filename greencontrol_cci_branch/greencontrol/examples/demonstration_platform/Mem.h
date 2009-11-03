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

//
// simplememory see usercode/example_simplebus
//

#ifndef __MEM_H__
#define __MEM_H__

#include "demo_globals.h"

// SimpleBus API
#include "greenbus/api/simplebusAPI.h"

// GreenControl and GreenConfig
#include "greencontrol/config.h"

#include "ddr_ram.h"


/// Memory for demonstration platform
/**
 *
 * Instead of using this class the simplebusSlavePort 
 * may be instatiated in the testbench directly:
 * \code
 * simplebusSlavePort *slaveport = new simplebusSlavePort("memoryport");
 * ddr *memory = new ddr("Mem");
 * slaveport->slave_port(*memory);
 * slaveport->base_addr = 0x200;
 * slaveport->high_addr = 0x300;
 * \endcode
 * But in that case the base and high addresses cannot be parameters.
 */
class Mem
: public sc_core::sc_module
{
public:
  /// SimpleBus slave port
  tlm::simplebusSlavePort slave_port;

  gs::gs_param<gs_uint64> base_addr;
  gs::gs_param<gs_uint64> high_addr;

  SC_HAS_PROCESS(Mem);
	
  /// Constructor
  Mem(sc_core::sc_module_name name)
    : sc_core::sc_module(name),
      slave_port("simplebus_slaveport"),
      base_addr("base_addr", 0x200),
      high_addr("high_addr", 0x400),
      ddr_mem("memory")
  { 
    ddr_mem.size = high_addr - base_addr;
    slave_port.slave_port(ddr_mem);
    slave_port.base_addr = tlm::MAddr(base_addr);
    slave_port.high_addr = tlm::MAddr(high_addr);
  }

private:
  /// Memory
  ddr ddr_mem;

};


#endif
