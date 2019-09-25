#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QThread>
#include <QDebug>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "ImageShowModel.hpp"
#include "PixmapData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;


class YOLOObject:public QObject
{
    Q_OBJECT
public:
    YOLOObject()
    {
        const QString model_config = QString("/usr/local/tf_libs/resources/darknet/yolo-voc.cfg");
        const QString model_weights = QString("/usr/local/tf_libs/resources/darknet/yolo-voc.weights");
        const QString class_file = QString("/usr/local/tf_libs/resources/darknet/voc.names");

        std::ifstream classNamesFile(class_file.toUtf8().data());
        if (classNamesFile.is_open())
        {
            std::string className = "";
            while (std::getline(classNamesFile, className))
                classes.push_back(className);
        }
        else{
            qDebug() << "can not open classNamesFile...";
        }

        net = cv::dnn::readNetFromDarknet(model_config.toUtf8().data(),
                                          model_weights.toUtf8().data());
        qDebug() << "Read Darknet...";
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    }
    ~YOLOObject()
    {
    }

    // Remove the bounding boxes with low confidence using non-maxima suppression
    void postprocess(cv::Mat& frame, std::vector<cv::Mat>& outs);

    // Get the names of the output layers
    std::vector<cv::String> getOutputsNames(const cv::dnn::Net& net);

signals:
    void sendResults(const QPixmap &);
    void sendDetections(const QPixmap &);

public slots:
    void runTask(const QPixmap &);

private:
    cv::dnn::Net net;
    std::vector<std::string> classes;
    // Initialize the parameters
    float confThreshold = 0.5; // Confidence threshold
    float nmsThreshold = 0.4;  // Non-maximum suppression threshold
    int inpWidth = 416;        // Width of network's input image
    int inpHeight = 416;
};


/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageCVYOLOModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageCVYOLOModel();

  ~ImageCVYOLOModel() override
  {
      _workerThread.quit();
      _workerThread.wait();
  }

// Threading
public slots:
  void handleResults(const QPixmap &);
  void handleDetections(const QPixmap &);

signals:
  void triggerTask(const QPixmap &);

public:

  QString
  caption() const override
  { return QString("CV_YOLO Inference Display"); }

  QString
  name() const override
  { return QString("ImageCVYOLOModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("CV_YOLO Inference Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

protected:

  YOLOObject *_yoloObj;
  QThread _workerThread;
  QPixmap _pixmap_out;
};
