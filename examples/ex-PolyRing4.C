// Copyright (c) 2005-2010  John Abbott, Anna M. Bigatti
// This file is part of the CoCoALib suite of examples.
// You are free to use any part of this example in your own programs.

#include "CoCoA/library.H"

using namespace std;

//----------------------------------------------------------------------
const string ShortDescription =
  "Example showing how to create more polynomial rings.              \n"
  "(follows ex-PolyRing1.C)                                          \n";

const string LongDescription =
  "More specialized and optimized polynomial rings                   \n";
//-----------------------------------------------------------------------

namespace CoCoA
{

  void SomeComputations(const PolyRing& P)
  {
    const long n = NumIndets(P);
    const vector<RingElem>& x = indets(P);

    // Put f = x[0] + x[1] + .. x[n-1]
    RingElem f(sum(indets(P)));

    // Put g = x[0] + 2*x[1] + ... + n*x[n-1] + 1234
    RingElem g(P, 1234);
    for (long i=0; i < n; ++i)  g += (i+1)*x[i];

    cout << "Some computations in the ring " << P << endl;
    cout << "Let f = " << f << endl
         << "and g = " << g << endl << endl;

    cout << "deg(f) = "  << deg(f) << endl;      // of type long
    cout << "wdeg(f) = " << wdeg(f) << endl;     // of type CoCoA::degree
    if (!IsZero(f-g))
      cout << "LPP(f-g) = " << LPP(f-g) << "   in " << owner(LPP(f-g)) << endl;
    if (!IsZero(f+g))
      cout << "LC(f+g) = " << LC(f+g) << "   in " << owner(LC(f+g)) << endl;
    cout << "--------------------------------------------" << endl << endl;
  }



  void program()
  {
    GlobalManager CoCoAFoundations;

    cout << ShortDescription << endl;

    // Define some coefficient rings.
    ring QQ    = RingQQ();
    ring Fp    = NewZZmod(NextProbPrime(BigInt(1234567) * BigInt(7654321)));
    ring QQa   = NewFractionField(NewPolyRing(QQ, symbols("a"))); // QQ(a)

    // The next few lines show some different ways of creating polynomial rings.
    SomeComputations(NewPolyRing(QQa, NewSymbols(1)));       // QQ(a)[#[1]] -- #[1] is a new indet
    SomeComputations(NewPolyRing(Fp, SymbolRange("z",2,4))); // FF_p[z[2..4]]
    // If we want, we can also specify the PP ordering:
    SomeComputations(NewPolyRing(QQ, NewSymbols(5), StdDegLex));  // indets are #[2],..,#[6]
    SomeComputations(NewPolyRing(QQ, symbols("x,y,z"), lex));// QQ[x,y,z]

    // inline power-product, optimized for sorting (PPMonoidOv)
    SparsePolyRing P = NewPolyRing_DMPI(Fp, symbols("x,y,z"));
    SomeComputations(P);
    SomeComputations(NewPolyRing_DMPI(QQ, SymbolRange("x", 1,6)));
    SomeComputations(NewPolyRing_DMPI(QQ, PPM(P)));
    SomeComputations(NewPolyRing_DMPI(QQ, SymbolRange("x", 0,5), lex));  
    // inline power-product and coefficient (Fp, with "small" p)
    SomeComputations(NewPolyRing_DMPII(NewZZmod(32003), NewSymbols(5)));
  }

} // end of namespace CoCoA

//----------------------------------------------------------------------
// Use main() to handle any uncaught exceptions and warn the user about them.
int main()
{
  try
  {
    CoCoA::program();
    return 0;
  }
  catch (const CoCoA::ErrorInfo& err)
  {
    cerr << "***ERROR***  UNCAUGHT CoCoA error";
    ANNOUNCE(cerr, err);
  }
  catch (const std::exception& exc)
  {
    cerr << "***ERROR***  UNCAUGHT std::exception: " << exc.what() << endl;
  }
  catch(...)
  {
    cerr << "***ERROR***  UNCAUGHT UNKNOWN EXCEPTION" << endl;
  }

  CoCoA::BuildInfo::PrintAll(cerr);
  return 1;
}
