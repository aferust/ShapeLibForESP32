/******************************************************************************
 *
 * Project:  Shapelib
 * Purpose:  Default implementation of file io based on stdio.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2007, Frank Warmerdam
 * Copyright (c) 2016-2024, Even Rouault <even dot rouault at spatialys.com>
 *
 * SPDX-License-Identifier: MIT OR LGPL-2.0-or-later
 ******************************************************************************
 *
 */

#include "shapefil_private.hpp"
#include "shapefil.hpp"
#include "fshooks.hpp"

#include <assert.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FS.h"

void SHPsetFileSystem(fs::FS &fs)
{
    shp_fs = &fs;
}

void SASetupDefaultHooks(SAHooks *psHooks)
{
    psHooks->FOpen = SADFOpen;
    psHooks->FRead = SADFRead;
    psHooks->FWrite = SADFWrite;
    psHooks->FSeek = SADFSeek;
    psHooks->FTell = SADFTell;
    psHooks->FFlush = SADFFlush;
    psHooks->FClose = SADFClose;
    psHooks->Remove = SADRemove;

    psHooks->Error = SADError;
    psHooks->Atof = atof;
    psHooks->pvUserData = SHPLIB_NULLPTR;
}