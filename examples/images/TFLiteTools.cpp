
#include <QDebug>
#include "TFLiteTools.hpp"


double get_us(struct timeval t) { return (t.tv_sec * 1000000 + t.tv_usec); }

// Takes a file name, and loads a list of labels from it, one per line, and
// returns a vector of the strings. It pads with empty strings so the length
// of the result is a multiple of 16, because our model expects that.
TfLiteStatus ReadLabelsFile(QString const & file_name,
                            std::vector<tflite::string>* result,
                            size_t* found_label_count) {
  std::ifstream file(file_name.toUtf8().constData());
  if (!file) {
    qDebug() << "Labels file " << file_name << " not found\n";
    return kTfLiteError;
  }
  result->clear();
  tflite::string line;
  while (std::getline(file, line)) {
    result->push_back(line);
  }
  *found_label_count = result->size();
  const int padding = 16;
  while (result->size() % padding) {
    result->emplace_back();
  }
  return kTfLiteOk;
}

// Returns the top N confidence values over threshold in the provided vector,
// sorted by confidence in descending order.
template <class T>
void get_top_n(T* prediction, int prediction_size, size_t num_results,
               float threshold, std::vector<std::pair<float, int>>* top_results,
               bool input_floating) {
  // Will contain top N results in ascending order.
  std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>,
                      std::greater<std::pair<float, int>>>
      top_result_pq;

  const long count = prediction_size;  // NOLINT(runtime/int)
  for (int i = 0; i < count; ++i) {
    float value;
    if (input_floating)
      value = prediction[i];
    else
      value = prediction[i] / 255.0;
    // Only add it if it beats the threshold and has a chance at being in
    // the top N.
    if (value < threshold) {
      continue;
    }

    top_result_pq.push(std::pair<float, int>(value, i));

    // If at capacity, kick the smallest value out.
    if (top_result_pq.size() > num_results) {
      top_result_pq.pop();
    }
  }

  // Copy to output vector and reverse into descending order.
  while (!top_result_pq.empty()) {
    top_results->push_back(top_result_pq.top());
    top_result_pq.pop();
  }
  std::reverse(top_results->begin(), top_results->end());
}



TFLiteTools::
TFLiteTools(QString const &tflite_file, QString const &label_file)
  : _tflite_file(tflite_file), _label_file(label_file)
{
    // prepare interpreter
    LoadModel( _tflite_file);
    qDebug("...load model done for interpreter...");
}

TFLiteTools::
TFLiteTools(QString const &tflite_file)
  : _tflite_file(tflite_file)
{
    // prepare interpreter
    LoadModel( _tflite_file);
    qDebug("...load model done for interpreter...");
}

TFLiteTools::~TFLiteTools()
{
    if (_interpreter != nullptr) {
        _interpreter.release();
    }
}


void TFLiteTools::LoadModel(QString &tflite_file)
{
    _model = tflite::FlatBufferModel::BuildFromFile(tflite_file.toUtf8().constData());
    if (!_model) {
      qDebug() << "\nFailed to mmap model " << tflite_file << "\n";
      exit(-1);
    }
    qDebug() << "Loaded model " << tflite_file << "\n";
    _model->error_reporter();
    qDebug() << "resolved reporter\n";

    tflite::ops::builtin::BuiltinOpResolver resolver;

    tflite::InterpreterBuilder(*_model, resolver)(&_interpreter);
    if (!_interpreter) {
      qDebug() << "Failed to construct interpreter\n";
      exit(-1);
    }

    _interpreter->UseNNAPI(0);  // use Android NNAPI or not
    _interpreter->SetAllowFp16PrecisionForFp32(1); //allow running fp32 models with fp16 or not

    if (_s.verbose) {
      qDebug() << "tensors size: " << _interpreter->tensors_size() << "\n";
      qDebug() << "nodes size: " << _interpreter->nodes_size() << "\n";
      qDebug() << "inputs: " << _interpreter->inputs().size() << "\n";
      qDebug() << "input(0) name: " << _interpreter->GetInputName(0) << "\n";

      int t_size = _interpreter->tensors_size();
      for (int i = 0; i < t_size; i++) {
        if (_interpreter->tensor(i)->name)
          qDebug() << i << ": " << _interpreter->tensor(i)->name << ", "
                    << _interpreter->tensor(i)->bytes << ", "
                    << _interpreter->tensor(i)->type << ", "
                    << _interpreter->tensor(i)->params.scale << ", "
                    << _interpreter->tensor(i)->params.zero_point << "\n";
      }
    }

    if (_s.number_of_threads != -1) {
      _interpreter->SetNumThreads(_s.number_of_threads);
    }
}


