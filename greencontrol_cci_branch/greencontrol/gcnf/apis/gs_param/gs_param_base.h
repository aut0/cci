//   GreenControl framework
//
// LICENSETEXT
//
//   Copyright (C) 2007-2009 : GreenSocs Ltd
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

// doxygen comments


#ifndef __GS_PARAM_BASE_H__
#define __GS_PARAM_BASE_H__

// TODO: remove check
#ifndef __ONLY_INCLUDED_BY_CONFIG_H__
#error You must not include this file directly, use config.h instead!
#endif

#include <string>
#include <iostream>
#include <map>
#include <set>

// included by config.h

#include "triple.h"
#include "greencontrol/gcnf/plugin/utils.h"

namespace gs {
namespace cnf {

  
/// Base class for all gs_param template specialisations.
/**
 * Manages the name of the parameter 
 *
 * Requires set and get with the string representation of the value.
 */
class gs_param_base
: public sc_object  
{
protected:

  /// Typedef to store a callback pointer
  typedef pair<bool, boost::shared_ptr<ParamCallbAdapt_b> > observer_and_callback_type;
  /// Typedef for a multimap saving pointers to callback functions with the void* pointer of the observer module as key.
  //typedef std::multimap<void*, boost::shared_ptr<ParamCallbAdapt_b> > observerParamCallback_type;
  /// Typedef for a vector saving observer_and_callback_types; vector to remember the registering order
  typedef std::vector<observer_and_callback_type> callback_list_type;
  
  /// Typedef for the map giving access to the different callback lists
  typedef std::map<callback_type, callback_list_type*> all_callback_lists_type;

public:

  // ////////   constructors, destructor   /////////// //

  /// Constructor with (local or hierarchical) name
  /**
   * - Sets the hierarchical parameter name (default name if name is empty).
   *   (If the parent_array is != NULL: 
   *            hierarchical name is the parent's name + local name)
   * - Sets the API pointer.
   * - Explicit constructor to avoid implicit construction of parameters.
   *
   * @param n  The local (or full hierarchical) parameter name (local: should not but may include points) 
   *           (local: unique inside a module, hierarchical: unique in the system).
   *           May be empty: name will be chosen automatically.
   * @param register_at_db   If this parameter should be registered at the database, 
   *                         default true, 
   *                         only the database itself may use false when creating 
   *                         parameters while adding them.
   * @param parent_array   Array where this parameter should be added as a member.
   * @param force_top_level_name If the given name n should be a top-level name (then no prefeix is attached to the name),
   *                             default false,
   *                             be carefull in using this.
   */
  explicit gs_param_base(const std::string &n, const bool register_at_db = true,
                           gs_param_array* parent_array = NULL, const bool force_top_level_name = false)
  : sc_object(sc_gen_unique_name("gs_param_base")) // fails if hierarchical name: sc_gen_unique_name(n.c_str())
  , m_currently_making_callbacks(false)
  , m_callback_list_delete_enqueued(0)
  , m_destruction_flag(false)
  , m_is_simple_array(false)
  , m_is_extended_array(false)
  , m_register_at_db(register_at_db)
  //, m_parent_array(parent_array)
  , m_update_event_enabled(false)
  {
    GS_PARAM_DUMP("gs_param_base(name='"<<n<<"', register_at_db="<<(unsigned int)register_at_db<<", parent_array="<<(unsigned long) parent_array<<", force_top_level_name="<<(unsigned int)force_top_level_name<<") constructor");

    // TODO: fill m_callback_lists with all callback lists this param has
    m_callback_lists[pre_read]      = &m_callback_list_pre_read;
    m_callback_lists[post_read]     = &m_callback_list_post_read;
    m_callback_lists[pre_write]     = &m_callback_list_pre_write;
    m_callback_lists[post_write]    = &m_callback_list_post_write;
    m_callback_lists[destroy_param] = &m_callback_list_destroy_param;
    
    // Name counter to make unnamed params unique
    static int cnt = 0;

    // Set the api pointer
    sc_core::sc_module *mod = get_parent_sc_module(this);
    m_api = GCnf_Api::getApiInstance(mod);

    if (isHierarchicalParameterName(n) && parent_array != NULL) {
      SC_REPORT_FATAL(name(), "Do not use hierarchical parameter names for Extended Parameter Array members!");
    }

    // Set the name
    std::stringstream ss;
    //   normally
    if(parent_array == NULL) {
      if (!force_top_level_name) ss << get_name_prefix(); 
      if (n.length() == 0)       ss << "no_name_" << cnt++;
      else                       ss << n;
      GS_PARAM_DUMP("Set parameter name = '"<< ss.str().c_str()<<"',");
    }
    //   as a member of an array
    else {
      if (force_top_level_name)  SC_REPORT_WARNING(name(), "You should not create parameter array members with enabled force_top_level_name! force_top_level_name be ignored.");
      ss << parent_array->getName() << SC_NAME_DELIMITER;
      if (n.length() == 0)       ss << "no_name_" << cnt++;
      else                       ss << n;
      GS_PARAM_DUMP("Set parameter array member name = '"<< ss.str().c_str()<<"',");
    }
    m_par_name = ss.str();
    
    if (parent_array != NULL) {
      GS_PARAM_DUMP("Init this parameter as a member of the Extended Parameter Array '"<< parent_array->getName().c_str()<<"'."); 
      if (!parent_array->addMember(*this)) {
        GS_PARAM_DUMP("Adding this parameter to the array failed!");
      }
    }

  }

  /// Destructor.
  virtual ~gs_param_base() { // force to be polymorphic
    // moved to callback method in the array class:
    //if (m_parent_array != NULL) {
    //  GS_PARAM_DUMP("Remove this parameter as a member of the Extended Parameter Array '"<< m_parent_array->getName().c_str()<<"'."); 
    //  m_parent_array->removeMember(this->getName());
    //}
    // delete pointer to this parameter in all callback adapters called by this parameter
    callback_list_type::iterator it;
    all_callback_lists_type::iterator map_iter;
    for ( map_iter=m_callback_lists.begin(); map_iter != m_callback_lists.end(); map_iter++ ) {
      for (it =  map_iter->second->begin(); it != map_iter->second->end(); ++it) {
        (*it).second->caller_param=NULL;
      }
    }
  }
    
  /// Destruction method to be called by the gs_param destructor
  /**
   * Sets the destruction flag true and
   * call remove for the database.
   *
   * The destruction flag lets the observers know that this object will be destoyed.
   */
  void destruct_gs_param() {
    // set destruction flag
    m_destruction_flag = true;

    GS_PARAM_DUMP("Destructing param '"<< m_par_name.c_str()<<"'.");

    make_destroy_param_callbacks();

    // remove this from database
    if (m_register_at_db)
      m_api->removePar(this);
  }
  
  // ////////   set, get   /////////// //


  /// Sets the value of this parameter given by a string.
  /** 
   * @param str the new value represented as a string.
   * @return If setting was successful.
   */
  virtual bool setString(const std::string &str) = 0;
  /// Deprecated: use setString(stringval) instead
  void set(const std::string &str) {
    DEPRECATED_WARNING(name(), "DEPRECATED: set(stringval) is deprecated! Use 'setString(stringval)' instead.");
    setString(str);
  }
  
  /// Get the string representation of this parameter's value.
  /**
   * @return  The value of this parameter represented as a string.
   */
  virtual const std::string& getString() const = 0;
  /// Deprecated: use getString(val) instead
  const std::string& get() const {
    DEPRECATED_WARNING(name(), "DEPRECATED: get() is deprecated! Use 'getString()' instead.");
    return getString();
  }

  /// Get this parameter's value converted to the type.
  /**
   * This function can be called if the user knows the type.
   *
   * The string value is converted to the user-chosen template type
   * by using the gs_param template specialization.
   *
   * This function does only work for types of gs_param<T>, not for 
   * any kind of gs_param_base, e.g. parameter arrays.
   *
   * @param value    The value of the parameter will be written to this parameter
   * @return         If the convertion was successfull.
   */
  template<class T>
  const bool getValue(T &value) const {
    make_pre_read_callbacks();
    //bool res = gs_param<T>::static_deserialize(value, this->get());
    //make_post_read_callbacks(); // TODO possible but deactived
    //return res;
    return gs_param<T>::static_deserialize(value, this->getString());
  }
  
  /// Get a parameter's value. Independent of the implicit or explicit status.
  /**
   * Better use getValue(parname, value) to avoid copying of the value.
   *
   * This function can be called if the user knows the type.
   *
   * The string value is converted to the user-chosen template type
   * by using the gs_param template specialization.
   *
   * This function does only work for types of gs_param<T>, not for 
   * any kind of gs_param_base, e.g. parameter arrays.
   *
   * @return         Value of the parameter, converted to the user-chosen type.
   */
  template<class T>
  const T getValue() const {
    make_pre_read_callbacks();
    static T val;
    gs_param<T>::static_deserialize(val, this->getString());
    //make_post_read_callbacks(); // TODO possible but deactivated
    return val;
  }
  
  
  /// Get a parameter pointer of the gs_param<T> type defined by the caller (does a dynamic_cast).
  /**
   * @return  Pointer to the parameter object (NULL if not existing or wrong type).
   */ 
  template<class T>
  gs_param<T>* get_gs_param() {
    return dynamic_cast<gs_param<T>*>(this);
  }
  
  // ////////   Type   /////////// //

  /// Returns the string representation of the type this parameter stores
  virtual const std::string getTypeString() const {
    return string("PARTYPE_NOT_AVAILABLE");
  }
  
  /// Returns the type identifying enum of the type this parameter stores
  virtual const Param_type getType() const{
    return PARTYPE_NOT_AVAILABLE;
  }

  
  // //////  param_attributes  ////// //

  /// Adds a param attribute.
  /**
   * @param attr  Param attribute that has to be added.
   * @return If successfully added (false if already contained).
   */
  bool add_param_attribute(const param_attribute attr) {
    GS_PARAM_DUMP("Add param attribute '"<<(unsigned int) attr<<"'");
    return (m_attributes.insert(attr)).second;
  }
  
  /// If a special param attribute is set.
  /**
   * @param attr  Param attribute that has to be checked.
   * @return If param attribute is set.
   */
  bool has_param_attribute(const param_attribute attr) const {
    return (m_attributes.find(attr) != m_attributes.end());
  }
  
  /// Returns a set containing all param attributes that are set for this param.
  /**
   * @return Set of contained param attributes.
   */
  const std::set<param_attribute> get_param_attributes() const {
    return m_attributes;
  }
  
  /// Removes a param attribute.
  /**
   * @param attr  Param attribute that should be removed.
   * @return If successfully removed (false if it was not contained).
   */
  bool remove_param_attribute(param_attribute attr) {
    GS_PARAM_DUMP("Remove param attribute '"<<(unsigned int) attr<<"'");
    return (m_attributes.erase(attr) > 0);
  }
  
  /// Removes all param attributes of this param.
  /**
   */
  void remove_all_param_attributes() {
    GS_PARAM_DUMP("Remove all param attributes");
    m_attributes.clear();
  }
  
  
  // ////////   diverse   /////////// //

  
  /// Get the name of this parameter.
  /**
   * @return   Name of the parameter.
   */
  const std::string &getName() const { 
    return m_par_name; 
  }

  // use GCnf_Api::getApiInstance(...) instead!!
  // removed for clearness, prevents from getting an API from a private parameter!
  /// Get a Pointer to the parameter's GreenConfig API.
  /**
   * @return  Pointer to the config API.
   */
  /*cnf_api* getApi() {
    return m_api;
  }*/
  
  /// Returns a void* pointer to the value
  /**
   * Can be used to cast the value to a base class of the value,
   * e.g. cast a sc_int<length> to sc_int_base.
   *
   * This is a virtual function, if possible use getValue instead!
   *
   * @return void* pointer to the value
   */
  virtual const void* get_value_pointer() const = 0;

  /// Returns the destrcution flag status: if this parameter is to be destroyed (needed for observers)
  const bool is_destructing() const {
    return m_destruction_flag;
  }
  
  /// Returns if this is a Simple Parameter Array of type T*
  const bool is_simple_array() const {
    return m_is_simple_array;
  }
  
  /// Returns if this is an Extended Parameter Array
  const bool is_extended_array() const {
    return m_is_extended_array;
  }
  
  // ///////////   Observer and callbacks   //////////////


  // //////////////////////////////////////////////////////////////////// //
  // ///////   Callback handling methods   ////////////////////////////// //
  
  /// Registers an observer callback function (with the signature of callback_func_ptr). Use the GC_REGISTER_PARAM_CALLBACK macro!
  /**
   * Several callbacks may be registered. Even several callbacks to the same method
   * in the same object can be registered!
   *
   * The callback works even during initialize-mode (elaboration time).
   *
   * Inside the callback functions no waits are allowed!
   *
   * The callback registration will fail if a wrong callback type is given.
   *
   * Note: For legacy support, the old macro uses the callback type post_write_and_destroy which 
   *       registers for post_write AND destroy_param.
   *       In any legacy code using this function, just add gs::cnf::post_write_and_destroy as 
   *       parameter to keep the legacy behavior.
   *
   * Use the callbacks to register observing analysis methods in other classes.
   * So first get a reference to this parameter (TODO: future).
   * 
   * The user may register any methods as callback functions which have
   * the following signature:
   * \code
   * void method_name(gs_param_base& changed_param);
   * \endcode
   *
   * Usage example:
   * \code
   * class MyIP_Class
   * : public sc_core::sc_module
   * {
   * public:
   *   // some code [...]
   *   
   *   gs_param<int> my_param;
   *
   *   MyIP_Class(sc_core::sc_module_name name)
   *    : sc_core::sc_module(name),
   *      my_param("my_param", 10) 
   *   { //...
   *   }
   *
   *   // Example code to register callback function
   *   void main_action() {
   *     // some code, parameters etc...
   *     GC_REGISTER_PARAM_CALLBACK(&my_param, MyIP_Class, config_callback);
   *   }
   *
   *   // Callback function with default signature.
   *   void config_callback(gs_param_base& changed_param) {
   *     // some action
   *   }
   * };
   * \endcode
   *
   * @param callb      boost shared pointer to the ParamCallbAdapt_b object which contains
   *                   the object pointer and the member function pointer.
   * @param type       Type the callback shall be, DEPRECATED default is post_write_and_destroy.
   *                   Allowed callback types: pre_read, post_read, pre_write, post_write, destroy_param, post_write_and_destroy
   * @return           boost shared pointer to the callback adapter (e.g. to be used for unregister calls).
   *
   * <em>Macro GC_REGISTER_PARAM_CALLBACK</em>
   *
   * Creates new ParamCallbAdapt object and registers it at registerCallback.
   *
   * Usage:
   * \code
   * GC_REGISTER_PARAM_CALLBACK(&my_param, class_name, callback_method_name)
   * \endcode
   * Example:
   * \code
   * GC_REGISTER_PARAM_CALLBACK(&my_param, MyIP,       config_callback)
   * \endcode
   */
  virtual boost::shared_ptr<ParamCallbAdapt_b> registerParamCallback(boost::shared_ptr<ParamCallbAdapt_b> callb, callback_type type = gs::cnf::post_write_and_destroy/*DEPRECATED, TODO: remove this default!*/) {
    GS_PARAM_CALLBACK_DUMP("registerParamCallback(shared_ptr<ParamCallbAdapt_b>)");      
    GS_PARAM_CALLBACK_DUMP("   param '"<<callb->get_caller_param()->getName()<<"' registerParamCallback(ParamCallbAdapt callb="<<callb->get_id()<<", type="<<callback_type_to_string(type)<<")");
    // Add function pointer to callbacks
    switch (type) {
      case pre_read:
      //case post_read: // not yet supported
      case pre_write:
      case post_write:
      case destroy_param:
        m_callback_lists[type]->push_back( observer_and_callback_type(true, callb) );
        break;
      // This is the legacy default case
      case post_write_and_destroy:
        m_callback_list_destroy_param.push_back( observer_and_callback_type(true, callb) );
        m_callback_list_post_write.push_back( observer_and_callback_type(true, callb) );
        break;
      default: // create_param
        std::stringstream ss; ss << "Given callback type ("<<callback_type_to_string(type)<<") cannot be handled by a gs_param!";
        GS_PARAM_CALLBACK_DUMP("   "<< ss);
        SC_REPORT_WARNING(GCNF_SC_REPORTER("callback"), ss.str().c_str());
    }
    //show_callbacks();
    return callb;
  }

  /// <code>GC_HAS_CALLBACKS();</code> creates a vector for the module where all parameter callback adapters are stored when being created by the macro GC_REGISTER_PARAM_CALLBACK. To be used by a user module in its (e.g. private) class body.
#define GC_HAS_CALLBACKS()  \
  std::map<void*, boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> > gcnf_ParamCallbAdapt_list;  \
  boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> gc_add_ParamCallbAdapt(boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> cba) {     \
    gcnf_ParamCallbAdapt_list.insert(std::pair<void*, boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> >(((void*)cba.get()), cba));               \
    return cba;   \
  }
  
  /// Makro for registering callback functions (for post_write and destroy callbacks), usage: GC_REGISTER_PARAM_CALLBACK(&my_param, MyIP_Class, config_callback);, better use GC_REGISTER_TYPED_PARAM_CALLBACK
#define GC_REGISTER_PARAM_CALLBACK(param, class, function)                \
  (param)->registerParamCallback( gc_add_ParamCallbAdapt(boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b>(new ::gs::cnf::ParamCallbAdapt<class>(this, &class::function, this, (param)))), gs::cnf::post_write_and_destroy )

  /// Makro for registering a typed callback functions, usage: GC_REGISTER_PARAM_TYPED_CALLBACK(&my_param, gs::cnf::callback_type, MyIP_Class, config_callback, type);
#define GC_REGISTER_TYPED_PARAM_CALLBACK(param, type, class, function)                \
(param)->registerParamCallback( gc_add_ParamCallbAdapt(boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b>(new ::gs::cnf::ParamTypedCallbAdapt<class>(this, &class::function, this, (param)))), type )
  
  /// <code>GC_UNREGISTER_CALLBACKS();</code> removes all parameter callback adapters of the module which results in unregistering them at the caller parameters.
#define GC_UNREGISTER_CALLBACKS()                           \
  for (std::map<void*, boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> >::iterator iter = gcnf_ParamCallbAdapt_list.begin(); \
       iter != gcnf_ParamCallbAdapt_list.end(); iter++) {   \
    (*iter).second->unregister_at_parameter(); }            \
  gcnf_ParamCallbAdapt_list.clear()
  
  // removes the callback adapter out of the vector and deletes it
  // the user must not directly delete the callback adapter
  /// <code>GC_UNREGISTER_CALLBACK(boost callbackAdaptPtr)</code> unregisters the callback belonging to the overgiven callback adapter pointer.  
#define GC_UNREGISTER_CALLBACK(callbackAdaptPtr)   \
  { \
    std::map<void*, boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> >::iterator iter__unusual_name_28476394 = gcnf_ParamCallbAdapt_list.find((void*) static_cast< ::gs::cnf::ParamCallbAdapt_b* >((callbackAdaptPtr).get())); \
    if (iter__unusual_name_28476394 != gcnf_ParamCallbAdapt_list.end()) {   \
      (callbackAdaptPtr)->unregister_at_parameter();   \
      gcnf_ParamCallbAdapt_list.erase(iter__unusual_name_28476394);         \
    } else SC_REPORT_WARNING("GC_UNREGISTER_CALLBACK macro", "ParamCallbAdapt not found in list! Cannot be unregistered!"); \
  }
  
  /// Unregisters the callback
  /**
   * Unregisters the one callback in each callback list.
   *
   * The callback adapter is NOT deleted because this should be done by the
   * surrounding shared pointer!
   *
   * This may be used by a user module which stored the shared pointer to a
   * specific callback or is used by the destructor of the param callback adapter.
   *
   * @param callb  Parameter callback adapter
   * @return       If the callback adapter existed in this parameter.
   */
  virtual bool unregisterParamCallback(ParamCallbAdapt_b *callb) {
    //show_callbacks();
    GS_PARAM_CALLBACK_DUMP("param '"<<m_par_name.c_str()<<"' unregisterParamCallback(ParamCallbAdapt callb="<<callb->get_id()<<")");
    bool result = false;
    callback_list_type::iterator it;
    all_callback_lists_type::iterator map_iter;
    for ( map_iter=m_callback_lists.begin(); map_iter != m_callback_lists.end(); map_iter++ ) {
      for ( it=map_iter->second->begin(); it != map_iter->second->end(); it++ ) {
        if (((*it).second).get() == callb) {
          conditional_callback_remove(*map_iter->second, it);
          result = true;
          break; // must break here, because callback remove altered the iterated map!
        }
      }
    }
    return result;
  }
  
  /// Unregisters all callbacks that belong to the observer and deletes the belonging callback adapter objects.
  /**
   * Unregisters all callbacks that belong to the observer.
   * Deletes the callback adapter object(s).
   *
   * This is used by the unregister macro performed by the config plugin.
   *
   * @param observer  void* pointer to the observer object which registered the parameter callbacks
   * @return          If there were any callbacks that had to be removed or not.
   */
  virtual bool unregisterParamCallbacks(void* observer) {
    //show_callbacks();
    GS_PARAM_CALLBACK_DUMP("param '"<<m_par_name<<"' unregisterParamCallbacks(observer)");
    bool success = false;
    // Remove all function pointer from callbacks variable that have the key observer and delete the adapter object(s)
    callback_list_type::iterator it;
    bool found = false; // if this do loop did find a callback
    do {
      found = false;
      all_callback_lists_type::iterator map_iter;
      for ( map_iter=m_callback_lists.begin(); map_iter != m_callback_lists.end(); map_iter++ ) {
        for ( it=map_iter->second->begin(); it != map_iter->second->end(); it++ ) {
          if (((*it).second->get_observer()) == observer) {
            success = true; // any observer removed
            found = true;
            conditional_callback_remove(*map_iter->second, it);
            break;
          }
        }
      }
    } while(found); // as long as there might be another one
    return success;
  }

  /// Operator used for StatCalc Calculator (GreenAV)
  triple operator+(const gs_param_base &second) { return triple("+", *this, second); }
  /// Operator used for StatCalc Calculator (GreenAV)
  triple operator-(const gs_param_base &second) { return triple("-", *this, second); }
  /// Operator used for StatCalc Calculator (GreenAV)
  triple operator/(const gs_param_base &second) { return triple("/", *this, second); }
  /// Operator used for StatCalc Calculator (GreenAV)
  triple operator*(const gs_param_base &second) { return triple("*", *this, second); }

  
  /// Get update event which is notified on parameter changes.
  /** 
   * Returns the event which is notified when the parameter changes.
   *
   * On the first call the callback function doing the event notify
   * is registered at this parameter.
   *
   * For callback registering please use
   * \code
   * GC_REGISTER_PARAM_CALLBACK(&my_gs_param, class, function);
   * \endcode
   *
   * @return  Event which is notified when this parameter changes.
   */
  sc_event& getUpdateEvent() {
    if (!m_update_event_enabled) {
      GS_PARAM_CALLBACK_DUMP("Param '"<<getName()<<"': create callback for update event");
      m_update_event_callbAdapt = boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b>(new ::gs::cnf::ParamCallbAdapt<gs_param_base>(this, &gs_param_base::update_event_callback, this, (this)));
      registerParamCallback( m_update_event_callbAdapt, post_write );
      GS_PARAM_CALLBACK_DUMP("  calback adapter = "<< m_update_event_callbAdapt->get_id());
      m_update_event_enabled = true;
    }
    return m_update_event;
  }
  
  /// Returns if this param has registered callbacks
  bool has_callbacks() { return ! ( m_callback_list_pre_read.empty() && m_callback_list_post_read.empty() &&
                                    m_callback_list_pre_write.empty() && m_callback_list_post_write.empty() &&
                                    m_callback_list_destroy_param.empty() ); } // more performant than iterating through m_callback_lists
  
  /// INTERNAL Callback function which does the event notify. Registered only if event needed!
  /**
   * This callback function is registered at this parameter the first
   * time the function getUpdateEvent is being called. This reduces the
   * overhead to zero if no update event has been asked for by the user.
   *
   * @param changed_param param that has been changed .
   */
  void update_event_callback(gs_param_base& changed_param) {
    m_update_event.notify();
  }
  
protected:

  /*void show_callbacks() {
    TODO: show all m_callback_lists members
    cout << endl << "**** Callback map for parameter "<<m_par_name << " - size = "<<m_callback_list.size() << endl;
    callback_list_type::iterator it;
    printf("   observer   -  callb adapt\n");
    for ( it=m_callback_list.begin() ; it != m_callback_list.end(); it++ ) {
      printf("   0x%x - 0x%x\n", (unsigned) (*it).first, (unsigned) ((*it).second).get());
    }        
    cout << endl;
  }*/
  
  /// For name creation in constructor (and for debug). Responsible for the top-level name.
  /**
   * Returns the name of the parent sc_core::sc_module with 
   * an SC_NAME_DELIMITER as postfix.
   * If no parent can be found, this is top-level
   * and returns the toplevel (empty) string.
   *
   * @return  Parent sc_core::sc_module name + SC_NAME_DELIMITER or top-level name (empty).
   */
  const std::string get_name_prefix() {
    std::string ret;
    sc_core::sc_module *mod = get_parent_sc_module(this);
    if (mod != NULL) {
      ret = mod->name();
      ret += SC_NAME_DELIMITER;
      GS_PARAM_DUMP("Setting parent name: '"<< ret.c_str()<< "'");
    } else {
      ret = ""; // old: "N/A"; 
      GS_PARAM_DUMP("Setting parent name: Parameter is at top-level (has no parent module)!");
    }
    return ret;
  }

protected:
  /// Makes the pre read callbacks.
  inline void make_pre_read_callbacks() const {
    GS_PARAM_CALLBACK_DUMP("Make pre read callbacks"); 
    make_enabled_callbacks_helper(m_callback_list_pre_read, pre_read);
  }

  /// Makes the post read callbacks.
  inline void make_post_read_callbacks() const {
    GS_PARAM_CALLBACK_DUMP("Make post read callbacks"); 
    make_enabled_callbacks_helper(m_callback_list_post_read, post_read);
  }

  /// Makes the pre write callbacks.
  inline void make_pre_write_callbacks() {
    GS_PARAM_CALLBACK_DUMP("Make pre write callbacks"); 
    make_enabled_callbacks_helper(m_callback_list_pre_write, pre_write);
  }

  /// Makes the post write callbacks and notifies the update event.
  /**
   * Iterates the post write observer callback vector and makes the callbacks.
   * Does not make disabled callbacks.
   * 
   * The update event will be notified by one of the callbacks 
   * (if needed).
   *
   * Callbacks may be removed (using the removeCallback function)
   * during calls, but not modify the vector.
   */
  inline void make_post_write_callbacks() {
    GS_PARAM_CALLBACK_DUMP("Make post write callbacks and notify event"); 
    make_enabled_callbacks_helper(m_callback_list_post_write, post_write);
  }
  /// Deprecated, use make_post_write_callbacks() instead
  inline void makeCallbacks() {
    DEPRECATED_WARNING(name(), "DEPRECATED: internal function makeCallbacks() is deprecated! Use 'make_post_write_callbacks()' instead.");
    make_post_write_callbacks();
  }
 
  /// Makes the destroy param callbacks.
  inline void make_destroy_param_callbacks() const {
    GS_PARAM_CALLBACK_DUMP("Make destroy param callbacks"); 
    make_enabled_callbacks_helper(m_callback_list_destroy_param, destroy_param);
  }
  
  /// Help function for make_..._callbacks() functions
  inline void make_enabled_callbacks_helper( callback_list_type& callb_list, callback_type reason ) const {
// This should not be used, see comment in config_globals.h
#ifdef GCNF_AVOID_NESTED_CALLBACKS_IN_PARAM
    // Avoid multiple (nested) callbacks
    if (m_currently_making_callbacks) {
      GS_PARAM_CALLBACK_DUMP("   "<< callback_type_to_string(reason) <<" callback omitted.");
      return;
    }
#endif
    m_currently_making_callbacks = true;
    for (unsigned int i = 0; i < callb_list.size(); i++ ) {
      if (callb_list[i].first) { // if not true, this has been disabled and will be removed outside this loop
        GS_PARAM_CALLBACK_DUMP("   call adapter "<< callb_list[i].second->get_id());
        callb_list[i].second->call(*const_cast<gs_param_base*>(this), reason);
      }
    }
    m_currently_making_callbacks = false;
    cleanup_callback_list();
  }    
  
  /// Remove diabled callbacks
  /**
   * Do NOT call during a callback! That may currupt the iterator.
   */
  inline void cleanup_callback_list() const {
    assert(!m_currently_making_callbacks && "Must not be called while making callbacks");
    // remove callbacks that have been marked for delete
    if (m_callback_list_delete_enqueued > 0) {
      GS_PARAM_CALLBACK_DUMP("Remove callback adapter(s) being enqueued for removal from all callback lists."); 
      callback_list_type::reverse_iterator rit;
      all_callback_lists_type::iterator map_iter;
      for ( map_iter=m_callback_lists.begin(); map_iter != m_callback_lists.end(); map_iter++ ) {
        for ( rit = map_iter->second->rbegin() ; rit < map_iter->second->rend(); ++rit ) {
          if (!(*rit).first) { // if this has been disabled and needs to be removed now
            map_iter->second->erase(rit.base());
            m_callback_list_delete_enqueued--;
          }
          if (m_callback_list_delete_enqueued == 0) break;
        }
      }
      assert(m_callback_list_delete_enqueued == 0);
    }
  }    

  /// Remove the callback iterator from the vector, but only if not currently making callbacks
  /**
   * called internally
   */
  inline void conditional_callback_remove(callback_list_type& cb_list, callback_list_type::iterator& it) {
    // only remove directly if not currently making callbacks
    if (!m_currently_making_callbacks) {
      (*it).second->caller_param = NULL;
      GS_PARAM_CALLBACK_DUMP("             removed parameter callback adapter "<< (*it).second->get_id());
      cb_list.erase(it);
    }
    // ONLY DISABLE callback - do not modify the vector because currently making callbacks!
    // will be removed after the callbacks have been done
    else {
      (*it).first = false;
      m_callback_list_delete_enqueued++;
      GS_PARAM_CALLBACK_DUMP("             enqueued parameter callback adapter "<< (*it).second->get_id() << " for removal after callbacks are made");
    }
  }            
  
protected:

  /// Name of this parameter.
  std::string m_par_name;

  /// Pointer to the API which has to be used.
  cnf_api* m_api;

  // Callback handling members:
  /// If make_callbacks is currently calling (TODO: this applies to all callback types)
  mutable bool m_currently_making_callbacks;
  /// If some (how many) callbacks need to be deleted, because delete has been enqueued during m_currently_making_callbacks==true
  mutable unsigned int m_callback_list_delete_enqueued;
  /// Stores pre read observer callback function pointers
  mutable callback_list_type m_callback_list_pre_read;
  /// Stores post read observer callback function pointers
  mutable callback_list_type m_callback_list_post_read;
  /// Stores pre write observer callback function pointers
  mutable callback_list_type m_callback_list_pre_write;
  /// Stores post write observer callback function pointers
  mutable callback_list_type m_callback_list_post_write;
  /// Stores destroy param observer callback function pointers
  mutable callback_list_type m_callback_list_destroy_param;
  /// Map of all callback lists, key is the callback type, only for usage in not performance critical code! Fill this map in the constructor!
  mutable all_callback_lists_type m_callback_lists;
  
  /// The destruction flag lets the observers know that this object will be destoyed.
  bool m_destruction_flag;
  
  /// If this is a Simple Parameter Array of (unnamed) parameters (of same type)
  bool m_is_simple_array;
  /// If this is an Extended Parameter Array
  bool m_is_extended_array;
  
  /// Stores construction parameter, if to register this parameter at the plugin database
  bool m_register_at_db;
  
  /// If the update event has been enabled by registering the callback.
  bool m_update_event_enabled;
  /// Callback adapter pointer for the update event callback
  boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> m_update_event_callbAdapt;
  /// Update event: notified when parameter value changed
  sc_event m_update_event;
  
  /// Parent array pointer if this is an array member
  //gs_param_array *m_parent_array;
  
  /// Set of the param attributes
  std::set<param_attribute> m_attributes;
  
};


} // namespace cnf
using cnf::gs_param_base; // make gs_param available in gs namespace
} // namespace gs

#define __GS_PARAM_BASE_H_DONE__
#endif
