/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.	                                          *
 *                                                                 *
 * This source is covered by the GNU LGPL, please refer to the     *
 * COPYING file for further information.                           *
 *                                                                 *
 *-----------------------------------------------------------------*
 *                                                                 *
 *  Original Contributors:                                         *
 *    Jeremie Allard,                                              *
 *    Thomas Arcila,                                               *
 *    Jean-Denis Lesage.                                           *	
 *    Clement Menier,                                              *
 *    Bruno Raffin                                                 *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
 * COPYRIGHT (C) 2006 by                                           *
 * Laboratoire Informatique et Distribution (UMR5132)              *
 * ALL RIGHTS RESERVED.	                                           *
 *                                                                 *
 * This source is covered by the GNU LGPL, please refer to the     *
 * COPYING file for further information.                           *
 *                                                                 *
 *-----------------------------------------------------------------*
 *                                                                 *
 *  Original Contributors:                                         *
 *    Jeremie Allard,                                              *
 *    Thomas Arcila,                                               *
 *    Jean-Denis Lesage.                                           *	
 *    Clement Menier,                                              *
 *    Bruno Raffin                                                 *
 *                                                                 *
 ******************************************************************/

#include "flowvr/app/core/arch.h"
#include "flowvr/app/core/msg.h"
#include <ctype.h>
#include <limits.h>

namespace flowvr { namespace app {

    void Machine::addNetwork(const Network& net) 
    {
        networks.push_back(net);
    };


    Processor::Processor( const xml::DOMElement* e) throw (XMLAttributeInvalid, TypeIncompatible) 
    {
        const char* type_ = e->Attribute("type");
        if (!type_)
            throw XMLAttributeInvalid("type", "Processor", e->Row(), e->Column(), __FUNCTION_NAME__);

        type = a;
        /// \todo translation of type

        const char* freq_ = e->Attribute("freq");
        if (!freq_)
            throw XMLAttributeInvalid("freq", "Processor", e->Row(), e->Column(), __FUNCTION_NAME__);
        try
            {
                freq = convertTo<unsigned int>(freq_);
            }
        catch (const BadConversion& e)
            {
                throw TypeIncompatible("freq", "unsigned int", __FUNCTION_NAME__);
            };
    };

    Device::Device(const xml::DOMElement* e) throw (XMLAttributeInvalid, CustomException )
    {
        const xml::DOMAttr* attribute = e->FirstAttribute();
        while(attribute && attribute != e->LastAttribute())
            {
                parametersmap[attribute->Name()] = std::string(e->Attribute(attribute->Name()));
                attribute = attribute->Next();
            }
        parametersmap[attribute->Name()] =  std::string(e->Attribute(attribute->Name()));


        const char* type_ = e->Attribute("type");

        if (!type_)
            throw XMLAttributeInvalid("type", "Device", e->Row(), e->Column(), __FUNCTION_NAME__);

        std::string stype_(type_);
        // We transform the string. all characters will be convert to uppercase
        for (std::string::iterator it = stype_.begin(); it != stype_.end(); ++it)
            {
                *it = toupper(*it);
            }

        if (stype_ == "INPUT")
            type = INPUTDEVICE;
        else 
            {
                if (stype_ == "OUTPUT")
                    type = OUTPUTDEVICE;
                else
                    throw CustomException("Device type must be INPUT or OUTPUT", __FUNCTION_NAME__);
            }

        const char* name_ = e->Attribute("name");

        if (!name_)
            throw XMLAttributeInvalid("name", "Device", e->Row(), e->Column(), __FUNCTION_NAME__);

        name = name_;

        const char* number_ = e->Attribute("number");

        if (!number_)
            number = -1; 
        else
            {
                try
                    {
                        number = convertTo<int>(number_);
                    }
                catch (const BadConversion& e)
                    {
                        throw TypeIncompatible("number", "int", __FUNCTION_NAME__);
                    };
            }

        const char* device_ = e->Attribute("device");

        if (!device_)
            device = 0; 
        else
            {
                try
                    {
                        device = convertTo<unsigned int>(device_);
                    }
                catch (const BadConversion& e)
                    {
                        throw TypeIncompatible("device", "unsigned int", __FUNCTION_NAME__);
                    };
            }

    };

