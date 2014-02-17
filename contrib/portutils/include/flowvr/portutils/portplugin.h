/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                        PortUtils library                        *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*                                                                 *
*   The source code is  covered by different licences,            *
*   mainly  GNU LGPL and GNU GPL. Please refer to the             *
*   copyright.txt file in each subdirectory.                      *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*  Ingo Assenmacher,                                              *
*  Bruno Raffin                                                   *
*                                                                 *
*******************************************************************/

#ifndef PORTPLUGIN_H_
#define PORTPLUGIN_H_

#include <map>
#include <string>

#include <flowvr/portutils/portparameter.h>

/**
 * @def DEFIMP_PORTPLUG(handler, service, plugclass)
 * Convenience macro to setup all functions need for dynamic linking of plugins.
 * This variant assumes that the plugin described will use a service layer in order
 * to function. This is the normal case.
 *
 * - first declare (local linkage) a serviced plugin (use template TServicesPlugin<>)
 * - setup the pointer to the singleton for this plugin (called SPlugin)
 * - setup two extern "C" linked functions (createPlugin and disposePlugin)
 *   -# the createPlugin checks whether the service layer has the expected type or not (failing in the latter case)
 *   -# in case the plugin was not created, yet... it will be, and then returned as result of a call to create.
 *      Note that this implies that a Plugin created with this macro is a singleton!
 *   -# disposePlugin will reset the singleton pointer to NULL when called (not thread safe!), so you can try to
 *      re-initialize the plugin if you want to. It also checks whether the pointer passed it the pointer to the
 *      singleton.
 *
 *  Usage of this macro:
 *  - handler := the class name/type of your handler (derived from PortHandlerBase, note that the
 *    constructor has to accept a pointer to an instance of type 'service')
 *  - service := the class name/type of the service to use (derived from IServiceLayer)
 *  - plugclass := the name of the plugin-class to define (use any name you like)
 *
 *  Requires:
 *  - handler defines a constructor that takes a pointer to the service as single argument
 *  - handler defines all full virtual methods
 */
#define DEFIMP_PORTPLUG(handler, service, plugclass) \
		namespace { \
		typedef flowvr::portutils::TServicedPlugin<handler, service> plugclass; \
		        plugclass *SPlugin = NULL; \
		} \
		\
		extern "C" flowvr::portutils::IPluginInterface *createPlugin(flowvr::portutils::IServiceLayer *layer, int flags, const flowvr::portutils::ARGS &args) \
		{ \
			service *real_layer = dynamic_cast< service *>(layer); \
			if(real_layer == NULL) \
				return NULL; \
			if(SPlugin == NULL) \
				SPlugin = new plugclass(real_layer, args); \
			\
			return SPlugin; \
		} \
		\
		extern "C" void disposePlugin(flowvr::portutils::IPluginInterface *iface) \
		{ \
			if( iface == SPlugin ) \
			{ \
				delete iface; \
				SPlugin = NULL; \
			} \
		}

#define DEFIMP_PORTPLUG_NOPRM(handler, service, plugclass) \
		namespace { \
		typedef flowvr::portutils::TServicedPlugin<handler, service> plugclass; \
		        plugclass *SPlugin = NULL; \
		} \
		\
		extern "C" flowvr::portutils::IPluginInterface *createPlugin(flowvr::portutils::IServiceLayer *layer, int flags, const flowvr::portutils::ARGS &args) \
		{ \
			service *real_layer = dynamic_cast< service *>(layer); \
			if(real_layer == NULL) \
				return NULL; \
			if(SPlugin == NULL) \
				SPlugin = new plugclass(real_layer, args); \
			\
			return SPlugin; \
		} \
		\
		extern "C" void disposePlugin(flowvr::portutils::IPluginInterface *iface) \
		{ \
			if( iface == SPlugin ) \
			{ \
				delete iface; \
				SPlugin = NULL; \
			} \
		}\
		\
		extern "C" void getParameters( flowvr::portutils::ARGS & ) {}


/**
 * @def DEFIMP_PORTPLUG_NOSERVICE(handler, plugclass)
 * Macro to expand all functions needed to create a plugin that does not depend on a service.
 * This is for simple plugins or the ones that can work locally on messages and normally do not
 * need any context.
 * - it creates an instance of TNonServicesPlugin.
 * - creates a pointer to a singleton to represent the plugin
 * - create two functions with "C" linkage: createPlugin and disposePlugin
 *   -# createPlugin creates the singleton (if not already), the argument to 'layer' is ignored.
 *   -# disposePlugin checks whether the pointer given to dispose matches the singleton. If that is
 *      the case, the plugin is deleted and the singleton pointer reset to NULL (to allow for re-initialization)
 *
 *  Usage:
 *  - handler := the class name/type of your handler (derived from PortHandlerBase). Note that the constructor of this
 *               handler typically does not need a pointer to any service layer, in fact, it has to be the default
 *               constructor.
 *  - plugclass := the class name/type of the plugin class to create (use any name you like)
 *
 *  Requires
 *  - handler defines all full virtual methods from baseclass
 */