std::string
TFLiteTools::doInference(const cv::Mat &frame)
{
    cv::Mat new_frame;
    cv::cvtColor(frame, new_frame, cv::COLOR_BGR2RGB);

    // resize to 224*224*3
    //cv::resize(new_frame, new_frame, cv::Size(224, 224), 0, 0, CV_INTER_LINEAR);

    // prepare image input
    int width = new_frame.cols;
    int height = new_frame.rows;
    int channels = 3;

    std::vector<uint8_t> in(width * height * channels);

    // copying the data into the corresponding tensor
    for (int y = 0; y < height; ++y) {
        int dst_pos = y * width  * channels;
        uchar* source_row = new_frame.ptr<uchar>(y);
        for (int x = 0; x < width * channels ; ++x) {
            in[dst_pos + x] = source_row[x];
        }
    }

    int input = _interpreter->inputs()[0];
    if (_s.verbose) qDebug() << "input: " << input << "\n";

    const std::vector<int> inputs = _interpreter->inputs();
    const std::vector<int> outputs = _interpreter->outputs();

    if (_s.verbose) {
      qDebug() << "number of inputs: " << inputs.size() << "\n";
      qDebug() << "number of outputs: " << outputs.size() << "\n";
    }

    if (_interpreter->AllocateTensors() != kTfLiteOk) {
      qDebug() << "Failed to allocate tensors!";
    }

    if (_s.verbose) PrintInterpreterState(_interpreter.get());

    // get input dimension from the input tensor metadata
    // assuming one input only
    TfLiteIntArray* dims = _interpreter->tensor(input)->dims;
    int wanted_height = dims->data[1];
    int wanted_width = dims->data[2];
    int wanted_channels = dims->data[3];

    switch (_interpreter->tensor(input)->type) {
      case kTfLiteFloat32:
        _s.input_floating = true;
        resize<float>(_interpreter->typed_tensor<float>(input), in.data(),
                      height, width, channels, wanted_height,
                      wanted_width, wanted_channels, &_s);
        break;
      case kTfLiteUInt8:
        resize<uint8_t>(_interpreter->typed_tensor<uint8_t>(input), in.data(),
                        height, width, channels, wanted_height,
                        wanted_width, wanted_channels, &_s);
        break;
      default:
        qDebug() << "cannot handle input type "
                   << _interpreter->tensor(input)->type << " yet";
        exit(-1);
    }

    tflite::profiling::Profiler* profiler = new tflite::profiling::Profiler();
    _interpreter->SetProfiler(profiler);

    if (_s.profiling) profiler->StartProfiling();

    struct timeval start_time, stop_time;
    gettimeofday(&start_time, nullptr);
    for (int i = 0; i < _s.loop_count; i++) {
      if (_interpreter->Invoke() != kTfLiteOk) {
        qDebug() << "Failed to invoke tflite!\n";
      }
    }
    gettimeofday(&stop_time, nullptr);
    qDebug() << "invoked \n";
    qDebug() << "average time: "
              << (get_us(stop_time) - get_us(start_time)) / (_s.loop_count * 1000)
              << " ms \n";

    const float threshold = 0.001f;

    std::vector<std::pair<float, int>> top_results;

    int output = _interpreter->outputs()[0];
    TfLiteIntArray* output_dims = _interpreter->tensor(output)->dims;
    // assume output dims to be something like (1, 1, ... ,size)
    auto output_size = output_dims->data[output_dims->size - 1];
    switch (_interpreter->tensor(output)->type) {
      case kTfLiteFloat32:
        get_top_n<float>(_interpreter->typed_output_tensor<float>(0), output_size,
                         _s.number_of_results, threshold, &top_results, true);
        break;
      case kTfLiteUInt8:
        get_top_n<uint8_t>(_interpreter->typed_output_tensor<uint8_t>(0),
                           output_size, _s.number_of_results, threshold,
                           &top_results, false);
        break;
      default:
        qDebug() << "cannot handle output type "
                   << _interpreter->tensor(input)->type << " yet";
        exit(-1);
    }

    std::vector<tflite::string> labels;
    size_t label_count;

    if (ReadLabelsFile(_label_file, &labels, &label_count) != kTfLiteOk)
      exit(-1);

    for (const auto& result : top_results) {
      const float confidence = result.first;
      const int index = result.second;
      qDebug() << confidence << ": " << index << " " << labels[index].c_str() << "\n";
    }
    return std::string(labels[top_results[0].second].c_str());
}

