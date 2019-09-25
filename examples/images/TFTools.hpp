#pragma once

#include <tensorflow/core/public/session.h>
#include <tensorflow/core/public/session_options.h>
#include <tensorflow/core/protobuf/meta_graph.pb.h>

#include "tensorflow/core/lib/core/status.h"

#include <tuple>
#include <sstream>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QtGui/QPixmap>


/// The class wraps TensorFlow's APIs so that
/// it is more easy to use.
class TFTools
{
public:
  TFTools(QString const &graph_fn, QString const &checkpoint_fn);
  TFTools(QString const &pb_fn);
  ~TFTools();

  tensorflow::Status LoadModel(tensorflow::Session *sess,
                               QString &graph_fn,
                               QString &checkpoint_fn);

  tensorflow::Status LoadFrozenModel(tensorflow::Session *sess,
                               QString &pb_fn);


  std::string doInference(const cv::Mat &frame);
  cv::Mat doFrozenInference(const cv::Mat &frame);

  std::vector<int> get_tensor_shape(tensorflow::Tensor& tensor)
  {
      std::vector<int> shape;
      int num_dimensions = tensor.shape().dims();
      for(int ii_dim=0; ii_dim<num_dimensions; ii_dim++) {
          shape.push_back(tensor.shape().dim_size(ii_dim));
      }
      return shape;
  }

private:
  tensorflow::Session *_sess;
  QString _graph_fn;
  QString _checkpoint_fn;
  QString _pb_fn;
};

class Label
{
public:
    Label(int cl, float prob, Eigen::Tensor<float,2> pts_prop)
    : _cl(cl), _prob(prob), _pts(pts_prop) {

        Eigen::array<int, 1> pts_dim({1});
        Eigen::Tensor<float, 1> pts1 = pts_prop.minimum(pts_dim);
        Eigen::Tensor<float, 1> pts2 = pts_prop.maximum(pts_dim);
        std::cout << "...pts1:" << pts1 << "...pts2:" << pts2 << std::endl;

        _tl = {pts1(0),pts1(1)};
        _br = {pts2(0),pts2(1)};

    }

    ~Label(){}

    std::string getStr() {
        std::stringstream ss;
        ss << "Class:" << _cl << ", top_left:(" << _tl[0] << "," << _tl[1] << "), ";
        ss << "bottom_right(x:" << _br[0] << ",y:" << _br[1] << ")";
        return ss.str();
    }

    Label copy(){
        return Label(_cl, _prob, _pts);
    }

    std::vector<float> wh() {
        std::vector<float> ret;
        ret.push_back(_br[0] - _tl[0]);
        ret.push_back(_br[1] - _tl[1]);
        return ret;
    }

    std::vector<float> cc(){
        std::vector<float> ret;
        ret.push_back(_tl[0] + wh()[0]*0.5f);
        ret.push_back(_tl[1] + wh()[1]*0.5f);
        return ret;
    }

    std::vector<float> tl(){
        return _tl;
    }

    std::vector<float> br(){
        return _br;
    }

    std::vector<float> tr(){
        std::vector<float> ret;
        ret.push_back(_br[0]);
        ret.push_back(_tl[1]);
        return ret;
    }

    std::vector<float> bl(){
        std::vector<float> ret;
        ret.push_back(_tl[0]);
        ret.push_back(_br[1]);
        return ret;
    }

    Eigen::Tensor<float,2> pts(){
        return _pts;
    }

    int cl(){
        return _cl;
    }

    float area(){
        return wh()[0] * wh()[1];
    }

    float prob(){
        return _prob;
    }

    void set_class(int cl){
        _cl = cl;
    }

    void set_tl(std::vector<float> tl){
        _tl = tl;
    }

    void set_br(std::vector<float> br){
        _br = br;
    }

    void set_wh(std::vector<float> wh){
        _tl[0] = cc()[0] - (wh[0] * 0.5f);
        _tl[1] = cc()[1] - (wh[1] * 0.5f);

        _br[0] = cc()[0] + (wh[0] * 0.5f);
        _br[1] = cc()[1] + (wh[1] * 0.5f);
    }

    void set_prob(float prob){
        _prob = prob;
    }


    float _prob = 0.0;
    Eigen::Tensor<float, 2> _pts;

private:
    int _cl = 0;
    std::vector<float> _tl;
    std::vector<float> _br;
};

class WPOD
{
public:

    WPOD() {}
    ~WPOD() {}

