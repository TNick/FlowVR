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

#include "flowvr/portutils/portparameter.h"

//#include <bzlib.h>
#include <vector>
#include <string.h>

namespace
{
	const flowvr::portutils::Parameter nilP;
	flowvr::portutils::Parameter nilp;

	/*
	** Translation Table as described in RFC1113
	*/
	const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	/*
	** Translation Table to decode (created by author)
	*/
	const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

	/*
	** encodeblock
	**
	** encode 3 8-bit binary bytes as 4 '6-bit' characters
	*/
	void encodeblock( unsigned char in[3], unsigned char out[4], int len )
	{
		out[0] = cb64[ in[0] >> 2 ];
		out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
		out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
		out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
	}

	/*
	** encode
	**
	** base64 encode a stream adding padding and line breaks as per spec.
	*/
	void encode( const std::vector<unsigned char> &input, std::string &output, int linesize )
	{
		unsigned char in[3], out[4];
		int i, len, blocksout = 0;

		std::vector<unsigned char>::const_iterator cit = input.begin();
		while( cit != input.end() )
		{
			len = 0;
			for( i = 0; i < 3; i++ )
			{
				in[i] = (unsigned char) *cit;
				if( cit != input.end() )
				{
					len++;
					++cit;
				}
				else
				{
					in[i] = 0;
				}
			}

			if( len )
			{
				encodeblock( in, out, len );
				output.append( (const char*)&out[0], (std::string::size_type) 4 );
				blocksout++;
			}

			if( linesize > 0 and blocksout >= (linesize/4) || cit == input.end() )
			{
				if( blocksout )
				{
					output.append( std::string("\r\n") );
				}
				blocksout = 0;
			}
		}
	}

	/*
	** decodeblock
	**
	** decode 4 '6-bit' characters into 3 8-bit binary bytes
	*/
	void decodeblock( unsigned char in[4], unsigned char out[3] )
	{
		out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
		out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
		out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
	}

	/*
	** decode
	**
	** decode a base64 encoded stream discarding padding, line breaks and noise
	*/
	void decode( const std::string &infile, std::vector<unsigned char> &outfile )
	{
		unsigned char in[4], out[3], v;
		int i, len;

		std::string::const_iterator cit = infile.begin();

		while( cit != infile.end() )
		{
			for( len = 0, i = 0; i < 4 && (cit != infile.end()); i++ )
			{
				v = 0;
				while( (cit != infile.end()) && v == 0 )
				{
					v = *cit++;

					v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
					if( v )
					{
						v = (unsigned char) ((v == '$') ? 0 : v - 61);
					}
				}
				if( cit != infile.end() )
				{
					len++;
					if( v )
					{
						in[ i ] = (unsigned char) (v - 1);
					}
				}
				else
				{
					in[i] = 0;
				}
			}

			if( len )
			{
				decodeblock( in, out );
				for( i = 0; i < len - 1; i++ )
				{
					outfile.push_back(out[i]);
				}
			}
		}
	}


	void stringify( const flowvr::portutils::Parameter &p, std::string &result, const std::string &strName )
	{
		std::string value = p.getValueString();

//		std::cout << "stringify [" << strName << "]" << std::endl;
		result += "p ";
		std::stringstream in;
		in << int(p.getType())
		   << " "
		   << int(p.getSubType())
		   << " "
		   << int(p.getSetType())
		   << " "
		   << int( p.getModeType() )
		   << " "
		   << strName.size()
		   << " "
		   << strName
		   << " "
		   << value.size()
		   << " " << value
		   << " ";

		result += in.str();
	}

