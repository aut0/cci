//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2008 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
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

#ifndef __ModuleA_h__
#define __ModuleA_h__

#include "globals.h"

#include "greencontrol/config.h"

#include "ModuleB.h"
#include "ModuleAB.h"
#include "ModuleTests.h"

/// Module having a private API: params are private expect explicitely excluded
/**
 Also contains different sub-modules, 
 e.g. - modules without private API 
        (which must be private automatically)
      - modules with further private APIs
        (which must connect to the API of this module which then may prevent
        public parameters from being public)
 */
class ModuleA
: public sc_core::sc_module
{
public:
  
  SC_HAS_PROCESS(ModuleA);
  /// Constructor
  ModuleA(sc_core::sc_module_name name)
  : sc_core::sc_module(name),
    m_privApi(this, "mA_int_param", "ModuleAB.mAB_int_param", "ModuleTests.myAddTestParamPub",
              "ModuleTests.myAddedParam1Pub", "ModuleTests.pub_par", END_OF_PUBLIC_PARAM_LIST),
    submod("ModuleAB"),
    submodP("ModuleAA"),
    submodTests("ModuleTests"),
    int_param("mA_int_param", 45),
    int_param2("int_param2", 245)
  {
    SC_THREAD(main_action);
    gs::gs_param<double> tmp_par("tmp_par");
    // Take care to destroy pr_Api after all child modules have been destroyed!
    //gs::cnf::GCnf_private_Api *pr_Api = gs::cnf::GCnf_Api::create_private_ApiInstance(this);
    
    mApi = gs::cnf::GCnf_Api::getApiInstance(this);
  }
  
  void main_action();
  
protected:
  /// Private API
  gs::cnf::GCnf_private_Api  m_privApi;
  
  /// API got with static function - will be the same as the m_privApi!
  gs::cnf::cnf_api *mApi;
  
  /// Sub-Module not handling with private
  ModuleAB submod;

  /// Sub-Module with another private API
  ModuleB submodP;
  
  /// Sub-Module for tests
  ModuleTests submodTests;
  
  /// Example parameter.
  gs::gs_param<int>   int_param;
  gs::gs_param<int>   int_param2;
};


void ModuleA::main_action() {
  std::cout << name() << ": BEGIN main_action" << std::endl;
  //cout << int_param << endl;
  gs::gs_param<double> tmp_par("tmp_par2");
  wait (1, sc_core::SC_US);

  std::vector<std::string> lst = mApi->getParamList();
  std::cout << std::endl << " *** Param List of "<< name() <<" ***:" <<std::endl; 
  gs::ctr::show_vector(lst);
  std::cout << std::endl;
  
  std::cout << name() << ": END main_action" << std::endl;
}

#endif
