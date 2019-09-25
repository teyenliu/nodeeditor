#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QDir>
#include <QStringList>
#include <QDirIterator>
#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <QWidget>
#include <QMediaPlayer>

#include "PixmapData.hpp"
#include "VideoFrameGrabber.hpp"


using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class VideoLoaderModel : public NodeDataModel
{
  Q_OBJECT

public:
  VideoLoaderModel();

  virtual
  ~VideoLoaderModel() {}

public:

  QString
  caption() const override
  { return QString("Video Source"); }

  QString
  name() const override { return QString("VideoLoaderModel"); }

public:

  virtual QString
  modelName() const
  { return QString("Source Video"); }

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData>, int) override
  { }

  QWidget *
  embeddedWidget() override { return _panel; }

  bool
  resizable() const override { return true; }

public slots:
    void setUrl(const QUrl &url);
    void play();
    void processFrame(QImage frame);

private slots:
    void handleError();
    void setPosition(int position);

protected:
  bool
  eventFilter(QObject *object, QEvent *event) override;

private:

  QLabel *_label;
  QAbstractButton *_playButton;
  QWidget * _panel;
  QGridLayout* _panelLayout;
  QSlider *_positionSlider;
  QMediaPlayer _mediaPlayer;
  VideoFrameGrabber *_grabber;
  QPixmap _pixmap;
  int _frame_count;
};
