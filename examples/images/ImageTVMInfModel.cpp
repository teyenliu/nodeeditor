
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "ImageTVMInfModel.hpp"
#include "PixmapData.hpp"


using namespace cv;


ImageTVMInfModel::
ImageTVMInfModel() :
    _tvmObj(new TVMObject())
{
    // connect task object and image model class
    connect(this, &ImageTVMInfModel::triggerTask,
                _tvmObj, &TVMObject::runTask);
    connect(_tvmObj, &TVMObject::sendResults,
                this, &ImageTVMInfModel::handleResults);

    _tvmObj->moveToThread(&_workerThread);
    _workerThread.start();

    // To change font size in QLabel
    QFont f = _label->font();
    f.setPixelSize(20);
    _label->setFont(f);
    _label->setAlignment(Qt::AlignLeft);
}

void
ImageTVMInfModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
  _nodeData = nodeData;

  if (_nodeData)
  {
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);

    // Danny Implementation
    if(!d->pixmap().isNull())
    {
        // trigger threading job to do tensorflow inference task
        emit triggerTask(d->pixmap());
    }
  }
  else
  {
    _label->setPixmap(QPixmap());
    _label->setText("No image for inferencing");
  }

  Q_EMIT dataUpdated(0);
}

bool
ImageTVMInfModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _panel)
  {
      int w = _panel->width();
      int h = _panel->height();

    if (event->type() == QEvent::Resize)
    {
      auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
      if (d)
      {
        _label->setText(_label_result);
        _label->setFixedSize(w, h);
      }
    }
  }

  return false;
}


void
ImageTVMInfModel::
handleResults(const QString &result)
{
    int w = _panel->width();
    int h = _panel->height();
    _label_result = result;
    _label->setText("Inference:\n" + _label_result);
    _label->setFixedSize(w, h);
    Q_EMIT dataUpdated(0);
}

void
TVMObject::
runTask(const QPixmap &frame)
{
    //qDebug()<<"from TVMObject's sub thread slot:" <<QThread::currentThreadId();
    Mat src = PixmapData::QPixmapToCvMat(frame);
    Mat input;
    Mat input_final;

    cv::cvtColor(src, input, cv::COLOR_BGR2RGB);
    // resize to 224*224*3
    cv::resize(input, input_final, cv::Size(224, 224), 0, 0);

    float data[224 * 224 * 3];
    PixmapData::Mat_to_CHW(data, input_final, 224, 224);

    // copy data from Mat
    TVMArrayCopyFromBytes(x, data, 3*224*224*sizeof(float));

    struct timeval  tv1,tv2;
    gettimeofday(&tv1, NULL);
    // TVM do the inference
    run();
    get_output(0, y);
    gettimeofday(&tv2, NULL);
    float diff = PixmapData::getElapse(&tv1,&tv2);
    qDebug() << "[TVM] Mobilenet_V2 costs " << diff << " fms";


    // get the maximum position in output vector
    float ret_data[1000];
    TVMArrayCopyToBytes(y, ret_data, 1000 * sizeof(float));
    auto max_iter = std::max_element(ret_data, ret_data + 1000);
    auto max_index = std::distance(ret_data, max_iter);
    //auto y_iter = static_cast<float*>(y->data);
    //auto max_iter = std::max_element(y_iter, y_iter + 1001);
    //auto max_index = std::distance(y_iter, max_iter);
    std::cout << "The maximum position in output vector is: " << max_index << std::endl;

    // print out the top 3
    auto result = static_cast<float*>(y->data);
    for (int i = 0; i < 3; i++)
        qDebug() << result[i] << endl;

    QString str_result = QString::number(max_index) ;
    emit sendResults(str_result);
}
