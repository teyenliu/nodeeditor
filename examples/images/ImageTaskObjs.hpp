#ifndef IMAGETASKOBJS_HPP
#define IMAGETASKOBJS_HPP

#include <QObject>
#include <QWidget>

class ImageTaskObject : public QObject
{
    Q_OBJECT
public:
    explicit ImageTaskObject(QObject *parent = nullptr);

signals:

public slots:
    void doTask();
};

#endif // IMAGETASKOBJS_HPP
