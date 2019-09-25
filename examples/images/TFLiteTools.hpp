#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <queue>
#include <thread>

#include <fcntl.h>      // NOLINT(build/include_order)
#include <getopt.h>     // NOLINT(build/include_order)
#include <sys/time.h>   // NOLINT(build/include_order)
#include <sys/types.h>  // NOLINT(build/include_order)
#include <sys/uio.h>    // NOLINT(build/include_order)
#include <unistd.h>     // NOLINT(build/include_order)

#include "tensorflow/contrib/lite/kernels/register.h"
#include "tensorflow/contrib/lite/model.h"
#include "tensorflow/contrib/lite/optional_debug_tools.h"
#include "tensorflow/contrib/lite/string_util.h"

#include "tensorflow/contrib/lite/builtin_op_data.h"
#include "tensorflow/contrib/lite/interpreter.h"
#include "tensorflow/contrib/lite/kernels/register.h"
#include "tensorflow/contrib/lite/string_util.h"
#include "tensorflow/contrib/lite/version.h"

#include "tensorflow/contrib/lite/string.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"


#include <QtGui/QPixmap>

struct Settings {
  bool verbose = false;
  bool accel = false;
  bool input_floating = false;
  bool profiling = false;
  bool allow_fp16 = false;
  int loop_count = 1;
  float input_mean = 127.5f;
  float input_std = 127.5f;
  tflite::string input_layer_type = "uint8_t";
  int number_of_threads = std::max(int(std::thread::hardware_concurrency())-1, 1);
  int number_of_results = 5;
};

template <class T>
void get_top_n(T* prediction, int prediction_size, size_t num_results,
               float threshold, std::vector<std::pair<float, int>>* top_results,
               bool input_floating);



/// The class wraps TensorFlow's APIs so that
/// it is more easy to use.
class TFLiteTools
{
public:
  TFLiteTools(QString const &tflite_file, QString const &label_file);
  TFLiteTools(QString const &tflite_file);
  ~TFLiteTools();

  void LoadModel(QString &tflite_file);

  std::string doInference(const cv::Mat &frame);
  std::string doInferenceWPOD(const cv::Mat &frame);

  template <class T>
  void resize(T* out, uint8_t* in, int image_height, int image_width,
              int image_channels, int wanted_height, int wanted_width,
              int wanted_channels, Settings* s);
private:
  std::unique_ptr<tflite::FlatBufferModel> _model;
  std::unique_ptr<tflite::Interpreter> _interpreter;

  QString _tflite_file;
  QString _label_file;
  Settings _s;
};
