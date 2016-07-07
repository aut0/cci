/*****************************************************************************
  Copyright 2006-2014 Accellera Systems Initiative Inc.
  All rights reserved.

  Copyright 2010-2015 CircuitSutra Technologies Pvt. Ltd.
  All rights reserved.

  Copyright 2016 Ericsson AB.
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
 *  @file    parameter_configurer.h
 *  @brief   This class demonstrates means to access and set various
 *           attributes of a cci parameter
 *  @author  P V S Phaneendra, CircuitSutra Technologies   <pvs@circuitsutra.com>
 *           Girish Verma, CircuitSutra Technologies   <girish@circuitsutra.com>
 *  @date    12th September, 2011 (Monday)
 */

#ifndef EXAMPLES_EX16_USER_DEFINED_DATA_TYPE_EX16_PARAMETER_CONFIGURER_H_
#define EXAMPLES_EX16_USER_DEFINED_DATA_TYPE_EX16_PARAMETER_CONFIGURER_H_

#include <cci_configuration>
#include <cassert>
#include <string>

#include "xreport.hpp"

/**
 *  @class  ex16_parameter_configurer
 *  @brief  This module instantiates a cci configuration broker which accessess
 *          and sets thevarious attributes of a cci_parameter
 */
SC_MODULE(ex16_parameter_configurer) {
 public:
  /**
   *  @fn     ex16_parameter_configurer
   *  @brief  The class constructor
   *  @return void
   */
  SC_CTOR(ex16_parameter_configurer) {
    // Get the broker responsible for this module using
    // 'get_current_broker' API
    myBrokerInterface =
        &cci::cci_broker_manager::get_current_broker(cci::cci_originator(*this));

    // Assert if broker handle returned is NULL
    assert(myBrokerInterface != NULL && "Broker Handle Returned is NULL");

    // Check for the broker type (default or private) using
    // 'is_private_broker()' API
    if (myBrokerInterface->is_private_broker()) {
      // Access broker's name using 'name()'
      XREPORT("[CFGR C_TOR] : Broker Type : " << myBrokerInterface->name());
    } else {
      XREPORT("[CFGR C_TOR] : Broker Type : "
              << myBrokerInterface->name() << " - is not a private broker.");
    }

    udt_param_str = "param_owner.User_data_type_param";

    // Check the existence of the user-defined data type cci-parameter
    if (myBrokerInterface->param_exists(udt_param_str)) {
      // If parameter exists, get handle of the parameter using 'get_param' API
      udt_param_ptr = myBrokerInterface->get_param(udt_param_str);

      // Report if parameter handle is returned NULL
      assert(udt_param_ptr != NULL
             && "User define data type CCI Parameter Handle returned NULL");
    } else {
      XREPORT("[CFGR C_TOR] : User define datatype  parameter does not exist");
    }

    // Registering SC_THREAD process
    SC_THREAD(run_accessor);
  }

  /**
   *  @fn     void run_accessor(void)
   *  @brief  Accessor function to access the CCI parameters
   *  @return void
   */
  void run_accessor(void) {
    while (1) {
      wait(4.0, SC_NS);

      XREPORT("@ " << sc_time_stamp());

      // Access parameter's name using 'get_name()' API
      XREPORT("[CFGR -> Retrieve] : Parameter name : "
              << udt_param_ptr->get_name());

      // Access parameter's value using 'json_serialize' API
      XREPORT("[CFGR -> Retrieve] : Parameter value: "
              << udt_param_ptr->json_serialize());

      // Access parameter's description using 'get_description()' API
      XREPORT("[CFGR -> Retrieve] : Parameter desc: "
              << udt_param_ptr->get_description());

      wait(2.0, SC_NS);

      XREPORT("@ " << sc_time_stamp());
      XREPORT("[CFGR -> Set] : Value - 's_address:1024,d_address:1280,index:3");

      std::string set_string("{\"s_address\":1024,\"d_address"
                             "\":1280,\"index\":3}");
      udt_param_ptr->json_deserialize(set_string);

      wait(2.0, SC_NS);
      XREPORT("@ " << sc_time_stamp());

      // Access parameter's name using 'get_name()' API
      XREPORT("[CFGR -> Retrieve] : Parameter name : "
              << udt_param_ptr->get_name());

      // Access parameter's value using 'json_serialize' API
      XREPORT("[CFGR -> Retrieve] : Parameter value: "
              << udt_param_ptr->json_serialize());

      wait(20.0, SC_NS);
    }
  }

 private:
  cci::cci_broker_if* myBrokerInterface; ///< CCI configuration broker instance

  std::string udt_param_str;  ///< std::string types for storing parameters hierarchical paths

  // Declaring cci_base_parameters
  cci::cci_base_param* udt_param_ptr;  ///< CCI base parameter pointer
};
// ex16_parameter_configurer

#endif  // EXAMPLES_EX16_USER_DEFINED_DATA_TYPE_EX16_PARAMETER_CONFIGURER_H_