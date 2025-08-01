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


#include "CoCoA/LinDepMill.H"
#include "CoCoA/error.H"
#include "CoCoA/MatrixFp.H"
#include "CoCoA/ring.H"
#include "CoCoA/RingFp.H"
#include "CoCoA/SmallFpImpl.H"
#include "CoCoA/utils.H"
#include "CoCoA/VectorOps.H"

//#include <vector>
using std::vector;


namespace CoCoA
{

  LinDepMill::~LinDepMill() {}


  class LinDepMillBase
  {
  public:
    virtual bool myAppendVec(const std::vector<RingElem>& v) = 0;
    virtual const std::vector<RingElem>& myLinReln() const = 0;
    virtual ~LinDepMillBase();
  };

  bool LinDepMill::myAppendVec(const std::vector<RingElem>& v)
  { return myImpl->myAppendVec(v); }

  const std::vector<RingElem>& LinDepMill::myLinReln() const
  { return myImpl->myLinReln(); }

  LinDepMillBase::~LinDepMillBase() {}


  //----- LinDepMill_generic --------------------------------------

  class LinDepMill_generic: public LinDepMillBase
  {
  public:
    LinDepMill_generic(const ring& k, long dim);
    bool myAppendVec(const std::vector<RingElem>& v) override;
    const std::vector<RingElem>& myLinReln() const override  { return myLinRelnValue; }
// DEFAULT DTOR is OK    virtual ~LinDepMill_generic();
  
  private:
    ring myRing;
    int myVecLen;
    std::vector< std::vector<RingElem> > myM;
    std::vector< std::vector<RingElem> > myRowRepr;
    std::vector<int> myColIndices;
    std::vector<RingElem> myLinRelnValue;
  };


  LinDepMill_generic::LinDepMill_generic(const ring& K, long dim):
      myRing(K),
      myVecLen(dim)
  {
    if (!IsField(K))  CoCoA_THROW_ERROR1(ERR::ReqField);
    if (dim <= 0)  CoCoA_THROW_ERROR2(ERR::ReqPositive, "dimension");
    myM.reserve(dim);
    myRowRepr.reserve(dim);
    myColIndices.reserve(dim);
    myLinRelnValue.reserve(dim);
  }

  
  // What is rtn value???
  bool LinDepMill_generic::myAppendVec(const std::vector<RingElem>& vec)
  {
    const long n = len(vec);
    if (n != myVecLen)  CoCoA_THROW_ERROR1(ERR::IncompatDims);
    if (owner(vec[0]) != myRing || !HasUniqueOwner(vec))
      CoCoA_THROW_ERROR1(ERR::MixedRings);
    vector<RingElem> v(vec);
    const long ncols = len(myM);
    myLinRelnValue.resize(ncols, zero(myRing));
    myLinRelnValue.push_back(one(myRing));
    for (long i=0; i < ncols; ++i)
    {
      const long col = myColIndices[i];
      if (IsZero(v[col])) continue;
      const RingElem c = v[col]/myM[i][col];
      for (long j=0; j < n; ++j)
        v[j] -= c*myM[i][j];
      for (long j=0; j < len(myRowRepr[i]); ++j)
        myLinRelnValue[j] -= c*myRowRepr[i][j];
    }
    for (long i=0; i < n; ++i)
    {
      if (IsZero(v[i])) continue;
      // if v!=0 add it to the mill:
      myM.push_back(v);
      myColIndices.push_back(i);
      myRowRepr.push_back(myLinRelnValue);
      myLinRelnValue.clear();
      break;
    }
    // v is reduced: if 0 -> myLinRelnValue; else -> mill
    return !myLinRelnValue.empty();
  }


  //----- LinDepMill_Fp --------------------------------------

  class LinDepMill_Fp: public LinDepMillBase
  {
  public:
    LinDepMill_Fp(const ring& k, long dim);
    bool myAppendVec(const std::vector<RingElem>& v) override;
    const std::vector<RingElem>& myLinReln() const override  { return myLinRelnValue; }
// DEFAULT DTOR is OK    virtual ~LinDepMill_Fp();
  
  private:
    ring myRing;
    int myVecLen;
    // std::vector< std::vector<RingElem> > myM;
    // std::vector< std::vector<RingElem> > myRowRepr;
    // std::vector<int> myColIndices;
    std::vector<RingElem> myLinRelnValue;
    SmallFpImpl myArithModP;
    LinDepFp myMill;
  };

  LinDepMill_Fp::LinDepMill_Fp(const ring& k, long dim):
      myRing(k),
      myVecLen(dim),
      myLinRelnValue(),
      myArithModP(ConvertTo<long>(characteristic(k))),
      myMill(myArithModP,dim)
  {
/* Check args*/
  }


  bool LinDepMill_Fp::myAppendVec(const std::vector<RingElem>& v)
  {
    /* check arg */
    vector<SmallFpImpl::value> vec(myVecLen);
    for (int i=0; i < myVecLen; ++i)
    {
      vec[i] = myArithModP.myReduce(ConvertTo<BigInt>(v[i]));
    }
    const bool GotALinDep = myMill.myAppendVec(vec);
    if (!GotALinDep) myLinRelnValue.clear();
    if (GotALinDep)
    {
      const vector<SmallFpImpl::value>& reln = myMill.myLinReln();
      myLinRelnValue.resize(len(reln), zero(myRing));
      for (int i=0; i < len(reln); ++i)
        myLinRelnValue[i] = myArithModP.myExportNonNeg(reln[i]);
    }
    return GotALinDep;
  }


  LinDepMill::LinDepMill(const ring& k, long dim)
  {
    if (IsRingFp(k))
      myImpl.reset(new LinDepMill_Fp(k, dim));
    else
      myImpl.reset(new LinDepMill_generic(k, dim));
  }



} // end of namespace CoCoA