	void stringify( const flowvr::portutils::ParameterContainer &pc, std::string &result, const std::string &strName )
	{
		result += "c ";
		std::stringstream in;
		in << strName.size() << " " << strName << " " << pc.size() << " ";
		result += in.str();

//		std::cout << "stringify [" << strName << "]" << std::endl;

		for( flowvr::portutils::ParameterContainer::const_iterator cit = pc.begin(); cit != pc.end(); ++cit )
		{
			if( (*cit).second.getType() == flowvr::portutils::Parameter::P_PARAMETERCONTAINER )
			{
				// code: c <num-elements> <string code>*num-elements
				const flowvr::portutils::ParameterContainer &c = (*cit).second.getSubContainerConstRef();
				stringify( c, result, (*cit).first );
			}
			else
			{
				stringify( (*cit).second, result, (*cit).first );
			}
		}
	}

	void unstringify( std::istringstream &in, flowvr::portutils::Parameter &p, std::string &sName )
	{
		size_t size, vsize;
		int  type;
		int  subtype;
		int  settype;
		int  mode;

		in >> type >> subtype >> settype >> mode >> size;


		std::vector<unsigned char> vname(size+2);
		in.read( (char*)&vname[0], size+2 );
		sName.assign(vname.begin()+1, vname.end()-1 );

		in >> vsize;

		std::vector<unsigned char> vvalue(vsize+2);
		in.read( (char*)&vvalue[0], vsize+2 );
		std::string svalue( vvalue.begin()+1, vvalue.end()-1 );

		p.setValue( svalue, flowvr::portutils::Parameter::eType(type), flowvr::portutils::Parameter::eSubType(subtype),
				            flowvr::portutils::Parameter::eSetType(settype), flowvr::portutils::Parameter::eMode(mode) );
	}

	void unstringify( std::istringstream &in, flowvr::portutils::ParameterContainer &pc )
	{
//		std::cout << in.rdbuf() << std::endl;

		size_t num = 0;

		in >> num;
//		std::cout << "found [" << num << "] number of children" << std::endl;

		for( int n=0; n < num; ++n)
		{
			std::string type;
			in >> type;
			switch(type[0])
			{
			case 'c':
			{

				size_t nlength = 0;
				in >> nlength;

//				std::cout << "it's a paramlist!" << std::endl
//						  << "with name-length of " << nlength << std::endl;

				std::vector<unsigned char> v(nlength+2);
				in.read( (char*)&v[0], nlength+2);
				std::string name(v.begin()+1, v.end()-1);

//				std::cout << " and name [" << name << "]" << std::endl;

				pc[name] = flowvr::portutils::Parameter( flowvr::portutils::ParameterContainer() );

				// avoid a copy in of parameters by getting the reference to the just entered
				// parameter [name]
				flowvr::portutils::Parameter &p1 = pc[name];

				::unstringify( in, p1.getSubContainer() );
				break;
			}
			case 'p':
			{
				flowvr::portutils::Parameter p;
				std::string name;

				::unstringify( in, p, name );

				pc[name] = p; // copy in

				break;
			}
			default:
				break;
			}
		}
	}

//	int compress( const std::vector<unsigned char> &value, std::vector<unsigned char> &compressed )
//	{
//		std::vector<unsigned char> v( (std::vector<unsigned char>::size_type) (value.size()*1.01)+600 );
//
//		unsigned int destLen = v.size();
//		std::cout << "resizing vector to " << destLen << std::endl;
//
//		bz_stream strm;
//		memset( &strm, 0, sizeof( bz_stream) );
//
//		BZ2_bzCompressInit( &strm, 9, 0, 30 );
//
//		if(BZ2_bzBuffToBuffCompress( (char*)&v[0], &destLen,
//				                  (char*)&value[0], value.size(),
//				                  9, 0, 30 ) == BZ_OK )
//		{
//			compressed.assign( v.begin(), v.begin() + destLen );
//		}
//		else
//		{
//			std::cerr << "ERROR DURING COMPRESSION" << std::endl;
//		}
//
//		BZ2_bzCompressEnd( &strm );
//
//		return destLen;
//	}
//
//	void uncompress( const std::vector<unsigned char> &compressed, std::vector<unsigned char> &uncompressed, size_t length )
//	{
//		uncompressed.resize( length );
//		unsigned int destLen = length;
//
//		bz_stream strm;
//		memset( &strm, 0, sizeof( bz_stream ) );
//
//		BZ2_bzDecompressInit( &strm, 0, 0 );
//
//		if( BZ2_bzBuffToBuffDecompress( (char*)&uncompressed[0], &destLen,
//				                        (char*)&compressed[0], compressed.size(),
//				                        0, 0 ) == BZ_OK )
//		{
//
//		}
//		BZ2_bzDecompressEnd( &strm );
//	}



} // namespace


