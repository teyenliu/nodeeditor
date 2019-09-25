
#include <cstdio>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

#include "ImageDDSSubModel.hpp"
#include "PixmapData.hpp"

using namespace cv;

cv::Mat
idlTocv(const dds::core::array<dds::core::array<HelloWorldImg::RGB,
        HelloWorldImg::SIZE_HORIZONTAL>,
        HelloWorldImg::SIZE_VERTICAL> & input_idl){

    cv::Mat output_mat(HelloWorldImg::SIZE_VERTICAL,
                       HelloWorldImg::SIZE_HORIZONTAL,
                       CV_8UC3);
    int nr= HelloWorldImg::SIZE_VERTICAL; // number of rows
    int nc= HelloWorldImg::SIZE_HORIZONTAL; // number of columns
    for (int j=0; j<nr; j++) {
        for (int i=0; i<nc; i++) {
            output_mat.at<cv::Vec3b>(j,i)[0] = input_idl[i][j].b();
            output_mat.at<cv::Vec3b>(j,i)[1] = input_idl[i][j].g();
            output_mat.at<cv::Vec3b>(j,i)[2] = input_idl[i][j].r();
        }
    }
    return output_mat;
}

DDSSubObject::DDSSubObject(QString const &str_filter) :
    dr(dds::core::null)
{
    /** A domain participant and topic are created identically as in
     the ::publisher */
    dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());
    dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                                                << dds::core::policy::Durability::Transient()
                                                << dds::core::policy::Reliability::Reliable();
    dds::topic::Topic<HelloWorldImg::Image> topic(dp, "HelloWorldImg_Image", topicQos);

    /** A dds::sub::Subscriber is created on the domain participant. */
    std::string name = "HelloWorld image example";
    dds::sub::qos::SubscriberQos subQos
        = dp.default_subscriber_qos()
            << dds::core::policy::Partition(name);
    dds::sub::Subscriber sub(dp, subQos);

    /** The dds::sub::qos::DataReaderQos are derived from the topic qos */
    dds::sub::qos::DataReaderQos drqos = topic.qos();

    /** A dds::topic::ContentFilteredTopic is created filtered on the image category supplied to the
     * program as an argument */
    std::stringstream ss;
    ss << str_filter.toUtf8().data(); //"category = 'danny'";
    dds::topic::Filter filter(ss.str());
    dds::topic::ContentFilteredTopic<HelloWorldImg::Image> cftopic(topic, "CFImageExclusive", filter);

    /** A dds::sub::DataReader is created on the Subscriber & Topic with the DataReaderQos. */
    dr = dds::sub::DataReader<HelloWorldImg::Image>(sub, cftopic, drqos);

    qDebug() << "=== [Subscriber] ImageDDSSubObject is constructed...";
}

ImageDDSSubModel::
ImageDDSSubModel() :
    _cvObj(new DDSSubObject("category = 'danny'")),
    _workerThread(new QThread()),
    _label_filter(new QLabel()),
    _lineedit1(new QLineEdit()),
    _lineedit2(new QLineEdit())
{
  // connect task object and image model class
    // connect task object and image model class
    connect(this, &ImageDDSSubModel::triggerTask,
                _cvObj, &DDSSubObject::runTask);
    connect(_cvObj, &DDSSubObject::sendResults,
                this, &ImageDDSSubModel::handleResults);

  _label->setText("Topic:");
  _lineedit1->setText("HelloWorldImg_Image");
  _lineedit2->setText("category = 'danny'");
  _label_filter->setText("Filter:" + _lineedit2->text());

  _panelLayout->addWidget(_lineedit1, 1, 1);
  _panelLayout->addWidget(_label_filter, 2, 0);
  _panel->setFixedSize(240, 100);

  _dialog = new QDialog(_panel);
  _form = new QFormLayout(_dialog);
  _form->addRow(new QLabel("Setup new content filter:"));
  _form->addRow(QString("Filter:"), _lineedit2);

  // Add Cancel and OK button
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      Qt::Horizontal, _dialog);
  _form->addRow(_buttonBox);

  QObject::connect(_buttonBox, SIGNAL(accepted()), _dialog, SLOT(accept()));
  QObject::connect(_buttonBox, SIGNAL(rejected()), _dialog, SLOT(reject()));

  _cvObj->moveToThread(_workerThread);
  _workerThread->start();

  // trigger the loop of subscribing job
  emit triggerTask();
}

