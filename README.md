This repository collects minor bug fixes to the VirtualRetina Simulator[^1].
The official website of the simulator is: [https://enas.inria.fr/virtual-retina.html](https://enas.inria.fr/virtual-retina.html)
Example files more extensive documentation can be found in the original package, this repository only holds the source code.

The bugs fixed:

 - when loading a configuration with `<units>` already specified, the attributes of the spiking channel are not transferred to the units
 	+ fixed with [this commit](https://github.com/jahuth/virtualretina/commit/d3cdcad73330fbb69dea87ebce37478830f4f83e)
 - while random positioning was implemented, there was no xml attribute to set positional noise `!= 0`
 	+ fixed with [this commit](https://github.com/jahuth/virtualretina/commit/d3cdcad73330fbb69dea87ebce37478830f4f83e)

Contributors to this project are:

 * Adrien Wohrer - original author
 * Emilie Mayer
 * Jacob Huth


```
*************************************************************************
************            Welcome to VirtualRetina        *****************
************                version 2.2.2               *****************
*************************************************************************

                         Spiking, large-scale retina simulation software.

      Author : Adrien Wohrer
      Institution : Group For Neural Theory, Ecole Normale Supérieure
      Contact : Adrien.Wohrer@ens.fr
      Project started December 2004. 
      Version 2.2.2 : December 2010.


This is the retina package. It contains VirtualRetina/, as well as other
libraries required by the software, that you may want to install.

Read
VirtualRetina/tutorial.pdf
for intructions.

Type " bash download_build_all.bash " for an automatic installation
of the library, which will:

1) Download external libraries MvaSpike and CImg and install them locally,
   plus home-made library xmlParameters++, in directory External_Libraries/

2) Compile VirtualRetina.
```

[^1]: Wohrer, A., & Kornprobst, P. (2009). Virtual Retina: a biological retina model and simulator, with contrast gain control. Journal of Computational Neuroscience, 26(2), 219–49. http://doi.org/10.1007/s10827-008-0108-4
