// Copyright (c) 2021  John Abbott,  Anna M. Bigatti
// This file is part of the CoCoALib suite of examples.
// You are free to use any part of this example in your own programs.

#include "CoCoA/library.H"
using namespace CoCoA;
using namespace std;

//----------------------------------------------------------------------
const string ShortDescription =
  "This example shows the container iterator \"for\" loop in C++.     \n"
  "See also ex-c++-loop-for1.C for \"continue\" and \"break\" commands. \n";

const string LongDescription =
  "This example shows the container iterator \"for\" loop in C++.     \n"
  "See also ex-c++-loop-for1.C for \"continue\" and \"break\" commands. \n"
  "We show how to state whether the elements are copied.            \n";

//----------------------------------------------------------------------

namespace CoCoA
{

  // SYNTAX of container-iterator "for" loop:
  //
  // for  ( <entry-type> <var-name>  :  <list/vector> )
  // {
  //   <commands>
  // }
  // In reality the "for" loop is more flexible than this.

  void program()
  {
    GlobalManager CoCoAFoundations;
    cout << ShortDescription << endl;

    vector<int> DigitsOfPi{3,1,4,1,5,9,2,6,5};

    // Print out the elements of DigitsOfPi:
    // [here each element of DigitsOfPi is copied into digit]
    for (int digit: DigitsOfPi)
    {
      cout << digit << "  ";
    }
    cout << endl;

    // Reduce each digit by 1:
    // In C++ "&" means reference to a value
    for (int& digit: DigitsOfPi)
    {
      --digit;
    }
    cout << endl;

    // Print out the elements of DigitsOfPi:
    // [without copying them]
    for (const int& digit: DigitsOfPi)
    {
      cout << digit << "  ";
    }
    cout << endl;
  }

} // end of namespace CoCoA

// IGNORE THE STUFF BELOW (at least for now)

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
