//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2007-2009 : GreenSocs Ltd
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

// SystemC library
#include <systemc>

// for CommandLine_Tool: timed or not timed models
//  value to be put into wait(...) statement, e.g.:
//    SC_ZERO_TIME or
//    sc_time(1, SC_NC)
#define TEST_TOOL_POLL_TIME sc_time(1, SC_MS)

#include "../demo_globals.h"

// GreenConfig
#include "greencontrol/config.h" 

#include "testtool.h"                  // IP with test tool
#include "testip.h"                    // test IP
#include "testip2.h"                   // test IP2
#include "testip3.h"                   // test IP3
#include "testipVec.h"                 // test IPVec
#include "ParamListIP.h"               // test IP
#include "AttributeTestsIP.h"
#include "sceventparam.h"
#include "toplevel_param_tests.h"      // toplevel param tests
#include "configfile_macro_test.h"     // test IP

#include "greencontrol/config_api_config_file_parser.h"  // API Tool to read configuration file(s)
#include "greencontrol/config_api_command_line_parser.h" // API Tool to process config command line parameters
#include "greencontrol/config_api_command_line_tool.h" // Tool which provides a command line
#include "greencontrol/core/helpfunctions.h"
#include "scattribparam.h"

/// Testbench for the example GreenConfig
/**
 * The testbench instantiates the GreenCotrol Core and the needed user modules.
 */
int sc_main(int argc, char *argv[]) {
  
  /// GreenControl Core instance
  gs::ctr::GC_Core       core("ControlCore");

  gs::cnf::ConfigDatabase cnfdatabase("ConfigDatabase");
  gs::cnf::ConfigPlugin configPlugin("ConfigPlugin", &cnfdatabase);

  TestIP       ip1("IP1");
  TestIP2      ip2("IP2");
  TestIP3      ip3("IP3");
  TestIPVec    IPVec("IPVec");
  
  TestTool     tool("TestTool");

  ParamListIP  plip("ParamListIP");
  AttributeTestsIP atip("AttributeTestsIP");
  Param_test_sc_attribute scatrIP("sc_attrib_IP");
  Param_test_sc_event scevIP("sc_event_IP");
  
  gs::gs_param<int> topLevelParam("topLevelParam", 42);

  Toplevel_Param_Tests  toplevelParam;
  toplevelParam.main();
  
  gs::cnf::ConfigFile_Tool configTool("ConfigFileTool");
   configTool.config("configtest.cfg");
  configTool.config("configtest2.cfg");
  configTool.config("configtestVectors.cfg");
  configTool.config("configmacros.cfg");
  configTool.parseCommandLine(argc, argv);
  
  gs::cnf::CommandLineConfigParser configParser("ConfigParser");
  configParser.parse(argc, argv);

  gs::cnf::CommandLine_Tool commandLineTool("CommandLineTool");

  ConfigFileMacroMod macroIP("ConfigFileMacroIP");

  sc_core::sc_start();
  
  std::cout << "END SIMULATION" << std::endl;
  std::cout << "Parameter list:" << std::endl;
  gs::ctr::show_vector<std::string>(gs::cnf::GCnf_Api::getApiInstance(NULL)->getParamList());
    
  return EXIT_SUCCESS; 
  
}
