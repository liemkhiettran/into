/* This file is part of Into.
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#ifndef _PIICOLORMODELMATCHER_H
#define _PIICOLORMODELMATCHER_H

#include <PiiDefaultOperation.h>
#include <PiiMatrix.h>

/**
 * An operation that converts a color image into an intensity map.
 * Pixel values in the intensity map are proportional to the
 * likelihood of that pixel belonging to a predefined color model.
 *
 * Inputs
 * ------
 *
 * @in image - the input color image. Any color image type.
 *
 * @in model - an image out of which the color model is to be derived.
 * Any color image type.
 *
 * Outputs
 * -------
 *
 * @out image - an intensity map image indicating the likelihood of
 * each pixel belonging to the model (PiiMatrix<float>) or a
 * thresholded image (PiiMatrix<unsigned char>), if [matchingThreshold]
 * is non-zero.
 *
 */
class PiiColorModelMatcher : public PiiDefaultOperation
{
  Q_OBJECT
  /**
   * matchingThreshold description
   */
  Q_PROPERTY(double matchingThreshold READ matchingThreshold WRITE setMatchingThreshold);

  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiColorModelMatcher();

  void setMatchingThreshold(double matchingThreshold);
  double matchingThreshold() const;

protected:
  void process();

private:
  template <class T> void calculateModel(const PiiVariant& obj);
  template <class T> void matchImageToModel(const PiiVariant& obj);

  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    Data();
    PiiMatrix<double> matBaseVectors, matCenter;
    double dMatchingThreshold;
  };
  PII_D_FUNC;
};



#endif //_PIICOLORMODELMATCHER_H
