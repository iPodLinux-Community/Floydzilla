/*
** 2003 October 31
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains the C functions that implement date and time
** functions for SQLite.  
**
** There is only one exported symbol in this file - the function
** sqlite3RegisterDateTimeFunctions() found at the bottom of the file.
** All other code has file scope.
**
** $Id: date.c,v 1.45 2005/06/25 18:42:14 drh Exp $
**
** NOTES:
**
** SQLite processes all times and dates as Julian Day numbers.  The
** dates and times are stored as the number of days since noon
** in Greenwich on November 24, 4714 B.C. according to the Gregorian
** calendar system.
**
** 1970-01-01 00:00:00 is JD 2440587.5
** 2000-01-01 00:00:00 is JD 2451544.5
**
** This implemention requires years to be expressed as a 4-digit number
** which means that only dates between 0000-01-01 and 9999-12-31 can
** be represented, even though julian day numbers allow a much wider
** range of dates.
**
** The Gregorian calendar system is used for all dates and times,
** even those that predate the Gregorian calendar.  Historians usually
** use the Julian calendar for dates prior to 1582-10-15 and for some
** dates afterwards, depending on locale.  Beware of this difference.
**
** The conversion algorithms are implemented based on descriptions
** in the following text:
**
**      Jean Meeus
**      Astronomical Algorithms, 2nd Edition, 1998
**      ISBM 0-943396-61-1
**      Willmann-Bell, Inc
**      Richmond, Virginia (USA)
*/
#include "sqliteInt.h"
#include "os.h"
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

/*
** If the library is compiled to omit the full-scale date and time
** handling (to get a smaller binary), the following minimal version
** of the functions current_time(), current_date() and current_timestamp()
** are included instead. This is to support column declarations that
** include "DEFAULT CURRENT_TIME" etc.
**
** This function uses the C-library functions time(), gmtime()
** and strftime(). The format string to pass to strftime() is supplied
** as the user-data for the function.
*/
static void currentTimeFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  time_t t;
  char *zFormat = (char *)sqlite3_user_data(context);
  char zBuf[20];

  time(&t);
#ifdef SQLITE_TEST
  {
    extern int sqlite3_current_time;  /* See os_XXX.c */
    if( sqlite3_current_time ){
      t = sqlite3_current_time;
    }
  }
#endif

  sqlite3OsEnterMutex();
  strftime(zBuf, 20, zFormat, gmtime(&t));
  sqlite3OsLeaveMutex();

  sqlite3_result_text(context, zBuf, -1, SQLITE_TRANSIENT);
}

/*
** This function registered all of the above C functions as SQL
** functions.  This should be the only routine in this file with
** external linkage.
*/
void sqlite3RegisterDateTimeFunctions(sqlite3 *db){
  static const struct {
     char *zName;
     char *zFormat;
  } aFuncs[] = {
    { "current_time", "%H:%M:%S" },
    { "current_date", "%Y-%m-%d" },
    { "current_timestamp", "%Y-%m-%d %H:%M:%S" }
  };
  int i;

  for(i=0; i<sizeof(aFuncs)/sizeof(aFuncs[0]); i++){
    sqlite3_create_function(db, aFuncs[i].zName, 0, SQLITE_UTF8, 
        aFuncs[i].zFormat, currentTimeFunc, 0, 0);
  }
}
