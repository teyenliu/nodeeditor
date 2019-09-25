
#include <cstdio>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

#include "ImageDDSPubModel.hpp"
#include "PixmapData.hpp"


using namespace cv;

dds::core::array<dds::core::array<HelloWorldImg::RGB,
HelloWorldImg::SIZE_HORIZONTAL>,
HelloWorldImg::SIZE_VERTICAL>
cvToidl(const cv::Mat& input_mat) {
    //create a new dds::core::array
    dds::core::array<dds::core::array<HelloWorldImg::RGB,
    HelloWorldImg::SIZE_HORIZONTAL>,
    HelloWorldImg::SIZE_VERTICAL> output_idl_img;

    int nr= HelloWorldImg::SIZE_VERTICAL; // number of rows
    int nc= HelloWorldImg::SIZE_HORIZONTAL; // number of columns
    for (int j=0; j<nr; j++) {
        for (int i=0; i<nc; i++) {
            output_idl_img[i][j].b(input_mat.at<cv::Vec3b>(j,i)[0]);
            output_idl_img[i][j].g(input_mat.at<cv::Vec3b>(j,i)[1]);
            output_idl_img[i][j].r(input_mat.at<cv::Vec3b>(j,i)[2]);
        }
    }
    return output_idl_img;
}


DDSPubObject::DDSPubObject() :
    dw(dds::core::null)
{
    /** A dds::domain::DomainParticipant is created for the default domain. */
    dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

    /** The Durability::Transient policy is specified as a dds::topic::qos::TopicQos
     * so that even if the subscriber does not join until after the sample is written
     * then the DDS will still retain the sample for it. The Reliability::Reliable
     * policy is also specified to guarantee delivery. */
    dds::topic::qos::TopicQos topicQos
         = dp.default_topic_qos()
            << dds::core::policy::Durability::Transient()
            << dds::core::policy::Reliability::Reliable();

    /** A dds::topic::Topic is created for our sample type on the domain participant. */
    dds::topic::Topic<HelloWorldImg::Image> topic(dp, "HelloWorldImg_Image", topicQos);

    /** A dds::pub::Publisher is created on the domain participant. */
    std::string name = "HelloWorld image example";
    dds::pub::qos::PublisherQos pubQos
        = dp.default_publisher_qos()
            << dds::core::policy::Partition(name);
    dds::pub::Publisher pub(dp, pubQos);

    /** The dds::pub::qos::DataWriterQos is derived from the topic qos and the
     * WriterDataLifecycle::ManuallyDisposeUnregisteredInstances policy is
     * specified as an addition. This is so the publisher can optionally be run (and
     * exit) before the subscriber. It prevents the middleware default 'clean up' of
     * the topic instance after the writer deletion, this deletion implicitly performs
     * DataWriter::unregister_instance */
    dds::pub::qos::DataWriterQos dwqos = topic.qos();
    dwqos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

    /** A dds::pub::DataWriter is created on the Publisher & Topic with the modififed Qos. */
    dw = dds::pub::DataWriter<HelloWorldImg::Image>(pub, topic, dwqos);

    qDebug() << "=== [Publisher] ImageDDSPubObject is constructed...";
}


ImageDDSPubModel::
ImageDDSPubModel() :
    _cvObj(new DDSPubObject()),
    _lineedit1(new QLineEdit())
{
  // connect task object and image model class
  connect(this, &ImageDDSPubModel::triggerTask,
              _cvObj, &DDSPubObject::runTask);

  _label->setText("Topic:");
  _lineedit1->setText("HelloWorldImg_Image");
  _lineedit1->setEnabled(false);
  _panelLayout->addWidget(_lineedit1, 1, 1);
  _panel->setFixedSize(240, 100);
  _cvObj->moveToThread(&_workerThread);
  _workerThread.start();
}

unsigned int
ImageDDSPubModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 1;
      break;

    case PortType::Out:
      result = 0;

    default:
      break;
  }

  return result;
}


bool
ImageDDSPubModel::
eventFilter(QObject *object, QEvent *event)
{
  // do nothing
  return false;
}

void
ImageDDSPubModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
  _nodeData = nodeData;

  if (_nodeData)
  {
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
    // Danny Implementation
    if(!d->pixmap().isNull())
    {
      // trigger threading job, but here we don't show the image
      //_label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
      emit triggerTask(d->pixmap());
    }
  }
  // For Publisher, it is no output port. So don't run dataUpdated()
  //Q_EMIT dataUpdated(0);
}

// implement save and restore for data flow graph
QJsonObject
ImageDDSPubModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();
  if (_lineedit1)
    modelJson["dds_pub_topic"] = _lineedit1->text();
  return modelJson;
}


void
ImageDDSPubModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["dds_pub_topic"];

  if (!v.isUndefined())
  {
    QString strNum = v.toString();

    bool ok;
    strNum.toInt(&ok);
    if (ok)
    {
      _lineedit1->setText(strNum);
    }
  }
}

void
DDSPubObject::
runTask(const QPixmap &frame)
{
  try {
    //qDebug()<<"from CannyObject's sub thread slot:" <<QThread::currentThreadId();
    Mat src = PixmapData::QPixmapToCvMat(frame);
    // resize to 224*224*3
    cv::resize(src, src, cv::Size(224, 224), 0, 0);

    // Do DDS Publish with Topic
    auto input_idl = cvToidl(src);

    // imgID, category, matrix
    HelloWorldImg::Image imgInstance(1, "danny", input_idl);

    int64 ms = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    qDebug() << "=== [Publisher] publishes at " << ms;
    dw << imgInstance;
    // print current time
    //auto current = std::chrono::system_clock::now();
    //std::time_t current_time = std::chrono::system_clock::to_time_t(current);
    //std::cout << "[Publisher] finished publish at " << std::ctime(&current_time);

    qDebug() << "=== [Publisher] writing a message containing :";
    qDebug() << "    imgID  : " << imgInstance.imgID() << "\"";
    //qDebug() << "    category  : " << imgInstance.category() << "\"";
    //qDebug() << "    matrix : \"" << imgInstance.matrix() << "\"" << std::endl;
  }
  catch (const dds::core::Exception& e)
  {
    qDebug() << "=== ERROR: Exception: " << e.what();
  }

}
