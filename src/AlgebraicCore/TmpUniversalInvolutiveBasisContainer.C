//   Copyright (c)  2015  John Abbott,  Anna M. Bigatti
//   Original author: 2015  Mario Albert

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

#include "CoCoA/TmpUniversalInvolutiveBasisContainer.H"

namespace CoCoA
{
  namespace Involutive
  {
    using std::map;
    using std::pair;
    using std::string;
    using std::unique_ptr;
    using std::vector;


    void UniversalInvolutiveBasisContainer::myInitializeJBMill() const
    {
      if (jbMillPtr == nullptr)
      {
        JBMill::Builder builder;
        builder.setInput(generators);
        builder.setStrategy(strategy);
        jbMillPtr = unique_ptr<JBMill>(new JBMill(builder));
      }
    }

    void UniversalInvolutiveBasisContainer::myInitializeAndValidatePBMill(const ErrorContext& FnName) const
    {
      if (pbMillPtr == nullptr)
      {
        myInitializeJBMill();
        if (!IamDeltaRegular())
        {
          CoCoA_THROW_ERROR_WITH_CONTEXT2("This ideal is not delta regular", FnName);
        }
        // The ideal must be delta regular and the pb is not already generated
        CoCoA_ASSERT(IamDeltaRegular());
        PBMill::Converter converter;
        converter.setJBMill(*jbMillPtr);
        pbMillPtr = unique_ptr<PBMill>(new PBMill(converter));
      }
    }

    void UniversalInvolutiveBasisContainer::myValidateHomogeneous(const ErrorContext& FnName) const
    {
      if (!IamHomogeneous())
      {
        CoCoA_THROW_ERROR_WITH_CONTEXT2(ERR::ReqHomog, FnName);
      }
    }

    void UniversalInvolutiveBasisContainer::myValidateDegRevLexOrdering(const ErrorContext& FnName) const
    {
      if (!(IsStdDegRevLex(ordering(PPM(owner(generators.front()))))))
      {
        CoCoA_THROW_ERROR_WITH_CONTEXT2(ERR::PPOrder, FnName);
      }
    }

    void UniversalInvolutiveBasisContainer::myValidateMonomialIdeal(const ErrorContext& FnName) const
    {
      if (!IamMonomial())
      {
        CoCoA_THROW_ERROR_WITH_CONTEXT2("Require monomial ideal", FnName);
      }
    }

    void UniversalInvolutiveBasisContainer::myValidateCohenMacaulay(const ErrorContext& FnName) const
    {
      if (!IamCohenMacaulay())
      {
        CoCoA_THROW_ERROR_WITH_CONTEXT2("Require Cohen Macualay ideal", FnName);
      }
    }

    bool UniversalInvolutiveBasisContainer::IamDeltaRegular() const
    {
      if (IsUncertain3(isDeltaRegular))
      {
        myInitializeJBMill();
        isDeltaRegular = jbMillPtr->IamPommaretBasis();
      }
      return IsTrue3(isDeltaRegular);
    }

    bool UniversalInvolutiveBasisContainer::IamMonomial() const
    {
      if (IsUncertain3(isMonomial))
      {
        myInitializeJBMill();
        isMonomial = jbMillPtr->IamMonomialIdeal();
      }
      return IsTrue3(isMonomial);
    }

    bool UniversalInvolutiveBasisContainer::IamHomogeneous() const
    {
      if (IsUncertain3(isHomogeneous))
      {
        myInitializeJBMill();
        isHomogeneous = jbMillPtr->IamHomogenous();
      }
      return IsTrue3(isHomogeneous);
    }

    map<PPMonoidElem, vector<bool> > UniversalInvolutiveBasisContainer::myMultVars() const
    {
      if (multVars.empty())
      {
        myInitializeJBMill();
        myMultAndNonMultVars();
      }
      return multVars;
    }

    map<PPMonoidElem, vector<bool> > UniversalInvolutiveBasisContainer::myNonMultVars() const
    {
      if (multVars.empty())
      {
        myInitializeJBMill();
        myMultAndNonMultVars();
      }
      return nonMultVars;
    }

    RingElem UniversalInvolutiveBasisContainer::myHilbertPol(ConstRefRingElem s) const
    {
      if (owner(hilbertPol) != owner(s))
      {
        myInitializeJBMill();
        hilbertPol = jbMillPtr->myHilbertPol(s);
      }
      return hilbertPol;
    }

    RingElem UniversalInvolutiveBasisContainer::myHilbertSeries(ConstRefRingElem s) const
    {
      if (owner(hilbertSeries) != owner(s))
      {
        myInitializeJBMill();
        hilbertSeries = jbMillPtr->myHilbertSeries(s);
      }
      return hilbertSeries;
    }

    BigInt UniversalInvolutiveBasisContainer::myHilbertFunc(const BigInt& s) const
    {
      myInitializeJBMill();
      return jbMillPtr->myHilbertFunc(s);
    }


    void UniversalInvolutiveBasisContainer::myMultAndNonMultVars() const
    {
      multVars      = jbMillPtr->myMultVars();
      nonMultVars   = multVars;
//???  for (auto& v: nonMultVars)  v.second.flip();
      for (map<PPMonoidElem, vector<bool> >::iterator i = nonMultVars.begin(); i != nonMultVars.end(); ++i)
      {
        (i->second).flip();
      }
    }

