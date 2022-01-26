//
// Created by shenk on 20.01.2022.
//

#include "gui_main.h"

int gui_main(int argc, char **argv) {

    qRegisterMetaType<ReceiveTimestamp>("ReceiveTimestamp");
    qRegisterMetaType<std::optional<SendTimestamp>>("std::optional<SendTimestamp>");

    QApplication a(argc, argv);

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