namespace flowvr
{
	namespace portutils
	{
		Parameter::Parameter()
		: m_eType( P_NONE )
		, m_eSubType( PS_NONE )
		, m_subParameter(NULL)
		, m_eSetType(ST_NONE)
		, m_eMode(MD_NONE)
		{

		}

		Parameter::Parameter( const std::string &value, eType tp, eSubType esTp,
							  const std::string &strHelpText, eSetType eSTp, eMode md )
		: m_eType( tp )
		, m_eSubType( esTp )
		, m_subParameter(NULL)
		, m_strValue(value)
		, m_strHelpText(strHelpText)
		, m_eSetType(eSTp)
		, m_eMode(md)
		{
		}

		Parameter::Parameter( const std::string &value, const std::string &strHelpText,
					eType tp,
					eSubType esTp,
					eSetType eSTp,
					eMode md)
		: m_eType( tp )
		, m_eSubType( esTp )
		, m_subParameter(NULL)
		, m_strValue(value)
		, m_strHelpText(strHelpText)
		, m_eSetType(eSTp)
		, m_eMode(md)
		{

		}



		Parameter::Parameter( eType tp,  eSubType esTp,
							  const std::string &strHelpText,
							  eMode md, eSetType setType)
		: m_eType(tp)
		, m_eSubType(esTp)
		, m_eMode(md)
		, m_eSetType(setType)
		, m_subParameter(NULL)
		, m_strValue()
		, m_strHelpText(strHelpText)
		{
			if( m_eType == P_PARAMETERCONTAINER )
				m_subParameter = new ParameterContainer;
		}

		Parameter::Parameter( const ParameterContainer &pc )
		: m_eType( P_PARAMETERCONTAINER )
		, m_eSubType( PS_NONE )
		, m_eSetType( ST_DEFAULT )
		, m_strValue()
		, m_subParameter( new ParameterContainer( pc ) )
		, m_eMode(MD_NONE)
		{

		}

		Parameter::Parameter( const Parameter &other )
		{
			m_eType       = other.m_eType;
			m_eSubType    = other.m_eSubType;
			m_strValue    = other.m_strValue;
			m_eSetType    = other.m_eSetType;
			m_eMode       = other.m_eMode;
			m_strHelpText = other.m_strHelpText;

			if( other.m_subParameter )
			{
				m_subParameter = new ParameterContainer( *other.m_subParameter ); // clone
			}
			else
				m_subParameter = NULL;
		}

		Parameter &Parameter::operator=( const Parameter &other )
		{
			m_eType       = other.m_eType;
			m_eSubType    = other.m_eSubType;
			m_strValue    = other.m_strValue;
			m_eSetType    = other.m_eSetType;
			m_eMode       = other.m_eMode;
			m_strHelpText = other.m_strHelpText;

			delete m_subParameter;

			if( other.m_subParameter )
			{
				m_subParameter = new ParameterContainer( *other.m_subParameter ); // clone
			}
			else
				m_subParameter = NULL;


			return *this;
		}

		Parameter::~Parameter()
		{
			delete m_subParameter;
		}


		void Parameter::setValue( const std::string &strValue, eType tTp, eSubType eSt, eSetType eS, eMode eM, const std::string &strHelp )
		{
			m_strValue = strValue;
			m_strHelpText = strHelp;
			m_eType    = tTp;
			m_eSubType = eSt;
			m_eSetType = eS;
			m_eMode    = eM;
		}

