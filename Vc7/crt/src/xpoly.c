/* _Poly function */
#include "xmath.h"
_STD_BEGIN

_CRTIMP2 double __cdecl _Poly(double x, const double *tab, int n)
	{	/* compute polynomial */
	double y;

	for (y = *tab; 0 <= --n; )
		y = y * x + *++tab;
	return (y);
	}
_STD_END

/*
 * Copyright (c) 1992-2002 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V3.13:0009 */
