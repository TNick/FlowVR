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
 * COPYRIGHT (C) 2006 by	                                   *
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

#ifndef __FLOWVR_APP_ARCHITECTURE__
#define __FLOWVR_APP_ARCHITECTURE__

#include <string>
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <flowvr/xml.h>
#include "flowvr/app/core/exception.h"

namespace flowvr { namespace app {

    /**
     * \file arch.h
     * \brief In this file, there are a parser and a datastructure for the architecture. 
     * This description supports :
     *    - Multiprocessor machines
     *    - Multi-networks
     *    - Input / Output Devices (screens, projector, camera, mouse, keyboard .... )
     *    - Several kind of processors architecture (heterogeneous architecture) 
     * The grammar of the XML description of the architecture is described in the dtd file 
     * \sa grammar.dtd 
     *
     * \todo grammar.dtd
     */

    /**
     * \enum ArchitectureType
     * \brief This emum represents the differents type of architecture.
     * \note Now only i386, i686 and x86_64 are supported
     */
#define I386 0 /// \todo find representation... we can't use digit 
#define I686 1
#define X86_64 2
    enum ArchitectureType { a=0, b=1, c=2 }; 

    /**
     * \class Processor
     * \brief This class correspond to a processor. A machine could contains several processors.
     * A processor is defined by it's frequency and it's kind of architecture (Xeon, Opteron, etc ...)
     */
    class Processor
    {
        private :
            ArchitectureType type; ///< The type of the processor
            unsigned int freq; ///< The frequency of the processor (in MHz)

            /**
             * \brief Default constructor
             * \warning this constructor has no sense. Don't use it
             */
            Processor() : type(a), freq(0) {};

        public :

            /**
             * \brief constructor with type and frequency
             * \param t the type of the machine
             * \param f the frequency of the processor in MHz
             */
            Processor( ArchitectureType t, unsigned int f ) : type(t), freq(f) {};

            /**
             * \brief constructor with a DOMElement
             * \param e the pointer to XML structure who describe the object
             * \throw XMLAttributeInvalid if the XML description doesn't follow the grammar
             * \throw TypeIncompatible if the function is not able to convert freq attribute to unsigned int
             */
            Processor( const xml::DOMElement* e ) throw (XMLAttributeInvalid, TypeIncompatible);

            /**
             * \brief copy constructor
             */
            Processor( const Processor& p ) : type(p.type), freq(p.freq) {};

            /**
             * \brief destructor
             */
            ~Processor() {};

            /**
             * \brief get the type of architecture
             * \return the type 
             */
            const ArchitectureType getType() const { return type; };

            /**
             * \brief get the frequency
             * \return the Processor frequency in MHz
             */
            const unsigned int getFreq() const { return freq; };

            /**
             * \brief the operator<<
             * \param out the out stream
             * \param p a processor
             * \return an out stream
             */
            friend std::ostream& operator<< (std::ostream& out, const Processor& p);

    };

    /**
     * \enum TypeDevice
     * \brief This enum describes the type of a device (input or output)
     */
    enum TypeDevice { INPUTDEVICE=0, OUTPUTDEVICE=1 };

    /**
     * \class Device
     * \brief A device is a peripheral used in the application. It could be a camera or a screen.
     * There is two kinds of devices (input or output devices)
     */
    class Device
    {
        private:
            TypeDevice type; ///< The type of the device (input or output)
            std::string name; ///< The name of the device (for example Mouse Logitech)
            unsigned int device; ///<the number
            int number;
            std::map<std::string, std::string> parametersmap; ///< the map with all parameters and their values

            /**
             * \brief Default constructor
             * \warning this constructor has no sens. Don't use it
             */
            Device() : type(INPUTDEVICE), name(""), parametersmap() {};

        public :
         
            /**
             * \brief Constructor with type and name
             * \param t the type of the Device
             * \param n the name of the Device
             */
            Device(TypeDevice t, std::string n) : type(t), name(n), device(0), number(-1), parametersmap() {};

            /**
             * \brief constructor with a DOMElement
             * \param e the pointer to XML structure who describe the object
             * \throw XMLAttributeInvalid if the XML description doesn't follow the grammar
             * \throw CustomException if the type is different of INPUT or OUTPUT
             * \note type is case-independant
             */
            Device( const xml::DOMElement* e ) throw (XMLAttributeInvalid, CustomException);


