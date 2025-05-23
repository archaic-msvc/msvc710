/***
*ti_inst.cxx - One instance of class typeinfo.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This module insures that an instance of class type_info
*       will be present in msvcrt.lib, providing access to type_info's
*       vftable when compiling MD.
*
****/

#define _TICORE
#include <typeinfo.h>

type_info::type_info(const type_info& rhs)
{
}

type_info& type_info::operator=(const type_info& rhs)
{
        return *this;
}