    static
    float IOU(std::vector<float> tl1,
        std::vector<float> br1,
        std::vector<float> tl2,
        std::vector<float> br2){

        std::vector<float> wh1;
        std::vector<float> wh2;

        wh1.push_back(br1[0]-tl1[0]);
        wh1.push_back(br1[1]-tl1[1]);

        wh2.push_back(br2[0]-tl2[0]);
        wh2.push_back(br2[1]-tl2[1]);
        std::cout << "......wh1:" << wh1[0] << "," << wh1[1] << std::endl;
        std::cout << "......wh2:" << wh2[0] << "," << wh2[1] << std::endl;

        //need to check all the elements > 0
        //assert((wh1>=.0).all() and (wh2>=.0).all())

        std::vector<float> np_min;
        np_min.push_back(std::min(br1[0],br2[0]));
        np_min.push_back(std::min(br1[1],br2[1]));
        std::cout << "......np_min:" << np_min[0] << "," << np_min[1] << std::endl;

        std::vector<float> np_max;
        np_max.push_back(std::max(tl1[0],tl2[0]));
        np_max.push_back(std::max(tl1[1],tl2[1]));
        std::cout << "......np_max:" << np_max[0] << "," << np_max[1] << std::endl;

        std::vector<float> intersection_wh;
        float ret_tmp = np_min[0] - np_max[0];
        if(ret_tmp > 0.0f)
            intersection_wh.push_back(ret_tmp);
        else
            intersection_wh.push_back(0.0f);

        ret_tmp = np_min[1] - np_max[1];
        if(ret_tmp > 0.0f)
            intersection_wh.push_back(ret_tmp);
        else
            intersection_wh.push_back(0.0f);

        float intersection_area = intersection_wh[0] * intersection_wh[1];
        float area1 = wh1[0] * wh1[1];
        float area2 = wh2[0] * wh2[1];
        float union_area = area1 + area2 - intersection_area;
        return intersection_area/union_area;
    }

    static
    float IOU_labels(Label l1, Label l2){
        auto ret = IOU(l1.tl(),l1.br(),l2.tl(),l2.br());
        return ret;
    }

    static
    std::vector<Label> nms(std::vector<Label> Labels,
                           float iou_threshold=.5f) {

        std::vector<Label> SelectedLabels;
        //sort Label
        std::sort(Labels.begin(), Labels.end(),
                  [](const Label& lhs, const Label& rhs) {
            return lhs._prob > rhs._prob;
        });
        for(auto label: Labels){
            bool non_overlap = true;
            if(SelectedLabels.size() > 0) {
                for(auto sel_label: SelectedLabels) {
                    if (IOU_labels(label, sel_label) > iou_threshold) {
                        non_overlap = false;
                        break;
                    }
                }
            }
            if(non_overlap)
                SelectedLabels.push_back(label);
        }
        return SelectedLabels;
    }
    
    static
    cv::Mat find_T_matrix(Eigen::Tensor<float, 2> pts,
                          Eigen::Tensor<float, 2> t_pts){
        Eigen::Tensor<float, 2> A(8,9);
        A.setZero();
        Eigen::Tensor<float, 1> xi(3);
        Eigen::Tensor<float, 1> xil(3);

        for(int i=0; i<4; i++){
            xi.setValues({pts(0,i), pts(1,i), pts(2,i)});
            xil.setValues({t_pts(0,i), t_pts(1,i), t_pts(2,i)});

            A(i*2,3) = -xil(2)*xi(0);
            A(i*2,4) = -xil(2)*xi(1);
            A(i*2,5) = -xil(2)*xi(2);

            A(i*2,6) = xil(1)*xi(0);
            A(i*2,7) = xil(1)*xi(1);
            A(i*2,8) = xil(1)*xi(2);

            A(i*2+1,0) = xil(2)*xi(0);
            A(i*2+1,1) = xil(2)*xi(1);
            A(i*2+1,2) = xil(2)*xi(2);

            A(i*2+1,6) = -xil(0)*xi(0);
            A(i*2+1,7) = -xil(0)*xi(1);
            A(i*2+1,8) = -xil(0)*xi(2);
        }

        cv::Mat mat(8, 9, CV_32FC1);
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 9; ++x) {
                mat.at<float>(y, x) = A(y,x);
            }
        }
        cv::Mat w, u, vt;
        cv::SVD::compute(mat, w, u, vt, cv::SVD::FULL_UV);
        cv::Mat H(3,3,CV_32FC1);

        // copy vt[8,:] to H
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                H.at<float>(i,j) = vt.at<float>(8,i*3+j);
            }
        }
        std::cout << "H: " << H << std::endl;
        std::cout << "vt: " << vt << std::endl;

        return H;
    }
};

class Shape
{
public:
    Shape(Eigen::Tensor<float,2> pts, int max_side, std::string text)
        : _pts(pts), _max_side(max_side), _text(text) {
        _dims = _pts.dimensions();
    }
    ~Shape() {}

    bool isValid(){
        return _dims[1] > 2;
    }

    Eigen::Tensor<float,2> _pts;
    Eigen::Tensor<float, 2>::Dimensions _dims;
    int _max_side;
    std::string _text;
};