#define DEFIMP_PORTPLUG_NOSERVICE(handler, plugclass) \
		namespace { \
		typedef flowvr::portutils::TNonServicedPlugin<handler> plugclass; \
		plugclass *SPlugin = NULL; \
		} \
		\
		extern "C" flowvr::portutils::IPluginInterface *createPlugin(flowvr::portutils::IServiceLayer *layer, int flags, const flowvr::portutils::ARGS &args) \
		{ \
			if(SPlugin == NULL) \
				SPlugin = new plugclass(args); \
			\
			return SPlugin; \
		} \
		\
		extern "C" void disposePlugin(flowvr::portutils::IPluginInterface *iface) \
		{ \
			if( iface == SPlugin ) \
			{ \
				delete iface; \
				SPlugin = NULL; \
			} \
		}


#define DEFIMP_PORTPLUG_NOSERVICE_NOPRM(handler, plugclass) \
		namespace { \
		typedef flowvr::portutils::TNonServicedPlugin<handler> plugclass; \
		plugclass *SPlugin = NULL; \
		} \
		\
		extern "C" flowvr::portutils::IPluginInterface *createPlugin(flowvr::portutils::IServiceLayer *layer, int flags, const flowvr::portutils::ARGS &args) \
		{ \
			if(SPlugin == NULL) \
				SPlugin = new plugclass(args); \
			\
			return SPlugin; \
		} \
		\
		extern "C" void disposePlugin(flowvr::portutils::IPluginInterface *iface) \
		{ \
			if( iface == SPlugin ) \
			{ \
				delete iface; \
				SPlugin = NULL; \
			} \
		}\
		extern "C" void getParameters( flowvr::portutils::ARGS & ) {} \
		\


namespace flowvr
{
	namespace portutils
	{
		class IServiceLayer;
		class PortHandlerBase;

		/**
		 * @brief The main interface for plugins.
		 *
		 *
		 * Plugins are description objects. They offer a method to get a port handler for messages served
		 * by this plugin. On the other hand, they offer a method to dispose the handler when not needed anymore.
		 * The actual workload is executed in the PortHandlerBase and derivatives, so look there for more information.
		 * Typically one needs to deal with IPluginInterfaces when one has special constraints on the service layer
		 * used, or has special management ideas about the PortHandlerBase instances that are to be created.
		 *
		 * @see PortHandlerBase
		 * @see getPortHandler()
		 */
		class IPluginInterface
		{
		public:
			virtual ~IPluginInterface() {}

			/**
			 * Creation method for PortHandlerBase instances.
			 * Note that the plugin decides whether it creates a new handler upon every call to this
			 * method or it creates just one handler, passing this upon every request.
			 * Users who claim a PortHandlerBase by using this method should call disposePortHandler()
			 * to assure correct deallocation of the handler.
			 *
			 * @see disposePortHandler()
			 * @return a pointer to a PortHandlerBase that is used by this plugin.
			 */
			virtual PortHandlerBase *getPortHandler( const std::string &strId ) const = 0;

			/**
			 * Disposal method for handlers that are not needed anymore.
			 * @return success of the disposal
			 *         - true in case the pointer given is not valid anymore
			 *         - false else (for example when passing a pointer that was not created by this plugin)
			 * @param ph the PortHandlerBase to dispose.
			 */
			virtual bool disposePortHandler( PortHandlerBase *ph ) = 0;
		};


		/**
		 * @brief template for services that work on services
		 *
		 * This is a simple template for a simple plugin using a service.
		 * More exactly: the service pointer passed during the creation of the service
		 * will be stored in this instance and is passed as sole argument to the constructor
		 * of the handler of type T.
		 *
		 * Note that the service layer is not considered to be 'owned' by this plugin and should
		 * outlive the plugin as well as be collected when not used anymore.
		 *
		 * @tparam T the handler type to create using new upon a call to getPortHandler()
		 *         The type T must accept a pointer to an instance of type K for the construction.
		 * @tparam K the service layer type to use for this plugin.
		 *         The service layer of type K will be passed to the constructor for the instance
		 *         ot type T during a call to getPortHandler().
		 */
		template<class T, class K>
		class TServicedPlugin : public IPluginInterface
		 {
		 public:
			/**
			 * @param layer the pointer to a an interface of type K, must not be NULL, unless type T expects to
			 *        work with NULL services.
			 */
			TServicedPlugin( K *layer, const ARGS &args )
			: m_layer(layer)
			, m_args(args)
			{

			}

			/**
			 * creation function, calls T(layer) (constructor with one argument, accepting K).
			 * Note that this plugin will create a new handler upen every call.
			 * @see disposePortHandler()
			 *
			 * @return a PortHandlerBase specialization of type T
			 */
			virtual PortHandlerBase *getPortHandler(const std::string &strId) const
			{
				return new T(m_layer, m_args);
			}

