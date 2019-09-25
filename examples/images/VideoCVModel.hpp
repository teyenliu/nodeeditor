#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QDir>
#include <QtWidgets>
#include <QAbstractButton>
#include <QStringList>
#include <QDirIterator>
#include <QApplication>
#include <QGridLayout>

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include "PixmapData.hpp"


using namespace std;
using namespace cv;

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;


/******************* VideoPlayer ********************************/
class VideoPlayer : public QThread
{    Q_OBJECT
 private:
    bool stop;
    QMutex mutex;
    QWaitCondition condition;
    Mat frame;
    int frameRate;
    VideoCapture capture;
    Mat RGBframe;
    QImage img;
    int frameSkipping = 3;
    int frameCount = 0;
 signals:
 //Signal to output frame to be displayed
    void processedImage(const QImage &);
 protected:
    void run();
    void msleep(int ms);
 public:
    //Constructor
    VideoPlayer(QObject *parent = 0);
    //Destructor
    ~VideoPlayer();
    //Load a video from memory
    bool loadVideo(string filename);
    //Play the video
    void Play();
    //Stop the video
    void Stop();
    //check if the player has been stopped
    bool isStopped() const;
};


/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class VideoCVModel : public NodeDataModel
{
  Q_OBJECT

public:
  VideoCVModel();

  virtual
  ~VideoCVModel() override;

public slots:
    void handleResults(const QImage &);
    void on_pushButton_clicked();

public:
  QString
  caption() const override
  { return QString("Video CV Source"); }

  QString
  name() const override { return QString("VideoCVModel"); }

public:
  virtual QString
  modelName() const
  { return QString("Source Video CV"); }

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

protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

private:
  QLabel * _label;
  QAbstractButton *_playButton;
  QWidget * _panel;
  QGridLayout* _panelLayout;
  QPixmap _pixmap;

  VideoPlayer * _myPlayer;
};

