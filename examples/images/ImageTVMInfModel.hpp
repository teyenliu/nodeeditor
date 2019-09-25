#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QThread>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <dlpack/dlpack.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/packed_func.h>
#include <fstream>
#include <sys/time.h>

#include "ImageShowModel.hpp"
#include "PixmapData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;


class TVMObject:public QObject
{
    Q_OBJECT
public:
    TVMObject()
    {
        const QString so_lib_path = QString("/usr/local/tf_libs/resources/deploy_lib.so");
        const QString graph_json = QString("/usr/local/tf_libs/resources/deploy_graph.json");
        const QString graph_params = QString("/usr/local/tf_libs/resources/deploy_param.params");

        tvm::runtime::Module mod_syslib = tvm::runtime::Module::LoadFromFile(
                    so_lib_path.toUtf8().data());
        std::ifstream json_in(graph_json.toUtf8().data(), std::ios::in);
        std::string json_data((std::istreambuf_iterator<char>(json_in)),
                              std::istreambuf_iterator<char>());
        json_in.close();
        std::ifstream params_in(graph_params.toUtf8().data(),
                                std::ios::binary);
        std::string params_data((std::istreambuf_iterator<char>(params_in)),
                                std::istreambuf_iterator<char>());
        params_in.close();

        params_arr.data = params_data.c_str();
        params_arr.size = params_data.length();
        int dtype_code = kDLFloat;
        int dtype_bits = 32;
        int dtype_lanes = 1;
        int device_type = kDLCPU;
        int device_id = 0;

        mod = (*tvm::runtime::Registry::Get("tvm.graph_runtime.create"))
                (json_data, mod_syslib, device_type, device_id);

        int in_ndim = 4;
        int64_t in_shape[4] = {1, 3, 224, 224};
        int out_ndim = 2;
        int64_t out_shape[4] = {1, 1001, };

        // Prepare TVM DLTensor x, y
        TVMArrayAlloc(in_shape, in_ndim, dtype_code,
                      dtype_bits, dtype_lanes,
                      device_type, device_id, &x);
        TVMArrayAlloc(out_shape, out_ndim, dtype_code,
                      dtype_bits, dtype_lanes,
                      device_type, device_id, &y);

        // get the function from the module(set input data)
        set_input = mod.GetFunction("set_input");
        // input tensor name: "input" should be the same when TVM compiles model
        set_input("input", x);
        // get the function from the module(load patameters)
        load_params = mod.GetFunction("load_params");
        load_params(params_arr);
        // get the function from the module(run it)
        run = mod.GetFunction("run");
        // get the function from the module(get output data)
        get_output = mod.GetFunction("get_output");

    }
    ~TVMObject()
    {
        TVMArrayFree(x);
        TVMArrayFree(y);
    }

signals:
    void sendResults(const QString &);

public slots:
    void runTask(const QPixmap &);

private:
    tvm::runtime::Module mod;
    TVMByteArray params_arr;
    DLTensor* x;
    DLTensor* y;
    tvm::runtime::PackedFunc set_input;
    tvm::runtime::PackedFunc load_params;
    tvm::runtime::PackedFunc run;
    tvm::runtime::PackedFunc get_output;

};


/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ImageTVMInfModel : public ImageShowModel
{
  Q_OBJECT

public:
  ImageTVMInfModel();

  ~ImageTVMInfModel() override
  {
      _workerThread.quit();
      _workerThread.wait();
  }

// Threading
public slots:
  void handleResults(const QString &);

signals:
  void triggerTask(const QPixmap &);

public:

  QString
  caption() const override
  { return QString("TVM Inference Display"); }

  QString
  name() const override
  { return QString("ImageTVMInfModel"); }

public:

  virtual QString
  modelName() const override
  { return QString("TVM Inference Image"); }

  void
  setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

protected:

  bool
  eventFilter(QObject *object, QEvent *event) override;

  TVMObject *_tvmObj;
  QThread _workerThread;
};
