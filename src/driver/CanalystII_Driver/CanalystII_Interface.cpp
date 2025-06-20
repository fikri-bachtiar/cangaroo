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

#include "CanalystII_Interface.h"
// #include "api/sam_uart2can_proto.h"

#include <core/Backend.h>
#include <core/MeasurementInterface.h>
#include <core/CanMessage.h>

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QThread>

CanalystII_Interface::CanalystII_Interface(CanalystII_Driver *driver, int index, QString name, bool fd_support)
  : CanInterface((CanDriver *)driver),
	_idx(index),
    _isOpen(false),
    // _serport(NULL),
    _msg_queue(),
    _name(name),
    // _rx_linbuf_ctr(0),
    // _rxbuf_head(0),
    // _rxbuf_tail(0),
    _ts_mode(ts_mode_SIOCSHWTSTAMP)
{
    // Set defaults
    // _settings.setBitrate(500000);
    _settings.setBitrate(250000);
    _settings.setSamplePoint(875);

    _config.supports_canfd = fd_support;
}

CanalystII_Interface::~CanalystII_Interface() {
}

QString CanalystII_Interface::getDetailsStr() const {
    if(_config.supports_canfd)
    {
        return "USBCAN/CANalyst-II with CANFD support";
    }
    else
    {
        return "USBCAN/CANalyst-II with standard CAN support";
    }
}

QString CanalystII_Interface::getName() const {
	return _name;
}

void CanalystII_Interface::setName(QString name) {
    _name = name;
}

QList<CanTiming> CanalystII_Interface::getAvailableBitrates()
{
    QList<CanTiming> retval;
    QList<unsigned> bitrates({10000, 20000, 50000, 100000, 125000, 250000, 500000, 800000, 1000000});
    QList<unsigned> bitrates_fd({0, 2000000, 5000000});

    QList<unsigned> samplePoints({875});

    unsigned i=0;
    foreach (unsigned br, bitrates) {
        foreach(unsigned br_fd, bitrates_fd) {
            foreach (unsigned sp, samplePoints) {
                retval << CanTiming(i++, br, br_fd, sp);
            }
        }
    }

    return retval;
}


void CanalystII_Interface::applyConfig(const MeasurementInterface &mi)
{
    // Save settings for port configuration
    _settings = mi;
}

bool CanalystII_Interface::updateStatus()
{
    return false;
}

bool CanalystII_Interface::readConfig()
{
    return false;
}

bool CanalystII_Interface::readConfigFromLink(rtnl_link *link)
{
    return false;
}

bool CanalystII_Interface::supportsTimingConfiguration()
{
    return _config.supports_timing;
}

bool CanalystII_Interface::supportsCanFD()
{
    return _config.supports_canfd;
}

bool CanalystII_Interface::supportsTripleSampling()
{
    return false;
}

unsigned CanalystII_Interface::getBitrate()
{
    return _settings.bitrate();
}

uint32_t CanalystII_Interface::getCapabilities()
{
    uint32_t retval =
        CanInterface::capability_config_os;

    if (supportsCanFD()) {
        retval |= CanInterface::capability_canfd;
    }

    if (supportsTripleSampling()) {
        retval |= CanInterface::capability_triple_sampling;
    }

    return retval;
}

bool CanalystII_Interface::updateStatistics()
{
    return updateStatus();
}

uint32_t CanalystII_Interface::getState()
{
    if(_isOpen)
        return state_ok;
    else
        return state_bus_off;
}

int CanalystII_Interface::getNumRxFrames()
{
    return _status.rx_count;
}

int CanalystII_Interface::getNumRxErrors()
{
    return _status.rx_errors;
}

int CanalystII_Interface::getNumTxFrames()
{
    return _status.tx_count;
}

int CanalystII_Interface::getNumTxErrors()
{
    return _status.tx_errors;
}

int CanalystII_Interface::getNumRxOverruns()
{
    return _status.rx_overruns;
}

int CanalystII_Interface::getNumTxDropped()
{
    return _status.tx_dropped;
}

int CanalystII_Interface::getIfIndex() {
    return _idx;
}

void CanalystII_Interface::open()
{
    _serport_mutex.lock();
    
    qDebug() << "Opening Canalyst-II interface: " << _name << _settings.bitrate();
    if(canalyst_dev_init(&_device, _settings.bitrate())) {
        // _serport_mutex.unlock();
        perror("CanalystII connected!");
    } else {
        perror("CanalystII connect failed!");
        _serport_mutex.unlock();
        _isOpen = false;
        return;
    }

    _isOpen = true;
    
    // Release port mutex
    _serport_mutex.unlock();
}

void CanalystII_Interface::close()
{
    _serport_mutex.lock();

    canalyst_dev_close(&_device);

    _isOpen = false;
    _serport_mutex.unlock();
}

bool CanalystII_Interface::isOpen()
{
    return _isOpen;
}

void CanalystII_Interface::sendMessage(const CanMessage &msg) {

    Canalyst_CANMessage canalyst_msg;
    canalyst_msg.can_id = msg.getId();
    canalyst_msg.extended = msg.isExtended();
    canalyst_msg.remote = msg.isRTR();
    canalyst_msg.data_len = msg.getLength();
    for(int i = 0; i < canalyst_msg.data_len; i++) {
        canalyst_msg.data[i] = msg.getByte(i);
    }

    _msg_queue.enqueue(canalyst_msg);
}

bool CanalystII_Interface::readMessage(QList<CanMessage> &msglist, unsigned int timeout_ms)
{
    // Don't saturate the thread. Read the buffer every 1ms.
    QThread().msleep(1);

    // Transmit all items that are queued
    while(!_msg_queue.empty())
    {
        _serport_mutex.lock();

        // Consume first item
        Canalyst_CANMessage tmp = _msg_queue.dequeue();

        // Write string to serial device
        canalyst_dev_frame_send(&_device, &tmp);
        // qDebug() << "UART[out] | len: " << tmp.length() << "|" << tmp.toHex();

        _serport_mutex.unlock();
    }

    bool ret = false;

    // Example receive
    Canalyst_CANMessage *rx_msgs = NULL;
    int rx_count = 0;
    if (canalyst_dev_frame_read(&_device, &rx_msgs, &rx_count) < 0) {
        // fprintf(stderr, "Failed to receive messages\n");
        return ret;
    } else if (rx_count > 0) {
        // printf("Received %d messages\n", rx_count);
        for (int i = 0; i < rx_count; i++) {
            _rxbuf_mutex.lock();
            CanMessage msg;
            ret = parseProtocol(msg, rx_msgs[i]);
            if(ret)
                msglist.append(msg);
            _rxbuf_mutex.unlock();

            ret = true;
        }
        free(rx_msgs);
    }

    return ret;
}

bool CanalystII_Interface::parseProtocol(CanMessage &msg, Canalyst_CANMessage _canalyst_msg)
{
    // Set timestamp to current time
    struct timeval tv;
    gettimeofday(&tv,NULL);
    msg.setTimestamp(tv);

    // Parse the Canalyst_CANMessage into CanMessage
    msg.setInterfaceId(getId());
    msg.setId(_canalyst_msg.can_id);
    msg.setExtended(_canalyst_msg.extended);
    msg.setRTR(_canalyst_msg.remote);
    msg.setLength(_canalyst_msg.data_len);

    for(int i = 0; i < _canalyst_msg.data_len; i++) {
        msg.setByte(i, _canalyst_msg.data[i]);
    }

    return true;
}