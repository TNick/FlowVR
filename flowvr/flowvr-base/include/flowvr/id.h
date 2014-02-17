/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                         Base Libraries                          *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA and                                                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (FRE 2490). ALL RIGHTS RESERVED.                                *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Ronan Gaugne,                                                *
*    Valerie Gouranton,                                           *
*    Loick Lecointre,                                             *
*    Sebastien Limet,                                             *
*    Bruno Raffin,                                                *
*    Sophie Robert,                                               *
*    Emmanuel Melin.                                              *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: include/flowvr/id.h                                       *
*                                                                 *
* Contacts:                                                       *
*  09/13/2004 Jeremie Allard <Jeremie.Allard@imag.fr>             *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_ID_H
#define FLOWVR_ID_H

#include <cstddef>
#include <map>

// hash_map is vendor defined...
#ifdef __GNUC__
# if __GNUC__ < 3
#  include <hash_map.h>
namespace std { using ::hash_map; }
# elif __GNUC__ == 3
#  include <ext/hash_map>
#  if __GNUC_MINOR__ != 0
namespace std { using __gnu_cxx::hash_map; }
#  endif
# elif __GNUC__ >= 4 
# include <tr1/unordered_map>
# else
#  include <ext/hash_map>
namespace std { using __gnu_cxx::hash_map; }
# endif
#else // ...  there are other compilers, right?
# include <hash_map>
#endif

namespace flowvr
{

#ifdef _MSC_VER
typedef unsigned __int64 ID;
#else
typedef unsigned long long ID;
#endif

/// Hash Function for IDs
struct hashID : public std::unary_function<ID, std::size_t> {
  std::size_t operator()(ID id) const { return static_cast<std::size_t>(id^(id>>32)); }
};

#if __GNUC__ == 4
template <class T>
class IDMap: public std::tr1::unordered_map<ID,T,hashID>
{
};
#else
template<class T>
class IDMap : public std::hash_map<ID,T,hashID>
{
};
#endif

} // namespace flowvr

#endif
