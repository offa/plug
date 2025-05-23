/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2025  offa
 * Copyright (C) 2010-2016  piorekf <piorek@piorekf.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "com/UsbContext.h"
#include "com/Mustang.h"
#include "ui/mainwindow.h"
#include "Version.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};
    QCoreApplication::setOrganizationName("offa");
    QCoreApplication::setApplicationName("Plug");
    QCoreApplication::setApplicationVersion(QString::fromStdString(plug::version()));

    plug::com::usb::Context context{};

    plug::MainWindow window;
    window.show();

    return app.exec();
}
