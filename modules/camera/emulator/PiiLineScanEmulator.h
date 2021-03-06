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

#ifndef _PIILINESCANEMULATOR_H
#define _PIILINESCANEMULATOR_H

#include <PiiCameraDriver.h>
#include <PiiWaitCondition.h>
#include <PiiCameraEmulatorGlobal.h>
#include <PiiTextureGenerator.h>

#include <QThread>
#include <QMutex>

class PII_CAMERAEMULATOR_EXPORT PiiLineScanEmulator : public PiiCameraDriver
{
  Q_OBJECT

  Q_PROPERTY(double inputPulseFrequency READ inputPulseFrequency WRITE setInputPulseFrequency);
  Q_PROPERTY(double triggerRate READ triggerRate WRITE setTriggerRate);

  /**
   * The size of a frame.
   */
  Q_PROPERTY(QSize frameSize READ frameSize WRITE setFrameSize);

  /**
   * The rect of the frame.
   */
  Q_PROPERTY(QRect frameRect READ frameRect WRITE setFrameRect);

  /**
   * This property tells how many frames we will keep on buffer.
   */
  Q_PROPERTY(int frameBufferCount READ frameBufferCount WRITE setFrameBufferCount);

  /**
   * The name of the class that produces background for the web. If
   * none is set, the background will be painted with
   * [backgroundColor]. The generator must be registered to the object
   * registry.
   *
   * ~~~(c++)
   * emulator->setProperty("textureGeneratorName", "PiiNonWovenGenerator");
   * ~~~
   */
  Q_PROPERTY(QString textureGeneratorName READ textureGeneratorName WRITE setTextureGeneratorName STORED false);

  /**
   * File name pattern for defect images.
   */
  Q_PROPERTY(QString defectImagePattern READ defectImagePattern WRITE setDefectImagePattern);

  /**
   * The fraction of deficient surface area. 1.0 means that the whole
   * surface will be faulty. 0 means there will be no defects.
   */
  Q_PROPERTY(double defectProbability READ defectProbability WRITE setDefectProbability);

  /**
   * The color of the material's background. Only the red channel is
   * used with gray-level images.
   */
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor);

  /**
   * The field of view of the camera's lens in degrees. If this value
   * is set to a non-zero value, the emulator will simulate optical
   * vignetting with the \(cos^4(\theta)\) law.
   */
  Q_PROPERTY(double fieldOfView READ fieldOfView WRITE setFieldOfView);

  /**
   * The maximum distance left edge is allowed to deviate from the
   * left image border, in pixels. The default value is zero, which
   * disables generation of the left edge.
   */
  Q_PROPERTY(int leftEdgeLimit READ leftEdgeLimit WRITE setLeftEdgeLimit);

  /**
   * The maximum distance right edge is allowed to deviate from the
   * right image border, in pixels. The default value is zero, which
   * disables generation of the right edge.
   */
  Q_PROPERTY(int rightEdgeLimit READ rightEdgeLimit WRITE setRightEdgeLimit);

  /**
   * The number of pixels the edge is allowed to fluctuate between
   * successive scan lines. The default value is 0.1.
   */
  Q_PROPERTY(double edgeElasticity READ edgeElasticity WRITE setEdgeElasticity);

  /**
   * The color of image borders, if edge generation is turned on. Only
   * the red channel is used with gray-level images. The default value
   * is black.
   */
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor);

  /**
   * The number of scan-lines the texture generator is requested to
   * generate in each batch. The default value is 128. It is seldom
   * necessary to change this value, but make sure it is larger than
   * [bufferHeight].
   */
  Q_PROPERTY(int textureBlockSize READ textureBlockSize WRITE setTextureBlockSize);

  /**
   * gain description
   */
  Q_PROPERTY(double gain READ gain WRITE setGain);

  /**
   * maxResolution description
   */
  Q_PROPERTY(QSize maxResolution READ resolution WRITE setMaxResolution);

  /**
   *exposureTime
   */
  Q_PROPERTY(int exposureTime READ exposureTime WRITE setExposureTime);

  /**
   *exposureTime
   */
  Q_PROPERTY(int baseExposureTime READ baseExposureTime WRITE setBaseExposureTime);

  Q_PROPERTY(int maxHeight READ maxHeight);

  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned int)
  {
    PII_SERIALIZE_BASE(archive, PiiCameraDriver);
    PiiSerialization::serializeProperties(archive, *this);
    archive & PII_NVP("generator", _pTextureGenerator);
    if (Archive::InputArchive && _pTextureGenerator)
      _pTextureGenerator->setParent(this);
  }
