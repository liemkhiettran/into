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

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
  DemoMainWindow(parent),
  _pSourceProbeInput(0),
  _pResultProbeInput(0),
  _pImageScaleOperation(0),
  _pImageRotationOperation(0)
{
  // Initialize engine
  initEngine();

  // Initialize ui
  init();

  // Start processing
  startProcessing();
  emit updateImage(0);
}

void MainWindow::init()
{
  setupUi(this);
  setWindowTitle(demoName());

  // Make ui-connections
  connect(_pScaleSlider, SIGNAL(valueChanged(int)), this, SLOT(scaleChanged(int)));
  connect(_pRotationSlider, SIGNAL(valueChanged(int)), this, SLOT(rotationChanged(int)));

  connect(_pSourceProbeInput, SIGNAL(objectReceived(PiiVariant)), _pSourceImageDisplay, SLOT(setImage(PiiVariant)));
  connect(_pResultProbeInput, SIGNAL(objectReceived(PiiVariant)), _pResultImageDisplay, SLOT(setImage(PiiVariant)));

  _updateTimer.setSingleShot(true);
  _updateTimer.setInterval(10);
  connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(updateValues()));
}

PiiEngine* MainWindow::createEngine()
{
  // Create engine
  PiiEngine *pEngine = new PiiEngine;

  // Create trigger source
  PiiOperation *pTriggerSource = pEngine->createOperation("PiiTriggerSource");

  // Create switch
  PiiOperation *pSwitch = pEngine->createOperation("PiiSwitch");
  pSwitch->setProperty("operationMode", "AsynchronousMode");
  pSwitch->setProperty("dynamicInputCount", 1);
  pSwitch->setProperty("passThrough", true);

  // Create probe input for source and result image display
  _pSourceProbeInput = new PiiProbeInput;
  _pResultProbeInput = new PiiProbeInput;

  // Create image source
  PiiOperation *pImageFileReader = pEngine->createOperation("PiiImageFileReader");
  pImageFileReader->setProperty("fileNames", QString("%1/olga.jpg").arg(defaultImageDirPath()));
  pImageFileReader->setProperty("repeatCount", 1);

  // Create scale operation
  _pImageScaleOperation = pEngine->createOperation("PiiImageScaleOperation");
  _pImageScaleOperation->setProperty("scaleMode", "ScaleAccordingToFactor");

  // Create rotation operation
  _pImageRotationOperation = pEngine->createOperation("PiiImageRotationOperation");
  _pImageRotationOperation->setProperty("transformedSize", "ExpandAsNecessary");
  _pImageRotationOperation->setProperty("backgroundColor", QColor(Qt::white));

  // Make operation connections
  connect(this, SIGNAL(updateImage(int)), pTriggerSource, SLOT(trigger(int)));

  pImageFileReader->connectOutput("image", pSwitch, "input0");
  pTriggerSource->connectOutput("trigger", pSwitch, "trigger");
  pSwitch->connectOutput("output0", _pImageScaleOperation, "image");
  _pImageScaleOperation->connectOutput("image", _pImageRotationOperation, "image");

  _pSourceProbeInput->connectOutput(pSwitch->output("output0"));
  _pResultProbeInput->connectOutput(_pImageRotationOperation->output("image"));

  return pEngine;
}

void MainWindow::scaleChanged(int value)
{
  _updateTimer.start();

  showSliderValue(_pScaleSlider, QString::number((double)value/100.0,'f',2));
}

void MainWindow::rotationChanged(int value)
{
  _updateTimer.start();

  showSliderValue(_pRotationSlider, QString::number(value));

}

void MainWindow::updateValues()
{
  // Change scale ratio
  _pImageScaleOperation->setProperty("scaleRatio", (double)_pScaleSlider->value() / 100.0);

  // Change rotation angle
  _pImageRotationOperation->setProperty("angleDeg", _pRotationSlider->value());

  // Trig the same image again
  emit updateImage(0);

}
