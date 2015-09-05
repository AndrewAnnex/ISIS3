/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/03/27 06:36:41 $
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
#include "CameraGroundMap.h"

#include <QDebug>

#include <iostream>

#include "IException.h"
#include "NaifStatus.h"
#include "SurfacePoint.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Target.h"

using namespace std;

namespace Isis {
  CameraGroundMap::CameraGroundMap(Camera *parent) {
    p_camera = parent;
    p_camera->SetGroundMap(this);
  }

  /** Compute ground position from focal plane coordinate
   *
   * This method will compute the ground position given an
   * undistorted focal plane coordinate.  Note that the latitude/longitude
   * value can be obtained from the camera class passed into the constructor.
   *
   * @param ux distorted focal plane x in millimeters
   * @param uy distorted focal plane y in millimeters
   * @param uz distorted focal plane z in millimeters
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::SetFocalPlane(const double ux, const double uy,
                                      double uz) {
    NaifStatus::CheckErrors();

    SpiceDouble lookC[3];
    lookC[0] = ux;
    lookC[1] = uy;
    lookC[2] = uz;

    SpiceDouble unitLookC[3];
    vhat_c(lookC, unitLookC);

    NaifStatus::CheckErrors();

    bool result = p_camera->SetLookDirection(unitLookC);
    return result;
  }

  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::SetGround(const Latitude &lat, const Longitude &lon) {
    if (p_camera->target()->shape()->name() == "Plane") {
      double radius = lat.degrees(); //  m
      // double azimuth = lon.degrees();
      Latitude lat(0., Angle::Degrees);
      if (radius < 0.0) radius = 0.0; // TODO: massive, temporary kluge to get around testing
                                                     // latitude at -90 in caminfo app (are there
                                                     // more issues like this? Probably)KE
      if (p_camera->Sensor::SetGround(SurfacePoint(lat, lon, Distance(radius, Distance::Meters)))) {
         LookCtoFocalPlaneXY();
         return true;
      }
    }
    else {
      Distance radius(p_camera->LocalRadius(lat, lon));
      if (radius.isValid()) {
        if(p_camera->Sensor::SetGround(SurfacePoint(lat, lon, radius))) {
          LookCtoFocalPlaneXY();
          return true;
        }
      }
    }

    return false;
  }

  //! Compute undistorted focal plane coordinate from camera look vector
  void CameraGroundMap::LookCtoFocalPlaneXY() {
    double lookC[3];
    p_camera->Sensor::LookDirection(lookC);

    //Get the fl as the z coordinate to handle instruments looking down the -z axis 2013-02-22.
    double focalLength = p_camera->DistortionMap()->UndistortedFocalPlaneZ();
    double scale = focalLength / lookC[2];

    p_focalPlaneX = lookC[0] * scale;
    p_focalPlaneY = lookC[1] * scale;
  }

  /** Compute undistorted focal plane coordinate from ground position that includes a local radius
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in meters
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::SetGround(const SurfacePoint &surfacePoint) {
    if(p_camera->Sensor::SetGround(surfacePoint)) {
      LookCtoFocalPlaneXY();
      return true;
    }

    return false;
  }


  /** Compute undistorted focal plane coordinate from ground position using current Spice from SetImage call
   *
   * This method will compute the undistorted focal plane coordinate for
   * a ground position, using the current Spice settings (time and kernels)
   * without resetting the current point values for lat/lon/radius/p_pB/x/y.  The
   * class value for p_look is set by this method.
   *
   * @param point
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::GetXY(const SurfacePoint &point, double *cudx, double *cudy) {

    std::vector<double> pB(3);
    pB[0] = point.GetX().kilometers();
    pB[1] = point.GetY().kilometers();
    pB[2] = point.GetZ().kilometers();

    // Check for Sky images
    if (p_camera->target()->isSky()) {
      return false;
    }

    // Should a check be added to make sure SetImage has been called???

    // Get spacecraft vector in body-fixed coordinates
    SpiceRotation *bodyRot = p_camera->bodyRotation();
    SpiceRotation *instRot = p_camera->instrumentRotation();
    std::vector<double> pJ = bodyRot->J2000Vector(pB);
    std::vector<double> sJ = p_camera->instrumentPosition()->Coordinate();

    // Calculate lookJ
    std::vector<double> lookJ(3);
    for(int ic = 0; ic < 3; ic++)   lookJ[ic] = pJ[ic] - sJ[ic];

    // Save pB for target body partial derivative calculations NEW *** DAC 8-14-2015
    p_pB = pB;
    
    // Check for point on back of planet by checking to see if surface point is viewable (test emission angle)
    // During iterations, we may not want to do the back of planet test???
    std::vector<double> lookB = bodyRot->ReferenceVector(lookJ);
    double upsB[3], upB[3], dist;
    vminus_c((SpiceDouble *) &lookB[0], upsB);
    unorm_c(upsB, upsB, &dist);
    unorm_c((SpiceDouble *) &pB[0], upB, &dist);
    double angle = vdot_c(upB, upsB);
    double emission;
    if (angle > 1) {
      emission = 0;
    }
    else if (angle < -1) {
      emission = 180.;
    }
    else {
      emission = acos(angle) * 180.0 / Isis::PI;
    }

    if (fabs(emission) > 90.) return false;

    // Get the look vector in the camera frame and the instrument rotation
    p_lookJ.resize(3);
    p_lookJ = lookJ;
    std::vector <double> lookC(3);
    lookC = instRot->ReferenceVector(p_lookJ);

    // Get focal length with direction for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    *cudx = lookC[0] * fl / lookC[2];
    *cudy = lookC[1] * fl / lookC[2];
    return true;
  }




  /** Compute undistorted focal plane coordinate from ground position using current Spice from SetImage call
   *
   * This method will compute the undistorted focal plane coordinate for
   * a ground position, using the current Spice settings (time and kernels)
   * without resetting the current point values for lat/lon/radius/p_pB/x/y.  The
   * class value for p_look is set by this method.
   *
   * @param lat Latitude in degrees 
   * @param lon Longitude in degrees
   * @param radius 
   *
   * @return conversion was successful
   */
  bool CameraGroundMap::GetXY(const double lat, const double lon,
                              const double radius, double *cudx, double *cudy) {
    SurfacePoint spoint(Latitude(lat, Angle::Degrees),
                    Longitude(lon, Angle::Degrees),
                    Distance(radius, Distance::Meters));
    return GetXY(spoint, cudx, cudy);
  }


