/*****************************************************************************
  Copyright 2006-2014 Accellera Systems Initiative Inc.
  All rights reserved.

  Copyright 2010-2015 CircuitSutra Technologies Pvt. Ltd.
  All rights reserved.

  Copyright 2016 Ericsson
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
 *  @file      processor.h
 *  @brief     This header implements 'processor' module validates the consistency of the system
 *  @author    P V S Phaneendra, CircuitSutra Technologies, Inc.  <pvs@circuitsutra.com>
 *  @date      4th August, 2011 (Thursday)
 */

#ifndef EXAMPLES_EX15_INTER_PARAMETER_VALUE_CONSTRAINTS_EX15_PROCESSOR_H_
#define EXAMPLES_EX15_INTER_PARAMETER_VALUE_CONSTRAINTS_EX15_PROCESSOR_H_

#include <math.h>
#include <cci_configuration>
#include <cassert>
#include <string>

#include "ex15_address_lines_register.h"
#include "ex15_memory_block.h"
#include "xreport.hpp"

/**
 *  @class  ex15_processor
 *  @brief  This module instantiates a processor module which register's callbacks
 *          on references of the cci_parameter of the two register modules and
 *          the memory stack module. It also does a few checks and comparisons
 *          in order to validate whether or not the system configuration meets
 *          the requirements
 */