    Machine::Machine(const xml::DOMElement* e, const std::vector<Network>& net) throw (XMLAttributeInvalid, XMLChildIncorrect, CustomException)
    {
        const xml::DOMElement* child = e->FirstChildElement();
        while (child)
            {
                std::string type(child->ValueStr());
                // We transform the string. all characters will be convert to uppercase
                for (std::string::iterator it = type.begin(); it != type.end(); ++it)
                    {
                        *it = toupper(*it);
                    }
                const unsigned int ntype = ( type == "DEVICE" ? 0 : ( type == "PROCESSOR" ? 1 : ( type == "NI" ? 2 : (type =="ID" ? 3 : 4))));
                switch(ntype)
                    {
                    case 0 : 
                        try
                            {
                                Device d(child);
                                devices.push_back(d);
                            }
                        catch (const CustomException& e)
                            {
                                Msg::warningException(e); 
                            };
                        break;

                    case 1 :
                        try
                            {
                                Processor p(child);
                                processors.push_back(p);
                            }
                        catch (const CustomException& e)
                            {
                                Msg::warningException(e);
                            };
                        break;

                    case 2 : 
                        {
                            const char* nameNetwork = child->Attribute("netid");
                            if (!nameNetwork)
                                {
                                    XMLAttributeInvalid exception = XMLAttributeInvalid("netid", "Machine/ni", child->Row(), child->Column(), __FUNCTION_NAME__);
                                    Msg::warningException(exception);
                                }
                            else
                                {
                                    std::string netid(nameNetwork);
                                    std::vector<Network>::const_iterator it = net.begin();
                                    while (it != net.end() && (it->getId() != netid))
                                        it++;
                                    if (it != net.end())
                                        {
                                            // We found the network
                                            networks.push_back(*it);
                                            // create the names list of this interface
                                            std::list<std::string> ids;
                                            aliases.push_back(ids);



                                            // Now find name of the machine in this network
                                            const xml::DOMElement* grandChild = child->FirstChildElement();
                                            while ( grandChild )
                                                {

                                                    const char* text = grandChild->GetText();
                                                    if (text)
                                                        {
                                                            const std::string id(text) ;
                                                            //the aliases will be in the last network found
                                                            aliases.back().push_back(id);
                                                        }
                                                    else
                                                        throw XMLAttributeInvalid("id", "Machine/ni/id", grandChild->Row(), grandChild->Column(), __FUNCTION_NAME__);

                                                    grandChild = dynamic_cast<const xml::DOMElement*>(child->IterateChildren(grandChild));
                                                }
                                        }
                                    else
                                        throw IdNotFoundException("network", netid, "architecture", "", __FUNCTION_NAME__);
                                }
                        }
                        break;
                    case 3 :
                        {
                            id_ = child->GetText();
                            Msg::debug("Create Machine with id = " + id_, __FUNCTION_NAME__);
                        }
                        break;
                    default:
                        throw XMLChildIncorrect(type, "Machine", e->Row(), e->Column(), __FUNCTION_NAME__);
                        break;
                    }

                child = dynamic_cast<const xml::DOMElement*>(e->IterateChildren(child)); 
            }
    };

    Network::Network(const xml::DOMElement* e) throw (TypeIncompatible, XMLAttributeInvalid)
    {

        const char* name_ = e->Attribute("netid");

        if (!name_)
            throw XMLAttributeInvalid("netid", "Network", e->Row(), e->Column(), __FUNCTION_NAME__);

        name.assign(name_);

        const char* bandwith_ = e->Attribute("bandwith");
        if (!bandwith_)
            bandwith = UINT_MAX ;
        else
            {
                try
                    {
                        bandwith = convertTo<unsigned int>(bandwith_);
                    }
                catch (const BadConversion& e)
                    {
                        throw TypeIncompatible("bandwith", "unsigned int", __FUNCTION_NAME__);
                    };
            }


    };

    const Network& Cluster::findNetwork(std::string& id_) const throw (CustomException)
    {
        unsigned int iNetwork = 0;
        while (iNetwork < getNbNetworks() && (networks[iNetwork]).getId() != id_)
            iNetwork++;
        if (iNetwork != getNbNetworks())
            return  networks[iNetwork];
        throw IdNotFoundException("network", id_, "cluster", this->id, __FUNCTION_NAME__);
    };