  /** Compute derivative w/r to position of focal plane coordinate from ground position using current Spice from SetImage call
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to a spacecraft position coordinate, using the current
   * Spice settings (time and kernels) without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful
   */
  //  also have a GetDxyDorientation and a GetDxyDpoint
  bool CameraGroundMap::GetdXYdPosition(const SpicePosition::PartialType varType, int coefIndex,
                                        double *dx, double *dy) {

    //  TODO  add a check to make sure p_lookJ has been set

    // Get directional fl for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    // Rotate look vector into camera frame
    SpiceRotation *instRot = p_camera->instrumentRotation();
    std::vector <double> lookC(3);
    lookC = instRot->ReferenceVector(p_lookJ);

    SpicePosition *instPos = p_camera->instrumentPosition();

    std::vector<double> d_lookJ = instPos->CoordinatePartial(varType, coefIndex);
    for(int j = 0; j < 3; j++) d_lookJ[j] *= -1.0;
    std::vector<double> d_lookC =  instRot->ReferenceVector(d_lookJ);
    *dx = fl * DQuotient(lookC, d_lookC, 0);
    *dy = fl * DQuotient(lookC, d_lookC, 1);
    return true;
  }

  /** Compute derivative of fp coordinate w/r to instrument using current state from SetImage call
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to the instrument orientation, using the current Spice
   * settings (time and kernels) without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful
   */
  //  also have a GetDxyDorientation and a GetDxyDpoint
  bool CameraGroundMap::GetdXYdOrientation(const SpiceRotation::PartialType varType, int coefIndex,
      double *dx, double *dy) {

    //  TODO  add a check to make sure p_lookJ has been set

    // Get directional fl for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    // Rotate J2000 look vector into camera frame
    SpiceRotation *instRot = p_camera->instrumentRotation();
    std::vector <double> lookC(3);
    lookC = instRot->ReferenceVector(p_lookJ);

    // Rotate J2000 look vector into camera frame through the derivative rotation
    std::vector<double> d_lookC = instRot->ToReferencePartial(p_lookJ, varType, coefIndex);

    *dx = fl * DQuotient(lookC, d_lookC, 0);
    *dy = fl * DQuotient(lookC, d_lookC, 1);
    return true;
  }

