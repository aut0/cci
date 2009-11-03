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

#include "OutputPluginEnabledParamsMod.h"

void EnaMod::main_action() {
  std::cout << name() << " main_action" << std::endl;
  
  // ////////////////////////////////////////////////////////////////////// //
  // /////////// Tests with new created output plugin 'any' /////////////// //
  
  std::cout << "create output plugin 'any'" << std::endl;
  gs::av::OutputPlugin_if* op = mAVApi->create_OutputPlugin(gs::av::STDOUT_OUT, "any");
  REGR_TEST("PASSED if created output plugin created the parameters \n\
             - EnaMod.main_action.STDOUT_OUT_any.EnaMod.ExplicitParamB, \n\
             - EnaMod.main_action.STDOUT_OUT_any.EnaMod.ImplicitParamB, \n\
             - EnaMod.main_action.STDOUT_OUT_any.top_param_expl_B, \n\
             - EnaMod.main_action.STDOUT_OUT_any.top_param_impl_B \n\
            and observes the existing related ones: \n\
             - EnaMod.ExplicitParamB\n\
             - top_param_expl_B\n\
            to check this, see next tests:");
  
  gs::gs_param<bool>* explicitParamB_enabled = mCnfApi->get_gs_param<bool>("EnaMod.main_action.STDOUT_OUT_any.EnaMod.ExplicitParamB");
  std::cout << std::endl;
  std::cout << "change observation enabled param 'EnaMod.main_action.STDOUT_OUT_any.EnaMod.ExplicitParamB' = true" << std::endl;
  *explicitParamB_enabled = true;
  REGR_TEST("PASSED if message 'observe parameter EnaMod.ExplicitParamB' but already observed.");
  std::cout << std::endl;

  explicitParamB = 10;
  explicitParamB = 20;
  REGR_TEST("PASSED if changes =10, =20 outputted to STDOUT for param 'EnaMod.ExplicitParamB'.");
  std::cout << std::endl;
  
  std::cout << "change observation enabled param 'EnaMod.main_action.STDOUT_OUT_any.EnaMod.ExplicitParamB' = false" << std::endl;
  *explicitParamB_enabled = false;
  REGR_TEST("PASSED if message 'remove observation of parameter EnaMod.ExplicitParamB'");
  std::cout << std::endl;

  explicitParamB = 30;
  REGR_TEST("PASSED if change =30 NOT outputted to STDOUT for param 'EnaMod.ExplicitParamB'.");
  std::cout << std::endl;
  
  std::cout << "change observation enabled param 'EnaMod.main_action.STDOUT_OUT_any.EnaMod.ImplicitParamB' which should fail." << std::endl;
  gs::gs_param<bool>* implicitParamB_enabled = mCnfApi->get_gs_param<bool>("EnaMod.main_action.STDOUT_OUT_any.EnaMod.ImplicitParamB");
  *implicitParamB_enabled = true;
  REGR_TEST("PASSED if changing failed with message 'The related parameter to an enabled parameter cannot be handled because it is not existing. Enabled parameter has been resetted to false.'.");
  *implicitParamB_enabled = false;
  REGR_TEST("PASSED if nothing happened (no change message for enable param).");
  std::cout << std::endl;

  mCnfApi->getPar("top_param_expl")->setString("50");
  REGR_TEST("PASSED if param 'top_param_expl' change =50 is outputted to stdout by default output plugin.");
  std::cout << std::endl;

  std::cout << "Test disable" << std::endl;
  mCnfApi->getPar("AnalysisPlugin.STDOUT_OUT_default.top_param_expl")->setString("false");
  mCnfApi->getPar("top_param_expl")->setString("60");
  REGR_TEST("PASSED if param 'top_param_expl' change =60 is NOT outputted by default output plugin!");
  std::cout << std::endl;

  // ////////////////////////////////////////////////////////////////////// //
  // /////////// Tests with default output plugin ///////////////////////// //
  
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;

  gs::gs_param<bool>* explicitParam_enabled = mCnfApi->get_gs_param<bool>("AnalysisPlugin.STDOUT_OUT_default.EnaMod.ExplicitParam");
  std::cout << std::endl;
  std::cout << "change observation enabled param 'AnalysisPlugin.STDOUT_OUT_default.EnaMod.ExplicitParam' = true" << std::endl;
  *explicitParam_enabled = true;
  REGR_TEST("PASSED if message 'observe parameter EnaMod.ExplicitParam' but already observed.");
  std::cout << std::endl;
  
  explicitParam = 11;
  explicitParam = 21;
  REGR_TEST("PASSED if changes =11, =21 outputted to STDOUT for param 'EnaMod.ExplicitParam'.");
  std::cout << std::endl;
  
  std::cout << "change observation enabled param 'AnalysisPlugin.STDOUT_OUT_default.EnaMod.ExplicitParam' = false" << std::endl;
  *explicitParam_enabled = false;
  REGR_TEST("PASSED if message 'remove observation of parameter EnaMod.ExplicitParam'");
  std::cout << std::endl;
  
  explicitParam = 32;
  REGR_TEST("PASSED if change =32 NOT outputted to STDOUT for param 'EnaMod.ExplicitParam'.");
  std::cout << std::endl;
  
  std::cout << "change observation enabled param 'AnalysisPlugin.STDOUT_OUT_default.EnaMod.ImplicitParam' which should fail." << std::endl;
  gs::gs_param<bool>* implicitParam_enabled = mCnfApi->get_gs_param<bool>("AnalysisPlugin.STDOUT_OUT_default.EnaMod.ImplicitParam");
  *implicitParam_enabled = true;
  REGR_TEST("PASSED if changing failed with message 'The related parameter to an enabled parameter cannot be handled because it is not existing. Enabled parameter has been resetted to false.'.");
  *implicitParam_enabled = false;
  REGR_TEST("PASSED if nothing happened (no change message for enable param).");
  std::cout << std::endl;
  
}
