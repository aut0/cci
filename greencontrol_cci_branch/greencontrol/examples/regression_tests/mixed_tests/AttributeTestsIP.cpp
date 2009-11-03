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

#include "AttributeTestsIP.h"

using namespace std;
using namespace sc_core;

#define TEST(a) cout << "AttributeTestsIP test: " << a <<endl

void AttributeTestsIP::main_action() {

  wait(2, SC_MS);
  
  cout << "."<<endl;
  cout << "."<<endl;
  DEMO_TRACE(name(), "This is the main action in AttributeTestsIP");

  gs::gs_param<int> p("param", 34);

  if (p.add_param_attribute(gs::cnf::param_attributes::config))
    TEST("PASSED: if config attribute added");
  else
    TEST("FAILED: adding config attribute");

  if(p.add_param_attribute(gs::cnf::param_attributes::output))
    TEST("PASSED: if output attribute added");
  else
    TEST("FAILED: adding output attribute");

  if(p.add_param_attribute(gs::cnf::param_attributes::temp))
    TEST("PASSED: if temp attribute added");
  else
    TEST("FAILED: adding temp attribute");
  
  if (p.has_param_attribute(gs::cnf::param_attributes::config))
    TEST("PASSED: p.has_param_attribute(gs::cnf::param_attribute::config) returned true.");
  else
    TEST("FAILED: p.has_param_attribute(gs::cnf::param_attribute::config) returned false.");

  if(p.has_param_attribute(gs::cnf::param_attributes::runtime_config))
    TEST("FAILED: p.has_param_attribute(gs::cnf::param_attribute::runtime_config) returned true.");
  else
    TEST("PASSED: p.has_param_attribute(gs::cnf::param_attribute::runtime_config) returned false.");
  
  std::set<gs::cnf::param_attribute> s = p.get_param_attributes();
  if (s.size() == 3)
    TEST("PASSED: p.get_all_param_attributes() size = 3 == " << s.size());
  else
    TEST("FAILED: p.get_all_param_attributes() size = 3 != " << s.size());
  
  
  if(p.remove_param_attribute(gs::cnf::param_attributes::output))
    TEST("PASSED: Attribute was removed");
  else
    TEST("FAILED: Attribute cannot be removed");
  if(p.has_param_attribute(gs::cnf::param_attributes::output))
    TEST("FAILED: if output attribute is set");
  else
    TEST("PASSED: output attribute is not set");
  

  s = p.get_param_attributes();
  if (s.size() == 2)
    TEST("PASSED: p.get_all_param_attributes() size = 2 == " << s.size());
  else
    TEST("FAILED: p.get_all_param_attributes() size = 2 != " << s.size());
  
  p.remove_all_param_attributes();  
  s = p.get_param_attributes();
  if (s.size() == 0)
    TEST("PASSED: all removed: p.get_all_param_attributes() size = 0 == " << s.size());
  else
    TEST("FAILED: NOT all removed: p.get_all_param_attributes() size = 0 != " << s.size());
  
  
  cout << "."<<endl;
  cout << "."<<endl;
  cout << "."<<endl;
}
