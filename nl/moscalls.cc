//==================================================================//
//=  Copyright (c) 1999-2001 by Manfred G. Grabherr                =//
//=  All rights reserved.                                          =//
//=                                                                =//
//==================================================================//

#include "nl/moscalls.h"
#include <stdio.h>

#ifdef WIN32
#include <windows.h>


MDLLEXPORT bool MGetSystemTime(CMString & result, long plusminusHours, long plusminusMinutes, bool bEuropean)
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  //GetTimeZone(

  char szFormat[64];
  if (bEuropean) {
    sprintf(szFormat, "%2d:%2d:%2d", systime.wHour, systime.wMinute, systime.wSecond);
  } else {
	long hour = systime.wHour;
	if (hour > 12)
      sprintf(szFormat, "%2d:%2d:%2dpm", hour-12, systime.wMinute, systime.wSecond);
	else
      sprintf(szFormat, "%2d:%2d:%2dam", systime.wHour, systime.wMinute, systime.wSecond);
  }

  for (int i=0; i<(int)strlen(szFormat); i++) {
    if (szFormat[i] == ' ')
	  szFormat[i] = '0';
  }
  result = szFormat;
  return true;
}

MDLLEXPORT long MGetSystemTimeHour(bool bEuropean)
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  long hour = systime.wHour;
  if (bEuropean)
    return hour;
  if (hour > 12)
	hour -= 12;
  return hour;
}

MDLLEXPORT long MGetSystemTimeMinutes()
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  return systime.wMinute;
}

MDLLEXPORT long MGetSystemTimeSeconds()
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  return systime.wSecond;
}

MDLLEXPORT long MGetSystemTimeMilliSeconds()
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  return systime.wMilliseconds;
}

MDLLEXPORT void MGetSystemTimeAmPm(CMString & result, long plusminus)
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  long hour = systime.wHour;
  if (hour > 12)
    result = "pm";
  else
    result = "am";
	  //return systime.;
}


//---------- Date functions ---------------
MDLLEXPORT bool MGetSystemDate(CMString & result, long plusminusDays, long plusminusMonths)
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);

  long month = systime.wMonth;
  long year = systime.wYear;
  long day = systime.wDay;

  long i;

  long daysOfMonth[12];
  daysOfMonth[0] = 31;
  if (year % 4 == 0)
    daysOfMonth[1] = 29;
  else
    daysOfMonth[1] = 28;
  daysOfMonth[2] = 31;
  daysOfMonth[3] = 30;
  daysOfMonth[4] = 31;
  daysOfMonth[5] = 30;
  daysOfMonth[6] = 31;
  daysOfMonth[7] = 31;
  daysOfMonth[8] = 30;
  daysOfMonth[9] = 31;
  daysOfMonth[10] = 30;
  daysOfMonth[11] = 31;
  

  month += plusminusMonths;

  long fullDay = 0;
  for (i=1; i<month; i++) {
    fullDay += daysOfMonth[i-1];
  }
  fullDay += day;

  fullDay += plusminusDays;
  long chkDays = 0;
  for (i=1; i<12; i++) {
	systime.wDay = (unsigned int)(fullDay - chkDays);
    chkDays += daysOfMonth[i-1];
	if (chkDays >= fullDay) {
	  systime.wMonth = (unsigned int)i;
	  break;
	}
  }


  char szFormat[64];
  sprintf(szFormat, "%2d/%2d/%4d", systime.wMonth, systime.wDay, systime.wYear);

  for (i=0; i<(int)strlen(szFormat); i++) {
    if (szFormat[i] == ' ')
	  szFormat[i] = '0';
  }
  result = szFormat;
  return true;
}

MDLLEXPORT long MGetSystemDateYear()
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  return systime.wYear;
}

MDLLEXPORT long MGetSystemDateMonth()
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  return systime.wMonth;
}

MDLLEXPORT long MGetSystemDateDay()
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  return systime.wDay;
}

MDLLEXPORT long MGetSystemDateDOW()
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  return systime.wDayOfWeek;
}

MDLLEXPORT void MGetSystemDateDOW(CMString & res, long plusminus)
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  long dow = systime.wDayOfWeek;

  dow += plusminus;
  if (dow >= 7)
	dow -= 7;
  //if (dow == 0)
	//dow = 7;

  switch(dow) {
  case 0:
	  res = "Sunday";
	  return;
  case 7:
	  res = "Sunday";
	  return;
  case 1:
	  res = "Monday";
	  return;
  case 2:
	  res = "Tuesday";
	  return;
  case 3:
	  res = "Wednesday";
	  return;
  case 4:
	  res = "Thursday";
	  return;
  case 5:
	  res = "Friday";
	  return;
  case 6:
	  res = "Saturday";
	  return;
  
  }

}


#else //Windows

//Dummies
MDLLEXPORT bool MGetSystemTime(CMString & result, long plusminusHours, long plusminusMinutes , bool bEuropean)
{
  return false;
}
//
MDLLEXPORT long MGetSystemTimeHour(bool bEuropean)
{
  return 0;
}
MDLLEXPORT long MGetSystemTimeMinutes()
{
  return 0;
}
MDLLEXPORT long MGetSystemTimeSeconds()
{
  return 0;
}
MDLLEXPORT long MGetSystemTimeMilliSeconds()
{
  return 0;
}
MDLLEXPORT void MGetSystemTimeAmPm(CMString & result, long plusminus)
{
}


//---------- Date functions ---------------
MDLLEXPORT bool MGetSystemDate(CMString & result, long plusminusDays, long plusminusMonths)
{
  return false;
}

MDLLEXPORT long MGetSystemDateYear()
{
  return 0;
}

MDLLEXPORT long MGetSystemDateMonth()
{
  return 0;
}
MDLLEXPORT long MGetSystemDateDay()
{
  return 0;
}
MDLLEXPORT long MGetSystemDateDOW()
{
  return 0;
}
MDLLEXPORT void MGetSystemDateDOW(CMString & dow, long plusminus)
{

}



#endif