SC_MODULE(ex15_processor) {
 public:
  /**
   *  @fn     ex15_processor
   *  @brief  The class constructor
   *  @return void
   */
  SC_CTOR(ex15_processor)
      : addr_lines_module("addr_lines_mod"),
        memory_block_module("memory_block") {
    // Get reference of the broker responsible for the module
    processor_BrokerIF = &cci::cci_broker_manager::get_current_broker(cci::cci_originator(*this));

    assert(processor_BrokerIF != NULL
           && "Broker's Handle for the 'processor' module is returned NULL");

    // Get handle of the 'no_of_addr_lines' cci-parameter of
    // 'address_lines_register'
    if (processor_BrokerIF != NULL) {
      std::string param_path(name());
      param_path.append(".addr_lines_mod.curr_addr_lines");

      if (processor_BrokerIF->param_exists(param_path)) {
        addr_lines_base_ptr = processor_BrokerIF->get_param(param_path);

        assert(addr_lines_base_ptr != NULL
               && "Returned handle of 'no_of_addr_lines' cci-parameter"
               " is NULL");
      } else {
        XREPORT("[PROCESSOR C_TOR] : Parameter " << param_path
                << "\tdoesn't not exists");
      }
    }

    // Get handle of the 'mem_size' cci-parameter of 'memory_block'
    if (processor_BrokerIF != NULL) {
      std::string param_path(name());
      param_path.append(".memory_block.mem_size");

      if (processor_BrokerIF->param_exists(param_path)) {
        mem_size_base_ptr = processor_BrokerIF->get_param(param_path);

        assert(mem_size_base_ptr != NULL
               && "Returned handle of 'memory_block_size' cci-parameter"
               " is NULL");
      } else {
        XREPORT("[PROCESSOR C_TOR] : Parameter " << param_path
                << "\tdoesn't not exists");
      }
    }

    // Checks for the condition whether the default total number of the
    // address lines can address the default address location
    total_addr_lines =
            atoi(addr_lines_base_ptr->get_cci_value().json_serialize().c_str());
    mem_block_size =
            atoi(mem_size_base_ptr->get_cci_value().json_serialize().c_str());
    TestCondition(total_addr_lines, mem_block_size);

    // Registering 'POST_WRITE' callbacks on the cci-parameters of the
    // two register modules
    addr_lines_post_wr_cb =
        addr_lines_base_ptr->register_callback(cci::post_write,
                                               this,
                                               cci::bind(&ex15_processor::addr_lines_post_wr_cb_func,
                                                         this, _1, _2,
                                                         mem_size_base_ptr));
    mem_block_post_wr_cb =
        mem_size_base_ptr->register_callback(cci::post_write,
                                             this,
                                             cci::bind(&ex15_processor::mem_block_post_wr_cb_func,
                                                       this, _1, _2,
                                                       addr_lines_base_ptr));
  }

  /**
   *  @fn     cci::callback_return_type addr_lines_post_wr_sb_func(cci::cci_base_param& _base_param, const cci::callback_type& cb_reason, cci::cci_base_param* _mem_size_base_ptr)
   *  @brief  Implementation of the callback function for address lines
   *  @param  _base_param The parameter for the callback
   *  @param  cb_reason Reason for the callback
   *  @param  _mem_size_base_ptr  Pointer to the base of the memory size
   *  @return The status of the execution of the function
   */
  cci::callback_return_type addr_lines_post_wr_cb_func(cci::cci_base_param & _base_param,
                                                            const cci::callback_type & cb_reason,
                                                            cci::cci_base_param * _mem_size_base_ptr) {
    XREPORT("[PROCESSOR addr_lines_post_wr_cb] : Parameter Name : "
            << _base_param.get_name() << "\tParameter Value : "
            << _base_param.get_cci_value().json_serialize());

    XREPORT("[PROCESSOR addr_lines_post_wr_cb] : Parameter Name : "
            << _mem_size_base_ptr->get_name() << "\tParameter Value : "
            << _mem_size_base_ptr->get_cci_value().json_serialize());

    total_addr_lines =
            atoi(_base_param.get_cci_value().json_serialize().c_str());
    mem_block_size =
            atoi(_mem_size_base_ptr->get_cci_value().json_serialize().c_str());

    // Test condition : X < 2^n - 1
    TestCondition(total_addr_lines, mem_block_size);

    return cci::return_nothing;
  }

  /**
   *  @fn     cci::callback_return_type mem_block_post_wr_cb_func(cci::cci_base_param& _base_param, const cci::callback_type& cb_reason, cci::cci_base_param* _addr_lines_base_ptr)
   *  @brief  The implementation of the callback function for post writes to the memory block
   *  @param  _base_param The CCI parameter for the callback
   *  @param  cb_reason The reason for the callback being called
   *  @param  _addr_lines_base_ptr  Pointer to the base of the address lines for the memory block
   *  @return The status result of the function execution
   */
  cci::callback_return_type mem_block_post_wr_cb_func(cci::cci_base_param & _base_param,
                                                           const cci::callback_type & cb_reason,
                                                           cci::cci_base_param * _addr_lines_base_ptr) {
    XREPORT("[PROCESSOR mem_block_post_wr_cb] : Parameter Name : "
            << _base_param.get_name() << "\tParameter Value : "
            << _base_param.get_cci_value().json_serialize());

    XREPORT("[PROCESSOR mem_block_post_wr_cb] : Parameter Name : "
            << _addr_lines_base_ptr->get_name() << "\tParameter Value : "
            << _addr_lines_base_ptr->get_cci_value().json_serialize());

    mem_block_size = atoi(_base_param.get_cci_value().json_serialize().c_str());
    total_addr_lines = atoi(
            _addr_lines_base_ptr->get_cci_value().json_serialize().c_str());

    TestCondition(total_addr_lines, mem_block_size);

    return cci::return_nothing;
  }

  /**
   *  @fn     void TestCondition(int lines, int memory_size)
   *  @brief  The function validates the consistency of the system base on the two input fed to it
   *  @param  lines The number of address lines
   *  @param  memory_size The size of the memory
   *  @return void
   */
  void TestCondition(int lines, int memory_size) {
    static int check = 0;

    if (memory_size < (1 << lines) - 1) {
      if (check == 0) {
        XREPORT("[PROCESSOR fn] : User may proceed with the present"
                " configuration");
      } else {
        XREPORT("[PROCESSOR fn] : The number of address lines can now address"
                " the current memory size");
      }

      check = 0;
    } else {
      XREPORT("[PROCESSOR fn] : The address lines cannot address the current"
              " memory size");

      check = 1;
    }
  }

 private:
  // Declare parameter_owner instance
  ex15_address_lines_register addr_lines_module;  ///< Declare address line register
  ex15_memory_block memory_block_module;  ///< Memory block module

  cci::cci_broker_if* processor_BrokerIF;  ///< Pointer to the broker interface

  int total_addr_lines; ///< The total number of address lines
  int mem_block_size; ///< The size of the memory block

  // CCI Base parameter pointer
  cci::cci_base_param* addr_lines_base_ptr;  ///< Pointer to the address lines
  cci::cci_base_param* mem_size_base_ptr;  ///< Pointer to the base of the memory size

  // Callback Adaptor Objects
  cci::shared_ptr<cci::callb_adapt> addr_lines_post_wr_cb; ///< Address lines callback adapter object
  cci::shared_ptr<cci::callb_adapt> mem_block_post_wr_cb;  ///< Memory block callback adapter object
};
// ex15_processor

#endif  // EXAMPLES_EX15_INTER_PARAMETER_VALUE_CONSTRAINTS_EX15_PROCESSOR_H_
