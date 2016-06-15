////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    main/main.cu
/// @brief   The main code
///
/// @author  Mu Yang <emfomy@gmail.com>
///

#include <iostream>
#include <nbfmm/core.hpp>
#include <nbfmm/visualization/stars.hpp>

using namespace std;
using namespace nbfmm;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Main function
///
int main( int argc, char const *argv[] ) {
  cout << "NBFMM "
       << NBFMM_VERSION_MAJOR << "."
       << NBFMM_VERSION_MINOR << "."
       << NBFMM_VERSION_PATCH << endl;

  return 0;
}