public:
  /**
   * Construct a new PiiLineScanEmulator.
   */
  PiiLineScanEmulator();

  /**
   * Destroy the PiiLineScanEmulator.
   */
  ~PiiLineScanEmulator();

  QVariant property(const char* name) const;
  bool setProperty(const char* name, const QVariant& value);


  QStringList cameraList() const;
  int cameraType() const { return (int)PiiCamera::LineScan; }
  void initialize(const QString& cameraId);
  bool close();
  bool startCapture(int frames);
  bool stopCapture();
  void* frameBuffer(uint frameIndex) const;
  bool isOpen() const;
  bool isCapturing() const;
  bool triggerImage();
  bool setTriggerMode(PiiCameraDriver::TriggerMode mode);
  PiiCameraDriver::TriggerMode triggerMode() const;
  int bitsPerPixel() const { return 8; }
  int imageFormat() const { return PiiCamera::MonoFormat; }
  bool setImageFormat(int /*format*/) { return true; }
  QSize resolution() const { return _resolution; }

  // Properties
  bool setInputPulseFrequency(double freq);
  bool setTriggerRate(double triggerRate);
  bool setFrameSize(const QSize& frameSize);
  bool setFrameRect(const QRect& frameRect);
  bool setFrameBufferCount(int frameBufferCount);
  bool setDefectImagePattern(const QString& defectImagePattern) { _strDefectImagePattern = defectImagePattern; return true; }
  bool setDefectProbability(double defectProbability)
  {
    _dDefectProbability = defectProbability;
    _dTempProbability = defectProbability;
    return true;
  }
  bool setBackgroundColor(const QColor& backgroundColor) { _backgroundColor = backgroundColor; return true; }
  bool setFieldOfView(double fieldOfView) { _dFieldOfView = fieldOfView; return true; }
  bool setBorderColor(QColor borderColor) { _borderColor = borderColor; return true; }
  bool setEdgeElasticity(double edgeElasticity) { _dEdgeElasticity = edgeElasticity; return true; }
  bool setLeftEdgeLimit(int leftEdgeLimit) { _iLeftEdgeLimit = leftEdgeLimit; return true; }
  bool setRightEdgeLimit(int rightEdgeLimit) { _iRightEdgeLimit = rightEdgeLimit; return true; }
  bool setTextureGeneratorName(const QString& textureGeneratorName);
  bool setTextureBlockSize(int textureBlockSize) { _iTextureBlockSize = textureBlockSize; return true; }
  bool setGain(double gain) { _dGain = qBound(0.0,gain,1.0); return true; }
  bool setExposureTime(int exposureTime) { _iExposureTime = qMax(1,exposureTime); return true; }
  bool setBaseExposureTime(int baseExposureTime) { _iBaseExposureTime = qMax(1,baseExposureTime); return true; }
  void setMaxResolution(const QSize& resolution);

  double inputPulseFrequency() const { return _dInputPulseFrequency; }
  double triggerRate() const { return _dTriggerRate; }
  QSize frameSize() const { return QSize(_iWidth, _iHeight); }
  QRect frameRect() const { return QRect(_iOffsetX, _iOffsetY, _iWidth, _iHeight); }
  int frameBufferCount() const { return _iFrameBufferCount; }
  QString defectImagePattern() const { return _strDefectImagePattern; }
  double defectProbability() const { return _dDefectProbability; }
  QColor backgroundColor() const { return _backgroundColor; }
  double fieldOfView() const { return _dFieldOfView; }
  QColor borderColor() const { return _borderColor; }
  double edgeElasticity() const { return _dEdgeElasticity; }
  int leftEdgeLimit() const { return _iLeftEdgeLimit; }
  int rightEdgeLimit() const { return _iRightEdgeLimit; }
  QString textureGeneratorName() const;
  int textureBlockSize() const { return _iTextureBlockSize; }
  double gain() const { return _dGain; }
  int exposureTime() const { return _iExposureTime; }
  int baseExposureTime() const { return _iBaseExposureTime; }
  int maxHeight() const { return _resolution.height(); }