		Parameter::eType    Parameter::getType() const
		{
			return m_eType;
		}

		Parameter::eSubType Parameter::getSubType() const
		{
			return m_eSubType;
		}


		Parameter::eSetType Parameter::getSetType() const
		{
			return m_eSetType;
		}

		std::string Parameter::getValueString() const
		{
			return m_strValue;
		}

		std::string Parameter::getHelpString() const
		{
			return m_strHelpText;
		}

		void Parameter::setHelpString( const std::string &strHelpString )
		{
			m_strHelpText = strHelpString;
		}

		Parameter::eMode    Parameter::getModeType() const
		{
			return m_eMode;
		}

		void Parameter::setSetType( eSetType md )
		{
			m_eSetType = md;
		}

		void Parameter::setSubContainer( ParameterContainer &cont, eSubType esTp )
		{
			if(m_eType != P_PARAMETERCONTAINER)
				return;

			delete m_subParameter;

			m_subParameter = new ParameterContainer( cont ); // clone
			m_eSubType = esTp;
		}

		template<> ParameterContainer Parameter::getValue() const
		{
			if( !m_subParameter )
				throw std::exception();

			return *m_subParameter;
		}

		template<> std::list<double>   Parameter::getValue() const
		{
			if( this->m_eType != P_LIST or this->m_eSubType != PS_NUMBER )
				throw std::exception();

			ParListN l;
			std::istringstream in(getValueString());

			// value string is expected to be "N0, N1,..."
			while( true )
			{
				double n;
				in >> n;
				l.push_back(n);
				char c;
				in >> c;
				if( !in.good() || in.eof() )
					break;
			}
			return l;
		}

		template<> std::vector<double> Parameter::getValue() const
		{
			if( this->m_eType != P_LIST or this->m_eSubType != PS_NUMBER )
				throw std::exception();

			ParVecN v;
			std::istringstream in(getValueString());

			// value string is expected to be "N0, N1,..."
			while( true )
			{
				double n;
				in >> n;
				v.push_back(n);
				char c;
				in >> c;
				if( !in.good() || in.eof() )
					break;
			}

			return v;
		}

		template<> Parameter::ParVecS  Parameter::getValue() const
		{
			if( this->m_eType != P_LIST or this->m_eSubType != PS_STRING )
							throw std::exception();

			ParVecS v;
			std::string sValue = getValueString();
			if (sValue.empty())
				return v;

			std::string strTemp;
			bool bTrim = true;
			char cSep = ',';


			// split list according to commas (sp?) and remove spaces
			unsigned int iPos0 = 0, iPos1 = 0, iPos2;
			while (iPos0<sValue.size())
			{
				if (bTrim)
				{
					// find first non-space
					while (iPos0 < sValue.size()
						&& isspace(sValue[iPos0]))
						++iPos0;
				}

				// find the separator
				iPos1 = iPos0;
				while (iPos1 < sValue.size()
					&& sValue[iPos1] != cSep)
					++iPos1;

				iPos2 = iPos1;
				if (bTrim)
				{
					// remove trailing spaces
					while (iPos0 < iPos1
						&& isspace(sValue[iPos1-1]))
						--iPos1;
				}

				v.push_back(sValue.substr(iPos0, iPos1-iPos0));
				iPos0 = iPos2+1;
			}

			// did we end with a separator??
			if (iPos2 < sValue.size() && sValue[iPos2] == cSep)
				v.push_back(std::string());

			return v;
		}

