// Copyright (c) 2017  John Abbott and Anna M. Bigatti
// Authors: Anna M. Bigatti, Alberto Griggio
// This file is part of the CoCoALib suite of examples.
// You are free to use any part of this example in your own programs.

#include "CoCoA/library.H"

///// first prototype
#include "CoCoA/ExternalLibs-MathSAT.H"

using namespace std;

//----------------------------------------------------------------------
const string ShortDescription =
  "This program shows a prototype for MathSAT communication.\n";

const string LongDescription =
  "This program shows the first prototype for MathSAT communication.\n"
  "The CoCoA class to use is MathSAT::env.                          \n"
  "Only linear constraints, defined by matrices or polynomials.     \n"
  "The matrices have n+1 columns: last col is for constants.        \n"
  "The linear polynomials are compared with 0 (eq0, neq0, leq0, lt0).\n";


//----------------------------------------------------------------------

namespace CoCoA
{  

  void program()
  {
    GlobalManager CoCoAFoundations;

    cout << ShortDescription << endl;
    cout << LongDescription << endl;
    cout << boolalpha; // so that bools print out as true/false

#ifndef CoCoA_WITH_MATHSAT
    cout << "MathSAT library is not available to CoCoALib." << endl;
#else // MathSAT is available

    ring QQ = RingQQ();

    ring P = NewPolyRing(QQ, symbols("x,y"));
    RingElem x(P,"x"), y(P,"y");

    cout << " --------------------------------------" << endl
         << " -- add the constraints via polynomials"
         << endl;

    // create a MathSAT environment: E
    MathSAT::env E;
    
    // polynomial syntax:
    MathSAT::AddLeq0(E, x-9);     //   x    -9 <= 0
    MathSAT::AddEq0(E,  x -y);    //   x -y     = 0
    MathSAT::AddLt0(E,  2*x -1);  // 2*x    -1  < 0
    
    cout << "A solution is " << MathSAT::LinSolve(E) << endl; //-> 0, 0

    cout << " ------------------------------------" << endl
         << " -- add more constraints via matrices"
         << endl;

    matrix M = NewDenseMat(QQ, 2, 3); // (2x3 matrix)
    SetEntry(M,0,0,  3);   SetEntry(M,0,1,  1);   SetEntry(M,0,2, -2);
    SetEntry(M,1,0,  1);   SetEntry(M,1,1, -1);   SetEntry(M,1,2,  0);
    MathSAT::AddLeq0(E, M);       // 3*x -y -2 <= 0
                                  //   x -y    <= 0
    M = NewDenseMat(QQ, 1, 3);  SetEntry(M,0,0, 1);  // (1x3 matrix)
    MathSAT::AddNeq0(E, M);       //   x       != 0

    cout << "A solution is " << MathSAT::LinSolve(E) << endl; //-> 1/4, 1/4
    
#endif // CoCoA_WITH_MATHSAT
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
