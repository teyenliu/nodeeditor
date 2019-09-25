#include <unistd.h>
#include "ImageLoaderModel.hpp"

#include <QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

ImageLoaderModel::
ImageLoaderModel()
  : _label(new QLabel("Double click to load image")),
    _checkbox(new QCheckBox("Batch Image Processing")),
    _panel(new QWidget()),
    _panelLayout(new QGridLayout())
{

  // display image on _label
  _label->setParent(_panel);
  _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
  QFont f = _label->font();
  f.setBold(true);
  f.setItalic(true);
  _label->setFont(f);
  //_label->setFixedSize(200, 200);
  //_label->installEventFilter(this);

  _checkbox->setParent(_panel);
  _checkbox->setChecked(false);
  _checkbox->setCheckable(true);
  _checkbox->setEnabled(true);

  _panelLayout->addWidget(_label, 1, 0);
  _panelLayout->addWidget(_checkbox, 2, 0);
  _panel->setLayout(_panelLayout);
  _panel->setFixedSize(200, 200);
  _panel->installEventFilter(this);

}


unsigned int
ImageLoaderModel::
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
ImageLoaderModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _panel)
  {
    int w = _panel->width();
    int h = _panel->height();

    if (event->type() == QEvent::MouseButtonPress)
    {

      if (!_checkbox->isChecked())
      {
        QString fileName =
          QFileDialog::getOpenFileName(nullptr,
                                     tr("Open Image"),
                                     QDir::homePath(),
                                     tr("Image Files (*.png *.jpg *.bmp)"));

        _pixmap = QPixmap(fileName);
        _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
        Q_EMIT dataUpdated(0);
      }
      else
      {
        // Batch ImageLoader
        QString img_dir =
          QFileDialog::getExistingDirectory(nullptr,
                                       tr("Open Image path"),
                                       QDir::homePath(),
                                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        QDirIterator it(img_dir, QStringList() << "*.png", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
          QString filename = it.next();
          qDebug() << "image name:" << filename.toUtf8().constData();
          _pixmap = QPixmap(filename);
          _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
          // to avoid from freezing GUI
          QCoreApplication::processEvents();
          Q_EMIT dataUpdated(0);
        }
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
ImageLoaderModel::
dataType(PortType, PortIndex) const
{
  return PixmapData().type();
}


std::shared_ptr<NodeData>
ImageLoaderModel::
outData(PortIndex)
{
  return std::make_shared<PixmapData>(_pixmap);
}
