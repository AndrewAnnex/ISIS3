#ifndef PlaneShape_h
#define PlaneShape_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2012/07/30 16:25:00 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include <vector>

#include <QVector>

#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

#include "ShapeModel.h"
#include "SurfacePoint.h"
#include "Pvl.h"

namespace Isis {
  /**
   * @brief Define plane shape model
   *
   * This class defines a plane shape model for Isis3 target bodies as well as
   * provide utilities to retrieve radii and photometric information.
   *
   *
   * @ingroup 
   *
   * @author 2012-07-30 Ken Edmundson
   *
   * @internal
   */
  class PlaneShape : public Isis::ShapeModel {
    public:
      // Constructors
      PlaneShape(Target *target, Isis::Pvl &pvl);
      PlaneShape(Target *target);
      PlaneShape();

      // Destructor
      ~PlaneShape() {};

      // Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);
      
      // Calculate the surface normal of the current intersection point
      void calculateSurfaceNormal() {};
      void calculateDefaultNormal() {};
      void calculateLocalNormal(QVector<double *> cornerNeighborPoints) {};

      double emissionAngle(const std::vector<double> & sB);
      double incidenceAngle(const std::vector<double> &uB);

      // Get the local radius for a point on the surface
      Distance localRadius(const Latitude &lat, const Longitude &lon);
  };
};

#endif

