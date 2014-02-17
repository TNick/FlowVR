/******************************************************************
*                                                                 *
*       File : topo.cpp                                       *
*                                                                 *
*                                                                 *
*       Contact : Ingo Assenmacher (ingo.assenmacher@imag.fr)     *
*                                                                 *
******************************************************************/

#include "flowvr/topo.h"
#include <flowvr/ipc/atomic.h>

namespace flowvr
{
	Topo::Topo(bool bInit)
	: valid(false)
	, refCnt(new flowvr::ipc::MTAtomicInt(1)) // init to 1, as we reference the pointer ourselves
	{
		if(bInit)
		{
			topo_topology_init(&topology.t_topology);
			topo_topology_load(topology.t_topology);
			topo_topology_get_info(topology.t_topology, &topology.t_topology_info);
			valid = true;
		}
	}

	Topo::Topo(const Topo &oOther)
	: valid( oOther.valid )
	, refCnt( oOther.refCnt ) // copy ref-count from oOther
	{
		(*refCnt).inc(); // increase by 1, this is us ;)
	}

	Topo::~Topo()
	{
		if( (*refCnt).dec_and_test_null() ) // check whether we are the last ones
		{
			if(valid) // yes, destroy the valid topology once
				topo_topology_destroy(topology.t_topology);
			delete refCnt; // and while at it: delete the refCnt as well
		}
	}

	void Topo::operator=( const Topo &oOther )
	{
		if( this->refCnt ) // see if we are already ref-counted
		{
			if(this->refCnt->dec_and_test_null() == true) // note: this can not evaluate
				                                          // to true when this->refCnt == oOther.refCnt
			{
				delete this->refCnt; // delete old ref-count, nobody uses it.
				if(valid)
					topo_topology_destroy(topology.t_topology);
			}
		}

		this->valid    = oOther.valid; // copy validity flag
		this->topology = oOther.topology; // copy topology
		this->refCnt   = oOther.refCnt; // adopt ref-counter

		(*this->refCnt).inc(); // and increase by one
	}

	bool Topo::getIsValid() const
	{
		return valid;
	}

	topo_cpuset_t Topo::getSystemCPUMask() const
	{
		topo_obj_t obj = obj = topo_get_system_obj(topology.t_topology);
		return obj->cpuset;
	}

