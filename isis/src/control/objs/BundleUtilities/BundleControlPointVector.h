#ifndef BundleControlPointVector_h
#define BundleControlPointVector_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2014/5/22 01:35:17 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include "BundleControlPoint.h"

namespace Isis {


  /**
   * This class is a container class for BundleControlPoints.
   *
   * Contained BundleControlPoints are stored as shared pointers, so are automatically deleted when
   * all shared pointers are deleted.
   *
   * @author 2014-05-22 Ken Edmundson
   *
   * @internal
   *   @history 2018-04-16 Ken Edmundson - Original version.
   */
  class BundleControlPointVector : public QVector<BundleControlPointQsp> {

    public:
      BundleControlPointVector();
      BundleControlPointVector(const BundleControlPointVector &src);
      ~BundleControlPointVector();

      BundleControlPointVector &operator=(const BundleControlPointVector &src);

      void applyParameterCorrections(SparseBlockMatrix &normalsMatrix,
                                     LinearAlgebra::Vector &imageSolution,
                                     const BundleTargetBodyQsp target);

      void computeMeasureResiduals();
      double vtpvContribution();
      double vtpvMeasureContribution();
      double vtpvRangeContribution();
  };
}

#endif // BundleControlPointVector_h