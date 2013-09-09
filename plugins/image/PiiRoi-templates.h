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

#ifndef _PIIROI_H
# error "Never use <PiiRoi-templates.h> directly; include <PiiRoi.h> instead."
#endif

#include "PiiImage.h"

namespace PiiImage
{
  template <class T> PiiMatrix<bool> alphaToMask(const PiiMatrix<PiiColor4<T> >& image)
  {
    const int iRows = image.rows(), iColumns = image.columns();
    PiiMatrix<bool> matMask(PiiMatrix<bool>::uninitialized(iRows, iColumns));
    for (int r=0; r<iRows; ++r)
      {
        const PiiColor4<T>* pSourceRow = image[r];
        bool* pTargetRow = matMask[r];
        for (int c=0; c<iColumns; ++c)
          pTargetRow[c] = bool(pSourceRow[c].argbA);
      }
    return matMask;
  }
  
  template <class T, class Processor> void handleRoiInput(PiiInputSocket* input,
                                                          RoiType roiType,
                                                          const PiiMatrix<T>& image,
                                                          Processor& process)
  {
    if (roiType == NoRoi)
      {
        process(image);
        return;
      }
    
    if (!input->isConnected())
      {
        // Special case: mask type ROI and no separate ROI object. If
        // the input image is a four-channel color image, the alpha
        // channel will be used as the ROI.
        if (roiType == MaskRoi)
          {
            if (Pii::IsColor4<T>::boolValue)
              {
                process(image, AlphaRoi<T>(image));
                return;
              }
          }
        process(image);
        return;
      }
    
    const int iRows = image.rows(), iColumns = image.columns();
    const PiiVariant& varRoi = input->firstObject();
    if (roiType == AutoRoi)
      {
        if (varRoi.type() == PiiYdin::IntMatrixType)
          roiType = RectangleRoi;
        else
          roiType = MaskRoi;
      }
    
    if (roiType == RectangleRoi)
      {
        if (varRoi.type() != PiiYdin::IntMatrixType)
          PII_THROW_UNKNOWN_TYPE(input);
        const PiiMatrix<int>& matRectangles = varRoi.valueAs<PiiMatrix<int> >();
        if (matRectangles.columns() != 4 || matRectangles.rows() < 1)
          PII_THROW_WRONG_SIZE(input, matRectangles, "1-N", 4);
        if (process.acceptsManyRegions() && !overlapping(matRectangles))
          {
            for (int r=0; r<matRectangles.rows(); ++r)
              {
                const PiiRectangle<int>& rect = matRectangles.rowAs<PiiRectangle<int> >(r);
                if (rect.x >=0 && rect.x < iColumns && rect.y >=0 && rect.y < iRows &&
                    rect.width > 0 && rect.x + rect.width <= iColumns &&
                    rect.height > 0 && rect.y + rect.height <= iRows)
                  process(image(rect.y, rect.x, rect.height, rect.width));
                else
                  PII_THROW(PiiExecutionException,
                            QCoreApplication::translate("PiiRoi", roiRectangleSizeError)
                            .arg(rect.x).arg(rect.y).arg(rect.width).arg(rect.height)
                            .arg(image.columns()).arg(image.rows()));
              }
          }
        else
          process(image, createRoiMask(iRows, iColumns, matRectangles));
      }
    else // roiType = MaskRoi
      {
        PiiMatrix<bool> matMask(toRoiMask(varRoi));
        if (matMask.rows() != image.rows() || matMask.columns() != image.columns())
          PII_THROW(PiiExecutionException,
                    QCoreApplication::translate("PiiRoi", roiMaskSizeError)
                    .arg(matMask.columns()).arg(matMask.rows())
                    .arg(image.columns()).arg(image.rows()));
        process(image, matMask);
      }
  }
}