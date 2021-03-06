#ifndef Isotropic1_h
#define Isotropic1_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AtmosModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief
   *
   * @ingroup RadiometricAndPhotometricCorrection
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 1998-12-21 Randy Kirk - USGS, Flagstaff - Original
   *                      code
   *  @history 2007-02-20 Janet Barrett - Imported from Isis2.
   *  @history 2007-08-15 Steven Lambright - Refactored code
   *  @history 2008-03-07 Janet Barrett - Moved code to set standard
   *                      conditions to the AtmosModel class
   *  @history 2008-06-18 Stuart Sides - Fixed doc error
   *  @history 2008-11-05 Jeannie Walldren - Replaced reference to
   *                      NumericalMethods::r8expint() with AtmosModel::En().
   *                      Added documentation from Isis2.
   *  @history 2011-08-18 Sharmila Prasad Moved common HNORM to base AtmosModel
   *  @history 2011-12-19 Janet Barrett - Added code to estimate the
   *                      shadow brightness value (transs). Also got rid of
   *                      unnecessary check for identical photometric angle values
   *                      between successive calls. This check should only be
   *                      made in the photometric models.
   */
  class Isotropic1 : public AtmosModel {
    public:
      Isotropic1(Pvl &pvl, PhotoModel &pmodel);
      virtual ~Isotropic1() {};
      
    protected:
      virtual void AtmosModelAlgorithm(double phase, double incidence, double emission);

    private:
      double p_wha2;
      double p_delta;
      double p_fixcon;
      double p_gammax, p_gammay;
      double p_e2, p_e3, p_e4, p_e5;
      double p_x0, p_y0;
      double p_alpha0, p_alpha1, p_alpha2;
      double p_beta0, p_beta1, p_beta2;
  };
};

#endif
