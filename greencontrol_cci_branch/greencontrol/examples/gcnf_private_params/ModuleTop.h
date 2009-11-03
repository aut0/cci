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

#ifndef __ModuleTop_h__
#define __ModuleTop_h__

#include "globals.h"

#include "greencontrol/config.h"

#include "ModuleA.h"

/// Module doing nothing with private APIs: params are public
class ModuleTop
  : public sc_core::sc_module
  {
  public:
    
    SC_HAS_PROCESS(ModuleTop);
    /// Constructor
    ModuleTop(sc_core::sc_module_name name)
    : sc_core::sc_module(name),
      submod("ModuleA"),
      my_top_int_param("my_top_int_param", 11)
    {
      SC_THREAD(main_action);
      
      mApi = gs::cnf::GCnf_Api::getApiInstance(this);
    }
    
    void main_action() {
      wait (1, sc_core::SC_NS);
      // parameter that should be seen (new param callback) by the ModuleTests observer!
      gs::gs_param<int> normal_pub_runtime_param("normal_pub_runtime_param", 42);
      
      std::vector<std::string> lst = mApi->getParamList();
      std::cout << std::endl << " *** Param List of "<< name() <<" ***:" <<std::endl; 
      gs::ctr::show_vector(lst);
      std::cout << std::endl;
    }
    
  protected:
    /// Sub-Module
    ModuleA submod;

    /// Example parameter.
    gs::gs_param<int>             my_top_int_param;

    /// API got with static function
    gs::cnf::cnf_api *mApi;
};

#endif
