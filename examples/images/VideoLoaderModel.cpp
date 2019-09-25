#include <unistd.h>
#include "VideoLoaderModel.hpp"

#include <QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtWidgets>
#include <QtWidgets/QFileDialog>


VideoLoaderModel::
VideoLoaderModel()
  : _mediaPlayer(0, QMediaPlayer::VideoSurface),
    _label(new QLabel()),
    _playButton(new QPushButton("Play")),
    _positionSlider(new QSlider(Qt::Horizontal)),
    _panel(new QWidget()),
    _panelLayout(new QGridLayout()),
    _grabber(new VideoFrameGrabber(_panel)),
    _frame_count(0)
{

  connect(_playButton, &QAbstractButton::clicked,
          this, &VideoLoaderModel::play);

  _positionSlider->setRange(0, 0);
  connect(_positionSlider, &QAbstractSlider::sliderMoved,
          this, &VideoLoaderModel::setPosition);

  // display image on _label
  _label->setParent(_panel);
  QFont f = _label->font();
  f.setBold(true);
  f.setItalic(true);
  _label->setFont(f);
  //_label->setFixedSize(200, 200);
  //_label->installEventFilter(this);

  _panelLayout->addWidget(_label, 1, 0);
  _panelLayout->addWidget(_playButton, 2, 0);
  _panelLayout->addWidget(_positionSlider, 3, 0);
  _panel->setLayout(_panelLayout);
  _panel->setFixedSize(200, 200);
  _panel->installEventFilter(this);

  // Danny implementation
  _mediaPlayer.setVideoOutput(_grabber);
  connect(_grabber, SIGNAL(frameAvailable(QImage)), this, SLOT(processFrame(QImage)));

  typedef void (QMediaPlayer::*ErrorSignal)(QMediaPlayer::Error);
  connect(&_mediaPlayer, static_cast<ErrorSignal>(&QMediaPlayer::error),
          this, &VideoLoaderModel::handleError);
}


unsigned int
VideoLoaderModel::
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


void
VideoLoaderModel::
processFrame(QImage frame)
{
    _frame_count++;
    // To reduce the processing of frames by 1/3
    if (_frame_count % 5 != 0)
        return;
    int w = _label->width();
    int h = _label->height();

    QPixmap pixmap;
    pixmap.convertFromImage(frame);
    _pixmap = pixmap;
    _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    Q_EMIT dataUpdated(0);
}


void
VideoLoaderModel::
play()
{
    switch(_mediaPlayer.state()) {
    case QMediaPlayer::PlayingState:
        _mediaPlayer.pause();
        break;
    default:
        _mediaPlayer.play();
        break;
    }
}

void
VideoLoaderModel::
setUrl(const QUrl &url)
{
    url.isLocalFile() ? url.toLocalFile() : QString();
    _mediaPlayer.setMedia(url);
    qDebug() << "URL:" << url;
    _playButton->setEnabled(true);
}


void
VideoLoaderModel::
setPosition(int position)
{
    _mediaPlayer.setPosition(position);
}


bool
VideoLoaderModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _panel)
  {
    int w = _panel->width();
    int h = _panel->height();

    if (event->type() == QEvent::MouseButtonPress)
    {
      {
        QFileDialog fileDialog(nullptr);
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog.setWindowTitle(tr("Open Movie"));
        QStringList supportedMimeTypes = _mediaPlayer.supportedMimeTypes();
        if (!supportedMimeTypes.isEmpty())
            fileDialog.setMimeTypeFilters(supportedMimeTypes);
        fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
        if (fileDialog.exec() == QDialog::Accepted)
            setUrl(fileDialog.selectedUrls().constFirst());
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
VideoLoaderModel::
dataType(PortType, PortIndex) const
{
  return PixmapData().type();
}


std::shared_ptr<NodeData>
VideoLoaderModel::
outData(PortIndex)
{
  return std::make_shared<PixmapData>(_pixmap);
}

void
VideoLoaderModel::
handleError()
{
    _playButton->setEnabled(false);
    const QString errorString = _mediaPlayer.errorString();
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(_mediaPlayer.error()));
    else
        message += errorString;
    _label->setText(message);
}
