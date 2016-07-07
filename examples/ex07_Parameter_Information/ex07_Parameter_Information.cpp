/*****************************************************************************
  Copyright 2006-2014 Accellera Systems Initiative Inc.
  All rights reserved.

  Copyright 2010-2015 CircuitSutra Technologies Pvt. Ltd.
  All rights reserved.
  
  Copyright 2006-2015 Intel Corporation
  All rights reserved.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 *****************************************************************************/

/**
 *  @brief   The main function starts here.
 *           The sc_main function instantiates 'PARAMETER_OWNER', 'PARAMETER_CONFIGURATOR'
 *           modules and 'OBSERVER' class
 *  @author  P V S Phaneendra, CircuitSutra Technologies   <pvs@circuitsutra.com>
 */
#include <systemc>
#include <cci_configuration>
#include <cassert>
#include <string>

#include "ex07_parameter_owner.h"
#include "ex07_parameter_configurer.h"
#include "ex07_observer.h"

/// Container (module) of some test parameters.
class ex07_parameter_container : public sc_core::sc_module
{
public:
	ex07_parameter_container(sc_core::sc_module_name name) : sc_module(name)
	, mutab_str_param("string_mutab_param", "String_Value_A")
	, immutab_str_param("string_immutab_param", "String_Value_A")
	, elab_str_param("string_elab_param", "String_Value_B")
	{}

	cci::cci_param<std::string, cci::mutable_param> mutab_str_param;
	cci::cci_param<std::string, cci::immutable_param> immutab_str_param;
	cci::cci_param<std::string, cci::elaboration_time_param> elab_str_param;
};

/**
 *  @fn     int sc_main(int argc, char* argv[])
 *  @brief  The testbench for the parameter information example
 *  @param  argc  The number of input arguments
 *  @param  argv  The list of input arguments
 *  @return An interger for the execution status
 */
int sc_main(int sc_argc, char* sc_argv[]) {
  // Creating an originator to access the global broker
  const std::string myOrgStr = "sc_main_originator";
  cci::cci_originator myOriginator(myOrgStr);

  // Get handle of the broker using the originator
  cci::cci_broker_if* globalBroker =
      &cci::cci_broker_manager::get_current_broker(myOriginator);

  // Assert if broker handle returned is NULL
  assert(globalBroker != NULL
         && "Reference to global broker returned (in sc_main) is NULL.");

  // Set initial value to the 'int_param' of 'parameter_owner' class before
  // their constructor begins
  SC_REPORT_INFO("sc_main",
                 "[MAIN] : Setting 'param_owner.mutable_string_param'"
                 " value to 'Initialized within sc_main()'");

  // Demonstrating use of 'json_deserialize_initial_value'
  // API to assign initial value before the
  // construction of the model hierarchy begins.
  globalBroker->json_deserialize_initial_value(
      "param_owner.mutable_string_param", "\"Initialized within sc_main\"");

  SC_REPORT_INFO("sc_main",
                 "[MAIN] : Demonstrating 'comparison' between the values"
                 " of a data type for different mutability types");

  // Instantiate parameter container of string parameters
  ex07_parameter_container pc("params");

  // Perform comparisons across mutability classes.  Access params directly
  // for convenience (generally we would look them up via broker).
  if (pc.mutab_str_param.get() == pc.immutab_str_param.get()) {
    SC_REPORT_INFO("sc_main", "[MAIN] : 'mutable' & 'immutable' type String"
                   " parameters - VALUES MATCH");
  } else {
    SC_REPORT_INFO("sc_main", "[MAIN] : 'mutable' & 'immutable' type String"
                   " parameters - VALUES DO NOT MATCH");
  }

  if (pc.mutab_str_param.get() == pc.elab_str_param.get()) {
    SC_REPORT_INFO("sc_main", "[MAIN] : 'mutable' & 'elaboration_time' type"
                   " String - VALUES MATCH");
  } else {
    SC_REPORT_INFO("sc_main", "[MAIN] : 'mutable' & 'elaboration_time' type"
                   " String - VALUES DO NOT MATCH");
  }

  // Infrastructure created within the example for example illustration
  SC_REPORT_INFO("sc_main", "[MAIN] : parameter_container module declares two"
                 " cci type parameters.");
  SC_REPORT_INFO("sc_main", "One is of 'Integer type' and the other is of"
                 " 'String type'");
  SC_REPORT_INFO("sc_main", "'Integer type' has 'pre_read', 'pre/post_write'"
                 " callback registered within the OBSERVER");

  // Instantiation of sc_modules
  ex07_parameter_owner param_owner("param_owner");
  ex07_parameter_configurer param_cfgr("param_cfgr");

  // Instantiating observer class
  ex07_observer observer_class("observer");

  // Simulation time
  sc_core::sc_start(30.0, sc_core::SC_NS);

  return EXIT_SUCCESS;
}  // End of sc_main