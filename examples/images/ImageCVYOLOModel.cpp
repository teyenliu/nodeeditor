
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "ImageCVYOLOModel.hpp"
#include "PixmapData.hpp"


using namespace cv;


ImageCVYOLOModel::
ImageCVYOLOModel() :
    _yoloObj(new YOLOObject())
{
    // connect task object and image model class
    connect(this, &ImageCVYOLOModel::triggerTask,
                _yoloObj, &YOLOObject::runTask);
    connect(_yoloObj, &YOLOObject::sendResults,
                this, &ImageCVYOLOModel::handleResults);
    connect(_yoloObj, &YOLOObject::sendDetections,
                this, &ImageCVYOLOModel::handleDetections);

    _yoloObj->moveToThread(&_workerThread);
    _workerThread.start();
}

void
ImageCVYOLOModel::
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
    _label->setText("No image for object detection in yolo ...");
  }

  Q_EMIT dataUpdated(0);
}

std::shared_ptr<NodeData>
ImageCVYOLOModel::
outData(PortIndex)
{
  //return _nodeData;
  return std::make_shared<PixmapData>(_pixmap_out);
}


void
ImageCVYOLOModel::
handleResults(const QPixmap &pixmap)
{
    int w = _panel->width();
    int h = _panel->height();
    auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
    if(!pixmap.isNull())
    {
        d->setPixmap(pixmap);
    }
    _label->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
    Q_EMIT dataUpdated(0);
}

void
ImageCVYOLOModel::
handleDetections(const QPixmap &pixmap)
{
    _pixmap_out = pixmap;
    Q_EMIT dataUpdated(0);
}

void
YOLOObject::
runTask(const QPixmap &frame)
{
    Mat src = PixmapData::QPixmapToCvMat(frame);

    // Create a 4D blob from a frame.
    cv::Mat blob;
    cv::dnn::blobFromImage(src, blob, 1/255.F, cv::Size(inpWidth, inpHeight), cv::Scalar(), true, false);

    //Sets the input to the network
    net.setInput(blob);

    // Runs the forward pass to get output of the output layers
    std::vector<cv::Mat> outs;
    struct timeval  tv1,tv2;
    gettimeofday(&tv1, NULL);

    // Darknet inferences
    net.forward(outs, getOutputsNames(net));

    gettimeofday(&tv2, NULL);
    float diff = PixmapData::getElapse(&tv1,&tv2);
    qDebug() << "[Darknet] YOLOv3 costs " << diff << " fms";

    // Remove the bounding boxes with low confidence
    postprocess(src, outs);
}


// Get the names of the output layers
std::vector<cv::String>
YOLOObject::getOutputsNames(const cv::dnn::Net& net)
{
    static std::vector<cv::String> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        std::vector<int> outLayers = net.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        std::vector<cv::String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}


// Remove the bounding boxes with low confidence using non-maxima suppression
void
YOLOObject::postprocess(cv::Mat& frame, std::vector<cv::Mat>& outs)
{
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            cv::Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);

            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }


    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    //In this tool, we only deal with the bounding box of the highest confidence
    if(indices.size() > 0)
    {        
        int idx = indices[0]; // the highest confidence
        cv::Rect box = boxes[idx];

        //Crop the image
        Mat ROI(frame, Rect(std::max(box.x,0),std::max(box.y,0),
                            std::max(box.width,0),std::max(box.height,0)));
        emit sendDetections(PixmapData::cvMatToQPixmap(ROI));

        PixmapData::drawPred(classIds[idx], confidences[idx], box.x, box.y,
            box.x + box.width, box.y + box.height, box.width, box.height, frame, classes);
        emit sendResults(PixmapData::cvMatToQPixmap(frame));
    }
}
