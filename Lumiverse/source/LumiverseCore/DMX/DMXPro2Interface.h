/*! \file DMXPro2Interface.h
* \brief Class for using an ENTTEC DMX USB Pro Mk 2 device
*/
#ifndef _DMXPRO2INTERFACE_H_
#define _DMXPRO2INTERFACE_H_

#pragma once
#include "LumiverseCoreConfig.h"

// Note: If you want Doxygen to generate documentation for this class, 
// you must have the following line uncommented.
// #define USE_DMXPRO2

#ifdef USE_DMXPRO2

#include "DMXInterface.h"
#include "pro_driver.h"
#include "../Logger.h"
#include "../lib/libjson/libjson.h"
#include <chrono>
#include <thread>
#include <cstring>
#include <sstream>

namespace Lumiverse {
  /*!
  * \class Lumiverse::DMXPro2Interface
  * \brief Class for using an ENTTEC DMX USB Pro Mk 2 Interface.
  */
  class DMXPro2Interface : public DMXInterface
  {
  public:
    // Makes a new interface for the ENTTEC DMX USB Pro Mk 2 interface.
    // If you have more than one interface connected, specify which interface
    // to open in the constructor. Default maps output 1 to universe 1 and output
    // 2 to universe 2 and opens the first found D2XX device.
    DMXPro2Interface(string id, int proNum = 0, int out1 = 0, int out2 = 1);

    // Destroys the interface (not the physical one)
    virtual ~DMXPro2Interface();

    // Initializes the DMX USB Pro Mk 2 device.
    virtual void init();

    // Sends DMX through the interface. Also includes the universe number, for
    // devices that may manage more than one universe or need to know the universe number (like this one)
    // NOTE: The universe passed in here is ZERO-INDEXED as it is in the DMXPatch class.
    virtual void sendDMX(unsigned char* data, unsigned int universe);

    // Closes the connection to the DMX device
    virtual void closeInt();

    // Resets the connected interface.
    // I think this function will actually kick out D2XX devices...
    // Might want to only use this in emergencies right now.
    virtual void reset();

    // Gets the JSON version of this object
    virtual JSONNode toJSON();

    virtual string getInterfaceType() { return "DMXPro2Interface"; }

    // Functions from the FTDI interface will be public in the event that someone needs
    // to access a certain property specific to this device.
    // Most of these functions were adapted from ENTTEC's sample code.

    /* Function : FTDI_ListDevices
    * Author	: ENTTEC
    * Purpose  : Returns the no. of PRO's conneced to the PC
    * Parameters: none
    **/
    int listDevices();

    /* Function : FTDI_SendData
    * Author	: ENTTEC
    * Purpose  : Send Data (DMX or other packets) to the PRO
    * Parameters: Label, Pointer to Data Structure, Length of Data
    **/
    int sendData(int label, unsigned char *data, int length);

    /* Function : FTDI_ReceiveData
    * Author	: ENTTEC
    * Purpose  : Receive Data (DMX or other packets) from the PRO
    * Parameters: Label, Pointer to Data Structure, Length of Data
    **/
    int receiveData(int label, unsigned char *data, unsigned int expected_length);

    /* Function : FTDI_OpenDevice
    * Author	: ENTTEC
    * Purpose  : Opens the PRO; Tests various parameters; outputs info
    * Parameters: device num (returned by the List Device fuc), Fw Version MSB, Fw Version LSB
    **/
    bool openDevice(int device_num);

    /* Function : FTDI_PurgeBuffer
    * Author	: ENTTEC
    * Purpose  : Clears the buffer used internally by the PRO
    * Parameters: none
    **/
    void purgeBuffer();

    // Settings for the two DMX/MIDI ports for the interface.
    // port1 - 0 = disabled, 1 = enabled for DMX
    // port2 - 0 = disabled, 1 = enabled for DMX, 2 = enabled for MIDI In and Out
    void setPorts(uint8_t port1, uint8_t port2);

    int getProNum() { return m_proNum; }
    void setProNum(int proNum) { m_proNum = proNum; }

    void setOut1Universe(int u) { m_out1Universe = u; }
    int getOut1Universe() { return m_out1Universe; }

    void setOut2Universe(int u) { m_out2Universe = u; }
    int getOut2Universe() { return m_out2Universe; }

  private:
    // Specifies which DMX PRO interface numnber is controlled by this instance
    int m_proNum;

    // Indicates if a device is connected.
    bool m_connected;

    // Handle to the connected interface.
    FT_HANDLE m_deviceHandle;

    // Parameters for the interface
    DMXUSBPROParamsType m_PROParams;

    // The universe that the first output is assigned to.
    int m_out1Universe;

    // The universe that the second output is assigned to.
    int m_out2Universe;

    // Defined by the DMX open function originally. Shouldn't need to adjust
    // unless there are problems.

    // Timeout in microseconds. Too high or too low values discouraged.
    int m_readTimeout;

    // Timeout in microseconds. Too high or too low values discouraged.
    int m_writeTimeout;

    int m_versionMSB;
    int m_versionLSB;
  };
}
#endif

#endif