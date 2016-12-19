/*
 * This library provides a Q server for R
 *
 * See kx wiki https://code.kx.com/trac/wiki/Cookbook/IntegratingWithR
 */

#include <errno.h>
#include <string.h>
#include <R.h>
#ifdef WIN32
#include <Windows.h>
#include <WinBase.h>
#endif
#include <Rdefines.h>
#include <k.h>

#include "common.c"
#include "rserver.c"
#include "qserver.c"
