This repository contains a recent (2023) working version of the VirtualRetina Simulator<sup>[1](#1)</sup>. It includes a number of bug fixes by [Jacob Huth](https://github.com/jahuth) and others. It has been forked from Jacob Huth's github [repository of the same name](https://github.com/jahuth/virtualretina) and will probably continue to exist alongside.


Virtual Retina was originally developped in 2009 at INRIA,  under INRIA [CeCill C open-source licence](http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html). The current official website of the simulator is: [https://team.inria.fr/biovision/virtualretina/](https://team.inria.fr/biovision/virtualretina/)

Example files and more extensive documentation can be found in the original package [here](https://team.inria.fr/biovision/virtualretina/), this repository only holds the source code.

The bugs fixed:

 - when loading a configuration with `<units>` already specified, the attributes of the spiking channel are not transferred to the units
 	+ fixed with [commit d3cdcad](https://github.com/jahuth/virtualretina/commit/d3cdcad73330fbb69dea87ebce37478830f4f83e)
 - while random positioning was implemented, there was no xml attribute to set positional noise `!= 0`
 	+ fixed with [commit d3cdcad](https://github.com/jahuth/virtualretina/commit/d3cdcad73330fbb69dea87ebce37478830f4f83e)
 - the saveMap command line option did not save the values of the attenuation signal
 	+ fixed with [commit 0c839cd](https://github.com/jahuth/virtualretina/commit/0c839cda66681529899efff16a3e4d2e3ebc0cec)
 - CImg is now downloaded from their github repository
 	+ fixed with [commit cb13f4b](https://github.com/jahuth/virtualretina/commit/cb13f4b8c7a93bc3d7a6d686b284b8ea3eb35391)
 	+ removed deprecated function call [commit f56ba20](https://github.com/jahuth/virtualretina/commit/f56ba203c1d7fe768cc27ed39aabb0610f07aa3d)
 - xmlParameters had clashing template names, leading to compiler warnings
	+ fixed with [commit 5f9d076](https://github.com/jahuth/virtualretina/commit/5f9d0763e224276ccb53ec80583102594419025e) by [Richard Veale](https://github.com/flyingfalling)
 - minor modifications to comply to new compiler requests (2023)
 - inclusion of library `Mvaspike` in the git source code. `Mvaspike` is an event-driven spiking neural network simulator that was originally developed by [Olivier Rochel](https://sed-nge.inria.fr/team/olivierr/) at INRIA. It is now directly included in the VirtualRetina archive, as it cannot be found online anymore.
 
Contributors to this project are:

 * Adrien Wohrer - original author
 * Emilie Mayer
 * Jacob Huth
 * Richard Veale


Installation
============

Tested on Ubuntu 14.04, 16.04, 17.04 and Debian 11.

 1. Install requirements
---------------------------

One of the dependencies requires headers for xml libraries: `libxml2-dev` and `libx11-dev`
It might be possible that `cmake` can be installed automatically, but it is recommended to
install the latest version via your package manager.

For Ubuntu run:

```
sudo apt-get install cmake libxml2-dev libx11-dev
```


 2. Download and Compile
---------------------------

Clone the repository to a folder of your choice
```
git clone https://github.com/AdrienWohrer/virtualretina
```

Run the script `download_build_all.bash` (using `bash`)
```
cd virtualretina
bash download_build_all.bash
```

This script will download multiple software projects and compile them one after another.
If you encounter an error, please file an [issue](https://github.com/AdrienWohrer/virtualretina/issues).



```
*************************************************************************
************            Welcome to VirtualRetina        *****************
************                version 2.2.3               *****************
*************************************************************************

                         Spiking, large-scale retina simulation software.

      Author : Adrien Wohrer
      Institution : Group For Neural Theory, Ecole Normale Supérieure
      Contact : Adrien.Wohrer@ens.fr
      Project started December 2004. 
      Version 2.2.3 : December 2023.


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

Once the package is compiled it can be tested by running:

```
VirtualRetina/bin/Retina -ret test/retina_files/EXAMPLE_primate_ParvoMagno.xml test/sequences/walking_finland/finland.10* -r 8 -nS 8
```

And to reconstruct the video:

```
VirtualRetina/bin/ReconstructRetina -i tmp/simulation.txt -ch 1 0 -f 2 -w 3 -o tmp/last_reconstruction.inr
VirtualRetina/bin/viewVideo tmp/last_reconstruction.inr -s 20
```

See `tutorial.pdf` for more examples.


<a name="1">1</a>: Wohrer, A., & Kornprobst, P. (2009). Virtual Retina: a biological retina model and simulator, with contrast gain control. Journal of Computational Neuroscience, 26(2), 219–49. http://doi.org/10.1007/s10827-008-0108-4
