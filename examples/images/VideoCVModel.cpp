#include <unistd.h>
#include "VideoCVModel.hpp"

#include <QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>


VideoCVModel::
VideoCVModel()
  : _label(new QLabel("Double click to load video")),
    _playButton(new QPushButton("Play")),
    _panel(new QWidget()),
    _panelLayout(new QGridLayout()),
    _myPlayer(new VideoPlayer())
{

  connect(_playButton, &QAbstractButton::clicked,
              this, &VideoCVModel::on_pushButton_clicked);
  connect(_myPlayer, &VideoPlayer::processedImage,
              this, &VideoCVModel::handleResults);

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
  _playButton->setEnabled(false);

  _panelLayout->addWidget(_label, 1, 0);
  _panelLayout->addWidget(_playButton, 2, 0);
  _panel->setLayout(_panelLayout);
  _panel->setFixedSize(200, 200);
  _panel->installEventFilter(this);
}

VideoCVModel::
~VideoCVModel() {
    delete _myPlayer;;
}

unsigned int
VideoCVModel::
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
VideoCVModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _panel)
  {
    int w = _panel->width();
    int h = _panel->height();

    if (event->type() == QEvent::MouseButtonPress)
    {
      {
        QString fileName =
          QFileDialog::getOpenFileName(nullptr,
                                     tr("Open Video"),
                                     QDir::homePath(),
                                     tr("Video Files (*.mov *.avi *.mp4)"));
        // set the url to worker thread
        qDebug() << "URL:" << fileName;
        _playButton->setEnabled(true);
        _myPlayer->loadVideo(fileName.toUtf8().data());
      }
      return true;
    }
    else if (event->type() == QEvent::Resize)
    {
      if (!_pixmap.isNull())
        _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    }
  }

  return false;
}


NodeDataType
VideoCVModel::
dataType(PortType, PortIndex) const
{
  return PixmapData().type();
}


std::shared_ptr<NodeData>
VideoCVModel::
outData(PortIndex)
{
  return std::make_shared<PixmapData>(_pixmap);
}

void
VideoCVModel::
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

void VideoCVModel::
on_pushButton_clicked()
{
    if (_myPlayer->isStopped())
    {
        _myPlayer->Play();
        _playButton->setText(tr("Stop"));
    }else
    {
        _myPlayer->Stop();
        _playButton->setText(tr("Play"));
    }
}

/******************* VideoPlayer ********************************/

VideoPlayer::VideoPlayer(QObject *parent)
    : QThread(parent)
{
    stop = true;
}

bool VideoPlayer::loadVideo(string filename) {
    capture.open(filename);
    if (capture.isOpened())
    {
        frameRate = (int) capture.get(CAP_PROP_FPS);
        return true;
    }
    else
        return false;
}

void VideoPlayer::Play()
{
    if (!isRunning()) {
        if (isStopped()){
            stop = false;
        }
        start(LowPriority);
    }
}

VideoPlayer::~VideoPlayer()
{
    mutex.lock();
    stop = true;
    capture.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void VideoPlayer::Stop()
{
    stop = true;
}
void VideoPlayer::msleep(int ms){
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

bool VideoPlayer::isStopped() const{
    return this->stop;
}

void VideoPlayer::run()
{
    qDebug()<<"from VideoPlayer's sub thread slot:" <<QThread::currentThreadId();

    int delay = (1000/frameRate);
    frameCount = 0;
    // skip 1 frame per frameSkipping number, exp: 3
    frameSkipping = 3;
    while(!stop){
        this->msleep(delay);
        if (!capture.read(frame))
        {
            stop = true;
        }
        //if((++frameCount) % frameSkipping != 0)
        //{
        //    // we skip this frame
        //    continue;
        //} else
        //{
        //    frameCount = 0;
        //}
        if(frame.empty())
            continue;

        if (frame.channels()== 3){
            cv::cvtColor(frame, RGBframe, cv::COLOR_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                         RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
        }
        else
        {
            img = QImage((const unsigned char*)(frame.data),
                         frame.cols,frame.rows,QImage::Format_Indexed8);
        }
        emit processedImage(img);
    }
}