            /**
             * \brief Copy constructor
             */
            Device(const Device& d) : type(d.type), name(d.name), device(d.device), number(d.number), parametersmap(d.parametersmap) {};

            /**
             * \brief Destructor
             */
            ~Device() {};

            /**
             * \brief Get the type
             * \return the type of the Device
             */
            const TypeDevice getType() const { return type;};

            /**
             * \brief get the name of the device
             * \return the name of the device
             */
            const std::string getName() const { return name;};         
        
            unsigned int getDevice() const {return device;};
            int getNumber() const { return number;};

            /**
             * \brief get a specific value on this device
             * \param parameter the name of the parameter
             * \return string with the value
             */
            std::string getValue(const std::string& parameter)  { return parametersmap[parameter]; }

            /**
             * \brief the operator<<
             * \param out the out stream
             * \param d a device
             * \return an out stream
             */
            friend std::ostream& operator<< (std::ostream& out, const Device& d);


    };

    class Network;

    /**
     * \class Machine
     * \brief this class represents a computer in the architecture.
     * A machine could have different network interfaces and several processors or devices
     */
    class Machine
    {
        private :
            std::string id_; ///< Id of the machine
            std::vector< std::list<std::string> > aliases; ///< For each netid, the different aliases of the machine
            std::vector<Network> networks;  ///< Network interfaces of the machine
            std::vector<Processor> processors;  ///< Set of the processors
            std::vector<Device> devices;  ///< Devices of the machine

        public :
            /**
             * \brief default constructor
             * \note the name of the machine will be "localhost"
             * \warning a name is required. You shouldn't use it
             */
            Machine() : id_("localhost"), aliases(), networks(), processors(), devices() {};

            /**
             * \brief constructor with a DOMElement
             * \param e the pointer to XML structure who describe the object
             * \param net a vector with all network in the architecture
             * \throw XMLAttributeInvalid if there isn't an attribute name
             * \throw XMLChildIncorrect if one of children hasn't type device or processor
             * \throw CustomException if a network doesn't exist in net
             */
            Machine( const xml::DOMElement* e, const std::vector<Network>& net  ) throw (XMLAttributeInvalid, XMLChildIncorrect, CustomException);


            /**
             * \brief copy constructor
             */
            Machine(const Machine& m) : id_(m.id_), aliases(m.aliases), networks(m.networks), processors(m.processors), devices(m.devices) {};

            /**
             * \brief destructor
             */
            ~Machine() {};

            /**
             * \brief add a processor
             * \param proc a processor
             */
            void addProcessor(const Processor& proc) { processors.push_back(proc); };

            /**
             * \brief add a device
             * \param dev a device
             */
            void addDevice(const Device& dev) { devices.push_back(dev); };

            /**
             * \brief add a Network
             * \param net a network
             * \throw IncorrectValueParameter if net == NULL
             */
            void addNetwork(const Network& net); 

            /**
             * \brief get the number of processors
             * \return the number of processors
             */
            const unsigned int getNumberProc() const { return processors.size(); };

            /**
             * \brief get the processor i
             * \param i the number of the processor
             * \return the processor i
             */
            const Processor& operator[](unsigned int i ) const {return processors[i];};

            /**
             * \brief get the number of devices
             * \return the number of devices
             */
            const unsigned int getNumberDevices() const { return devices.size(); };

            /**
             * \brief get the device i
             * \param i the number of the device
             * \return the device i
             */
            const Device& getDevice(unsigned int i) const { return devices[i]; };

            /**
             * \brief get the number of networks
             * \return the number of networks
             */
            const unsigned int getNumberNetworks() const { return networks.size(); };

            /**
             * \brief get the network i
             * \param i the number of the network
             * \return the network i
             */
            const Network& getNetwork(unsigned int i) const { return networks[i]; };

            /**
             * \brief get the list of name on the network i
             * \param i the number of the network
             * \return the list of name in the network i
             */
            const std::list<std::string>& getAliases(unsigned int i) const { return aliases[i]; };

            /**
             * \brief get the id of the machine
             * \return the machine id
             */
            const std::string& getId() const { return id_;};

            /**
             * \brief the operator<<
             * \param out the out stream
             * \param m a machine
             * \return an out stream
             */
            friend std::ostream& operator<< (std::ostream& out, const Machine& m);

    };

    /**
     * \class Network
     * \brief A network is a map between name of computers and computers. A bandwith is associated to the network.
     * The model of the network is a hub.
     */
    class Network : public std::map<const std::string, Machine*>
    {
        private :
            unsigned int bandwith;  ///< bandwith of the network in Mb/s
            std::string name;       ///< name of the network