    const Machine& Cluster::findMachine(const std::string& id_) const throw (CustomException)
    {
        for (std::vector<Machine>::const_iterator it = machines.begin(); it != machines.end(); ++it)
            {
                if (it->getId() == id_)
                    return *it;
            }
        throw IdNotFoundException("machine", id_, "cluster", this->id, __FUNCTION_NAME__);
    };	
	
    const Machine* Cluster::findMachine(const Network& net, const std::string& id_) const
    {
        unsigned int iMachine = 0; 
        while (iMachine < getNbMachines())
            {
                const Machine& m = getMachine(iMachine);
                unsigned int iNetwork = 0;
                while (iNetwork < m.getNumberNetworks() && m.getNetwork(iNetwork) != net)
                    iNetwork++;

                if (iNetwork < m.getNumberNetworks())
                    {
                        const std::list<std::string>& listAliases = m.getAliases(iNetwork);
                        for(std::list<std::string>::const_iterator it = listAliases.begin(); it != listAliases.end(); ++it)
                            {
                                if (*it == id_)
                                    return &m;
                            }
                    }
                iMachine++;
            }
        return NULL;
    };


    Cluster::Cluster(const xml::DOMElement* e) throw (XMLAttributeInvalid)
    {

        const char* id_ = e->Attribute("id");
        if (id_)
            id = id_;
        else
            throw XMLAttributeInvalid("id", "cluster", e->Row(), e->Column(), __FUNCTION_NAME__);

        const xml::DOMElement* child = e->FirstChildElement();
        while (child)
            {
                std::string type(child->ValueStr());
                // Convert to upper case
                for (std::string::iterator it = type.begin(); it != type.end(); ++it)
                    *it = toupper(*it);

                try 
                    {
                        if (type == "HOST")
                            {
                                Machine m(child, (const std::vector<Network>&) networks);
                                machines.push_back(m);
                            }
                        else
                            {
                                if (type == "NETWORK")
                                    {
                                        Network n(child);
                                        networks.push_back(n);
                                    }
                                else
                                    {
                                        if( type == "PARAMETER")
                                            {
                                                std::string key = std::string(child->Attribute("key"));
                                                parameters[key] = std::string(child->GetText());
                                            }
		
                                        else
                                            throw XMLChildIncorrect(type, "Architecture", e->Row(), e->Column(), __FUNCTION_NAME__);
                                    }
                            }
                    }
                catch (const CustomException& e)
                    {
                        Msg::warningException(e);
                    }


                child = dynamic_cast<const xml::DOMElement*>(e->IterateChildren(child));
            };

    };


    Architecture::Architecture() : clusters(), fileName("")
    {
        Cluster c; 
        Machine m;
        c.addMachine(m);
        clusters.push_back(c);
        iterator_cluster = 0; 
    };

    Architecture::Architecture(const std::string& fileName_):fileName(fileName_)
    {
        importArchitecture(fileName_);
    };
	
    void Architecture::importArchitecture(const std::string& fileName_)
    {
        // clean clusters
        clusters.clear();
        fileName = fileName_;
        xml::DOMDocument file;
        if (! file.LoadFile(fileName_) )
            throw FileException(file.ErrorDesc(), fileName_, __FUNCTION_NAME__);
        const xml::DOMElement* e = file.RootElement();

        // CONSTRUCTION
        const xml::DOMElement* child = e->FirstChildElement();
        while (child)
            {
                Cluster c(child);
                add(child);
                child = dynamic_cast<const xml::DOMElement*>(e->IterateChildren(child));
            }
        iterator_cluster = 0; 
    };


    const Machine* Architecture::findMachine(const Network& net, const std::string& id, unsigned int current_cluster) const
    {
        const Machine * m = NULL;
        /*
          for(std::vector<Cluster>::const_iterator it = clusters.begin(); m == NULL && it < clusters.end() ; it++)
          {
          m = it->findMachine(net,id);
          }*/
        m = clusters[current_cluster].findMachine(net, id);
        return m;
    };

