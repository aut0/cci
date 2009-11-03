//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2007 : GreenSocs Ltd
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

#include "testip6.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

#define IP6test(a) { cout << "IP6 test: " << a <<endl; }
#define SPACE {cout << endl; }

void TestIP6::main_action() {

  // /////////////////////////////////////////////////////////////////////////
  // ///////    sc_int< W >:   Full Test of all operators    /////////////////////////
  // /////////////////////////////////////////////////////////////////////////

  IP6test("scint_param (initial)="<<scint_param);

  // original compare variable
  sc_int<32> orig_sc_int;
  
  unsigned int a = 1000;
  orig_sc_int = a;
  scint_param = a;
  IP6test("'scint_param = (unsigned int) 1000' (orig="<<orig_sc_int<<") scint_param="<<scint_param);
  
  orig_sc_int = scint_param;
  orig_sc_int = 500 - orig_sc_int;
  scint_param = 500 - scint_param.getValue(); // TODO?
  IP6test("'scint_param = 500 - scint_param' : (orig="<<orig_sc_int<<") scint_param="<<scint_param);


  orig_sc_int += 1500;
  scint_param += 1500;
  IP6test("'scint_param += 1500' : (orig="<<orig_sc_int<<") scint_param="<<scint_param);

  IP6test("scint_param++="<<scint_param++);
  IP6test("scint_param="<<scint_param);

  IP6test("++scint_param="<<++scint_param);
  IP6test("scint_param="<<scint_param);

  IP6test("scint_param--="<<scint_param--);
  IP6test("scint_param="<<scint_param);

  IP6test("--scint_param="<<--scint_param);
  IP6test("scint_param="<<scint_param);

  SPACE;

  scint_param += 1500;
  IP6test("'scint_param += 1500' : scint_param="<<scint_param);

  scint_param -= 200;
  IP6test("'scint_param -= 200' : scint_param="<<scint_param);

  scint_param /= 4;
  IP6test("'scint_param /= 4' : scint_param="<<scint_param);

  scint_param *= 3;
  IP6test("'scint_param *= 3' : scint_param="<<scint_param);

  scint_param %= 6;
  IP6test("'scint_param %= 6' : scint_param="<<scint_param);

  scint_param ^= 5;
  IP6test("'scint_param ^= 5' : scint_param="<<scint_param);

  scint_param &= 60;
  IP6test("'scint_param &= 60' : scint_param="<<scint_param);

  scint_param |= 3;
  IP6test("'scint_param |= 3' : scint_param="<<scint_param);

  int u = scint_param.getValue(); u <<= 2; // TODO?
  scint_param <<= 2;
  IP6test("'scint_param <<= 2' : scint_param ("<<u<<") ="<<scint_param);

  u >>= 2;
  scint_param >>= 2;
  IP6test("'scint_param >>= 2' : scint_param ("<<u<<") ="<<scint_param);

  SPACE;

  IP6test("'scint_param != 6' ");
  if (scint_param.getValue() != 6)  
    cout<<"               true" <<endl;
  else
    cout<<"               false"<<endl;

  IP6test("'scint_param != 4' ");
  if (scint_param.getValue() != 4)  
    cout<<"               true" <<endl;
  else
    cout<<"               false"<<endl;


}
