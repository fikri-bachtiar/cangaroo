/*

  Copyright (c) 2025 Fikri Bachtiar

  This file is part of cangaroo.

  cangaroo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  cangaroo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with cangaroo.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "CanalystII_Driver.h"
#include "CanalystII_Interface.h"
#include <core/Backend.h>
#include <driver/GenericCanSetupPage.h>

#include <errno.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//
#include <QCoreApplication>
#include <QDebug>

CanalystII_Driver::CanalystII_Driver(Backend &backend)
  : CanDriver(backend),
    setupPage(new GenericCanSetupPage())
{
    QObject::connect(&backend, SIGNAL(onSetupDialogCreated(SetupDialog&)), setupPage, SLOT(onSetupDialogCreated(SetupDialog&)));
}

CanalystII_Driver::~CanalystII_Driver() {
}


bool CanalystII_Driver::update() {

    deleteAllInterfaces();

    int interface_cnt = 0;
    
    // Scan for Canalyst-II devices
    if (!canalyst_dev_scan(&_device)) {
        // qDebug() << "Canalyst-II device scan failed: " << strerror(errno);

        canalyst_dev_close(&_device);
        return false;
    }

    fprintf(stderr, "Name : %s \r\n", canalyst_get_name());
    fprintf(stderr, "   PID : 0x%04x \r\n", USB_ID_PRODUCT);
    fprintf(stderr, "   VID: 0x%04x \r\n", USB_ID_VENDOR);
    perror("   ++ Chuangxin Tech USBCAN/CANalyst-II");

    CanalystII_Interface *intf = createOrUpdateInterface(interface_cnt, canalyst_get_name(), false);
    interface_cnt++;

    canalyst_dev_close(&_device);
    return true;
}

QString CanalystII_Driver::getName() {
    return "USBCAN/CANalyst-II";
}



CanalystII_Interface *CanalystII_Driver::createOrUpdateInterface(int index, QString name, bool fd_support) {

    foreach (CanInterface *intf, getInterfaces()) {
        CanalystII_Interface *scif = dynamic_cast<CanalystII_Interface*>(intf);
		if (scif->getIfIndex() == index) {
			scif->setName(name);
            return scif;
		}
	}


    CanalystII_Interface *scif = new CanalystII_Interface(this, index, name, fd_support);
    addInterface(scif);
    return scif;
}
