#include <unistd.h>
#include "VideoTCPSocketModel.hpp"

#include <QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>


VideoTCPSocketModel::
VideoTCPSocketModel()
  : _label(new QLabel("Double click to load video")),
    _playButton(new QPushButton("Play")),
    _panel(new QWidget()),
    _panelLayout(new QGridLayout()),
    _tcpSocket(new TCPSocket())
{

  connect(_playButton, &QAbstractButton::clicked,
              this, &VideoTCPSocketModel::on_pushButton_clicked);
  connect(_tcpSocket, &TCPSocket::processedImage,
              this, &VideoTCPSocketModel::handleResults);

  // display image on _label
  _label->setParent(_panel);
  _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
  QFont f = _label->font();
  f.setBold(true);
  f.setItalic(true);
  _label->setFont(f);
  //_label->setFixedSize(200, 200);
  //_label->installEventFilter(this);

  _playButton->setParent(_panel);
  _playButton->setEnabled(true);

  _panelLayout->addWidget(_label, 1, 0);
  _panelLayout->addWidget(_playButton, 2, 0);
  _panel->setLayout(_panelLayout);
  _panel->setFixedSize(200, 200);
  _panel->installEventFilter(this);
}

VideoTCPSocketModel::
~VideoTCPSocketModel() {
    delete _tcpSocket;
}

unsigned int
VideoTCPSocketModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 0;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }

  return result;
}


bool
VideoTCPSocketModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _panel)
  {
    int w = _panel->width();
    int h = _panel->height();

    if (event->type() == QEvent::Resize)
    {
      if (!_pixmap.isNull())
        _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    }
  }

  return false;
}


NodeDataType
VideoTCPSocketModel::
dataType(PortType, PortIndex) const
{
  return PixmapData().type();
}


std::shared_ptr<NodeData>
VideoTCPSocketModel::
outData(PortIndex)
{
  return std::make_shared<PixmapData>(_pixmap);
}

void
VideoTCPSocketModel::
handleResults(const QImage &frame)
{
    int w = _panel->width();
    int h = _panel->height();
    QPixmap pixmap;
    pixmap.convertFromImage(frame);
    _pixmap = pixmap;
    _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    Q_EMIT dataUpdated(0);
}

void VideoTCPSocketModel::
on_pushButton_clicked()
{
    if (_tcpSocket->isStopped())
    {
        _tcpSocket->Play();
        _playButton->setText(tr("Stop"));
    }else
    {
        _tcpSocket->Stop();
        _playButton->setText(tr("Play"));
    }
}

/******************* VideoPlayer ********************************/

TCPSocket::TCPSocket(QObject *parent)
    : QThread(parent)
{
    stop = true;
}

void TCPSocket::Play()
{
    if (!isRunning()) {
        if (isStopped()){
            stop = false;
        }
        start(LowPriority);
    }
}

TCPSocket::~TCPSocket()
{
    mutex.lock();
    stop = true;
    close(sockfd);
    close(forClientSockfd);
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void TCPSocket::Stop()
{
    stop = true;
}

void TCPSocket::msleep(int ms){
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

bool TCPSocket::isStopped() const{
    return this->stop;
}

void TCPSocket::run()
{
    qDebug()<<"from VideoTCPSocket's sub thread slot:" <<QThread::currentThreadId();

    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        qDebug()<< "Fail to create a socket.\n";
    }

    //socket connection
    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo,sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(6789);
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
    listen(sockfd,5);

    Mat frame;
    // only handle the fixed size image 224 * 224
    frame = Mat::zeros(224 , 224, CV_8UC3);
    int imgSize = frame.total() * frame.elemSize();
    uchar *iptr = frame.data;
    int bytes = 0;

    qDebug() << "Wait for the connection....!!";
    forClientSockfd = accept(sockfd,(struct sockaddr*) &clientInfo, (socklen_t*) &addrlen);
    qDebug() << "Accept the connection....!!";
    while(!stop){
        // Capture frame-by-frame
        if ((bytes = recv(forClientSockfd, iptr, imgSize , MSG_WAITALL)) == -1) {
          qDebug() << "recv failed, received bytes = " << bytes ;
        }

        if (forClientSockfd < 0) {
          qDebug() << "accept connection failed!";
          stop = true;
        }

        // If the frame is empty, break immediately
        if (frame.empty()) {
          stop = true;
          break;
        }

        img = PixmapData::cvMatToQImage(frame);
        emit processedImage(img);

        this->msleep(25);
    }
    close(sockfd);
    close(forClientSockfd);
}
