#pragma once
#include <sys/time.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <QtGui/QPixmap>
#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class PixmapData : public NodeData
{
public:

  PixmapData() {}

  PixmapData(QPixmap const &pixmap)
    : _pixmap(pixmap)
  {}

  NodeDataType
  type() const override
  {
    //       id      name
    return {"pixmap", "P"};
  }

  QPixmap
  pixmap() const { return _pixmap; }

  void
  setPixmap(QPixmap const &pixmap) {
      _pixmap = pixmap;
  }

  // Danny Implementation
  static QImage cvMatToQImage( const cv::Mat &inMat );
  static QPixmap cvMatToQPixmap( const cv::Mat &inMat );
  static cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true);
  static cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true);
  static void Mat_to_CHW(float *data, cv::Mat &frame, unsigned int h, unsigned int w);

  // Object Detection
  static void drawPred(int classId, float conf, int left, int top, int right, int bottom,
                       int width, int height, cv::Mat& frame, std::vector<std::string>& classes);
  static void drawOCRPred(int classId, float conf, int left, int top, int right, int bottom,
                       int width, int height, cv::Mat& frame, std::vector<std::string>& classes);

  // time estimation
  static float getElapse(struct timeval *tv1,struct timeval *tv2);

private:

  QPixmap _pixmap;
};