    FGModule UniversalInvolutiveBasisContainer::myFirstSyzygy() const
    {
      if (firstSyzygyPtr == nullptr)
      {
        myInitializeJBMill();
        firstSyzygyPtr = unique_ptr<FGModule>(new FGModule(jbMillPtr->mySyzygy()));
      }
      return *firstSyzygyPtr;
    }

    long UniversalInvolutiveBasisContainer::myDimension() const
    {
      if (dimension < 0)
      {
        myInitializeJBMill();
        dimension = jbMillPtr->myDim();
      }
      return dimension;
    }

    long UniversalInvolutiveBasisContainer::myDegree() const
    {
      if (degree < 0)
      {
        myInitializeJBMill();
        degree = jbMillPtr->myDeg();
      }
      return degree;
    }

    UniversalInvolutiveBasisContainer::ComplementaryDecomposition UniversalInvolutiveBasisContainer::myComplementaryDecomposition() const
    {
      if (complementaryDecompositionPtr == nullptr)
      {
        myInitializeJBMill();
        complementaryDecompositionPtr = unique_ptr<ComplementaryDecomposition>(new ComplementaryDecomposition(jbMillPtr->myComplementaryDecompositionPolynomial()));
      }
      return *complementaryDecompositionPtr;
    }

    long UniversalInvolutiveBasisContainer::myDepth() const
    {
      if (depth < 0)
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        myValidateHomogeneous(CoCoA_ERROR_CONTEXT);
        depth = pbMillPtr->myDepth();
      }
      return depth;
    }

    long UniversalInvolutiveBasisContainer::myProjDim() const
    {
      if (projDim < 0)
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        myValidateHomogeneous(CoCoA_ERROR_CONTEXT);
        projDim = pbMillPtr->myProjDim();
      }
      return projDim;
    }

    vector<RingElem> UniversalInvolutiveBasisContainer::mySocle() const
    {
      if (soclePtr == nullptr)
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        myValidateHomogeneous(CoCoA_ERROR_CONTEXT);
        myValidateDegRevLexOrdering(CoCoA_ERROR_CONTEXT);
        myValidateCohenMacaulay(CoCoA_ERROR_CONTEXT);
        soclePtr = unique_ptr<vector<RingElem> >(new vector<RingElem>(pbMillPtr->mySocle()));
      }
      return *soclePtr;
    }

    map<pair<long, long>, long> UniversalInvolutiveBasisContainer::myExtremalBettiNumbers() const
    {
      if (extremalBettiNumbers.empty())
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        myValidateHomogeneous(CoCoA_ERROR_CONTEXT);
        myValidateDegRevLexOrdering(CoCoA_ERROR_CONTEXT);
        extremalBettiNumbers = pbMillPtr->myExtremalBettiNumbers();
      }
      return extremalBettiNumbers;
    }

    vector<RingElem> UniversalInvolutiveBasisContainer::myRegularSequence() const
    {
      if (regularSequence.empty())
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        regularSequence = pbMillPtr->myRegSeq();
      }
      return regularSequence;
    }

    vector<RingElem> UniversalInvolutiveBasisContainer::myMaximalStronglyIndependentSet() const
    {
      if (maximalStronglyIndependentSet.empty())
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        maximalStronglyIndependentSet = pbMillPtr->myMaxStronglyIndependentSet();
      }
      return maximalStronglyIndependentSet;
    }

    long UniversalInvolutiveBasisContainer::myRegularity() const
    {
      if (regularity < 0)
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        myValidateHomogeneous(CoCoA_ERROR_CONTEXT);
        myValidateDegRevLexOrdering(CoCoA_ERROR_CONTEXT);
        regularity = pbMillPtr->myRegularity();
      }
      return regularity;
    }

    long UniversalInvolutiveBasisContainer::mySatiety() const
    {
      if (satiety < 0)
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        myValidateHomogeneous(CoCoA_ERROR_CONTEXT);
        myValidateDegRevLexOrdering(CoCoA_ERROR_CONTEXT);
        satiety = pbMillPtr->mySatiety();
      }
      return satiety;
    }

    vector<RingElem> UniversalInvolutiveBasisContainer::mySaturation() const
    {
      if (saturation.empty())
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        myValidateHomogeneous(CoCoA_ERROR_CONTEXT);
        myValidateDegRevLexOrdering(CoCoA_ERROR_CONTEXT);
        saturation = pbMillPtr->mySaturation();
      }
      return saturation;
    }

    bool UniversalInvolutiveBasisContainer::IamCohenMacaulay() const
    {
      if (IsUncertain3(isCohenMacaulayRing))
      {
        myInitializeAndValidatePBMill(CoCoA_ERROR_CONTEXT);
        isCohenMacaulayRing = (myDepth() == myDimension());
      }
      return IsTrue3(isCohenMacaulayRing);
    }

    const std::vector<RingElem>& UniversalInvolutiveBasisContainer::myJanetBasis() const
    {
      if (janetBasis.empty())
      {
        myInitializeJBMill();
        janetBasis = jbMillPtr->myReturnJB();
      }
      return janetBasis;
    }

  } // end namespace Involutive
} // end namespace CoCoA