		template<> Parameter::ParListS  Parameter::getValue() const
		{
			if( this->m_eType != P_LIST or this->m_eSubType != PS_STRING )
							throw std::exception();
			ParListS v;
			std::string sValue = getValueString();
			if (sValue.empty())
				return v;

			std::string strTemp;
			bool bTrim = true;
			char cSep = ',';


			// split list according to commas (sp?) and remove spaces
			unsigned int iPos0 = 0, iPos1 = 0, iPos2;
			while (iPos0<sValue.size())
			{
				if (bTrim)
				{
					// find first non-space
					while (iPos0 < sValue.size()
						&& isspace(sValue[iPos0]))
						++iPos0;
				}

				// find the separator
				iPos1 = iPos0;
				while (iPos1 < sValue.size()
					&& sValue[iPos1] != cSep)
					++iPos1;

				iPos2 = iPos1;
				if (bTrim)
				{
					// remove trailing spaces
					while (iPos0 < iPos1
						&& isspace(sValue[iPos1-1]))
						--iPos1;
				}

				v.push_back(sValue.substr(iPos0, iPos1-iPos0));
				iPos0 = iPos2+1;
			}

			// did we end with a separator??
			if (iPos2 < sValue.size() && sValue[iPos2] == cSep)
				v.push_back(std::string());

			return v;
		}


		const ParameterContainer &Parameter::getSubContainerConstRef() const
		{
			if(!m_subParameter)
				throw std::exception();

			return *m_subParameter;
		}

		ParameterContainer &Parameter::getSubContainer() const
		{
			if(!m_subParameter)
				throw std::exception();

			return *m_subParameter;
		}

		void Parameter::print() const
		{
			std::cout << " " << ( getValueString().empty() ? "<EMPTY-VALUE>" : getValueString() )
					  << " "
					  << (m_strHelpText.empty() ? "" : (std::string("# ") + m_strHelpText ))
					  << "\t["
					  << getTypeString(getType())
					  << " ; "
					  << getSubTypeString( getSubType() )
					  << " ; "
					  << getSetTypeString( getSetType() )
					  << " ; "
					  << getModeString( getModeType() )
					  << "]" << std::endl;
		}

		std::string Parameter::getTypeString( flowvr::portutils::Parameter::eType t )
		{
			switch(t)
			{
			case flowvr::portutils::Parameter::P_STRING:
				return "STRING";
			case flowvr::portutils::Parameter::P_NUMBER:
				return "NUMBER";
			case flowvr::portutils::Parameter::P_BOOLEAN:
				return "BOOLEAN";
			case flowvr::portutils::Parameter::P_LIST:
				return "LIST";
			case flowvr::portutils::Parameter::P_PARAMETERCONTAINER:
				return "PARAMETERCONTAINER";
			default:
				return "NONE";
			}
		}

		std::string Parameter::getSubTypeString( eSubType st )
		{
			switch(st)
			{
			case flowvr::portutils::Parameter::PS_STRING:
				return "STRING";
			case flowvr::portutils::Parameter::PS_NUMBER:
				return "NUMBER";
			case flowvr::portutils::Parameter::PS_BOOLEAN:
				return "BOOLEAN";
			case flowvr::portutils::Parameter::PS_LIST:
				return "LIST";
			default:
				return "NONE";
			}
		}

		std::string Parameter::getSetTypeString( eSetType st )
		{
			switch( st )
			{
			case flowvr::portutils::Parameter::ST_NOVALUE:
				return "NOVALUE";
			case flowvr::portutils::Parameter::ST_DEFAULT:
				return "DEFAULT";
			case flowvr::portutils::Parameter::ST_CODE:
				return "CODE";
			case flowvr::portutils::Parameter::ST_FILE:
				return "FILE";
			default:
				return "NONE";
			}
		}

		std::string Parameter::getModeString( eMode md )
		{
			switch( md )
			{
			case flowvr::portutils::Parameter::MD_OPTIONAL:
				return "OPTIONAL";
			case flowvr::portutils::Parameter::MD_MANDATORY:
				return "MANDATORY";
			default:
				return "NONE";
			}
		}


		// ########################################################################

