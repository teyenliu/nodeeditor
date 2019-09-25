#include "VideoFrameGrabber.hpp"


VideoFrameGrabber::VideoFrameGrabber(QWidget *widget, QObject *parent)
    : QAbstractVideoSurface(parent)
    , widget(widget)
    , imageFormat(QImage::Format_Invalid)
{
}

QList<QVideoFrame::PixelFormat> VideoFrameGrabber::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB24;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool VideoFrameGrabber::isFormatSupported(const QVideoSurfaceFormat &format) const
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    return imageFormat != QImage::Format_Invalid
            && !size.isEmpty()
            && format.handleType() == QAbstractVideoBuffer::NoHandle;
}

bool VideoFrameGrabber::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        this->imageFormat = imageFormat;
        imageSize = size;
        sourceRect = format.viewport();

        QAbstractVideoSurface::start(format);

        widget->updateGeometry();
        updateVideoRect();

        return true;
    } else {
        return false;
    }
}

void VideoFrameGrabber::stop()
{
    currentFrame = QVideoFrame();
    targetRect = QRect();

    QAbstractVideoSurface::stop();

    widget->update();
}

bool VideoFrameGrabber::present(const QVideoFrame &frame)
{
    if (frame.isValid())
    {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        const QImage image(cloneFrame.bits(),
                           cloneFrame.width(),
                           cloneFrame.height(),
                           QVideoFrame::imageFormatFromPixelFormat(cloneFrame .pixelFormat()));
        emit frameAvailable(image); // this is very important
        cloneFrame.unmap();
    }

    if (surfaceFormat().pixelFormat() != frame.pixelFormat()
            || surfaceFormat().frameSize() != frame.size()) {
        setError(IncorrectFormatError);
        stop();

        return false;
    } else {
        currentFrame = frame;

        widget->repaint(targetRect);

        return true;
    }
}

void VideoFrameGrabber::updateVideoRect()
{
    QSize size = surfaceFormat().sizeHint();
    size.scale(widget->size().boundedTo(size), Qt::KeepAspectRatio);

    targetRect = QRect(QPoint(0, 0), size);
    targetRect.moveCenter(widget->rect().center());
}

void VideoFrameGrabber::paint(QPainter *painter)
{
    if (currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {
        const QTransform oldTransform = painter->transform();

        if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
           painter->scale(1, -1);
           painter->translate(0, -widget->height());
        }

        QImage image(
                currentFrame.bits(),
                currentFrame.width(),
                currentFrame.height(),
                currentFrame.bytesPerLine(),
                imageFormat);

        painter->drawImage(targetRect, image, sourceRect);

        painter->setTransform(oldTransform);

        currentFrame.unmap();
    }
}
