/* 
 * Copyright © 2009 CNRS, INRIA, Université Bordeaux 1
 *
 * This software is a computer program whose purpose is to provide
 * abstracted information about the hardware topology.
 *
 * This software is governed by the CeCILL-B license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL-B
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#include <flowvr/config.h>
#include <flowvr/topology/topology.h>
#include <flowvr/topology/private.h>
#include <flowvr/topology/debug.h>

#ifdef HAVE_XML

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <assert.h>

int
topo_backend_xml_init(struct topo_topology *topology, const char *xmlpath)
{
  xmlDoc *doc = NULL;

  assert(topology->backend_type == TOPO_BACKEND_NONE);

  LIBXML_TEST_VERSION;

  doc = xmlReadFile(xmlpath, NULL, 0);
  if (!doc)
    return -1;

  topology->backend_params.xml.doc = doc;
  topology->is_fake = 1;
  topology->backend_type = TOPO_BACKEND_XML;
  return 0;
}

void
topo_backend_xml_exit(struct topo_topology *topology)
{
  assert(topology->backend_type == TOPO_BACKEND_XML);
  xmlFreeDoc((xmlDoc*)topology->backend_params.xml.doc);
  xmlCleanupParser();
  topology->backend_type = TOPO_BACKEND_NONE;
}

static void
topo__process_root_attr(struct topo_topology *topology,
			const xmlChar *_name, const xmlChar *_value)
{
  const char *name = (const char *) _name;
/* unused for now   const char *value = (const char *) _value; */

  fprintf(stderr, "ignoring unknown root attribute %s\n", name);
}

static void
topo__process_object_attr(struct topo_topology *topology, struct topo_obj *obj,
			  const xmlChar *_name, const xmlChar *_value)
{
  const char *name = (const char *) _name;
  const char *value = (const char *) _value;

  if (!strcmp(name, "type")) {
    /* already handled */
    return;
  }

  else if (!strcmp(name, "os_index"))
    obj->os_index = strtoul(value, NULL, 10);
  else if (!strcmp(name, "cpuset"))
    topo_cpuset_from_string(value, &obj->cpuset);

  else if (!strcmp(name, "memory_kB")) {
    unsigned long lvalue = strtoul(value, NULL, 10);
    switch (obj->type) {
      case TOPO_OBJ_CACHE:
	obj->attr->cache.memory_kB = lvalue;
	break;
      case TOPO_OBJ_NODE:
	obj->attr->node.memory_kB = lvalue;
	break;
      case TOPO_OBJ_MACHINE:
	obj->attr->machine.memory_kB = lvalue;
	break;
      case TOPO_OBJ_SYSTEM:
	obj->attr->system.memory_kB = lvalue;
	break;
      default:
	fprintf(stderr, "ignoring memory_kB attribute for object type without memory\n");
	break;
    }
  }

  else if (!strcmp(name, "depth")) {
    unsigned long lvalue = strtoul(value, NULL, 10);
    switch (obj->type) {
      case TOPO_OBJ_CACHE:
	obj->attr->cache.depth = lvalue;
	break;
      case TOPO_OBJ_MISC:
	obj->attr->misc.depth = lvalue;
	break;
      default:
	fprintf(stderr, "ignoring depth attribute for object type without depth\n");
	break;
    }
  }

  else if (!strcmp(name, "huge_page_free")) {
    unsigned long lvalue = strtoul(value, NULL, 10);
    switch (obj->type) {
      case TOPO_OBJ_NODE:
	obj->attr->node.huge_page_free = lvalue;
	break;
      case TOPO_OBJ_MACHINE:
	obj->attr->machine.huge_page_free = lvalue;
	break;
      case TOPO_OBJ_SYSTEM:
	obj->attr->system.huge_page_free = lvalue;
	break;
      default:
	fprintf(stderr, "ignoring huge_page_free attribute for object type without huge pages\n");
	break;
    }
  }

  else if (!strcmp(name, "huge_page_size_kB")) {
    unsigned long lvalue = strtoul(value, NULL, 10);
    switch (obj->type) {
      case TOPO_OBJ_MACHINE:
	obj->attr->machine.huge_page_size_kB = lvalue;
	break;
      case TOPO_OBJ_SYSTEM:
	obj->attr->system.huge_page_size_kB = strtoul(value, NULL, 10);
	break;
      default:
	fprintf(stderr, "ignoring huge_page_size_kB attribute for object type without huge pages\n");
	break;
    }
  }

  else if (!strcmp(name, "dmi_board_vendor")) {
    switch (obj->type) {
      case TOPO_OBJ_MACHINE:
	obj->attr->machine.dmi_board_vendor = strdup(value);
	break;
      case TOPO_OBJ_SYSTEM:
	obj->attr->system.dmi_board_vendor = strdup(value);
	break;
      default:
	fprintf(stderr, "ignoring dmi_board_vendor attribute for object type without DMI board\n");
	break;
    }
  }

  else if (!strcmp(name, "dmi_board_name")) {
    switch (obj->type) {
      case TOPO_OBJ_MACHINE:
	obj->attr->machine.dmi_board_name = strdup(value);
	break;
      case TOPO_OBJ_SYSTEM:
	obj->attr->system.dmi_board_name = strdup(value);
	break;
      default:
	fprintf(stderr, "ignoring dmi_board_name attribute for object type without DMI board\n");
	break;
    }
  }

  else
    fprintf(stderr, "ignoring unknown object attribute %s\n", name);
}

