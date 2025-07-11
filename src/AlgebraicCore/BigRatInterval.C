//   Copyright (c)  2018  John Abbott,  Anna M. Bigatti

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


#include "CoCoA/BigRatInterval.H"
#include "CoCoA/error.H"
#include "CoCoA/BigRatOps.H"
#include "CoCoA/MachineInt.H"
#include "CoCoA/NumTheory-SimplestRat.H"

#include <algorithm>
using std::max;
using std::min;
#include <iostream>
using std::ostream;

namespace CoCoA
{

  BigRatInterval::BigRatInterval(const BigRat& lwb, const BigRat& upb):
      myMin(lwb),
      myMax(upb)
  { if (lwb > upb) CoCoA_THROW_ERROR1(ERR::IncompatArgs); }


  BigRatInterval operator*(const BigRat& A, const BigRatInterval& B)
  {
    if (IsZero(A)) return BigRatInterval(A,A); // zero interval
    if (A > 0) return BigRatInterval(A*min(B), A*max(B));
    return BigRatInterval(A*max(B), A*min(B));
  }
  
  BigRatInterval operator*(const BigRatInterval& A, const BigRat& B)
  {
    return B*A;
  }


// impl is simple and obviously correct, may not be so efficient...
  BigRatInterval operator*(const BigRatInterval& A, const BigRatInterval& B)
  {
    const BigRat ProdMinMin = min(A)*min(B);
    const BigRat ProdMinMax = min(A)*max(B);
    const BigRat ProdMaxMin = max(A)*min(B);
    const BigRat ProdMaxMax = max(A)*max(B);
    const BigRat LWB = min(min(ProdMinMin, ProdMinMax), min(ProdMaxMin, ProdMaxMax));
    const BigRat UPB = max(max(ProdMinMin, ProdMinMax), max(ProdMaxMin, ProdMaxMax));
    return BigRatInterval(LWB, UPB);
    // if (sign(min(A)) >= 0)
    // {
    //   if (sign(min(B)) >= 0)
    //     return BigRatInterval(min(A)*min(B), max(A)*max(B));
    //   if (sign(max(B)) <= 0)
    //     return BigRatInterval(max(A)*min(B), min(A)*max(B));
    //   return BigRatInterval(max(A)*min(B), max(A)*max(B));
    // }
    // if (sign(max(A)) <= 0)
    // {
    //   if (sign(min(B)) >= 0)
    //     return BigRatInterval(min(A)*max(B), max(A)*min(B), );
    //   if (sign(max(B)) <= 0)
    //     return BigRatInterval(min(A)*min(B), max(A)*max(B));
    //   return BigRatInterval(max(A)*min(B), min(A)*min(B));
    // }
  }
  

// impl is simple and obviously correct, may not be so efficient...
  BigRatInterval operator/(const BigRatInterval& A, const BigRatInterval& B)
  {
    if (min(B) <= 0 && max(B) >= 0)  CoCoA_THROW_ERROR1(ERR::DivByZero);
    const BigRat QuotMinMin = min(A)/min(B);
    const BigRat QuotMinMax = min(A)/max(B);
    const BigRat QuotMaxMin = max(A)/min(B);
    const BigRat QuotMaxMax = max(A)/max(B);
    const BigRat LWB = min(min(QuotMinMin, QuotMinMax), min(QuotMaxMin, QuotMaxMax));
    const BigRat UPB = max(max(QuotMinMin, QuotMinMax), max(QuotMaxMin, QuotMaxMax));
    return BigRatInterval(LWB, UPB);
  }


  bool IsZeroInside(const BigRatInterval& A)
  {
    return (sign(min(A)) < 0 && sign(max(A)) > 0);
  }


  // Better than A*A if the interval contains 0
  BigRatInterval square(const BigRatInterval& A)
  {
    if (min(A) >= 0) return BigRatInterval(power(min(A),2), power(max(A),2));
    if (max(A) <= 0) return BigRatInterval(power(max(A),2), power(min(A),2));
    if (-min(A) > max(A)) return BigRatInterval(BigRat(0), power(min(A),2));
    return BigRatInterval(BigRat(0), power(max(A),2));
  }


  BigRatInterval merge(const BigRatInterval& A, const BigRatInterval& B)
  {
    if (min(A) < min(B))
    {
      if (max(A) < min(B))  CoCoA_THROW_ERROR1(ERR::IncompatArgs);
      return BigRatInterval(min(A), max(max(A), max(B)));
    }
    // Here min(B) <= min(A)
      if (max(B) < min(A))
        CoCoA_THROW_ERROR1(ERR::IncompatArgs);
      return BigRatInterval(min(B), max(max(A), max(B)));
  }


  // Widen interval slightly so that end points are simple "binary rationals"
  BigRatInterval soften(const BigRatInterval& A)
  {
    const BigRat w = width(A);
    const BigRat lwb = SimplestBinaryRatBetween(min(A)-w/256, min(A));
    const BigRat upb = SimplestBinaryRatBetween(max(A), max(A)+w/256);
    return BigRatInterval(lwb, upb);
  }


  std::ostream& operator<<(std::ostream& out, const BigRatInterval& I)
  {
    if (!out) return out;
    out << "interval(" << min(I) << ",  " << max(I) << ")";
    return out;
  }

} // end of namespace CoCoA