  /** Compute derivative of focal plane coordinate w/r to target body using current state
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to the target body orientation, using the current Spice
   * settings (time and kernels) without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful
   */
  //  also have a GetDxyDorientation and a GetDxyDpoint
  bool CameraGroundMap::GetdXYdTOrientation(const SpiceRotation::PartialType varType, int coefIndex,
      double *dx, double *dy) {

    //  TODO  add a check to make sure p_pB and p_lookJ have been set. 
    // 0.  calculate or save from previous GetXY call lookB.  We need ToJ2000Partial that is 
    //     like a derivative form of J2000Vector  
    // 1.  we will call d_lookJ = bodyrot->ToJ2000Partial (Make sure the partials are correct for the target body
    //             orientation matrix.
    // 2.  we will then call d_lookC = instRot->ReferenceVector(d_lookJ)
    // 3.  the rest should be the same.

    // Get directional fl for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    // Rotate body-fixed look vector into J2000 through the derivative rotation
    SpiceRotation *bodyRot = p_camera->bodyRotation();
    SpiceRotation *instRot = p_camera->instrumentRotation();
    std::vector<double> dlookJ = bodyRot->ToJ2000Partial(p_pB, varType, coefIndex);
    std::vector <double> lookC(3);
    std::vector <double> dlookC(3);

    // Rotate both the J2000 look vector and the derivative J2000 look vector into the camera
    lookC = instRot->ReferenceVector(p_lookJ);
    dlookC = instRot->ReferenceVector(dlookJ);

    *dx = fl * DQuotient(lookC, dlookC, 0);
    *dy = fl * DQuotient(lookC, dlookC, 1);
    return true;
  }

  /** Compute derivative of focal plane coordinate w/r to ground point using current state
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to lat, lon, or radius, using the current Spice settings (time
   *  and kernels) without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful 
   */
  bool CameraGroundMap::GetdXYdPoint(std::vector<double> d_pB, double *dx, double *dy) {

    //  TODO  add a check to make sure p_lookJ has been set

    // Get directional fl for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    // Rotate look vector into camera frame
    SpiceRotation *instRot = p_camera->instrumentRotation();
    std::vector <double> lookC(3);
    lookC = instRot->ReferenceVector(p_lookJ);

    SpiceRotation *bodyRot = p_camera->bodyRotation();
    std::vector<double> d_lookJ = bodyRot->J2000Vector(d_pB);
    std::vector<double> d_lookC = instRot->ReferenceVector(d_lookJ);

    *dx = fl * DQuotient(lookC, d_lookC, 0);
    *dy = fl * DQuotient(lookC, d_lookC, 1);
    return true;
  }