static void
topo__look_xml_attr(struct topo_topology *topology, struct topo_obj *obj,
		    const xmlChar *attrname, xmlNode *node)
{
  /* use the first valid attribute content */
  for (; node; node = node->next) {
    if (node->type == XML_TEXT_NODE) {
      if (node->content && node->content[0] != '\0' && node->content[0] != '\n') {
	if (obj)
	  topo__process_object_attr(topology, obj, attrname, node->content);
	else
	  topo__process_root_attr(topology, attrname, node->content);
	break;
      }
    } else {
	fprintf(stderr, "ignoring unexpected xml attr node type %u name %s\n", node->type, (const char*) node->name);
    }
  }
}

static void
topo__look_xml_node(struct topo_topology *topology, xmlNode *node, int depth)
{
  for (; node; node = node->next) {
    if (node->type == XML_ELEMENT_NODE) {
      xmlAttr *attr = NULL;
      struct topo_obj *obj = NULL;

      if (depth == 0) {
	/* root node should be in root class */
	if (strcmp((const char *) node->name, "root"))
	  fprintf(stderr, "root node of class `%s' instead of `root'\n", (const char*) node->name);

	/* no object yet */

      } else {
	/* object node should be in object class */
	if (strcmp((const char*) node->name, "object"))
	  fprintf(stderr, "object node of class `%s' instead of `object'\n", (const char*) node->name);

	if (depth > 1)
	  obj = topo_alloc_setup_object(TOPO_OBJ_TYPE_MAX, -1);
	else
	  obj = topology->levels[0][0];
      }

      /* first determine the object type */
      for (attr = node->properties; attr; attr = attr->next) {
	if (attr->type == XML_ATTRIBUTE_NODE) {
	  xmlNode *node;
	  for (node = attr->children; node; node = node->next) {
	    if (node->type == XML_TEXT_NODE) {
	      if (node->content && node->content[0] != '\0' && node->content[0] != '\n') {
		if (!strcmp((const char*) attr->name, "type")) {
		  obj->type = topo_obj_type_of_string((const char*) node->content);
		  if (obj->type == TOPO_OBJ_TYPE_MAX)
		    fprintf(stderr, "ignoring unknown object type %s\n", (const char*) node->content);
		  else
		    break;
		}
	      }
	    } else {
		fprintf(stderr, "ignoring unexpected xml attr node type %u name %s\n", node->type, (const char*) node->name);
	    }
	  }
	  if (obj->type == TOPO_OBJ_TYPE_MAX) {
	    fprintf(stderr, "ignoring attributes of object without type\n");
	    return;
	  }
	} else {
	  fprintf(stderr, "ignoring unexpected xml attr type %u\n", node->type);
	}
      }

      /* process attributes */
      for (attr = node->properties; attr; attr = attr->next) {
	if (attr->type == XML_ATTRIBUTE_NODE) {
	  if (attr->children)
	    topo__look_xml_attr(topology, obj, attr->name, attr->children);
	} else {
	  fprintf(stderr, "ignoring unexpected xml attr type %u\n", node->type);
	}
      }

      if (depth == 0) {
	/* no object in xml doc root */
	assert (!obj);

      } else if (depth == 1) {
	/* system object is already there */
	if (obj->type != TOPO_OBJ_SYSTEM) {
	  fprintf(stderr, "enforcing System type in top level instead of %s\n",
		  topo_obj_type_string(obj->type));
	  obj->type = TOPO_OBJ_SYSTEM;
	}

      } else {
	/* add object */
	if (obj->type == TOPO_OBJ_TYPE_MAX) {
	  fprintf(stderr, "ignoring object with invalid type %u\n", obj->type);
	  free(obj);
	} else if (obj->type == TOPO_OBJ_SYSTEM) {
	  fprintf(stderr, "ignoring system object at invalid depth %d\n", depth);
	  free(obj);
	} else {
	  if (!topo_cpuset_isincluded(&obj->cpuset, &topology->levels[0][0]->cpuset))
	    fprintf(stderr, "ignoring object (cpuset %s) not covered by system (cpuset %s)\n",
		    TOPO_CPUSET_PRINTF_VALUE(&obj->cpuset),
		    TOPO_CPUSET_PRINTF_VALUE(&topology->levels[0][0]->cpuset));
	  else
	    topo_add_object(topology, obj);
	}
      }

      /* process children */
      if (node->children)
	topo__look_xml_node(topology, node->children, depth+1);

    } else if (node->type == XML_TEXT_NODE) {
      if (node->content && node->content[0] != '\0' && node->content[0] != '\n')
	fprintf(stderr, "ignoring object text content %s\n", (const char*) node->content);      
    } else {
      fprintf(stderr, "ignoring unexpected xml node type %u\n", node->type);
    }
  }
}

static int
topo_xml_set_cpubind(void) {
  return 0;
}

void
topo_look_xml(struct topo_topology *topology)
{
  xmlNode* root_node;

  topology->set_cpubind = (void*) topo_xml_set_cpubind;
  topology->set_thisproc_cpubind = (void*) topo_xml_set_cpubind;
  topology->set_thisthread_cpubind = (void*) topo_xml_set_cpubind;
  topology->set_proc_cpubind = (void*) topo_xml_set_cpubind;
  topology->set_thread_cpubind = (void*) topo_xml_set_cpubind;

  root_node = xmlDocGetRootElement((xmlDoc*) topology->backend_params.xml.doc);

  topo__look_xml_node(topology, root_node, 0);
}

#endif /* HAVE_XML */