    const Machine& Architecture::findMachine(const std::string& id, unsigned int current_cluster) const throw (CustomException)
    {
        const Machine *  m = NULL;
        bool one = false;
        bool multiple = false;
      
        //      for(std::vector<Cluster>::const_iterator it = clusters.begin(); !multiple  && it < clusters.end() ; it++)
        //	{
        try
            {
                m =&(clusters[current_cluster].findMachine(id));
                if (one) multiple = true;
                one =true; 
            }
        catch (const CustomException& e)
            {
            }
        //	}

        if (!one) throw  IdNotFoundException("machine", id, "clusters", "", __FUNCTION_NAME__);
        if (multiple) throw  SameIdException("machine", id, "clusters", "", __FUNCTION_NAME__);
        return *m; 
    };

    std::ostream& operator<< (std::ostream& out, const Processor& p)
    {
        out << "Processor : type ( " << p.type << " ) with freq = " << p.freq << " MHz";
        return out;
    };


    std::ostream& operator<< (std::ostream& out, const Device& d)
    {
        out << "Device : Name ( " << d.name << " ) type ( " << d.type << " )";
        return out;
    };

    std::ostream& operator<< (std::ostream& out, const Machine& m)
    {
        out << "Machine : " << m.id_ << std::endl;
        for (std::vector<Processor>::const_iterator it = m.processors.begin(); it != m.processors.end(); ++it)
            {
                out << *it << std::endl;
            }
        for (std::vector<Device>::const_iterator it = m.devices.begin(); it != m.devices.end(); ++it)
            {
                out << *it << std::endl;
            }
        out << "Network interfaces :";
        // We print the nbNetwork-th network
        unsigned int nbNetwork = 0;
        for (std::vector<Network>::const_iterator it = m.networks.begin(); it != m.networks.end(); ++it)
            {
                out << std::endl << "Network : " << it->getId() << ". Name : " ; 
                for(std::list<std::string>::const_iterator itname = m.aliases[nbNetwork].begin(); itname != m.aliases[nbNetwork].end(); ++itname)
                    {
                        out << std::endl << "   + " << *itname ;
                    }
                nbNetwork++;
            }
        return out;
    };


    std::ostream& operator<< (std::ostream& out, const Network& n)
    {
        out << "Network : name ( " << n.name << " ) with bandwith " << n.bandwith << " Mb/s";
        return out;
    };

    bool operator==(const Network& n1, const Network& n2) { return n1.name == n2.name; };
    
    bool operator!=(const Network& n1, const Network& n2) { return n1.name != n2.name; };

    std::ostream& operator<< (std::ostream& out, const Cluster& a)
    {
        std::cout << "cluster " << a.id << std::endl;
        for (std::vector<Network>::const_iterator it = a.networks.begin(); it != a.networks.end(); ++it)
            {
                out << *it << std::endl; 
            }

        for (std::vector<Machine>::const_iterator it = a.machines.begin(); it != a.machines.end(); ++it)
            {
                out << *it << std::endl; 
            }
        return out;
    };

    std::ostream& operator<< (std::ostream& out, const Architecture& a)
    {
        for (std::vector<Cluster>::const_iterator it = a.clusters.begin(); it != a.clusters.end(); ++it)
            {
                out << *it;
            };
        return out;
    };


    std::string Architecture::getHostsList(unsigned int nbCameras, const std::string& name, unsigned int current_cluster) const
    {
        std::vector<std::string> cameraHostvector(nbCameras);
        //		for (std::vector<Cluster>::const_iterator it = clusters.begin(); it != clusters.end(); ++it)
        //		{
        for (unsigned int i = 0; i != clusters[current_cluster].getNbMachines(); ++i)
            {
                for (unsigned j = 0; j != clusters[current_cluster].getMachine(i).getNumberDevices(); ++j)
                    {
                        if(clusters[current_cluster].getMachine(i).getDevice(j).getName() == name && clusters[current_cluster].getMachine(i).getDevice(j).getNumber() > -1)
                            {
                                cameraHostvector[clusters[current_cluster].getMachine(i).getDevice(j).getNumber()] = clusters[current_cluster].getMachine(i).getId();
                            }
                    }
            }
        //		}
        std::string result;
        for (unsigned int i = 0; i != nbCameras; ++i)
            {
                result += (cameraHostvector[i] + " ");
            }
        return result;
    };