template <class T>
void TFLiteTools::resize(T* out, uint8_t* in, int image_height, int image_width,
            int image_channels, int wanted_height, int wanted_width,
            int wanted_channels, Settings* s) {
  int number_of_pixels = image_height * image_width * image_channels;
  std::unique_ptr<tflite::Interpreter> interpreter(new tflite::Interpreter);

  int base_index = 0;

  // two inputs: input and new_sizes
  interpreter->AddTensors(2, &base_index);
  // one output
  interpreter->AddTensors(1, &base_index);
  // set input and output tensors
  interpreter->SetInputs({0, 1});
  interpreter->SetOutputs({2});

  // set parameters of tensors
  TfLiteQuantizationParams quant;
  interpreter->SetTensorParametersReadWrite(
      0, kTfLiteFloat32, "input",
      {1, image_height, image_width, image_channels}, quant);
  interpreter->SetTensorParametersReadWrite(1, kTfLiteInt32, "new_size", {2},
                                            quant);
  interpreter->SetTensorParametersReadWrite(
      2, kTfLiteFloat32, "output",
      {1, wanted_height, wanted_width, wanted_channels}, quant);

  tflite::ops::builtin::BuiltinOpResolver resolver;
  const TfLiteRegistration* resize_op =
      resolver.FindOp(tflite::BuiltinOperator_RESIZE_BILINEAR, 1);
  auto* params = reinterpret_cast<TfLiteResizeBilinearParams*>(
      malloc(sizeof(TfLiteResizeBilinearParams)));
  params->align_corners = false;
  interpreter->AddNodeWithParameters({0, 1}, {2}, nullptr, 0, params, resize_op,
                                     nullptr);

  interpreter->AllocateTensors();

  // fill input image
  // in[] are integers, cannot do memcpy() directly
  auto input = interpreter->typed_tensor<float>(0);
  for (int i = 0; i < number_of_pixels; i++) {
    input[i] = in[i];
  }

  // fill new_sizes
  interpreter->typed_tensor<int>(1)[0] = wanted_height;
  interpreter->typed_tensor<int>(1)[1] = wanted_width;

  interpreter->Invoke();

  auto output = interpreter->typed_tensor<float>(2);
  auto output_number_of_pixels = wanted_height * wanted_width * wanted_channels;

  for (int i = 0; i < output_number_of_pixels; i++) {
    if (_s.input_floating)
      out[i] = (output[i] - s->input_mean) / s->input_std;
    else
      out[i] = (uint8_t)output[i];
  }
}
