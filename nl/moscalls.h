//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//

#ifndef MOSCALLS_H_
#define MOSCALLS_H_


#include "util/mutil.h"

//Note: FORMATS ARE U.S. ENGLISH...
//---------- Time functions ---------------
MDLLEXPORT bool MGetSystemTime(CMString & result, long plusminusHours = 0, long plusminusMinutes = 0, bool bEuropean = false);
//
MDLLEXPORT long MGetSystemTimeHour(bool bEuropean = false);
MDLLEXPORT long MGetSystemTimeMinutes();
MDLLEXPORT long MGetSystemTimeSeconds();
MDLLEXPORT long MGetSystemTimeMilliSeconds();
MDLLEXPORT void MGetSystemTimeAmPm(CMString & result, long plusminus = 0);


//---------- Date functions ---------------
MDLLEXPORT bool MGetSystemDate(CMString & result, long plusminusDays = 0, long plusminusMonths = 0);

MDLLEXPORT long MGetSystemDateYear();
MDLLEXPORT long MGetSystemDateMonth();
MDLLEXPORT long MGetSystemDateDay();
MDLLEXPORT long MGetSystemDateDOW();
MDLLEXPORT void MGetSystemDateDOW(CMString & dow, long plusminus = 0);


#endif /* MOSCALLS_H_ */