            /**
             * \brief default constructor
             * \warning This constructor has no sense. Don't use it
             */
            Network() : bandwith(0), name("") {};

        public:
            /**
             * \brief constructor with the bandwith and the name
             * \param b the bandwith
             * \param n the name of the network
             */
            Network(unsigned int b, const std::string& n) : bandwith(b), name(n) {};

            /**
             * \brief copy constructor
             */
            Network(const Network& n) : std::map<const std::string, Machine*>(), bandwith(n.bandwith), name(n.name) 
            {
                for(std::map<const std::string, Machine*>::const_iterator it = n.begin(); it != n.end(); ++it)
                    {
                        (*this)[it->first] = it->second;
                    };
            };

            /**
             * \brief constructor with a DOMElement
             * \param e the pointer to XML structure who describe the object
             * \throw TypeIncompatible if bandwith cannot be converted to unsigned int
             * \warning bandwith is optional (to keep compatibility with former architecture description file). If not specified, bandwith would be equals to UINT_MAX.
             */
            Network( const xml::DOMElement* e ) throw (XMLAttributeInvalid, TypeIncompatible);

            /**
             * \brief get the bandwith
             * \return the bandwith
             */
            const unsigned int getBandwith() const { return bandwith; };

            /**
             * \brief get the name of the network
             * \return the name
             */
            const std::string& getId() const { return name;};
            /**
             * \brief the operator<<
             * \param out the out stream
             * \param n a network
             * \return an out stream
             */
            friend std::ostream& operator<< (std::ostream& out, const Network& n);

            /**
             * \brief equal function
             * \param n1 first network
             * \param n2 second network
             * \return true if name are equals
             */
            friend bool operator==(const Network& n1, const Network& n2); 
			
            /**
             * \brief non-equal function
             * \param n1 first network
             * \param n2 second network
             * \return true if name are differents
             */
            friend bool operator!=(const Network& n1, const Network& n2); 

    };


    /**
     * \class Cluster
     * \brief A cluster is a set of machines and networks.
     * The datastructure is filled thanks to the constructor from a XML structure
     */
    class Cluster 
    {
        private :
            std::string id; ///< name of the cluster
            std::vector<Machine> machines; ///< set of machines
            std::vector<Network> networks; ///< set of networks
            std::map<std::string, std::string> parameters; ///< map of parameters

        public :
            /**
             * \brief default constructor
             */
            Cluster() : id(""), machines(), networks(), parameters() { };

            /**
             * \brief constructor from a XML description
             * \param e the pointer to XML description
             * \throw XMLAttributeInvalid if cannot read cluster id 
             */
            Cluster(const xml::DOMElement* e) throw (XMLAttributeInvalid);

            /**
             * \brief get the number of machines
             * \return the number of machines
             */
            const unsigned int getNbMachines() const { return machines.size(); };

            /**
             * \brief get the number of networks
             * \return the number of networks
             */
            const unsigned int getNbNetworks() const { return networks.size(); };



            /**
             * \brief get a machine
             * \param i the number of the machine
             * \return the machine i
             */
            const Machine& getMachine(unsigned int i) const { return machines[i]; };

            /**
             * \brief get a network
             * \param i the number of the network
             * \return the network i
             */
            const Network& getNetwork(unsigned int i) const { return networks[i]; };

            /**
             * \brief find a network with an id
             * \param id the id of the network
             * \return pointer to the network (NULL if the network doesn't exist)
             * \throw CustomException if the network doesn't exist
             */
            const Network& findNetwork(std::string& id) const throw (CustomException);
	 
            /**
             * \brief find a machine with an alias on the network net
             * \param net the network
             * \param id the aliase of the machine
             * \return pointer to the machine (NULL if the machine doesn't exist)
             */
            const Machine* findMachine(const Network& net, const std::string& id) const; 
	 
            /**
             * \brief find a machine with an alias on the network net
             * \param id the aliase of the machine
             * \return ref to the machine
             * \throw CustomException if machine doesn't exist
             */
            const Machine& findMachine(const std::string& id) const throw (CustomException);
	 

            /**
             * \brief add a machine to the cluster
             * \param m a pointer to a machine
             */
            void  addMachine(const Machine& m) { machines.push_back(m); };

