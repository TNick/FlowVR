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

#ifndef PORTSERVICELAYER_H_
#define PORTSERVICELAYER_H_

#include <map>
#include <string>

#include <flowvr/portutils/portparameter.h>

/**
 * @def DEFIMP_SERVICEPLUG(service)
 * @brief A macro to setup a simple service.
 * A simple service exhibits a function for creation (createService) and disposal (disposeService), both with
 * "C" linkage, so they can be found easily using dlOpen/dlSym.
 * Optionally, we define a setupArgs for the service to handle prefixing correctly.
 * Note that this macro does silently dispose (delete) the service, regardless of the type.
 * Note as well that the setupArgs() function is <i>just declared</i> and <i>not implemented</i>.
 * The dynamic loaded will query for the existence of the symbol, and assign NULL in case it is not
 * in the so file. This is basically well behavior.
 */
#define DEFIMP_SERVICEPLUG(service) \
		namespace flowvr { namespace portutils { \
			class Module; \
			}\
		}\
		\
		extern "C" flowvr::portutils::IServiceLayer *createService(flowvr::portutils::Module &parent) \
		{ \
			return new service(parent); \
		} \
		\
		extern "C" void disposeService(flowvr::portutils::IServiceLayer *iface) \
		{ \
			delete iface; \
		} \
		\
		extern "C" void setupArgs( const std::string &strPrefix );

#define DEFIMP_SERVICEPLUG_NOPRM(service) \
		namespace flowvr { namespace portutils { \
				class Module; \
			}\
		}\
		\
		extern "C" flowvr::portutils::IServiceLayer *createService(flowvr::portutils::Module &parent) \
		{ \
			return new service(parent); \
		} \
		\
		extern "C" void disposeService(flowvr::portutils::IServiceLayer *iface) \
		{ \
			delete iface; \
		} \
		\
		extern "C" void setupArgs( const std::string &strPrefix );\
		\
		extern "C" void getParameters( flowvr::portutils::ARGS & ) {}





namespace ftl
{
	class BaseOption;
}


namespace flowvr
{
	namespace portutils
	{

		class IServiceLayer;
		class Module;

		/**
		 * @fn createParameterName( const std::string &strPrefix, const std::string &strParam )
		 * A utility function to create a proper prefixed string for this parameter symbol given.
		 * As prefix strings are user determined during configuration, we have to assign the
		 * prefix to the Options-long arg name during the call to createArgs().
		 */
		std::string createParameterName( const std::string &strPrefix, const std::string &strParam );





		/**
		 * @brief A descriptive structure to define a service.
		 *
		 * It is usually a read-only structure, that can be used for debugging and cleanup.
		 * Typically, a service is created during the configuration process of the module at run-time.
		 */
		struct ServicePlug
		{
			/**
			 * @brief A typedef for a pointer to a creation function, called crMthd
			 *
			 * A service is created by passing all options given by the command line (that is: all options
			 * that were defined by all services used in the same module).
			 * The options can be used to setup the service correctly.
			 */
			typedef IServiceLayer *(*crMthd)(const Module &);

			/**
			 * @brief A typedef for a pointer to a deletion function, called dlMthd
			 *
			 * The services are loaded from so files, so they have to be disposed in the scope of
			 * the so file as well. The disposal function accepts a pointer to the service layer to
			 * be disposed. You can consider the pointer to be not usable after a call to dispose.
			 */
			typedef void (*dlMthd)(IServiceLayer*);


			/**
			 * @brief a typedef for the function to use when setting up the arguments for this
			 *        service.
			 *
			 * Every service can append any number of options to the commandline to be parsed by
			 * the module. User sets up a unique prefix for the service to find the options in the
			 * option container correctly.
			 * @param strServicePrefix a (module-level-unique) prfix string to prepend to the options.
			 */
			typedef void (*argsMthd)( const std::string &strServicePrefix );

			typedef void (*paramGMthd)( flowvr::portutils::ParameterContainer & );

			/**
			 * setup an empty service plug (all members set to NULL).
			 */
			ServicePlug()
			: m_soHandle(NULL)
			, m_crMthd(NULL)
			, m_dlMthd(NULL)
			, m_layer(NULL)
			, m_dlArgs(NULL)
			, m_dlParmsG(NULL)
			{

			}

			std::string m_strName, /**< user given name of this service */
						m_soName,  /**< the file path to the so file used for creating this service */
						m_Prefix;  /**< the prefix to use for the options used by this service */
			void*       m_soHandle; /**< the dlOpen retrieved handle to the so file used for this service */
			crMthd      m_crMthd;  /**< the pointer to the creation method for a service from the so.
										Must not be NULL for valid services */
			dlMthd      m_dlMthd;  /**< the pointer to the disposal method for a service from the so.
										Must not be NULL for valid services */
			argsMthd    m_dlArgs;  /**< the pointer to the method to setup arguments for this service.
										Can be NULL if the service does not setup any arguments. */
			paramGMthd   m_dlParmsG;
			IServiceLayer *m_layer; /**< a pointer to the service created using the so. This is a convinience
										method. Typically used for normal applications and cleanup. */

			bool isValid() const
			{
				if( m_layer )
					return true;

				return (m_soHandle != NULL);
			}

			void dump()
			{
				std::cout << "Plug dump: " << std::endl
						  << "Name\t:" << m_strName << std::endl
						  << "SoName\t:" << m_soName << std::endl
						  << "Handle\t:" << std::hex << m_soHandle << std::endl
						  << "crMthd\t:" << m_crMthd << std::endl
						  << "dlMthd\t:" << m_dlMthd << std::endl
						  << "dlArgs\t:" << m_dlArgs << std::endl
						  << "dlPrmG\t:" << m_dlParmsG << std::endl
						  << "layer\t:" << m_layer << std::dec << std::endl;
			}
		};


		/**
		 * @typedef SERV
		 * A convenience typedef to map names to services. Typically, one would use SERV m and ServicePlug sp
		 * like this: m[sp.m_strName] = sp; to put the service to the map (redundantly store the service name
		 * for lookup). Of course you could use any other name/alias.
		 */
		typedef std::map<std::string, ServicePlug> SERV;


		/**
		 * @brief the interface for a service layer.
		 *
		 * The IServiceLayer interface does not pose too many constraints (no real virtual methods).
		 * A service gets computation time every update frame, before and after the processing of the messages
		 * on the message bus.
		 * @see PreLoop()
		 * @see PostLoop()
		 */
		class IServiceLayer
		{
		public:
			IServiceLayer( Module &parent )
			: m_parent(parent)
			{}

			virtual bool setParameters( const ARGS & ) { return true; }

			virtual ~IServiceLayer() {}

			virtual bool Once() { return true; }

			/**
			 * Executed before the call to moduleAPI::wait()
			 * @return by default: true
			 */
			virtual bool PreLoop() { return true; }

			/**
			 * Executed after the message dispatching (after the call the moduleAPI::wait() and the message processing.
			 * @return by default: true
			 */
			virtual bool PostLoop() { return true; }

			Module &getParent() const;
		protected:
	//		IServiceLayer() {}
		private:
			/// private copy constructor (no sense in making a copy of a service layer)
			/// @private
			IServiceLayer( const IServiceLayer &other) : m_parent(other.m_parent) {}

			/// private assignment operator (no sense in assigning a service layer)
			/// @private
			IServiceLayer &operator=(const IServiceLayer &) { return *this; }

			Module &m_parent;
		};
	} // namespace portutils
} // namespace flowvr


#endif // PORTSERVICELAYER_H_
