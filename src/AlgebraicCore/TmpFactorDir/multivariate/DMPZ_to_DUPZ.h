//   Copyright (c)  1997-2006  John Abbott,  Anna M Bigatti

//   This file is part of the source of CoCoALib, the CoCoA Library.
//
//   CoCoALib is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   CoCoALib is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with CoCoALib.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DMPZ_to_DUPZ_h
#define DMPZ_to_DUPZ_h

#include "DUPZ.h"
#include "DMPZ.h"

DUPZ DMPZ_to_DUPZ(const DMPZ f, int var);
DMPZ DUPZ_to_DMPZ(const DUPZ f, int var);

#endif