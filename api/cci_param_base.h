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


#ifndef __CCI_PARAM_BASE_H__
#define __CCI_PARAM_BASE_H__


#include <string>
#include <iostream>
#include <map>
#include <set>
#include <boost/shared_ptr.hpp>
#include "cci_datatypes.h"
#include "cci_paramcallbadapt.h"


/*#include "greencontrol/gcnf/plugin/config_globals.h"
#include "greencontrol/core/gc_core.h"
#include "greencontrol/gcnf/apis/GCnf_Api/GCnf_Api_t.h"
#include "greencontrol/gcnf/apis/GCnf_private_Api/gcnf_private_api.h"
#include "greencontrol/gcnf/plugin/utils.h"
*/

namespace cci {

  template <typename T> 
  class cci_param;

  /// Base class for all cci_param template specialisations.
  /**
   * Manages the name of the parameter 
   *
   * Requires set and get with the string representation of the value.
   */
  class cci_param_base
  {
  protected:

    /// Typedef for a multimap saving pointers to callback functions with the void* pointer of the observer module as key.
    typedef std::multimap<void*, boost::shared_ptr<ParamCallbAdapt_b> > observerParamCallback_type;

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
     * @param force_top_level_name If the given name n should be a top-level name (then no prefeix is attached to the name),
     *                             default false,
     *                             be carefull in using this.
     */
    explicit cci_param_base(const std::string &n, const bool register_at_db = true,
                            const bool force_top_level_name = false);
    
    /// Destructor.
    virtual ~cci_param_base();
    
    /// Destruction method to be called by the cci_param destructor
    /**
     * Sets the destruction flag true and
     * call remove for the database.
     *
     * The destruction flag lets the observers know that this object will be destoyed.
     */
    void destruct_cci_param();
    
    // ////////   set, get   /////////// //


    /// Sets the value of this parameter given by a string.
    /** 
     * @param str the new value represented as a string.
     * @return If setting was successful.
     */
    virtual bool setString(const std::string &str) = 0;
    
    /// Get the string representation of this parameter's value.
    /**
     * @return  The value of this parameter represented as a string.
     */
    virtual const std::string& getString() const = 0;


    /// Get this parameter's value converted to the type.
    /**
     * This function can be called if the user knows the type.
     *
     * The string value is converted to the user-chosen template type
     * by using the cci_param template specialization.
     *
     * This function does only work for types of cci_param<T>, not for 
     * any kind of cci_param_base, e.g. parameter arrays.
     *
     * @param value    The value of the parameter will be written to this parameter
     * @return         If the convertion was successfull.
     */
    template<class T>
    const bool getValue(T &value) const;
    
    /// Get a parameter's value. Independent of the implicit or explicit status.
    /**
     * Better use getValue(parname, value) to avoid copying of the value.
     *
     * This function can be called if the user knows the type.
     *
     * The string value is converted to the user-chosen template type
     * by using the cci_param template specialization.
     *
     * This function does only work for types of cci_param<T>, not for 
     * any kind of cci_param_base, e.g. parameter arrays.
     *
     * @return         Value of the parameter, converted to the user-chosen type.
     */
    template<class T>
    const T getValue() const;  
    
    /// Get a parameter pointer of the cci_param<T> type defined by the caller (does a dynamic_cast).
    /**
     * @return  Pointer to the parameter object (NULL if not existing or wrong type).
     */ 
    template<class T>
    cci_param<T>* get_cci_param();
    
    // ////////   Type   /////////// //

    /// Returns the string representation of the type this parameter stores
    virtual const std::string getTypeString() const;
    
    /// Returns the type identifying enum of the type this parameter stores
    virtual const Param_type getType() const;
    