			/**
			 * Disposes the port handler given as argument (calls delete, the pointer is not usable after a
			 * call to this method).
			 * This method checks whether to pointer passed is of type T or not, usign a dynamic cast.
			 *
			 * @param ph the pointer to a PortHandlerBase (of type T) to be disposed.
			 * @return success or failure
			 *         - true if ph is of type T (ph was deleted during the call to this method)
			 *         - false else (ph was not touched)
			 */
			virtual bool disposePortHandler( PortHandlerBase *ph )
			{
				if( dynamic_cast<T*>(ph) )
				{
					delete ph;
					return true;
				}

				return false;
			}

			K *m_layer; /**< pointer to the service layer to be passed to PortHandlerBase instances of type T */
			ARGS m_args;
		 };


		/**
		 * @brief template for services that do not work on a service
		 *
		 * Simplification of a plugin that which handlers do not use a service to work on.
		 * This can be the case if you have handlers that work only on message content or routing.
		 * @tparam T the PortHandlerBase type to create using new during a call to getPortHandler()
		 */
		template<class T>
		class TNonServicedPlugin : public IPluginInterface
		{
		public:
			TNonServicedPlugin( const ARGS &args )
			: IPluginInterface()
			, m_args(args)
			{

			}
			/**
			 * Create method for PortHandlerBase instances of type T.
			 * Uses the default constructor for the creation of type T.
			 */
			virtual PortHandlerBase *getPortHandler(const std::string &strId ) const
			{
				return new T(m_args);
			}

			/**
			 * Disposes the port handler given as argument (calls delete, the pointer is not usable after a
			 * call to this method).
			 * This method checks whether to pointer passed is of type T or not, usign a dynamic cast.
			 *
			 * @param ph the pointer to a PortHandlerBase (of type T) to be disposed.
			 * @return success or failure
			 *         - true if ph is of type T (ph was deleted during the call to this method)
			 *         - false else (ph was not touched)
			 */
			virtual bool disposePortHandler( PortHandlerBase *ph )
			{
				if( dynamic_cast<T*>(ph) )
				{
					delete ph;
					return true;
				}
				return false;
			}

			ARGS m_args;
		};

		// PLUGIN STUFF

		/**
		 * @brief The description class to hold properties of a plugin that was loaded during run-time.
		 *
		 * This is mainly a read-only structure, unless you know what you are doing. It is used during
		 * the configuration process of the flowvr::portutils::PortModule and the configuration routines used in there.
		 */
		struct PortPlugin
		{

			/**
			 * @brief A typedef for a pointer to a creation function, called crMthd.
			 * The creation function accepts a pointer to a service layer and an int that is passed as
			 * flags to use.
			 * The creation method returns a pointer to a IPluginInterface.
			 * @todo check the semantics of the flags... it is probably a legacy concept.
			 * @see dlMthd
			 */
			typedef IPluginInterface *(*crMthd)(IServiceLayer *, int, const ARGS & );

			/**
			 *  @brief A typedef for a pointer to a deletion function, called dlMthd.
			 *  The disposal function releases the IPluginInterface created by a call to the
			 *  creation method stored in the PortPlugin.
			 *  @see crMthd
			 */
			typedef void (*dlMthd)(IPluginInterface*);

			typedef void (*pmMthd)( ARGS & );

			/**
			 * @brief map-name to plugin struct
			 *
			 * This is a utility typedef to match PortPlugin structures to their name given by
			 * m_strName. So typically one would store in PLGS m with PortPlugin pp like this:
			 * m[pp.m_strName] = pp; (redundantly store the name in the map as well).
			 */
			typedef std::map<std::string, PortPlugin> PLGS;

			/**
			 * Initialize an empty PortPlugin (all members set to NULL or 0)
			 */
			PortPlugin()
			: m_soHandle(0)
			, m_crMthd(NULL)
			, m_dlMthd(NULL)
			, m_pmMthd(NULL)
			, m_ifc(NULL)
			{

			}

			std::string m_strName,  /**< the name of this plugin (user defined, can be any string,
										must be unique in the overall run) */
						m_soName;   /**< the name of the so file that was loaded for this plugin (user given,
										mainly used for debugging purposes) */
			void*       m_soHandle; /**< the so handle that is current for this plugin, retrieved by dlopen */
			crMthd      m_crMthd;   /**< a pointer to the creation function (must not be NULL) */
			dlMthd      m_dlMthd;   /**< a pointer to the disposal function (must not be NULL) */
			pmMthd      m_pmMthd;   /**< retrieve parameters for this plugin (can be NULL) */
			ARGS        m_args;     /**< filled args structure for this plugin */

			IPluginInterface *m_ifc; /**< a pointer to the plugin already created using this PortPlugin structure.
										  This is a simplification for cleanups, you can create as many instances
										  as are necessary (though there is no use case currently to create more than one) */

		};
	} // namespace portutils
} // namespace flowvr

#endif /* PORTPLUGIN_H_ */