            /**
             * \brief the operator<<
             * \param out the out stream
             * \param a a cluster
             * \return an out stream
             */
            friend std::ostream& operator<< (std::ostream& out, const Cluster& a);
	
            /**
             * \brief get Id
             * \return id
             */
            std::string getId() const { return id; }

            /**
             *\brief get Parameter
             *\param key parameter key
             *\return value of the paramter
             */
            std::string getParameter(const std::string& key) const 
            {
                for(std::map<std::string, std::string>::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
                    {
                        if ((*it).first == key)
                            return it->second;
                    }
                return "";
            }

    };

    /**
     * \class Architecture
     * \brief this class creates all clusters from the XML architecture descriptions
     */
    class Architecture
    {
        private :
            std::vector<Cluster> clusters; ///< the list of clusters described in the XML description
            std::string fileName; ///< the file name the architecture is loaded from
            unsigned int iterator_cluster; ///< Current cluster. Query are done on it

        public :
            /**
             * \brief default constructor
             * \note send a architecture with only localhost
             */
            Architecture();
            /**
             * \brief constructor from a XML file
             * \param fileName_ the name of the XML file
             * \throw CustomException if cannot open the file
             */
            Architecture(const std::string& fileName_);


            /**
             * \brief clean clusters and import architecture from XML file
             */
            void importArchitecture(const std::string& fileName_);

            /**
             * \brief get the arch file name
             * \return arch file name
             */
            const std::string  getFileName() const { return fileName; };


            /**
             * \brief get the number of clusters
             * \return the number of clusters
             */
            const unsigned int getNbClusters() const { return clusters.size(); };


            /**
             * \brief get the ith  cluster
             * \param i the index of the cluster
             * \return the cluster i
             */
            const Cluster& getCluster(unsigned int i) const 
            {
                if (i >= clusters.size())
                    {
                        throw CustomException("Cluster requested doesn't exist", "const Cluster& Architecture::getCluster(unsigned int)");
                    }
                return clusters[i];
	
            };

            /**
             * \brief add a cluster
             * \param c the cluster to add
             */
            void  add(const Cluster& c)  { clusters.push_back(c); };

            /**
             * \brief find a machine in clusters  with an alias on the network net
             * \param net the network
             * \param id the aliase of the machine
             * \return pointer to the machine (NULL if the machine doesn't exist - first machine encountered if several with same name)
             */
            const Machine* findMachine(const Network& net, const std::string& id, unsigned int current_cluster = 0) const; 
	 
            /**
             * \brief find a machine in clusters with an alias on the network net
             * \param id the aliase of the machine
             * \return ref to the machine
             * \throw CustomException if machine doesn't exist or multiple occurences
             */
            const Machine& findMachine(const std::string& id, unsigned int current_cluster = 0) const throw (CustomException);

            /**
             * \brief the operator<<
             * \param out the out stream
             * \param a an architecture
             * \return an out stream
             */
            friend std::ostream& operator<< (std::ostream& out, const Architecture& a);

            std::string getHostsList(unsigned int nbCameras, const std::string& name, unsigned int current_cluster = 0) const;
            std::string getDevicesList(unsigned int nbCameras, const std::string& name, unsigned int current_cluster = 0) const;
            std::string getDevice(const std::string& name, unsigned int deviceNumber, unsigned int current_cluster = 0) const;
            /**
             * \brief get a parameter form a device 
             * \param name name of the device
             * \param devicenumber number of the device
             * \param parameter name of the parameter
             * \return value of the parameter
             */
            std::string getDeviceParameter(const std::string& name, unsigned int deviceNumber, const std::string& parameter, unsigned int current_cluster = 0) const;
            unsigned int getNbDevices(const std::string& name, unsigned int current_cluster = 0) const;

            /**
             * \brief go to the first cluster
             * \return true if there is at least on cluster
             */
            bool firstCluster();

            /**
             * \brief change to next cluster
             * \return true if is able to go to next cluster... Round robin
             */
            bool nextCluster();

            /**
             * \brief getId cluster
             * \return id of current cluster
             */
            std::string getIdCluster() const { return clusters[iterator_cluster].getId(); }

            /**
             *\brief get cluster from a host name. Find the cluster where hosted this host
             *\param hostName the name of the host
             *\return id of the cluster. If cannot find it send the number of clusters
             */
            unsigned int getClusterFromHost(const std::string& hostName) const;
    };

}; };
#endif //__ARCHITECTURE__