protected:
  bool requiresInitialization(const char* name) const;

private:
  void buffer();
  void capture();
  void stopBuffering();
  void stopCapturing();
  void releaseFrames(int start = 0, int end = -1);

  QStringList _lstCriticalProperties;
  bool _bOpen, _bCapturingRunning, _bBufferingRunning;

  int _iSkippingLimit, _iFrameSizeInBytes;
  PiiMatrix<unsigned char> _frameBuffer;
  QVector<unsigned char*> _vecBufferPointers;

  QThread *_pBufferingThread, *_pCapturingThread;
  QMutex _frameBufMutex;
  quint64 _iFrameIndex, _iLastHandledFrame;
  int _iMaxFrames, _iHandledFrameCount;
  PiiWaitCondition _frameWaitCondition, _triggerWaitCondition;
  PiiCameraDriver::TriggerMode _triggerMode;

  QSize _resolution;
  int _iWidth, _iHeight, _iOffsetX, _iOffsetY;
  int _iFrameBufferCount, _iFrameBufferHeight;
  QString _strDefectImagePattern;
  double _dDefectProbability;
  QColor _backgroundColor;
  double _dFieldOfView;
  QColor _borderColor;
  double _dEdgeElasticity;
  int _iLeftEdgeLimit;
  int _iRightEdgeLimit;
  int _iTextureBlockSize;
  double _dGain;
  int _iExposureTime;
  int _iBaseExposureTime;
  double _dTriggerRate, _dInputPulseFrequency, _dOutputPulseFrequency;

  // Is this the first scan-line?
  bool _bFirstScanLine;

  QVariantMap _mapGeneratorProperties;
  PiiTextureGenerator* _pTextureGenerator;

  bool loadImages();
  void lineAdded();

  // Counts the total amounts of lines (== frames) after the
  // startCapture call
  int _iLineCounter;
  // Counts the lines (== frames) after the last
  // Controller::frameCaptured call.
  int _iCurrentLineIndex;


  void setImages(const QList<QImage>& images, const QList<QImage>& alphaMasks);

  QList<QImage> _lstImages;
  QList<QImage> _lstAlphaMasks;
  QList<int> _lstCurrDefImages; // List of indices to current defects images
  int _iCurrLineInImage;
  // The following list contains coordinates used in drawing the
  // defects. The x-coordinate contains the x coordinate of the defect
  // image relative to the buffer. The y coordinate contains the
  // current row being drawn of the defect image. The coordinate is
  // stored relative to the defect image. The x coordinate remains
  // same until the whole defect has been drawn. The y coordinate is
  // incremented after a line has been drawn in a buffer. The y
  // coordinates starts always from 0.
  QList<QPoint> _lstDefectImageCoords;
  /* The propability, which is used in deciding, whether a new defect
     image is added or not. The value is increased ar decreased
     depenging, which is the current defect rate.*/
  double _dTempProbability;
  /* Current calculated defect rate.*/
  double _dTotalDefectRate;
  /* Counts the number of all lines generated. Is used in calculating the
     total defect rate (_dTotalDefectRate)*/
  int _iTotalLineCounter;
  double* _dpMultipliers;
  double _dLeftEdgePos, _dRightEdgePos;
  QPoint _leftTargetPoint, _rightTargetPoint;

  bool newDefect();
  int getRandomImage();
  QPoint getRandomCoord(const QImage& image);
  void generateLine();
  void generateTexture();
  void updateTotalDefRate(double currRowDefRate);
  void generateMultipliers();
  double updateEdgePos(double pos, QPoint& targetPoint, int limit);
};

#endif //_PIILINESCANEMULATOR_H
