// LICENSETEXT
//
//   Copyright (C) 2009-2010 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by:
//    Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//    Mark Burton, mark@greensocs.com
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


#include <cci.h>
#include "cci_api.h"

namespace cci {
  namespace cnf {


  cci_cnf_broker* singleton_api = NULL;

  cci_cnf_broker* get_cnf_broker_instance(sc_core::sc_module* mod) {
    if (!singleton_api) singleton_api = new gs_cci_cnf_broker();
    if (mod != NULL) return cci_broker_manager::search_for_broker(mod);
    CCI_CNF_DUMP("   got global broker "<< typeid(singleton_api).name()<<" 0x"<<(std::hex)<<singleton_api<<(std::dec));
    return singleton_api;
  }

  
  } // end namespace cci
} // end namespace cci