    // //////  param_attributes  ////// //
    /*/// Adds a param attribute.
    bool add_param_attribute(const param_attribute attr);
    /// If a special param attribute is set.
    bool has_param_attribute(const param_attribute attr) const;
    /// Returns a set containing all param attributes that are set for this param.
    const std::set<param_attribute> get_param_attributes() const;
    /// Removes a param attribute.
    bool remove_param_attribute(param_attribute attr);
    /// Removes all param attributes of this param.
    void remove_all_param_attributes();*/
    
    
    // ////////   diverse   /////////// //

    
    /// Get the name of this parameter.
    /**
     * @return   Name of the parameter.
     */
    const std::string &getName() const;
   
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
     * Use the callbacks to register observing analysis methods in other classes.
     * So first get a reference to this parameter (TODO: future).
     * 
     * The user may register any methods as callback functions which have
     * the following signature:
     * \code
     * void method_name(cci_param_base& changed_param);
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
     *   cci_param<int> my_param;
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
     *   void config_callback(cci_param_base& changed_param) {
     *     // some action
     *   }
     * };
     * \endcode
     *
     * @param callb      boost shared pointer to the ParamCallbAdapt_b object which contains
     *                   the object pointer and the member function pointer.
     * @return           boost shared pointer to the callback adapter (e.g. to be used for unregister calls).
     *
     * <em>Macro REGISTER_CALLBACK</em>
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
    virtual boost::shared_ptr<ParamCallbAdapt_b> registerParamCallback(boost::shared_ptr<ParamCallbAdapt_b> callb);

    /// <code>GC_HAS_CALLBACKS();</code> creates a vector for the module where all parameter callback adapters are stored when being created by the macro GC_REGISTER_PARAM_CALLBACK. To be used by a user module in its (e.g. private) class body.
  #define GC_HAS_CALLBACKS()  \
    std::map<void*, boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> > gcnf_ParamCallbAdapt_list;  \
    boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> gc_add_ParamCallbAdapt(boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> cba) {     \
      gcnf_ParamCallbAdapt_list.insert(std::pair<void*, boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b> >(((void*)cba.get()), cba));               \
      return cba;   \
    }
    
    /// Makro for registering callback functions, usage: GC_REGISTER_PARAM_CALLBACK(&my_param, MyIP_Class, config_callback);
  #define GC_REGISTER_PARAM_CALLBACK(param, class, function)                \
    (param)->registerParamCallback( gc_add_ParamCallbAdapt(boost::shared_ptr< ::gs::cnf::ParamCallbAdapt_b>(new ::gs::cnf::ParamCallbAdapt<class>(this, &class::function, this, (param)))) )

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
    
    /// Unregisters the callback and (default) deletes the callback adapter.
    /**
     * Unregisters the one callback.
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
    virtual bool unregisterParamCallback(ParamCallbAdapt_b *callb);
    
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
    virtual bool unregisterParamCallbacks(void* observer);
      
    /// Returns if this param has registered callbacks
    bool has_callbacks();
    
    /// INTERNAL Callback function which does the event notify. Registered only if event needed!
    /**
     * This callback function is registered at this parameter the first
     * time the function getUpdateEvent is being called. This reduces the
     * overhead to zero if no update event has been asked for by the user.
     *
     * @param changed_param param that has been changed .
     */
    void update_event_callback(cci_param_base& changed_param);
    
  protected:

    /*void show_callbacks();*/
    
    /// For name creation in constructor (and for debug). Responsible for the top-level name.
    /**
     * Returns the name of the parent sc_core::sc_module with 
     * an SC_NAME_DELIMITER as postfix.
     * If no parent can be found, this is top-level
     * and returns the toplevel (empty) string.
     *
     * @return  Parent sc_core::sc_module name + SC_NAME_DELIMITER or top-level name (empty).
     */
    const std::string get_name_prefix();

  protected:
    /// Makes the callbacks and notifies the update event.
    /**
     * Iterates the observer callback vector and makes the callbacks.
     * 
     * The update event will be notified by one of the callbacks 
     * (if needed).
     *
     * An segfault occurs if the called function will remove more 
     * callbacks than the current called!
     */
    inline void makeCallbacks();
    
    /// Perform secure callbacks for destruction callbacks
    /**
     * This function is secure for callbacks being unregistered
     * during one of the callbacks: Before each call it is checked
     * if the call that should be done is still registered!
     */
    inline void makeSecureCallbacks();
    
  protected:

    /// Name of this parameter.
    std::string m_par_name;

    /// Pointer to the API which has to be used.
    cci_cnf_api* m_api;

    /// Variable to save an observer callback function pointer for each parameter (if needed).
    observerParamCallback_type m_observer_param_callback_mmap;

    /// The destruction flag lets the observers know that this object will be destoyed.
    bool m_destruction_flag;
    
    /// Stores construction parameter, if to register this parameter at the plugin database
    bool m_register_at_db;
    
    /// Set of the param attributes
    std::set<param_attribute> m_attributes;
    
  };

} // namespace cci

#include "cci_param_base.hpp"

#endif
