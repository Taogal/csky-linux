/*
 * lshrdi3.c extracted from gcc-2.95.2/libgcc2.c which is: 
 *
 * This file is part of GNU CC.
 *
 * GNU CC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU CC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU CC; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 * 
 * Copyright (C) 1989, 92-98, 1999 Free Software Foundation, Inc.
 * Copyright (C) 2009 Hangzhou C-SKY Microsystems co.,ltd. 
 *
 */

#include "math-64bits.h"

DItype
__lshrdi3 (DItype u, word_type b)
{
	DIunion w;
	word_type bm;
	DIunion uu;

	if (b == 0)
		return u;

	uu.ll = u;

	bm = (sizeof (SItype) * BITS_PER_UNIT) - b;
	if (bm <= 0)
	{
		w.s.high = 0;
		w.s.low = (USItype)uu.s.high >> -bm;
	}
	else
	{
		USItype carries = (USItype)uu.s.high << bm;
		w.s.high = (USItype)uu.s.high >> b;
		w.s.low = ((USItype)uu.s.low >> b) | carries;
	}

	return w.ll;
}