  /** Compute derivative of focal plane coordinate w/r to one of the ellipsoidal radii (a, b, or c)
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to the a (major axis), b (minor axis), or c (polar axis) radius, 
   * using the current Spice settings (time and kernels) without resetting the current point 
   * values for lat/lon/radius/x/y.
   *
   * @param raxis Radius axis enumerated partial type (definitions in TBD)
   * @param spoint Surface point whose derivative is to be evalutated
   *
   * @return partialDerivative of body-fixed  point with respect to selected ellipsoid axis
   */
  std::vector<double> CameraGroundMap::EllipsoidPartial(SurfacePoint spoint, PartialType raxis) {
    double rlat = spoint.GetLatitude().radians();
    double rlon = spoint.GetLongitude().radians();
    double sinLon = sin(rlon);
    double cosLon = cos(rlon);
    double sinLat = sin(rlat);
    double cosLat = cos(rlat);

    std::vector<double> v(3);

    switch(raxis) {
      case WRT_MajorAxis:   
         v[0] = cosLat * cosLon;
         v[1] = 0.0;
         v[2] =  0.0;
         break;
      case WRT_MinorAxis:  
         v[0] = 0.0;
         v[1] =  cosLat * sinLon;
         v[2] =  0.0;
         break;
      case WRT_PolarAxis: 
         v[0] = 0.0;
         v[1] = 0.0;
         v[2] = sinLat;
         break;
      default:
        QString msg = "Invalid partial type for this method";
        throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return v;
  }


  /** Compute derivative of focal plane coordinate w/r to one of the ellipsoidal radii (a, b, or c)
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to the a (major axis), b (minor axis), or c (polar axis) radius, 
   * using the current Spice settings (time and kernels) without resetting the current point 
   * values for lat/lon/radius/x/y.
   *
   * @param spoint Surface point whose derivative is to be evalutated
   *
   * @return partialDerivative of body-fixed point with respect to mean radius
   * TODO This method assumes the radii of all points in the adjustment have been set identically
   *            to the  
   */
  std::vector<double> CameraGroundMap::MeanRadiusPartial(SurfacePoint spoint, Distance meanRadius) {
    double radkm = meanRadius.kilometers();

    std::vector<double> v(3);

    v[0] = spoint.GetX().kilometers() / radkm;
    v[1] = spoint.GetY().kilometers() / radkm;
    v[2] = spoint.GetZ().kilometers() / radkm;

    return v;
  }


  /** Compute derivative with respect to indicated variable of conversion function from lat/lon/rad to rectangular coord
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in meters
   * @param wrt take derivative with respect to this value
   *
   * @return partialDerivative
   */
  std::vector<double> CameraGroundMap::PointPartial(SurfacePoint spoint, PartialType wrt) {
    double rlat = spoint.GetLatitude().radians();
    double rlon = spoint.GetLongitude().radians();
    double sinLon = sin(rlon);
    double cosLon = cos(rlon);
    double sinLat = sin(rlat);
    double cosLat = cos(rlat);
    double radkm = spoint.GetLocalRadius().kilometers();

    std::vector<double> v(3);
    if(wrt == WRT_Latitude) {
      v[0] = -radkm * sinLat * cosLon;
      v[1] = -radkm * sinLon * sinLat;
      v[2] =  radkm * cosLat;
    }
    else if(wrt == WRT_Longitude) {
      v[0] = -radkm * cosLat * sinLon;
      v[1] =  radkm * cosLat * cosLon;
      v[2] =  0.0;
    }
    else {
      v[0] = cosLon * cosLat;
      v[1] = sinLon * cosLat;
      v[2] = sinLat;
    }

    return v;
  }


  /**
   * Convenience method for quotient rule applied to look vector
   *
   * This method will compute the derivative of the following function
   * (coordinate x or y) / (coordinate z)
   *
   * @param look  look vector in camera frame
   * @param dlook derivative of look vector in camera frame
   * @param index vector value to differentiate
   *
   * @return derivative
   */
  double CameraGroundMap::DQuotient(std::vector<double> &look,
                                    std::vector<double> &dlook,
                                    int index) {
    return (look[2] * dlook[index] - look[index] * dlook[2]) /
           (look[2] * look[2]);
  }
}
