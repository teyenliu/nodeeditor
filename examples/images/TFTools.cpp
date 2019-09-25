
#include <QDebug>
#include "PixmapData.hpp"
#include "TFTools.hpp"


TFTools::
TFTools(QString const &graph_fn, QString const &checkpoint_fn)
  : _graph_fn(graph_fn), _checkpoint_fn(checkpoint_fn)
{
    // prepare session
    tensorflow::SessionOptions options;
    TF_CHECK_OK(tensorflow::NewSession(options, &_sess));
    TF_CHECK_OK(LoadModel(_sess, _graph_fn, _checkpoint_fn));
    qDebug("...load model done in getSession...");
}

TFTools::
TFTools(QString const &pb_fn)
  : _pb_fn(pb_fn)
{
    // prepare session
    tensorflow::SessionOptions options;
    TF_CHECK_OK(tensorflow::NewSession(options, &_sess));
    TF_CHECK_OK(LoadFrozenModel(_sess, _pb_fn));
    qDebug("...load frozen model done in getSession...");
}

TFTools::~TFTools()
{
    if (_sess != nullptr)
        _sess->Close();
}

typedef std::vector<std::pair<std::string, tensorflow::Tensor>> tensor_dict;

/**
 * @brief load a previous store model
 * @details [long description]
 *
 * in Python run:
 *
 *    saver = tf.train.Saver(tf.global_variables())
 *    saver.save(sess, './exported/my_model')
 *    tf.train.write_graph(sess.graph, '.', './exported/graph.pb, as_text=False)
 *
 * this relies on a graph which has an operation called `init` responsible to initialize all variables, eg.
 *
 *    sess.run(tf.global_variables_initializer())  # somewhere in the python file
 *
 * @param sess active tensorflow session
 * @param graph_fn path to graph file (eg. "./exported/graph.pb")
 * @param checkpoint_fn path to checkpoint file (eg. "./exported/my_model", optional)
 * @return status of reloading
 */
tensorflow::Status TFTools::LoadModel(tensorflow::Session *sess,
                             QString &graph_fn,
                             QString &checkpoint_fn)
{
    tensorflow::Status status;

    // Read in the protobuf graph we exported
    tensorflow::MetaGraphDef graph_def;
    status = ReadBinaryProto(tensorflow::Env::Default(),
                             graph_fn.toUtf8().constData(), &graph_def);
    if (status != tensorflow::Status::OK())
        return status;

    // create the graph in the current session
    status = sess->Create(graph_def.graph_def());
    if (status != tensorflow::Status::OK())
        return status;

    // restore model from checkpoint, iff checkpoint is given
    if (checkpoint_fn != "")
    {
        const std::string restore_op_name = graph_def.saver_def().restore_op_name();
        const std::string filename_tensor_name = graph_def.saver_def().filename_tensor_name();

        tensorflow::Tensor filename_tensor(tensorflow::DT_STRING, tensorflow::TensorShape());
        filename_tensor.scalar<std::string>()() = checkpoint_fn.toUtf8().constData();

        std::cout << "filename_tensor's scalar:" << filename_tensor.dims() << std::endl;

        tensor_dict feed_dict = {{filename_tensor_name, filename_tensor}};
        status = sess->Run(feed_dict,
            {}, {restore_op_name}, nullptr);

        if (status != tensorflow::Status::OK())
            return status;
    } else
    {
        status = sess->Run({}, {}, {"init"}, nullptr);
        if (status != tensorflow::Status::OK())
            return status;
    }

    return tensorflow::Status::OK();
}


tensorflow::Status TFTools::LoadFrozenModel(tensorflow::Session *sess,
                             QString &pb_fn)
{
    tensorflow::Status status;

    // Read in the protobuf graph we exported
    // (The path seems to be relative to the cwd. Keep this in mind
    // when using `bazel run` since the cwd isn't where you call
    // `bazel run` but from inside a temp folder.)
    tensorflow::GraphDef graph_def;
    status = ReadBinaryProto(tensorflow::Env::Default(), pb_fn.toUtf8().data(), &graph_def);
    if (!status.ok()) {
      std::cout << status.ToString() << "\n";
      return status;
    }

    // Add the graph to the session
    status = sess->Create(graph_def);
    if (!status.ok()) {
      std::cout << status.ToString() << "\n";
      return status;
    }
    return tensorflow::Status::OK();
}


