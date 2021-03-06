/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <cstdlib>
#include "LunarLambert.h"
#include "PhotoModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  std::cout << "UNIT TEST for LunarLambert photometric function" <<
            std::endl << std::endl;

  PvlGroup alg("Algorithm");
  alg += PvlKeyword("Name", "LunarLambert");

  PvlObject o("PhotometricModel");
  o.addGroup(alg);

  Pvl pvl;
  pvl.addObject(o);
  std::cout << pvl << std::endl << std::endl;

  try {
    LunarLambert *pm = (LunarLambert *)PhotoModelFactory::Create(pvl);

    std::cout << "PhotoL = " << pm->PhotoL() << std::endl;

    std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0, 0.0, 0.0) <<
              std::endl;
    std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0, 45.0, 30.0) <<
              std::endl;
    std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0, 90.0, 90.0) <<
              std::endl << std::endl;

    pm->SetPhotoL(0.0);
    std::cout << "PhotoL = " << pm->PhotoL() << std::endl;

    std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0, 0.0, 0.0) <<
              std::endl;
    std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0, 45.0, 30.0) <<
              std::endl;
    std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0, 90.0, 90.0) <<
              std::endl << std::endl;

    pm->SetPhotoL(0.5);
    std::cout << "PhotoL = " << pm->PhotoL() << std::endl;

    std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0, 0.0, 0.0) <<
              std::endl;
    std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0, 45.0, 30.0) <<
              std::endl;
    std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0, 90.0, 90.0) <<
              std::endl << std::endl;

    pm->SetPhotoL(2.0);
    std::cout << "PhotoL = " << pm->PhotoL() << std::endl;

    std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0, 0.0, 0.0) <<
              std::endl;
    std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0, 45.0, 30.0) <<
              std::endl;
    std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
              std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0, 90.0, 90.0) <<
              std::endl << std::endl;
  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
