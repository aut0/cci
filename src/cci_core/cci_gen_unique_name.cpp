// LICENSETEXT
//
//   Copyright (C) 2011 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by:
//    Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//
//
// The contents of this file are subject to the licensing terms specified
// in the file LICENSE. Please consult this file for restrictions and
// limitations that may apply.
// 
// ENDLICENSETEXT

#include "cci_core/cci_gen_unique_name.h"
#include <sstream>
#include <cassert>

inline
const char* cci::cci_gen_unique_name(const char* name) {
  
  // static (system-wide) map <name, current counter - 1>
  static std::map<std::string, int> unique_name_map;
  
  std::pair<std::map<std::string,int>::iterator,bool> ret;
  
  // TODO: add error reporting
  assert(name && *name && "cci_gen_unique_name: empty name not allowed!");

  // Try to insert name
  ret = unique_name_map.insert(std::pair<std::string, int>(name, 0));
  if (ret.second==false)
  {
    // insert failed, so name clash
    std::stringstream new_name;
    new_name << name << "_" << ret.first->second; // generate new name
    ret.first->second++; // update counter
    // recursive call, to store the new generated name in the map as well!
    cci_gen_unique_name(new_name.str().c_str());
    // return pointer to stored new name which is persistent
    return unique_name_map.find(new_name.str())->first.c_str();
  }
  // insert successful
  return name;
  
}