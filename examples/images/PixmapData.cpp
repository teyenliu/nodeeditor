
#include <QDebug>
#include <opencv2/imgproc/imgproc.hpp>
#include <QtGui/QPixmap>
#include <nodes/NodeDataModel>
#include "PixmapData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph


QImage
PixmapData::cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_ARGB32 );

        return image;
    }

    // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_RGB888 );

        return image.rgbSwapped();
    }

    // 8-bit, 1 channel
    case CV_8UC1:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_Grayscale8 );//Format_Alpha8 and Format_Grayscale8 were added in Qt 5.5


        return image;
    }

    default:
        qWarning() << "CVS::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}


QPixmap
PixmapData::cvMatToQPixmap( const cv::Mat &inMat )
{
    return QPixmap::fromImage( cvMatToQImage( inMat ) );
}

cv::Mat
PixmapData::QImageToCvMat( const QImage &inImage, bool inCloneImageData)
{
    qDebug() << "...The image format is :" << inImage.format();
    switch ( inImage.format() )
    {
    // 8-bit, 4 channel
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC4,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return (inCloneImageData ? mat.clone() : mat);
    }

    // 8-bit, 3 channel
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
    {
        if ( !inCloneImageData )
        {
            qWarning() << "CVS::QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
        }

        QImage   swapped = inImage;

        if ( inImage.format() == QImage::Format_RGB32 )
        {
            swapped = swapped.convertToFormat( QImage::Format_RGB888 );
        }

        swapped = swapped.rgbSwapped();

        return cv::Mat( swapped.height(), swapped.width(),
                        CV_8UC3,
                        const_cast<uchar*>(swapped.bits()),
                        static_cast<size_t>(swapped.bytesPerLine())
                        ).clone();
    }

    // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
    {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC1,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return (inCloneImageData ? mat.clone() : mat);
    }

    default:
        qWarning() << "CVS::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
        break;
    }

    return cv::Mat();
}


cv::Mat
PixmapData::QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData)
{
    return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
}

void
PixmapData::Mat_to_CHW(float *data, cv::Mat &frame, unsigned int h, unsigned int w)
{
    assert(data && !frame.empty());
    unsigned int volChl = h * w;

    for(int c = 0; c < 3; ++c)
    {
        for (unsigned j = 0; j < volChl; ++j)
            data[c*volChl + j] = static_cast<float>(float(frame.data[j * 3 + c]) / 255.0);
    }

}

// Draw the predicted bounding box
void
PixmapData::drawPred(int classId, float conf, int left, int top, int right, int bottom,
                     int width, int height, cv::Mat& frame, std::vector<std::string>& classes)
{
    //Draw a rectangle displaying the bounding box
    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 0, 255), 3);

    //Get the label for the class name and its confidence
    std::string label = cv::format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ":" + label;
    }
    else
    {
        qDebug() <<"classes is empty...";
    }

    //Display the label at the top of the bounding box
    int baseLine;
    cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX,
                                             0.5, 1, &baseLine);
    top = std::max(top, labelSize.height);
    cv::putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX,
                    0.5, cv::Scalar(255,255,255), 2);
}


// Draw the predicted bounding box
void
PixmapData::drawOCRPred(int classId, float conf, int left, int top, int right, int bottom,
                     int width, int height, cv::Mat& frame, std::vector<std::string>& classes)
{
    //Draw a rectangle displaying the bounding box
    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0), 3);

    //Get the label for the class name and its confidence
    std::string label;
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId];
    }
    else
    {
        qDebug() <<"classes is empty...";
    }

    //Display the label at the top of the bounding box
    int baseLine;
    cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX,
                                             1, 2, &baseLine);
    top = std::max(top, labelSize.height);
    cv::putText(frame, label, cv::Point(left+0.5*width, top), cv::FONT_HERSHEY_SIMPLEX,
                    1, cv::Scalar(255,0,0), 2);
}


float
PixmapData::getElapse(struct timeval *tv1,struct timeval *tv2)
{
    float t = 0.0f;
    if (tv1->tv_sec == tv2->tv_sec)
        t = (tv2->tv_usec - tv1->tv_usec)/1000.0f;
    else
        t = ((tv2->tv_sec - tv1->tv_sec) * 1000 * 1000 + tv2->tv_usec - tv1->tv_usec)/1000.0f;
    return t;
}
