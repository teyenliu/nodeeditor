#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/ConnectionStyle>
#include <nodes/FlowViewStyle>
#include <QtWidgets/QApplication>
#include <QCoreApplication>
#include <QAction>
#include <QToolBar>
#include <QMenuBar>

#include "ImageShowModel.hpp"
#include "ImageLoaderModel.hpp"
#include "ImageOpenCVModel.hpp"
#include "ImageGrayModel.hpp"
#include "ImageBilateralFilterModel.hpp"
#include "ImageMedianBlurModel.hpp"
#include "ImageTFInfModel.hpp"
#include "ImageTFLiteInfModel.hpp"
#include "ImageTVMInfModel.hpp"

//DDS Pub/Sub Components
#include "ImageDDSPubModel.hpp"
#include "ImageDDSSubModel.hpp"

// FanOut Component
#include "ImageFanOutModel.hpp"

#include "ImageCannyModel.hpp"
#include "ImageErodeModel.hpp"
#include "VideoLoaderModel.hpp"
#include "VideoCVModel.hpp"
#include "VideoTCPSocketModel.hpp"
#include "ImageCVYOLOModel.hpp"
#include "ImageOCR_YOLOModel.hpp"
#include "ImageWPOD_NETModel.hpp"
#include "dragwidget.hpp"


using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::FlowViewStyle;
using QtNodes::ConnectionStyle;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<ImageShowModel>("Image");
  ret->registerModel<ImageLoaderModel>("Image");
  // ImageOpenCVModel has a pure virtual function
  //ret->registerModel<ImageOpenCVModel>();
  ret->registerModel<ImageGrayModel>("Image");
  ret->registerModel<ImageBilateralFilterModel>("Image");
  ret->registerModel<ImageMedianBlurModel>("Image");
  ret->registerModel<ImageTFInfModel>("Image");
  ret->registerModel<ImageTFLiteInfModel>("Image");
  ret->registerModel<ImageTVMInfModel>("Image");
  ret->registerModel<VideoCVModel>("Video");
  ret->registerModel<ImageCannyModel>("Image");
  ret->registerModel<ImageDDSPubModel>("Image");
  ret->registerModel<ImageDDSSubModel>("Image");
  ret->registerModel<ImageErodeModel>("Image");
  ret->registerModel<VideoLoaderModel>("Video");
  ret->registerModel<VideoTCPSocketModel>("Video");
  ret->registerModel<ImageCVYOLOModel>("Image");
  ret->registerModel<ImageOCR_YOLOModel>("Image");
  ret->registerModel<ImageWPOD_NETModel>("Image");
  ret->registerModel<ImageFanOutModel>("Image");

  return ret;
}

static
void
setStyle()
{
  FlowViewStyle::setStyle(
  R"(
  {
    "FlowViewStyle": {
      "BackgroundColor": [255, 255, 240],
      "FineGridColor": [245, 245, 230],
      "CoarseGridColor": [235, 235, 220]
    }
  }
  )");

  ConnectionStyle::setConnectionStyle(
  R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "black",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": true
    }
  }
  )");
}


int
main(int argc, char *argv[])
{
  //Q_INIT_RESOURCE(draggableicons);

  QApplication app(argc, argv);
  setStyle();
  std::shared_ptr<DataModelRegistry> reg = registerDataModels();
  QWidget mainWidget;

  //QHBoxLayout *button_layout = new QHBoxLayout();
  //auto saveButtion = new QPushButton("Save..");
  //auto loadButtion = new QPushButton("Load..");
  //button_layout->addWidget(saveButtion);
  //button_layout->addWidget(loadButtion);

  // Use QAction
  QAction *saveAction = new QAction(QIcon("/usr/local/tf_libs/resources/icons/save.png"), "Save");
  //saveAction->setShortcuts(QKeySequence::Save);
  saveAction->setStatusTip("Save Model");
  QAction *loadAction = new QAction(QIcon("/usr/local/tf_libs/resources/icons/load.png"), "Load");
  loadAction->setStatusTip("Load Model");

  QToolBar *toolBar = new QToolBar(&mainWidget);
  toolBar->addAction(saveAction);
  toolBar->addAction(loadAction);


  auto scene = new FlowScene(reg, &mainWidget);

  QHBoxLayout *flow_layout = new QHBoxLayout();
  flow_layout->addWidget(new DragWidget(&mainWidget, reg));
  flow_layout->addWidget(new FlowView(scene));

  QVBoxLayout *l = new QVBoxLayout(&mainWidget);
  l->addWidget(toolBar);
  l->addLayout(flow_layout);
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  //QObject::connect(saveButtion, &QPushButton::clicked,
  //                 scene, &FlowScene::save);
  QObject::connect(saveAction, &QAction::triggered, scene, &FlowScene::save);
  //QObject::connect(loadButtion, &QPushButton::clicked,
  //                 scene, &FlowScene::load);
  QObject::connect(loadAction, &QAction::triggered, scene, &FlowScene::load);


  mainWidget.setWindowTitle("Node-based image preprocessing and inference tool");
  mainWidget.resize(1024, 768);
  mainWidget.showNormal();

  return app.exec();
}