std::string
TFTools::doInference(const cv::Mat &frame)
{
    // prepare image input
    // Flag=1 for RGB
    int height = 28;
    int width = 28;
    int depth = 1;

    cv::Mat gray_mat;
    qDebug("...to convert gray image for doInference...");
    cv::cvtColor(frame, gray_mat, cv::COLOR_BGR2GRAY);
    cv::Mat resized_frame;
    // resize to 28*28*1 frame
    qDebug("...to resize image for doInference...");
    cv::resize(gray_mat, resized_frame, cv::Size(height, width), 0, 0);

    tensorflow::Tensor inputImg(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, 784})); //frame.rows * frame.cols
    auto inputImg_mapped = inputImg.tensor<float, 2>();

    // copying the data into the corresponding tensor
    for (int y = 0; y < height; ++y) {
        uchar* source_row = resized_frame.ptr<uchar>(y);
        for (int x = 0; x < width; ++x) {
            inputImg_mapped(0, y * width + x) = source_row[x];
        }
    }

    qDebug("...convert mat to intput tensor done...");

    tensor_dict feed_dict = {
        { "inputs/X:0", inputImg},
    };

    std::vector<tensorflow::Tensor> outputs;
    TF_CHECK_OK(_sess->Run(feed_dict, {"output/output/BiasAdd:0"}, {}, &outputs));
    qDebug() << "...debug for outputs[0]: " << outputs[0].DebugString().data();


    // finding the labels for prediction
    tensorflow::Tensor output = std::move(outputs.at(0));
    auto scores = output.flat<float>();

    // sorting the inference result to find the top labels
    std::vector<std::pair<float, int>> sorted;
    for (unsigned int i =0; i<10 ;++i){
        sorted.emplace_back(scores(i), i);
    }

    std::sort(sorted.begin(),sorted.end());
    std::reverse(sorted.begin(),sorted.end());

    // get the top 3 of results
    for(unsigned int i = 0 ; i< 10; ++i){
      qDebug() << "The output of the current graph has category  " << sorted[i].second << " with value "<< sorted[i].first;
    }

    return "The predicted digit is: " +  std::to_string(sorted[0].second);
}