	int Topo::getNumberOfPhysicalCPUs() const
	{
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_SOCKET);
		return getLevelCount(depth);
	}

	int Topo::getNumberOfLogicalCPUs() const
	{
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_PROC);
		return getLevelCount(depth);
	}


	int Topo::getNumberOfCores() const
	{
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_CORE);
		return getLevelCount(depth);
	}

	unsigned long int Topo::getL3CacheSize(int nSocket) const
	{
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_NODE);
		topo_obj *obj = topo_get_obj_by_depth( topology.t_topology, depth, nSocket );
		if(obj)
			return obj->attr->machine.memory_kB;
		else
			return 0;
	}

	unsigned long int Topo::getL2CacheSize(int nCore) const
	{
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_CORE);
		topo_obj *obj = topo_get_obj_by_depth( topology.t_topology, depth, nCore);
		if(obj)
		{
			// get parent (cache-level)
			obj = obj->father;
			if( obj->attr->cache.depth == 1 )
				obj = obj->father;

			return obj->attr->cache.memory_kB;
		}
		else
			return 0;
	}

	unsigned long int Topo::getL1CacheSize(int nCore) const
	{
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_CORE);
		topo_obj *obj = topo_get_obj_by_depth( topology.t_topology, depth, nCore);
		if(obj)
		{
			// get parent (cache-level)
			obj = obj->father;
			if(obj->attr->cache.depth == 1)
				return obj->attr->cache.memory_kB;
			else
				return 0;
		}
		else
			return 0;
	}


	std::deque<topo_cpuset_t> Topo::getL2GroupSet() const
	{
		std::deque<topo_cpuset_t> ret;

		// cache information may be a multiple in the topology tree,
		// so iterate from socket to socket and collect all l2 caches
		unsigned depth = topo_get_type_or_above_depth(topology.t_topology, TOPO_OBJ_SOCKET);

		// iterate over sockets, find l2 cache entries

		for(int i=0; i < topo_get_depth_nbobjs(topology.t_topology, depth); ++i)
		{
			topo_cpuset_t cpuset;
			// get socket
			topo_obj *obj = topo_get_obj_by_depth( topology.t_topology, depth, i );
			// iterate over children of this socket
			if( obj->first_child && obj->first_child->type != TOPO_OBJ_CACHE )
			{
				// NUMA?
				obj = obj->first_child; // skip
			}

			for( topo_obj *child = obj->first_child; child; child=child->next_cousin)
			{
				if(child->type == TOPO_OBJ_CACHE)
				{
					if(child->attr->cache.depth == 2)
					{
						topo_cpuset_orset(&cpuset, &child->cpuset);
					}
				}
			}
			ret.push_back(cpuset);
		}
		return ret;
	}

	std::deque<topo_cpuset_t> Topo::getL3GroupSet() const
	{
		std::deque<topo_cpuset_t> ret;
//		// cache information may be a multiple in the topology tree,
//		// so iterate from socket to socket and collect all l2 caches
//		unsigned depth = topo_get_type_or_above_depth(topology.t_topology, TOPO_OBJ_SOCKET);
//
//		// iterate over sockets, find l2 cache entries
//
//
//		for(int i=0; i < topo_get_depth_nbobjs(topology.t_topology, depth); ++i)
//		{
//			topo_cpuset_t cpuset;
//			// get socket
//			topo_obj *obj = topo_get_obj_by_depth( topology.t_topology, depth, i );
//			// iterate over children of this socket
//			if( obj->first_child && obj->first_child->type == TOPO_OBJ_CACHE )
//			{
//				// NON-NUMA!
//				continue;
//			}
//			topo_cpuset_orset(&cpuset, &obj->cpuset);
//		}

		return ret;
	}

//	static void dive( topo_obj *root, topo_obj_type_t tp, std::list<topo_obj*> &storage)
//	{
//		if( root->type == tp )
//			storage.push_back(root);
//		else
//		{
//			for(topo_obj *child = root->first_child; child; child = child->next_sibling )
//			{
//				dive( child, tp, storage );
//			}
//		}
//	}

	std::deque<topo_cpuset_t> Topo::getSocketGroupSet() const
	{
		std::deque<topo_cpuset_t> ret;
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_SOCKET);

		for( int i=0; i < topo_get_depth_nbobjs(topology.t_topology, depth); ++i)
		{
			topo_obj *obj = topo_get_obj_by_depth( topology.t_topology, depth, i );
			if(obj)
				ret.push_back(obj->cpuset);
		}
		return ret;
	}



	bool Topo::getL2CacheShared(int OSCoreId) const
	{
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_PROC);
		for( int j=0; j < getLevelCount(depth); ++j)
		{
			topo_obj *obj = topo_get_obj_by_depth( topology.t_topology, depth, j);
			if(obj && obj->os_index == OSCoreId )
			{
				obj = obj->father->father; // proc -> core -> cache
				if(obj->attr->cache.depth == 1)
					obj = obj->father; // l1 cache -> l2 cache

				return (obj->first_child != obj->last_child);
			}
		}
		return false;
	}

	bool Topo::getL1CacheShared(int OSCoreId) const
	{
		unsigned depth = topo_get_type_or_below_depth(topology.t_topology, TOPO_OBJ_PROC);
		topo_obj *obj = topo_get_obj_by_depth( topology.t_topology, depth, 0);
		if(obj)
		{
			for( int j=0; j < getLevelCount(depth); ++j)
				if( obj->os_index == OSCoreId )
				{
					obj = obj->father;
					if(obj->attr->cache.depth == 1)
						return (obj->first_child != obj->last_child);
				}
		}
		return false;
	}


	int Topo::getLevelCount(int depth) const
	{
		return topo_get_depth_nbobjs(topology.t_topology, depth);
	}
}