    std::string Architecture::getDevicesList(unsigned int nbCameras, const std::string& name, unsigned int current_cluster) const
    {
        std::vector<unsigned int> cameraDevicevector(nbCameras);
        //	for (std::vector<Cluster>::const_iterator it = clusters.begin(); it != clusters.end(); ++it)
        //	{
        for (unsigned int i = 0; i != clusters[current_cluster].getNbMachines(); ++i)
            {
                for (unsigned j = 0; j != clusters[current_cluster].getMachine(i).getNumberDevices(); ++j)
                    {
                        if(clusters[current_cluster].getMachine(i).getDevice(j).getName() == name && clusters[current_cluster].getMachine(i).getDevice(j).getNumber() != -1)
                            {
                                cameraDevicevector[clusters[current_cluster].getMachine(i).getDevice(j).getNumber()] = clusters[current_cluster].getMachine(i).getDevice(j).getDevice();
                            }
                    }
            }
        //	}
        std::string result;
        for (unsigned int i = 0; i != nbCameras; ++i)
            {
                result += (toString<unsigned int>(cameraDevicevector[i]) + " ");
            }
        return result;
    };

    std::string Architecture::getDevice(const std::string& name, unsigned int deviceNumber, unsigned int current_cluster) const
    {
        //		for (std::vector<Cluster>::const_iterator it = clusters.begin(); it != clusters.end(); ++it)
        //		{
        for (unsigned int i = 0; i != clusters[current_cluster].getNbMachines(); ++i)
            {
                for (unsigned j = 0; j != clusters[current_cluster].getMachine(i).getNumberDevices(); ++j)
                    {
                        if(clusters[current_cluster].getMachine(i).getDevice(j).getName() == name && clusters[current_cluster].getMachine(i).getDevice(j).getNumber() ==(int) deviceNumber)
                            return toString<unsigned int>(clusters[current_cluster].getMachine(i).getDevice(j).getDevice());
                    }
            }
        //	}
        return "";
    };

    std::string Architecture::getDeviceParameter(const std::string& name, unsigned int deviceNumber, const std::string& parameter, unsigned int current_cluster) const
    {
        //		for (std::vector<Cluster>::const_iterator it = clusters.begin(); it != clusters.end(); ++it)
        //		{
        for (unsigned int i = 0; i != clusters[current_cluster].getNbMachines(); ++i)
            {
                for (unsigned j = 0; j != clusters[current_cluster].getMachine(i).getNumberDevices(); ++j)
                    {
                        if(clusters[current_cluster].getMachine(i).getDevice(j).getName() == name && clusters[current_cluster].getMachine(i).getDevice(j).getNumber() ==(int) deviceNumber)
                            {	
                                return ((Device)(clusters[current_cluster].getMachine(i).getDevice(j))).getValue(parameter);
                            }
                    }
            }
        //		}
        return "";
    };


    unsigned int Architecture::getNbDevices(const std::string& name, unsigned int current_cluster) const
    {
        unsigned int result = 0;
        //		for (std::vector<Cluster>::const_iterator it = clusters.begin(); it != clusters.end(); ++it)
        //		{
        for (unsigned int i = 0; i != clusters[current_cluster].getNbMachines(); ++i)
            {
                for (unsigned j = 0; j != clusters[current_cluster].getMachine(i).getNumberDevices(); ++j)
                    {
                        if(clusters[current_cluster].getMachine(i).getDevice(j).getName() == name)
                            ++result;
                    }
            }
        //		}
        return result;
    }

    bool Architecture::firstCluster()
    {
        iterator_cluster = 0;
        return clusters.size() != 0;
    }


    bool Architecture::nextCluster()
    {
        ++iterator_cluster;
        if(iterator_cluster == clusters.size())
            {
                iterator_cluster = 0;
                return false;
            }
        return true;
    }


    unsigned int Architecture::getClusterFromHost(const std::string& hostName) const
    {
        bool found = false;
        unsigned int result = 0;
        while(result < clusters.size() && !found)
            {
                try
                    {
                        clusters[result].findMachine(hostName);
                        found = true;
                    }
                catch(CustomException& e)
                    {
                        ++result;
                    }
            }
        return result;
    } 

}; };
