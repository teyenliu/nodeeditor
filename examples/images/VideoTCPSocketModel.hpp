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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
class TCPSocket : public QThread
{    Q_OBJECT
 private:
    bool stop;
    QMutex mutex;
    QWaitCondition condition;
    int sockfd = 0, forClientSockfd = 0;
    QImage img;

 signals:
 //Signal to output frame to be displayed
    void processedImage(const QImage &);
 protected:
    void run();
    void msleep(int ms);

 public:
    //Constructor
    TCPSocket(QObject *parent = 0);
    //Destructor
    ~TCPSocket();
    //Play the video
    void Play();
    //Stop the video
    void Stop();
    //check if the player has been stopped
    bool isStopped() const;
};


/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class VideoTCPSocketModel : public NodeDataModel
{
  Q_OBJECT

public:
  VideoTCPSocketModel();

  virtual
  ~VideoTCPSocketModel() override;

public slots:
    void handleResults(const QImage &);
    void on_pushButton_clicked();

public:
  QString
  caption() const override
  { return QString("Video TCPSocket Source"); }

  QString
  name() const override { return QString("VideoTCPSocketModel"); }

public:
  virtual QString
  modelName() const
  { return QString("Source Video TCPSocket"); }

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

  TCPSocket * _tcpSocket;
};

