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

#pragma once

#include <QString>
#include <core/Backend.h>
#include <driver/CanDriver.h>
#include "api/canalyst.h"

class CanalystII_Interface;
class SetupDialogInterfacePage;
class GenericCanSetupPage;

class CanalystII_Driver: public CanDriver {
public:
    CanalystII_Driver(Backend &backend);
    virtual ~CanalystII_Driver();

    virtual QString getName();
    virtual bool update();

private:
    CanalystII_Interface *createOrUpdateInterface(int index, QString name, bool fd_support);
    GenericCanSetupPage *setupPage;

    Canalyst_Device _device;
};
