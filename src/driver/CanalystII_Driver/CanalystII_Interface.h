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

#include "../CanInterface.h"
#include <core/MeasurementInterface.h>
#include <QMutex>
#include <QQueue>

#include "api/canalyst.h"

class CanalystII_Driver;

typedef struct {
    bool supports_canfd;
    bool supports_timing;
    uint32_t state;
    uint32_t base_freq;
    uint32_t sample_point;
    uint32_t ctrl_mode;
    uint32_t restart_ms;
} can_config_t;

typedef struct {
    uint32_t can_state;

    uint64_t rx_count;
    int rx_errors;
    uint64_t rx_overruns;

    uint64_t tx_count;
    int tx_errors;
    uint64_t tx_dropped;
} can_status_t;

class CanalystII_Interface: public CanInterface {
public:
    CanalystII_Interface(CanalystII_Driver *driver, int index, QString name, bool fd_support = false);
    virtual ~CanalystII_Interface();

    QString getDetailsStr() const;
    virtual QString getName() const;
    void setName(QString name);

    virtual QList<CanTiming> getAvailableBitrates();

    virtual void applyConfig(const MeasurementInterface &mi);
    virtual bool readConfig();
    virtual bool readConfigFromLink(struct rtnl_link *link);

    bool supportsTimingConfiguration();
    bool supportsCanFD();
    bool supportsTripleSampling();

    virtual unsigned getBitrate();
    virtual uint32_t getCapabilities();


	virtual void open();
    virtual void close();
    virtual bool isOpen();

    virtual void sendMessage(const CanMessage &msg);
    virtual bool readMessage(QList<CanMessage> &msglist, unsigned int timeout_ms);

    virtual bool updateStatistics();
    virtual uint32_t getState();
    virtual int getNumRxFrames();
    virtual int getNumRxErrors();
    virtual int getNumRxOverruns();

    virtual int getNumTxFrames();
    virtual int getNumTxErrors();
    virtual int getNumTxDropped();

    int getIfIndex();

private:
    typedef enum {
        ts_mode_SIOCSHWTSTAMP,
        ts_mode_SIOCGSTAMPNS,
        ts_mode_SIOCGSTAMP
    } ts_mode_t;

    int _idx;
    bool _isOpen;
    // QSerialPort* _serport;
    QQueue<Canalyst_CANMessage> _msg_queue;
    QMutex _serport_mutex;
    QString _name;
    // char _rx_linbuf[SLCAN_MTU];
    // int _rx_linbuf_ctr;

    // char _rxbuf[RXCIRBUF_LEN];
    // uint32_t _rxbuf_head;
    // uint32_t _rxbuf_tail;
    QByteArray _rxbuf;

    QMutex _rxbuf_mutex;
    MeasurementInterface _settings;

    can_config_t _config;
    can_status_t _status;
    ts_mode_t _ts_mode;

    Canalyst_Device _device;

    bool updateStatus();
    bool parseProtocol(CanMessage &msg, Canalyst_CANMessage _canalyst_msg);
};
