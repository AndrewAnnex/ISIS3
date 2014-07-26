#include "BundleObservationVector.h"

#include "BundleObservation.h"
#include "BundleSettings.h"
#include "IException.h"


namespace Isis {

  /**
   * constructor
   */
  BundleObservationVector::BundleObservationVector() {
  }


  /**
   * destructor
   */
  BundleObservationVector::~BundleObservationVector() {
    qDeleteAll(*this);
    clear();
  }


  /**
   * add new BundleObservation method
   */
  BundleObservation* BundleObservationVector::addnew(BundleImage* bundleImage,
                                                     QString observationNumber,
                                                     QString instrumentId,
                                                     BundleSettings& bundleSettings) {

    BundleObservation* bundleObservation;
    bool bAddToExisting = false;

    if (bundleSettings.solveObservationMode() &&
        m_observationNumberToObservationMap.contains(observationNumber)) {
      bundleObservation = m_observationNumberToObservationMap.value(observationNumber);

      if (bundleObservation->instrumentId() == instrumentId)
        bAddToExisting = true;
    }

    if (bAddToExisting) {
      // if we have already added a BundleObservation with this number, we have to add the new
      // BundleImage to this observation
      bundleObservation->append(bundleImage);

      bundleImage->setParentObservation(bundleObservation);

      // update observation number to observation ptr map
      m_observationNumberToObservationMap.insertMulti(observationNumber,bundleObservation);

      // update image serial number to observation ptr map
      m_imageSerialToObservationMap.insertMulti(bundleImage->serialNumber(),bundleObservation);
    }
    else {
      // create new BundleObservation and append to this vector
      bundleObservation = new BundleObservation(bundleImage, observationNumber, instrumentId);

      if (!bundleObservation) {
        QString message = "unable to allocate new BundleObservation ";
        message += "for " + bundleImage->fileName();
        throw IException(IException::Programmer, message, _FILEINFO_);
      }

      bundleImage->setParentObservation(bundleObservation);

      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // TODO: problem when using settings from gui that no instrument id is set
      //
      BundleObservationSolveSettings solveSettings;
      if ( bundleSettings.numberSolveSettings() == 1) {
        solveSettings = bundleSettings.observationSolveSettings(0);
      }
      else
        solveSettings = bundleSettings.observationSolveSettings(instrumentId);
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      bundleObservation->setSolveSettings(solveSettings);

      bundleObservation->setIndex(size());

      append(bundleObservation);

      // update observation number to observation ptr map
      m_observationNumberToObservationMap.insertMulti(observationNumber,bundleObservation);

      // update image serial number to observation ptr map
      m_imageSerialToObservationMap.insertMulti(bundleImage->serialNumber(),bundleObservation);
    }

    return bundleObservation;
  }


  /**
   * TODO
   */
  int BundleObservationVector::numberPositionParameters() {
    int positionParameters = 0;

    for (int i = 0; i < size(); i++) {
      BundleObservation* observation = at(i);
      positionParameters += observation->numberPositionParameters();
    }

    return positionParameters;
  }


  /**
   * TODO
   */
  int BundleObservationVector::numberPointingParameters() {
    int pointingParameters = 0;

    for (int i = 0; i < size(); i++) {
      BundleObservation* observation = at(i);
      pointingParameters += observation->numberPointingParameters();
    }

    return pointingParameters;
  }


  /**
   * TODO
   */
  int BundleObservationVector::numberParameters() {
    return numberPositionParameters() + numberPointingParameters();
  }

  /**
   * TODO
   */
  BundleObservation* BundleObservationVector::
      getObservationByCubeSerialNumber(QString cubeSerialNumber) {

    if (m_imageSerialToObservationMap.contains(cubeSerialNumber))
      return m_imageSerialToObservationMap.value(cubeSerialNumber);

    return NULL;
  }

  /**
   * TODO
   */
  bool BundleObservationVector::initializeExteriorOrientation() {
    int nObservations = size();
    for (int i = 0; i < nObservations; i++) {
      BundleObservation *observation = at(i);
      observation->initializeExteriorOrientation();
    }

    return true;
  }

}

