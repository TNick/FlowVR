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

#ifndef PORTPARAMETER_H_
#define PORTPARAMETER_H_

#include <map>
#include <list>
#include <vector>
#include <string>
#include <ftl/convert.h>
#include <exception>

namespace flowvr
{
	namespace portutils
	{
		class ParameterContainer;

		class Parameter
		{
		public:
			enum eType
			{
				P_NONE   = -1,
				P_STRING = 0,
				P_NUMBER = 1,
				P_BOOLEAN = 2,
				P_LIST = 3,
				P_PARAMETERCONTAINER = 4
			};

			enum eSubType
			{
				PS_NONE = -1,
				PS_STRING,
				PS_NUMBER,
				PS_BOOLEAN,
				PS_LIST
			};

			enum eSetType
			{
				ST_NONE=-1,
				ST_NOVALUE,
				ST_DEFAULT,
				ST_CODE,
				ST_FILE
			};

			enum eMode
			{
				MD_NONE      =  0,
				MD_OPTIONAL  =  1,
				MD_MANDATORY =  2,
				MD_NONNULL   =  4
			};


			Parameter();

			Parameter( const std::string &value, eType tp = P_STRING,  eSubType esTp = PS_NONE,
					   const std::string &strHelpText = "",
					   eSetType = ST_DEFAULT,
					   eMode md = MD_OPTIONAL );

			Parameter( const std::string &value, const std::string &strHelpText, eType tp = P_STRING,
						eSubType esTp = PS_NONE,
						eSetType = ST_DEFAULT,
						eMode md = MD_OPTIONAL );

			Parameter( eType tp,  eSubType esTp = PS_NONE,
					   const std::string &strHelpText = "",
					   eMode md = MD_MANDATORY, eSetType = ST_NOVALUE );

			Parameter( const ParameterContainer & );
			Parameter( const Parameter & );

			Parameter &operator=( const Parameter & );

			~Parameter();


			eType    getType() const;
			eSubType getSubType() const;
			eSetType getSetType() const;
			void     setSetType( eSetType md );

			eMode    getModeType() const;

			std::string getValueString() const;
			std::string getHelpString() const;

			static std::string getTypeString( eType );
			static std::string getSubTypeString( eSubType );
			static std::string getSetTypeString( eSetType );
			static std::string getModeString( eMode );

			void setHelpString( const std::string &strHelpString );

			void setSubContainer( ParameterContainer &cont, eSubType esTp );
			void setValue( const std::string &strValue, eType tTp = P_STRING, eSubType = PS_NONE, eSetType = ST_CODE, eMode = MD_OPTIONAL, const std::string &strHelp = "" );

			template<class T> T getValue() const
			{
				return ftl::convertTo<T>( getValueString() );
			}

			typedef std::vector<double> ParVecN;
			typedef std::vector<std::string> ParVecS;
			typedef std::list<double>   ParListN;
			typedef std::list<std::string> ParListS;

			const ParameterContainer &getSubContainerConstRef() const;

			ParameterContainer &getSubContainer() const;

			void print() const;

		private:
			eType    m_eType;
			eSubType m_eSubType;
			eSetType m_eSetType;
			eMode    m_eMode;

			std::string m_strValue,
						m_strHelpText;

			ParameterContainer *m_subParameter;
		};

		template<> ParameterContainer  Parameter::getValue() const;
		template<> Parameter::ParListN Parameter::getValue() const;
		template<> Parameter::ParVecN  Parameter::getValue() const;
		template<> Parameter::ParVecS  Parameter::getValue() const;
		template<> Parameter::ParListS  Parameter::getValue() const;

		class ParameterContainer : public std::map<std::string, Parameter>
		{
		public:

			bool hasParameter( const std::string &pname ) const;
			void print( int nCurDepth = 0, int nMaxDepth = -1 ) const;

			std::string stringify() const;
			void unstringify( const std::string &strpc );

			static size_t toBase64( const ParameterContainer &, std::string &base64 );
			static std::string fromBase64( const std::string &sBase64, ParameterContainer &c, size_t outputsize );

			const Parameter &operator()(const std::string &) const;
			Parameter &operator()(const std::string &);
		};


		Parameter createParameter( const std::string &strName, const std::string &strValue );

		/**
		 * @typedef ARGS
		 * @brief Convenience typedef to map an ftl::BaseOption to its long name
		 */
	//	typedef std::map<std::string, ftl::BaseOption*> ARGS;
		typedef ParameterContainer ARGS;

		class ParameterException : public std::exception
		{
		public:
			ParameterException( const std::string &strParamName, const std::string &strReason )
			: std::exception()
			, m_pname(strParamName)
			, m_reason( strReason )
			{}

			virtual ~ParameterException() throw() {}

			std::string m_pname;
			std::string m_reason;

			mutable std::string text;

			virtual const char* what() const throw()
			{
				text = m_pname + std::string(": ") + m_reason;
				return text.c_str();
			}
		};
	} // namespace portutils
} // namespace flowvr


#endif // PORTPARAMETER_H_
