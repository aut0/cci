/*****************************************************************************
  Copyright 2006-2014 Accellera Systems Initiative Inc.
  All rights reserved.

  Copyright 2010-2015 Texas Instruments Inc.
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
 *  @file   testbench.cpp
 *  @brief  A testbench that demonstrates possible updates to a elaboration-time 
 *          integer parameter.
 *  @detail sim_ip:
 *          1. Instantiates a structure_param with a default value. 
 *             a. Updates are made to this param at these simulation points:
 *                - Before end of elaboration (valid)
 *                - After start of simulation (invalid)
 *          2. Creates a struc_param_post_eoe, after end of elaboration with a default value.
 *             a. Updates are made to this param at these simulation points:
 *                - After start of simulation (invalid)        
 *          cfg_ip:
 *          1. Updates parameters viz. structure_param and struc_param_post_eoe, both updations are invalid.
 *
 *  @author Udaya Ranga, TI
 */

#include "ex03_simple_ip.h"
#include "ex03_config_ip.h"

/**
 *  @fn     int sc_main(int argc, char* argv[])
 *  @brief  Testbench for the CCI elab_time_param example
 *  @param  argc  An integer representing the number of input arguments
 *  @param  argv  A list of the input arguments
 *  @return An integer representing the status of the execution of the function
 */
int sc_main(int argc, char *argv[]) {
  ex03_simple_ip sim_ip("sim_ip");
  ex03_config_ip cfg_ip("cfg_ip");

  SC_REPORT_INFO("sc_main", "Begin Simulation.");
  sc_core::sc_start();
  SC_REPORT_INFO("sc_main", "End Simulation.");

  return EXIT_SUCCESS;
}