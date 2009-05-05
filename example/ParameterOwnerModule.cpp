//   OSCI CCI WG
//
// LICENSETEXT
//
//   Copyright (C) 2009 : GreenSocs Ltd
// 	 http://www.greensocs.com/ , email: info@greensocs.com
//
//   Developed by :
//   
//   Christian Schroeder <schroeder@eis.cs.tu-bs.de>,
//     Technical University of Braunschweig, Dept. E.I.S.
//     http://www.eis.cs.tu-bs.de
//
//
// 
// ENDLICENSETEXT


#include "ParameterOwnerModule.h"

void ParameterOwnerModule::main_action() {

  wait(1, SC_NS);

  wait(1, SC_US);

  
  wait(1, SC_NS);
  cout << name() << ": set int_param = 100" << endl;
  int_param = 100;
  cout << endl;
  wait(SC_ZERO_TIME);

  /*cout << name() << ": set str_param = 'Hello World!'" << endl;
  str_param = "Hello World!";
  cout << endl;
  wait(SC_ZERO_TIME);*/
  
  
  wait(1, SC_NS);
  cout << name() << ": set int_param = 200" << endl;
  int_param = 200;
  cout << endl;
  wait(SC_ZERO_TIME);
  
  /*cout << name() << ": set str_param = 'Hello Germany!'" << endl;
  str_param = "Hello Germany!";
  cout << endl;
  wait(SC_ZERO_TIME);*/
  
  
  cout << name() << ": Create two new parameters which can be observed with new param callbacks/events and set them" << endl;

  cout << name() << ": unnamed param" << endl;
  cci::cci_param<unsigned char> uchar_param;
  cout << endl;
  wait(SC_ZERO_TIME);
  cout << name() << ": Set the new parameter uchar_param" << endl;
  uchar_param = 'u';
  cout << endl;

  cout << endl << "**** Parameter list: " << endl;
  std::vector<std::string> vec = cci::cci_cnf_Api::getApiInstance(this)->getParamList();
  // Show vector
  std::vector<std::string>::iterator iter;
  std::stringstream ss_show;
  iter = vec.begin();
  while( iter != vec.end() ) {
    if (iter != vec.begin()) ss_show << ", ";
    ss_show << *iter;
    iter++;
  }
  std::cout << "   " << ss_show.str() << std::endl<<std::endl<<std::endl;
  
  wait(SC_ZERO_TIME);
  gs::gs_param<unsigned long long> ull_param("ull_param", 1000ULL);
  std::cout << std::endl;
  wait(SC_ZERO_TIME);
  std::cout << name() << ": Set the new parameter ull_param" << std::endl;
  ull_param = 2000LL;
  std::cout << std::endl;
 
  wait(100, SC_US);
  std::cout << "----------------------------" << std::endl;

}