		namespace
		{
			void output( const Parameter &param, const std::string &strName, int nCurDepth, int nMaxDepth )
			{
				for( int n=0; n < nCurDepth; ++n)
					std::cout << " ";

				std::cout << "[" << strName << "]:";

				if( param.getType() == Parameter::P_PARAMETERCONTAINER )
				{
					if(nMaxDepth > 0 and (nCurDepth >= nMaxDepth ))
						std::cout <<  std::endl << "<snip -- content culled> (" << nCurDepth << ", " << nMaxDepth << ")";
					else
					{
						std::cout << std::endl;
						if( !param.getSubContainerConstRef().empty() )
							param.getSubContainerConstRef().print(nCurDepth+1, nMaxDepth);
						else
						{
							for( int n=0; n < nCurDepth; ++n)
								std::cout << " ";
							std::cout << " <EMPTY-SUB-CONTAINER>" << std::endl;
						}
					}
				}
				else
				{
					param.print();
				}
			}
		}

		void ParameterContainer::print( int nCurDepth, int nMaxDepth ) const
		{
			for( ParameterContainer::const_iterator it = begin(); it != end(); ++it )
			{
				output( (*it).second, (*it).first, nCurDepth, nMaxDepth );
			}
			if(nCurDepth == 0)
				std::cout << std::endl;
		}


		std::string ParameterContainer::stringify() const
		{
			std::string result;
			::stringify( *this, result, "root");
			return result;
		}

		void ParameterContainer::unstringify( const std::string &strpc )
		{
			std::istringstream in(strpc);

			size_t length;
			char type;
			in >> type >> length;

			std::vector<unsigned char> v(length+2);
			in.read( (char*)&v[0], length+2 );
			std::string root( v.begin()+1, v.end()-1 );

			if( root == "root")
				::unstringify( in, *this );
			else
			{
				std::cerr << "wrong format... no root for parameterlist" << std::endl
						  << "read type=" << type << " with size =" << length
						  << " and header ["<< root << "]" << std::endl;
				std::cerr << strpc << std::endl;
			}
		}

		size_t ParameterContainer::toBase64(const ParameterContainer &ct, std::string &base64 )
		{
			std::string result;
			::stringify( ct, result, "root");

	//		std::cout << "full string: " << result << std::endl;

			std::vector<unsigned char> v( result.begin(), result.end() ), vout;

	//		int length = ::compress( v, vout );

			::encode( v, base64, -1 );
			return base64.size();
		}

		std::string ParameterContainer::fromBase64( const std::string &sBase64, ParameterContainer &c, size_t outputsize )
		{
			std::string result;
			std::vector<unsigned char> v, vout;

			::decode( sBase64, v );

	//		::uncompress(v, vout, outputsize);

			std::string raw( v.begin(), v.end() );

			c.unstringify( raw );

			return raw;
		}

		bool ParameterContainer::hasParameter( const std::string &pname ) const
		{
			return (find(pname) != end());
		}


		const Parameter &ParameterContainer::operator()(const std::string &pName) const
		{
			const_iterator cit = find(pName);
			if( cit == end() )
				return nilP;
			return (*cit).second;
		}

		Parameter &ParameterContainer::operator()(const std::string &pname)
		{
			iterator cit = find(pname);
			if( cit == end() )
				return nilp;
			return (*cit).second;
		}
	} // namespace portutils
} // namespace flowvr

/* make function easily accessible from outside */

extern "C" {

int encode_string_to_base64(const char *s_in, char *s_out, int buf_size) {
  std::string s(s_in), vout; 
  std::vector<unsigned char> v( s.begin(), s.end() );
  ::encode(v, vout, -1);
  strncpy(s_out, vout.c_str(), buf_size);
  return strlen(s_out);
}

int decode_string_from_base64(const char *s_in, char *s_out, int buf_size) {
  std::string s(s_in), vout;
  std::vector<unsigned char> v;
  ::decode(s, v); 
  int ndecoded = v.size();
  v.push_back(0);
  strncpy(s_out, (const char*)&v[0], buf_size);
  return ndecoded;
}


};
