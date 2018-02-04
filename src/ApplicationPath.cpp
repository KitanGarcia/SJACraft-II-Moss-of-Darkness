/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.

    All source material (source code, images, sounds, etc.) have been provided to
    University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course without
    prior permission from the copyright holder.

    All sound files, sound fonts, midi files, and images that have been included 
    that were extracted from original Warcraft II by Blizzard Entertainment 
    were found freely available via internet sources and have been labeld as 
    abandonware. They have been included in this distribution for educational 
    purposes only and this copyright notice does not attempt to claim any 
    ownership of this material.
*/

/**
*
* Gets application pathname
*
* @return CPath(ModuleInfo.dli_fname) 1)no pathname, if symbol from overlapping address not found | 2)pathname of shared object that contains address
*
* @author Wonhee Park
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: wonpark@ucdavis.edu 
*
*/

#include "ApplicationPath.h"
#include <dlfcn.h>

// Code from: http://jeffcode.blogspot.com/2010/03/getmodulefilename-on-os-x.html

CPath GetApplicationPath(){
    Dl_info ModuleInfo;
    
    if(0 == dladdr(reinterpret_cast<void*>(GetApplicationPath), &ModuleInfo)){
        // Failed to find this symbol
        return CPath();
    }
    return CPath(ModuleInfo.dli_fname);
}

