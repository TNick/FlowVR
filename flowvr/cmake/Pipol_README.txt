
#######################################################################
 How to use the pipol platform to launch tests and remote compilations 
#######################################################################

Pipol (https://pipol.inria.fr) is a testing platform, developed and hosted by an Inria research team (www.inrialpes.fr).

First of all, if you want to use this functionality, you need to request a pipol account (https://pipol.inria.fr/users/).

Once you set up your ssh keys correctly, you can activate the advanced cmake option "USE_PIPOL".
You will be asked to enter your login (PIPOL_USER).

Now, the generate cmake command adds multiple targets to the make command (use <tab> to see all of them).
Targets prefixed by "pipol_" now allow you to :
  - deploy a distant machine, 
  - upload your local sources, 
  - launch a compilation on the chosen operating system automatically.

For example, if you choose:
make pipol_x86-linux-debian-testing

you will deploy a distant x86 architecture machine, hosting a debian-testing operating system. All needed flowvr dependencies 
will be automatically installed on that machine, and your local source directory will be uploaded.
 
A "make install" command will be launched.

When the build is finish, the distant terminal is alive for 2 hours, to allow you launching tests or whatever ...  

  
Fill free to contact antoine.vanel@inrialpes.fr if you encounter problems.

Enjoy.