cv::Mat
TFTools::doFrozenInference(const cv::Mat &frame)
{
    // Preprocess for the input of WPOD-NET
    float lp_threshold = .9;
    int net_step = int(pow(2.0,4.0));

    cv::Mat new_frame;
    frame.copyTo(new_frame);
    //cv::cvtColor(frame,new_frame,CV_BGR2RGB);

    int img_w = new_frame.size().width; //mat's cols
    int img_h = new_frame.size().height;//mat's rows

    Eigen::Tensor<float, 2> Iorig(2, 1);
    Iorig.setValues({{float(img_w)},{float(img_h)}});

    float ratio = std::max(img_w, img_h) /
            float(std::min(img_w, img_h));

    int side = int(ratio * 288.0);
    int bound_dim = std::min(side + (side % net_step), 608);

    std::cout << "ratio:" << ratio <<
            "side:" << side <<
            "bound_dim" << bound_dim << std::endl;

    new_frame = new_frame/255.f;
    float factor = bound_dim / float(std::min(img_w, img_h));

    int w = int(img_w * factor);
    int h = int(img_h * factor);

    w += (w%net_step!=0)*(net_step - w%net_step);
    h += (h%net_step!=0)*(net_step - h%net_step);

    cv::Mat frame_resized;
    cv::resize(new_frame, frame_resized, cv::Size(w,h), 0, 0);

    std::cout << "...w:" << w << ", ...h:" << h << ", ...factor:" << factor << std::endl;

    int channels = 3;
    tensorflow::Tensor inputImg(tensorflow::DT_FLOAT, tensorflow::TensorShape({ 1, h, w, channels }));
    // copying the frame_resized data into the corresponding tensor via inputImgMat
    float *p = inputImg.flat<float>().data();
    cv::Mat inputImgMat(h, w, CV_32FC3, p);
    frame_resized.convertTo(inputImgMat, CV_32FC3);

    qDebug("...convert mat to intput tensor done...");

    tensor_dict feed_dict = {
        { "input:0", inputImg},
    };

    struct timeval  tv1,tv2;
    gettimeofday(&tv1, NULL);

    std::vector<tensorflow::Tensor> outputs;
    TF_CHECK_OK(_sess->Run(feed_dict, {"concatenate_1/concat:0"}, {}, &outputs));

    gettimeofday(&tv2, NULL);
    float diff = PixmapData::getElapse(&tv1,&tv2);
    qDebug() << "[TFTools] frozen graph costs " << diff << " fms";

    //qDebug() << "...debug for outputs[0]: " << outputs[0].DebugString().data();
    std::vector<int> result_shape = get_tensor_shape(outputs[0]);
    int D_0 = result_shape[0];
    int D_1 = result_shape[1];
    int D_2 = result_shape[2];
    int D_3 = result_shape[3];
    std::cout << "...D_0:" << D_0
              << "...D_1:" << D_1
              << "...D_2:" << D_2
              << "...D_3:" << D_3
              << std::endl;

    auto result = outputs[0].shaped<float, 4>({D_0,D_1,D_2,D_3});
    // get the top 8 of results
    for(unsigned int i = 0 ; i< 8; ++i){
      qDebug() << "data:" << result(0,0,0,i);
    }

    /************ Probs ************/
    Eigen::Tensor<float, 2> Probs(D_1, D_2);
    Probs.setZero();
    for(int i=0; i<D_1; i++){
        for(int j=0; j<D_2; j++){
            Probs(i,j) = result(0,i,j,0);
        }
    }
    //std::cout << "...Probs:" << Probs << std::endl;

    /************ Affines ************/
    Eigen::Tensor<float, 3> Affines(D_1, D_2, 6);
    Affines.setZero();
    for(int i=0; i<D_1; i++){
        for(int j=0; j<D_2; j++){
            for(int k=0; k<6; k++){
                Affines(i,j,k) = result(0,i,j,k+2);
            }
        }
    }
    //std::cout << "...Affines:" << Affines << std::endl;

    /************ ywh, iwh ************/
    std::vector<int> ywh{D_2, D_1};
    std::vector<int> iwh{w, h};
    std::cout << "...ywh:(" << ywh[0] << "," << ywh[1] << ")" << std::endl;
    std::cout << "...iwh:(" << iwh[0] << "," << iwh[1] << ")" << std::endl;

    /************ xx, yy ************/
    std::vector<int> xx;
    std::vector<int> yy;
    for(int i=0; i<D_1; i++){
        for(int j=0; j<D_2; j++){
            if (Probs(i,j) > lp_threshold) {
                std::cout << "...Probs(" << i << "," << j << ")=" << Probs(i,j) << std::endl;
                xx.push_back(i);
                yy.push_back(j);
            }
        }
    }
    std::cout << "...xx:[" << xx[0] << "," << xx[1] << "," << xx[2] << "," << xx[3] << "," << xx[4] << "...]" << std::endl;
    std::cout << "...yy:[" << yy[0] << "," << yy[1] << "," << yy[2] << "," << yy[3] << "," << yy[4] << "...]" << std::endl;

    /************ WH, HW ************/
    Eigen::Tensor<float, 1> WH(2);
    WH(0) = float(w);
    WH(1) = float(h);
    Eigen::Tensor<float, 1> MN(2);
    MN(0) = WH(0) / float(net_step);
    MN(1) = WH(1) / float(net_step);

    float vxx = 0.5; //alpha
    float vyy = 0.5; //alpha
    std::cout << "...WH:" << WH << ",MN:" << MN << std::endl;

    Eigen::Tensor<float, 2> base(3, 4);
    base.setValues({{-vxx,vxx,vxx,-vxx},{-vyy,-vyy,vyy,vyy},{1.0f,1.0f,1.0f,1.0f}});
    std::cout << "...base:" << base << std::endl;

    float new_side = ((208.f + 40.f)/2.f)/ float(net_step);
    /*** calculate label ***/
    std::vector<Label> labels;
    std::vector<Label> final_labels;
    std::vector<Label> TLps;
    for( int i=0; i< xx.size(); i++){
        int y = xx[i];
        int x = yy[i];
        std::cout << "...x:" << x << ",y:" << y << std::endl;
        Eigen::Tensor<float, 1> affine(6);
        for (int ii=0; ii<6; ii++) {
            affine(ii) = Affines(y,x,ii);
        }
        std::cout << "...affine:" << affine << std::endl;

        float prob = Probs(y,x);
        std::cout << "...prob:" << prob << std::endl;
        Eigen::Tensor<float, 1> mn(2);
        mn.setValues({x + 0.5f, y + 0.5f});
        std::cout << "...mn:" << mn << std::endl;

        Eigen::Tensor<float, 2> A(2,3);
        for(int ii=0; ii<2; ii++){
            for(int jj=0; jj<3; jj++){
                A(ii,jj) = affine(ii*3+jj);
            }
        }
        A(0,0) = std::max(A(0,0), 0.0f);
        A(1,1) = std::max(A(1,1), 0.0f);
        std::cout << "...A:" << A << std::endl;

        Eigen::array<Eigen::IndexPair<int>, 1> product_dims = { Eigen::IndexPair<int>(1, 0) };
        Eigen::Tensor<float, 2> pts = A.contract(base, product_dims); //alpha
        std::cout << "...pts:" << pts << std::endl;

        Eigen::Tensor<float, 2> pts_MN_center_mn = pts * new_side;
        std::cout << "...pts_MN_center_mn:" << pts_MN_center_mn << std::endl;

        Eigen::array<int, 2> pts_mn_dims{{2,1}};
        Eigen::Tensor<float, 2> mn_ret = mn.reshape(pts_mn_dims);
        std::cout << "...mn_ret:" << mn_ret << std::endl;

        Eigen::Tensor<float, 2> pts_MN(2,4);
        for(int ii=0; ii<4; ii++){
            pts_MN(0,ii) = pts_MN_center_mn(0,ii) + mn_ret(0,0);
            pts_MN(1,ii) = pts_MN_center_mn(1,ii) + mn_ret(1,0);
        }
        std::cout << "...pts_MN:" << pts_MN << std::endl;

        Eigen::array<int, 2> pts_prop_dims{{2,1}};
        Eigen::Tensor<float, 2> MN_ret = MN.reshape(pts_prop_dims);
        Eigen::Tensor<float, 2> pts_prop(2,4);
        for(int ii=0; ii<4; ii++){
            pts_prop(0,ii) = pts_MN(0,ii) / MN_ret(0,0);
            pts_prop(1,ii) = pts_MN(1,ii) / MN_ret(1,0);
        }
        std::cout << "...pts_prop:" << pts_prop << std::endl;

        Label my_label = Label(0,prob,pts_prop);
        labels.push_back(my_label);
    }

    final_labels = WPOD::nms(labels, 0.1f);
    std::cout << "...Finish nms()..." << std::endl;

    cv::Mat new_frame_dst;
    if (final_labels.size() > 0){
        //sort final_labels in reverse order
        std::sort(final_labels.begin(), final_labels.end(),
                  [](const Label& lhs, const Label& rhs) {
            return lhs._prob > rhs._prob;
        });
        for(auto my_final_label: final_labels){
            std::cout << "...Final Label in the reverse order:" << my_final_label.getStr() << std::endl;
        }

        Eigen::Tensor<float, 2> t_ptsh(3, 4);
        t_ptsh.setValues({{0.0f,240.0f,240.0f,0.0f},
                          {0.0f,0.0f,80.0f,80.0f},
                          {1.0f,1.0f,1.0f,1.0f}});

        for(auto label: final_labels){
            //Eigen::array<Eigen::IndexPair<int>, 1> product_dims = { Eigen::IndexPair<int>(1, 0)};
            Eigen::Tensor<float, 2> ptsh_before = label.pts();
            Eigen::Tensor<float, 2> ptsh(3,4);
            ptsh.setConstant(1.0f);
            for(int ii=0; ii<2; ii++) {
                for(int jj=0; jj<4; jj++) {
                    ptsh(ii,jj) = ptsh_before(ii,jj) * Iorig(ii);
                }
            }
            std::cout << "...ptsh:" << ptsh << std::endl;
            cv::Mat H = WPOD::find_T_matrix(ptsh,t_ptsh);

            cv::warpPerspective(new_frame,new_frame_dst,H,cv::Size(240,80),.0);
            cv::cvtColor(new_frame_dst, new_frame_dst, cv::COLOR_BGR2GRAY);
            cv::cvtColor(new_frame_dst, new_frame_dst, cv::COLOR_GRAY2BGR);
            new_frame_dst *= 255.f;

            Shape s(label.pts(),4,"");
            std::cout << "...pts:" << label.pts() << std::endl;

            return new_frame_dst;
        }
    }
    return new_frame_dst;
    ;
}
