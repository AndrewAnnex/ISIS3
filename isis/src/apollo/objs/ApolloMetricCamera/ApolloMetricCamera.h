#ifndef ApolloMetricCamera_h
#define ApolloMetricCamera_h
/**
 * @file
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

#include "FramingCamera.h"

#include <QString>

namespace Isis {
  /**
   * @brief Apollo Metric Camera Model
   *
   * This is the camera model for the Apollo metric camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Apollo
   *
   * @author 2006-11-14 Jacob Danton
   *
   * @internal
   *   @history 2006-11-14 Jacob Danton - Original Version
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2010-07-20 Sharmila Prasad - Modified documentation to remove
   *                           Doxygen Warning
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                           pure virtual in Camera, implemented in mission
   *                           specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                           method. Updated unitTest to test for new methods.
   *                           Updated documentation. Removed Apollo namespace
   *                           wrap inside Isis namespace. Added Isis Disclaimer
   *                           to files.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards.   References #972.
   *   @history 2014-01-17 Kris Becker - Set CkReferenceID to J2000 to resolve
   *                           problem with ckwriter. Also, set the SpkReferenceId
   *                           to J2000. References #1737 and #1739.
   *   @history 2015-08-24 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods and added test data for Apollo 16 and 17.
   */
  class ApolloMetricCamera : public FramingCamera {
    public:
      ApolloMetricCamera(Cube &cube);
      //! Destroys the ApolloMetricCamera Object
      ~ApolloMetricCamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);

      //QString name();
      //void setName(QString name);

      /**
       * CK frame ID -
       * Apollo 15 instrument code (A15_METRIC) = -915240
       * Apollo 16 instrument code (A16_METRIC) = -916240
       * Apollo 17 instrument code (A17_METRIC) = -917240 
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return p_ckFrameId; }

      /**
       * CK Reference ID -
       * APOLLO_15_NADIR = 1
       * APOLLO_16_NADIR = 1
       * APOLLO_17_NADIR = 1
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" Kernel 
       *         Reference ID
       */
      virtual int CkReferenceId() const { return p_ckReferenceId; }

      /**
       * SPK Target Body ID -
       * Apollo 15 = -915
       * Apollo 16 = -916
       * Apollo 17 = -917 
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return p_spkTargetId; }

      /** 
       *  SPK Reference ID - J2000
       *  
       *  Even thought the ephemeris is relative to B1950, this specification
       *  is for writing SPK kernels.  We should stay with the J2000 epoch in
       *  these cases.
       *
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Reference ID,
       */
      virtual int SpkReferenceId() const { return (1); }
      
      virtual QString instrumentNameLong() const;
      virtual QString instrumentNameShort() const;
      virtual QString spacecraftNameLong() const;
      virtual QString spacecraftNameShort() const;

    private:
      int p_ckFrameId;       //!< "Camera-matrix" Kernel Frame ID
      int p_ckReferenceId;   //!< "Camera-matrix" Kernel Reference ID
      int p_spkTargetId;     //!< Spacecraft Kernel Target ID
      
      QString m_instrumentNameLong; //!< Full instrument name
      QString m_instrumentNameShort; //!< Shortened instrument name
      QString m_spacecraftNameLong; //!< Full spacecraft name
      QString m_spacecraftNameShort; //!< Shortened spacecraft name
  };
};
#endif
