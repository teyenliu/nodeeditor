#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/ConnectionStyle>
#include <nodes/TypeConverter>

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>

#include <nodes/DataModelRegistry>

#include "NumberSourceDataModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "AdditionModel.hpp"
#include "SubtractionModel.hpp"
#include "MultiplicationModel.hpp"
#include "DivisionModel.hpp"
#include "ModuloModel.hpp"
#include "Converters.hpp"


using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::ConnectionStyle;
using QtNodes::TypeConverter;
using QtNodes::TypeConverterId;

static std::shared_ptr<DataModelRegistry>
registerDataModels()
{
  auto ret = std::make_shared<DataModelRegistry>();
  ret->registerModel<NumberSourceDataModel>("Sources");

  ret->registerModel<NumberDisplayDataModel>("Displays");

  ret->registerModel<AdditionModel>("Operators");

  ret->registerModel<SubtractionModel>("Operators");

  ret->registerModel<MultiplicationModel>("Operators");

  ret->registerModel<DivisionModel>("Operators");

  ret->registerModel<ModuloModel>("Operators");

  ret->registerTypeConverter(std::make_pair(DecimalData().type(),
                                            IntegerData().type()),
                             TypeConverter{DecimalToIntegerConverter()});



  ret->registerTypeConverter(std::make_pair(IntegerData().type(),
                                            DecimalData().type()),
                             TypeConverter{IntegerToDecimalConverter()});

  return ret;
}


static
void
setStyle()
{
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
  QApplication app(argc, argv);

  setStyle();

  QWidget mainWidget;

  auto saveButtion = new QPushButton("Save..");
  auto loadButtion = new QPushButton("Load..");
  QHBoxLayout *button_layout = new QHBoxLayout();
  button_layout->addWidget(saveButtion);
  button_layout->addWidget(loadButtion);

  QVBoxLayout *l = new QVBoxLayout(&mainWidget); 
  auto scene = new FlowScene(registerDataModels(), &mainWidget);

  l->addLayout(button_layout);
  l->addWidget(new FlowView(scene));
  l->setContentsMargins(0, 0, 0, 0);
  l->setSpacing(0);

  QObject::connect(saveButtion, &QPushButton::clicked,
                   scene, &FlowScene::save);

  QObject::connect(loadButtion, &QPushButton::clicked,
                   scene, &FlowScene::load);

  mainWidget.setWindowTitle("Dataflow tools: simplest calculator");
  mainWidget.resize(800, 600);
  mainWidget.showNormal();

  return app.exec();
}