// implement save and restore for data flow graph
QJsonObject
ImageDDSSubModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();
  if (_lineedit1)
    modelJson["dds_sub_topic"] = _lineedit1->text();
  if (_lineedit2)
    modelJson["dds_sub_contentfiltered"] = _lineedit2->text();
  return modelJson;
}


void
ImageDDSSubModel::
restore(QJsonObject const &p)
{
  QJsonValue v1 = p["dds_sub_topic"];
  if (!v1.isUndefined())
  {
    QString strNum = v1.toString();

    bool ok;
    strNum.toInt(&ok);
    if (ok)
    {
      _lineedit1->setText(strNum);
    }
  }

  QJsonValue v2 = p["dds_sub_contentfiltered"];
  if (!v2.isUndefined())
  {
    QString strNum = v2.toString();

    bool ok;
    strNum.toInt(&ok);
    if (ok)
    {
      _lineedit1->setText(strNum);
    }
  }
}

unsigned int
ImageDDSSubModel::
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

std::shared_ptr<NodeData>
ImageDDSSubModel::
outData(PortIndex)
{
  return std::make_shared<PixmapData>(_pixmap);
}

void
ImageDDSSubModel::
handleResults(const QPixmap &pixmap)
{
    int w = _panel->width();
    int h = _panel->height();

    if(!pixmap.isNull())
    {
        _pixmap = pixmap;
    }
    // Don't neeed to display the imaeg on component
    //_label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    Q_EMIT dataUpdated(0);
}


bool
ImageDDSSubModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _panel)
  {
    if (event->type() == QEvent::MouseButtonPress)
    {
      // Show the dialog as modal
      if (_dialog->exec() == QDialog::Accepted) {
        _label_filter->setText("Filter:" + _lineedit2->text());
        _cvObj->isFinished = true;
        _workerThread->exit(0);
        while (_workerThread->isFinished() == false);
        qDebug() << "_workerThread is finished...";
        delete _workerThread;
        delete _cvObj;

        _cvObj = new DDSSubObject(_lineedit2->text());
        _workerThread = new QThread();

        connect(this, &ImageDDSSubModel::triggerTask,
                    _cvObj, &DDSSubObject::runTask);
        connect(_cvObj, &DDSSubObject::sendResults,
                    this, &ImageDDSSubModel::handleResults);

        _cvObj->moveToThread(_workerThread);
        _workerThread->start();
        // trigger the loop of subscribing job
        emit triggerTask();
      }
    }
  }

  return false;
}


void
DDSSubObject::
runTask()
{
  try {
    do
    {
        dds::sub::LoanedSamples<HelloWorldImg::Image> samples = dr->take();
        for (dds::sub::LoanedSamples<HelloWorldImg::Image>::const_iterator sample = samples.begin();
             sample < samples.end();
             ++sample)
        {
            if (sample->info().valid())
            {
                // print current time
                //auto current = std::chrono::system_clock::now();
                //std::time_t current_time = std::chrono::system_clock::to_time_t(current);
                //std::cout << "[Subscriber] finished subscribe at " << std::ctime(&current_time);
                int64 ms = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                qDebug() << "=== [Subscriber] subscribes at " << ms;

                qDebug() << "=== [Subscriber] message received :";
                qDebug() << "    userID  : " << sample->data().imgID();
                //qDebug() << "    category  : " << sample->data().category();
                //qDebug() << "    matrix : \"" << sample->data().<< "\"";
                cv::Mat output_cv = idlTocv(sample->data().matrix());

                //qDebug() << "from DDSSubObject's sub thread slot:" <<QThread::currentThreadId();
                emit sendResults(PixmapData::cvMatToQPixmap(output_cv));
            }
        }
        exampleSleepMilliseconds(100);
    }
    while (isFinished == false);
    qDebug() << "=== [Subscriber] is to closed...";
  }
  catch (const dds::core::Exception& e)
  {
    qDebug() << "ERROR: Exception: " << e.what();
  }
}
