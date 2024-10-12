/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2024  offa
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
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};
    QCoreApplication::setOrganizationName("offa");
    QCoreApplication::setApplicationName("Plug");
    QCoreApplication::setApplicationVersion(QString::fromStdString(plug::version()));

    // following https://doc.qt.io/qt-6/qcommandlineparser.html
    QCommandLineParser parser;
    parser.setApplicationDescription("Linux program to provide similar services to Fender Fuse/Fender Tone");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption enableV3UsbDevicesOption(
        "enable-v3usb-devices",
        QCoreApplication::translate(
            "main",
            "Enable incomplete support for USB connected V3 devices controllable with Windows/macOS FenderTone applications "
            "(Mustang LT25/LT40S/LT50, Rumble LT 25)."
            // TODO: Mustang LT50 PID not integrated in DeviceModel.cpp yet
            ));
    parser.addOption(enableV3UsbDevicesOption);
    parser.process(app);

    plug::com::usb::Context context{};

    if (parser.isSet(enableV3UsbDevicesOption))
    {
        plug::MainWindow::enable_v3usb_devices();
    }
    plug::MainWindow window;
    

    window.show();

    return app.exec();
}
