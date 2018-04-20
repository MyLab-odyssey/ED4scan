//--------------------------------------------------------------------------------
// (c) 2018 by MyLab-odyssey
//
// Licensed under "MIT License (MIT)", see license file for more information.
//
// THIS SOFTWARE IS ONLY INTENDED FOR SCIENTIFIC USAGE
// AND IS PROVIDED BY THE COPYRIGHT HOLDER OR CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//--------------------------------------------------------------------------------
//! \file    canDiagED4.cpp
//! \brief   Library module for retrieving diagnostic data.
//! \date    2018-April
//! \author  MyLab-odyssey
//! \version 0.4.3
//--------------------------------------------------------------------------------
#include "canDiagED4.h"

//--------------------------------------------------------------------------------
//! \brief   Standard constructor / destructor
//--------------------------------------------------------------------------------
canDiag::canDiag() {
}

canDiag::~canDiag() {
  delete[] data;
}

//--------------------------------------------------------------------------------
//! \brief   Manage memory for cell statistics
//--------------------------------------------------------------------------------
void canDiag::reserveMem_CellVoltage() {
  CellVoltage.init(CELLCOUNT);
}

void canDiag::reserveMem_CellCapacity() {
  CellCapacity.init(CELLCOUNT);
  //Serial.print(F("RAM: ")); Serial.println(this->_getFreeRam());
}

void canDiag::freeMem_CellVoltage() {
  CellVoltage.freeMem();
}

void canDiag::freeMem_CellCapacity() {
  CellCapacity.freeMem();
  //Serial.print(F("RAM: ")); Serial.println(this->_getFreeRam());
}

//--------------------------------------------------------------------------------
//! \brief   Memory available between Heap and Stack, works only on UNO!
//--------------------------------------------------------------------------------
int canDiag::_getFreeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

//--------------------------------------------------------------------------------
//! \brief   Get method for CellVoltages
//--------------------------------------------------------------------------------
uint16_t canDiag::getCellVoltage(byte n) {
  return CellVoltage.get(n);
}

//--------------------------------------------------------------------------------
//! \brief   Get method for CellCapacities
//--------------------------------------------------------------------------------
uint16_t canDiag::getCellCapacity(byte n) {
  return CellCapacity.get(n);
}

//--------------------------------------------------------------------------------
//! \brief   Initialize CAN-Object and MCP2515 Controller
//--------------------------------------------------------------------------------
void canDiag::begin(MCP_CAN *_myCAN, CTimeout *_myCAN_Timeout) {
  //Set Pointer to MCP_CANobj
  myCAN0 = _myCAN;
  myCAN_Timeout = _myCAN_Timeout;

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters enabled.
  if (myCAN0->begin(MCP_STD, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    DEBUG_UPDATE(F("MCP2515 Init Okay!!\r\n"));
  } else {
    DEBUG_UPDATE(F("MCP2515 Init Failed!!\r\n"));
  }
  this->data = new byte[DATALENGTH];
}

//--------------------------------------------------------------------------------
//! \brief   Clear CAN ID filters.
//--------------------------------------------------------------------------------
void canDiag::clearCAN_Filter() {
  myCAN0->init_Mask(0, 0, 0x00000000);
  myCAN0->init_Mask(1, 0, 0x00000000);
  //delay(100);
  myCAN0->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
}

//--------------------------------------------------------------------------------
//! \brief   Set all filters to one CAN ID.
//--------------------------------------------------------------------------------
void canDiag::setCAN_Filter(unsigned long filter) {
  this->respID = filter;
  filter = filter << 16;
  myCAN0->init_Mask(0, 0, 0x07FF0000);
  myCAN0->init_Mask(1, 0, 0x07FF0000);
  myCAN0->init_Filt(0, 0, filter);
  myCAN0->init_Filt(1, 0, filter);
  myCAN0->init_Filt(2, 0, filter);
  myCAN0->init_Filt(3, 0, filter);
  myCAN0->init_Filt(4, 0, filter);
  myCAN0->init_Filt(5, 0, filter);
  //delay(100);
  myCAN0->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
}


void canDiag::setCAN_Filter_DRV() {
  myCAN0->init_Mask(0, 0, 0x07FF0000);
  myCAN0->init_Mask(1, 0, 0x07FF0000);
  myCAN0->init_Filt(0, 0, 0x02000000);
  myCAN0->init_Filt(1, 0, 0x03180000);
  myCAN0->init_Filt(2, 0, 0x03CE0000);
  myCAN0->init_Filt(3, 0, 0x03F20000);
  myCAN0->init_Filt(4, 0, 0x03D70000);
  myCAN0->init_Filt(5, 0, 0x05040000);
  //delay(100);
  myCAN0->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
}

//--------------------------------------------------------------------------------
//! \brief   Set request CAN ID and response CAN ID for get functions
//--------------------------------------------------------------------------------
void canDiag::setCAN_ID(unsigned long _respID) {
  if (this->respID != _respID) {
    this->setCAN_Filter(_respID);
  }
  this->respID = _respID;
}
void canDiag::setCAN_ID(unsigned long _rqID, unsigned long _respID) {
  rqID = _rqID;
  if (this->respID != _respID) {
    this->setCAN_Filter(_respID);
  }
  this->respID = _respID;
}

//--------------------------------------------------------------------------------
//! \brief   Set method for capacity readout (As/10 or As/100)
//--------------------------------------------------------------------------------
void canDiag::setCAPmode(byte _mode) {
  CapMode = _mode;
}

//--------------------------------------------------------------------------------
//! \brief   Try to wakeup EV CAN bus ***experimental and not working right now!***
//--------------------------------------------------------------------------------
boolean canDiag::WakeUp() {
  //--- Send WakeUp Pattern ---
  DEBUG_UPDATE(F("Send WakeUp Request\n\r"));
  //myCAN0->sendMsgBuf(0x423, 0, 7, rqWakeUp);    // send data: Request diagnostics data, 423!, 452?, 236?
  return true;
}

//--------------------------------------------------------------------------------
//! \brief   Send diagnostic request to ECU.
//! \param   byte* rqQuery
//! \see     rqBattADCref ... rqBattVolts
//! \return  received lines count (uint16_t) of function #Get_RequestResponse
//--------------------------------------------------------------------------------
uint16_t canDiag::Request_Diagnostics(const byte *_rqQuery) {

  // Detect skip data mode; size of request standard 4 parameter; skip enable (start param 5, stop param 6)
  if (SkipEnable) {
    //Copy request from prog memory with skip start and end
    /*memcpy_P(rqMsg + 1, _rqQuery, 5 * sizeof(byte)); // Fill byte 01 to 04 of rqMsg with rqQuery content (from PROGMEM)
      SkipStart = rqMsg[4];
      SkipEnd = rqMsg[5];
      rqMsg[4] = 0x00; rqMsg[5] = 0x00; // mark elements 5, 6 as unused data for correct query */
    //Serial.print("> "); Serial.print(SkipStart); Serial.print(" > "); Serial.println(SkipEnd);
  } else {
    //Copy request from prog memory and fill up for UDS request size of 8 parameters
    memcpy_P(rqMsg + 1, _rqQuery, 3 * sizeof(byte)); // Fill byte 02 to 04 of rqMsg with rqQuery content (from PROGMEM)
  }

  //Set Length of Query
  if (rqMsg[1] == 0x21) rqMsg[0] = 0x02; //Set Query Length to 2
  if (rqMsg[1] == 0x10) rqMsg[0] = 0x02; //Set Query Length to 2
  if (rqMsg[1] == 0x3E) rqMsg[0] = 0x02; //Set Query Length to 2
  if (rqMsg[1] == 0x22) rqMsg[0] = 0x03; //Set Query Length to 3

  /*Serial.println();
    for (byte i = 0; i < 8; i++) {
    Serial.print(rqMsg[i], HEX); Serial.print(", ");
    }
    Serial.println();*/

  myCAN_Timeout->Reset();                     // Reset Timeout-Timer

  //digitalWrite(CS_SD, HIGH);                // Disable SD card, or other SPI devices if nessesary

  //--- Diag Request Message ---
  DEBUG_UPDATE(F("Send Diag Request\n\r"));
  myCAN0->sendMsgBuf(rqID, 0, 8, rqMsg);      // send data: Request diagnostics data

  return this->Get_RequestResponse();         // wait for response of first frame
}

uint16_t canDiag::Request_Diagnostics(byte* _rqQuery) {

  myCAN_Timeout->Reset();                     // Reset Timeout-Timer

  //digitalWrite(CS_SD, HIGH);                // Disable SD card, or other SPI devices if nessesary

  //--- Diag Request Message ---
  DEBUG_UPDATE(F("Send Diag Request\n\r"));
  myCAN0->sendMsgBuf(rqID, 0, 8, _rqQuery);      // send data: Request diagnostics data

  return this->Get_RequestResponse();         // wait for response of first frame
}

//--------------------------------------------------------------------------------
//! \brief   Wait and read initial diagnostic response
//! \return  lines count (uint16_t) of received lines á 7 bytes
//--------------------------------------------------------------------------------
uint16_t canDiag::Get_RequestResponse() {

  byte i;
  uint16_t items = 0;
  boolean fDataOK = false;

  do {
    //--- Read Frames ---
    if (!digitalRead(2))                        // If pin 2 is LOW, read receive buffer
    {
      do {
        myCAN0->readMsgBuf(&rxID, &len, rxBuf);    // Read data: len = data length, buf = data byte(s)

        if (rxID == this->respID) {
          if (rxBuf[0] < 0x10) {
            if ((rxBuf[1] != 0x7F)) {
              for (i = 0; i < len; i++) {       // read data bytes: offset +1, 1 to 7
                data[i] = rxBuf[i + 1];
              }
              DEBUG_UPDATE(F("SF reponse: "));
              DEBUG_UPDATE(rxBuf[0] & 0x0F); DEBUG_UPDATE("\n\r");
              items = 0;
              fDataOK = true;
            } else if (rxBuf[3] == 0x78) {
              DEBUG_UPDATE(F("pending reponse...\n\r"));
            } else {
              DEBUG_UPDATE(F("ERROR\n\r"));
            }
          }
          if ((rxBuf[0] & 0xF0) == 0x10) {
            items = (rxBuf[0] & 0x0F) * 256 + rxBuf[1]; // six data bytes already read (+ two type and length)
            for (i = 0; i < len; i++) {               // read data bytes: offset +1, 1 to 7
              data[i] = rxBuf[i + 1];
            }
            //--- send rqFC: Request for more data ---
            myCAN0->sendMsgBuf(this->rqID, 0, 8, rqFlowControl);
            DEBUG_UPDATE(F("Resp, i:"));
            DEBUG_UPDATE(items - 6); DEBUG_UPDATE("\n\r");
            fDataOK = Read_FC_Response(items - 6);
          }
        }
      } while (!digitalRead(2) && !myCAN_Timeout->Expired(false) && !fDataOK);
    }
  } while (!myCAN_Timeout->Expired(false) && !fDataOK);

  this->SkipEnable = false;

  if (fDataOK) {
    return (items + 7) / 7;
    DEBUG_UPDATE(F("success!\n\r"));
  } else {
    DEBUG_UPDATE(F("Event Timeout!\n\r"));
    this->ClearReadBuffer();
    return 0;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read remaining data and sent corresponding Flow Control frames
//! \param   items still to read (int)
//! \return  fDiagOK (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::Read_FC_Response(int16_t items) {
  myCAN_Timeout->Reset();

  byte i;
  int16_t n = 7;
  uint16_t rspLine = 0;
  int16_t FC_count = 0;
  byte FC_length = rqFlowControl[1];
  boolean fDiagOK = false;

  do {
    //--- Read Frames ---
    if (!digitalRead(2))                        // If pin 2 is LOW, read receive buffer
    {
      do {
        myCAN0->readMsgBuf(&rxID, &len, rxBuf);    // Read data: len = data length, buf = data byte(s)
        if ((rxBuf[0] & 0xF0) == 0x20) {
          FC_count++;
          items = items - len + 1;
          for (i = 0; i < len; i++) {           // copy each byte of the rxBuffer to data-field
            if ((n < (DATALENGTH - 6)) && (i < 7)) {
              data[n + i] = rxBuf[i + 1];
            }
          }
          //--- FC counter -> then send Flow Control Message ---
          if (FC_count % FC_length == 0 && items > 0) {
            // send rqFC: Request for more data
            myCAN0->sendMsgBuf(this->rqID, 0, 8, rqFlowControl);
            DEBUG_UPDATE(F("FCrq\n\r"));
          }
          //--- Skip read data by using a write pointer (n) and a line counter (rspLine)
          if (this->SkipEnable && (rspLine + 1) >= this->SkipStart && (rspLine + 1) <= this->SkipEnd) {
            rspLine = rspLine + 1;
          } else {
            rspLine = rspLine + 1;
            n = n + 7;
          }
        }
      } while (!digitalRead(2) && !myCAN_Timeout->Expired(false) && items > 0);
    }
  } while (!myCAN_Timeout->Expired(false) && items > 0);
  if (!myCAN_Timeout->Expired(false)) {
    fDiagOK = true;
    DEBUG_UPDATE(F("Items left: ")); DEBUG_UPDATE(items); DEBUG_UPDATE("\n\r");
    DEBUG_UPDATE(F("FC count: ")); DEBUG_UPDATE(FC_count); DEBUG_UPDATE("\n\r");
  } else {
    fDiagOK = false;
    DEBUG_UPDATE(F("Event Timeout!\n\r"));
  }
  this->ClearReadBuffer();
  return fDiagOK;
}

//--------------------------------------------------------------------------------
//! \brief   Output read buffer
//! \param   lines count (uint16_t)
//--------------------------------------------------------------------------------
void canDiag::PrintReadBuffer(uint16_t lines) {
  if (VERBOSE_ENABLE) {
    uint16_t pos;
    Serial.println(lines);
    for (uint16_t i = 0; i < lines; i++) {
      Serial.print(F("Data: "));
      for (byte n = 0; n < 7; n++)              // Print each byte of the data.
      {
        pos = n + 7 * i;
        if (pos <= DATALENGTH) {
          if (data[pos] < 0x10)            // If data byte is less than 0x10, add a leading zero.
          {
            Serial.print('0');
          }
          Serial.print(data[pos], HEX);
          Serial.print(" ");
        }
      }
      Serial.println();
    }
  }
}

//--------------------------------------------------------------------------------
//! \brief   Cleanup after switching filters
//--------------------------------------------------------------------------------
boolean canDiag::ClearReadBuffer() {
  if (!digitalRead(2)) {                       // still messages? pin 2 is LOW, clear the two rxBuffers by reading
    for (byte i = 1; i <= 2; i++) {
      myCAN0->readMsgBuf(&rxID, &len, rxBuf);
    }
    DEBUG_UPDATE(F("Buffer cleared!\n\r"));
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate actual EV key state
//! \param   enable verbose / debug output (boolean)
//! \return  report state (on/off = true/false)
//--------------------------------------------------------------------------------
boolean canDiag::getKeyState(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  boolean fKey = false;

  this->setCAN_ID(rqID_EVC, respID_EVC);
  items = this->Request_Diagnostics(rqKeyState);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    fKey = data[3];
    myBMS->KeyState = fKey;
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate actual DC/DC state
//! \param   enable verbose / debug output (boolean)
//! \return  report state (on/off = true/false)
//--------------------------------------------------------------------------------
boolean canDiag::getDCDC_State(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;
  boolean fOK = false;

  this->setCAN_ID(rqID_EVC, respID_EVC);

  items = this->Request_Diagnostics(rqDCDC_Amps);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->LV_DCDC_amps = data[3];
    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqDCDC_State);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->LV_DCDC_state = data[3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqDCDC_Load);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->LV_DCDC_load = data[3];
    if (myBMS->LV_DCDC_load == 0xFE) myBMS->LV_DCDC_load = 0; //0xFE DC/DC is OFF
    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqDCDC_Power);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 3, 1);
    myBMS->LV_DCDC_power = value;
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in temperature array
//--------------------------------------------------------------------------------
void canDiag::ReadBatteryTemperatures(BatteryDiag_t *myBMS, byte data_in[], uint16_t highOffset, uint16_t length) {

  for (uint16_t n = 0; n < (length * 2); n = n + 2) {
    int16_t value = data_in[n + highOffset] * 256 + data_in[n + highOffset + 1];
    if (n > 4) {
      if (value >> 11) {
        //value = (value | 0xF700); //make negative
        value = value - 0xA00; //minus offset
      }
    }
    myBMS->Temps[n / 2] = value;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in CellCapacity obj
//--------------------------------------------------------------------------------
void canDiag::ReadCellCapacity(byte data_in[], uint16_t highOffset, uint16_t length) {
  for (uint16_t n = 0; n < (length * 2); n = n + 2) {
    CellCapacity.push((data_in[n + highOffset] * 256 + data_in[n + highOffset + 1]));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in CellVoltage obj
//--------------------------------------------------------------------------------
void canDiag::ReadCellVoltage(byte data_in[], uint16_t highOffset, uint16_t length) {
  uint16_t CV;
  for (uint16_t n = 0; n < (length * 2); n = n + 2) {
    CV = (data_in[n + highOffset] * 256 + data_in[n + highOffset + 1]);
    CV = CV / 1024.0 * 1000;
    CellVoltage.push(CV);
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data
//! \param   address to output data array (uint16_t)
//! \param   address to input data array (uint16_t)
//! \param   start of first high byte in data array (uint16_t)
//! \param   length of data submitted (uint16_t)
//--------------------------------------------------------------------------------
void canDiag::ReadDiagWord(uint16_t data_out[], byte data_in[], uint16_t highOffset, uint16_t length) {
  for (uint16_t n = 0; n < (length * 2); n = n + 2) {
    data_out[n / 2] = data_in[n + highOffset] * 256 + data_in[n + highOffset + 1];
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery temperatures (values / 64 in deg C)
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryTemperature(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;

  this->setCAN_ID(rqID_BMS, respID_BMS);

  //Read nine temperatures per module (á 32 cells)
  items = this->Request_Diagnostics(rqBattTemperatures);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    //Serial.print(data[3], HEX); Serial.println(data[4], HEX);
    this->ReadBatteryTemperatures(myBMS, data, 3, 31);
    //Serial.println((float) myBMS->Temps[0] / 64, 1);
    //myBMS->Temps[33] = data[69]; //always 0xFF
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery and BMS production date
//! \brief   and date of factory acceptacnce test
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryDate(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;

  this->setCAN_ID(rqID_BMS, respID_BMS);

  //Get date of Factory Acceptance Testing (FAT)
  items = this->Request_Diagnostics(rqBattProdDate);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->ProdYear = data[4];
    myBMS->ProdMonth = data[5];
    myBMS->ProdDay = data[6];
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read the VIN stored in the battery and compare it to myVIN def.
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryVIN(BatteryDiag_t *myBMS, boolean debug_verbose) {
  (void) myBMS;
  uint16_t items = 0;

  this->setCAN_ID(0x7E7, 0x7EF);
  //items = this->Request_Diagnostics(rqBattVIN);

  byte OKcount = 0;
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    for (byte n = 0; n < 17; n++) {
      //myBMS->BattVIN[n] =  data[n + 4];
      //if (myBMS->BattVIN[n] == myVIN[n]) OKcount++;
    }
    //return true if data completely matches
    if (OKcount == 17) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery high voltage status
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBalancingStatus(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;
  byte balState = 0;

  this->setCAN_ID(rqID_BMS, respID_BMS);
  items = this->Request_Diagnostics(rqBattBalancing);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    for (byte i = 0; i < CELLCOUNT; i++) {
      balState = balState ^ data[i + 3];
    }
    myBMS->BattBalXOR = balState;
    this->ReadDiagWord(&value, data, 99, 1);
    myBMS->BattBalState = value;
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery isolation resistance
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getIsolationValue(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;

  this->setCAN_ID(rqID_BMS, respID_BMS);
  items = this->Request_Diagnostics(rqBattIsolation);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 2, 1);
    myBMS->Isolation = (signed) value;
    myBMS->DCfault = data[4]; //Flags for isolation measurement
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate capacity data
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryCapacity(BatteryDiag_t *myBMS, boolean debug_verbose) {
  debug_verbose = debug_verbose & VERBOSE_ENABLE;
  uint16_t items;
  boolean fOK = false;

  this->setCAN_ID(rqID_BMS, respID_BMS);

  items = this->Request_Diagnostics(rqBattCapacity_dSOC_P1);
  delay(1000);
  items = this->Request_Diagnostics(rqBattCapacity_dSOC_P1);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }

    //this->ReadDiagWord(&myBMS->HVoff_time,data,3,1); //myBMS->HVoff_time = (unsigned long) data[5] * 65535 + (uint16_t) data[6] * 256 + data[7];
    //this->ReadDiagWord(&myBMS->HV_lowcurrent,data,5,1); //myBMS->HV_lowcurrent = (unsigned long) data[9] * 65535 + (uint16_t) data[10] * 256 + data[11];
    //this->ReadDiagWord(&myBMS->OCVtimer,data,7,1); //myBMS->OCVtimer = (uint16_t) data[12] * 256 + data[13];
    myBMS->fSOH = data[9];
    //this->ReadDiagWord(&myBMS->Cap_As.max,data,10,1);
    //this->ReadDiagWord(&myBMS->Cap_As.min,data,12,1);
    //this->ReadDiagWord(&myBMS->Cap_As.mean,data,14,1);

    if (CapMode == 1) {
      CellCapacity.clear();
      this->ReadCellCapacity(data, 16, (CELLCOUNT / 2)); //data starting at #16, but two mean values pushed before!
    }

    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqBattCapacity_dSOC_P2);
  delay(1000);
  items = this->Request_Diagnostics(rqBattCapacity_dSOC_P2);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    uint16_t value;
    this->ReadDiagWord(&value, data, 99, 1);
    myBMS->Cap_combined_quality = value / 65535.0;
    this->ReadDiagWord(&myBMS->LastMeas_days, data, 101, 1);
    this->ReadDiagWord(&value, data, 103, 1);
    myBMS->Cap_meas_quality = value / 65535.0;

    if (CapMode == 1) {
      this->ReadCellCapacity(data, 3, CELLCOUNT / 2);

      myBMS->Ccap_As.min = CellCapacity.minimum((int16_t *)&myBMS->CAP_min_at);
      CellCapacity.push(myBMS->Ccap_As.min); CellCapacity.push(myBMS->Ccap_As.min);
      myBMS->Ccap_As.max = CellCapacity.maximum((int16_t *)&myBMS->CAP_max_at);
      myBMS->Ccap_As.mean = CellCapacity.mean();
    }

    fOK &= true;
  } else {
    fOK &= false;
  }

  //*** Read second capacity values in As/100 ***
  items = this->Request_Diagnostics(rqBattCapacity_P1);
  delay(1000);
  items = this->Request_Diagnostics(rqBattCapacity_P1);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    if (CapMode == 2) {
      CellCapacity.clear();
      this->ReadCellCapacity(data, 5, CELLCOUNT / 2);
      this->ReadDiagWord(&myBMS->CAP2_mean, data, 3, 1);
    }

    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqBattCapacity_P2);
  delay(1000);
  items = this->Request_Diagnostics(rqBattCapacity_P2);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    if (CapMode == 2) {
      this->ReadCellCapacity(data, 3, CELLCOUNT / 2);
      myBMS->Ccap_As.min = CellCapacity.minimum((int16_t *)&myBMS->CAP_min_at) * 10;
      myBMS->Ccap_As.max = CellCapacity.maximum((int16_t *)&myBMS->CAP_max_at) * 10;
      myBMS->Ccap_As.mean = CellCapacity.mean() * 10;
    }

    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate experimental data of the bms
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryExperimentalData(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;
  boolean fOK = false;

  this->setCAN_ID(rqID_BMS, respID_BMS);

  items = this->Request_Diagnostics(rqBattMeas_Capacity);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 2, 1);
    myBMS->CapMeas = value;
    fOK = true;
  } else {
    fOK = false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate voltage data
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryVoltage(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  boolean fOK = false;

  this->setCAN_ID(rqID_BMS, respID_BMS);
  items = this->Request_Diagnostics(rqBattVoltages_P1);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    CellVoltage.clear();
    this->ReadCellVoltage(data, 3, CELLCOUNT / 2);
    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqBattVoltages_P2);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadCellVoltage(data, 3, CELLCOUNT / 2);
    myBMS->Cvolts.min = CellVoltage.minimum(&myBMS->CV_min_at);
    myBMS->Cvolts.max = CellVoltage.maximum(&myBMS->CV_max_at);
    myBMS->Cvolts.mean = CellVoltage.mean();
    myBMS->Cvolts_stdev = CellVoltage.stddev();
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief  Evaluate voltage data for distribution, calc. percentiles & outliners
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryVoltageDist(BatteryDiag_t *myBMS) {
  byte _Count = CellVoltage.getCount();

  //Sort voltges in ascending order
  CellVoltage.bubble_sort();

  //Get quartiles
  myBMS->Cvolts.p25 = CellVoltage.percentile(_Count / 4);
  myBMS->Cvolts.median = CellVoltage.percentile(_Count / 2);
  myBMS->Cvolts.p75 = CellVoltage.percentile(_Count * 3 / 4);

  //Get outliners in the IQR-FACTOR range, excluding the min- and max-values
  uint16_t p3IQR = (myBMS->Cvolts.p75 - myBMS->Cvolts.p25) * IQR_FACTOR;
  byte p25_Out = 0;
  for (byte n = 1; n < (CellVoltage.getCount() / 4); n++) {
    if (CellVoltage.get(n) < ( myBMS->Cvolts.p25 - p3IQR)) {
      p25_Out++;
      //Serial.println(ave.get(n));
    }
  }
  byte p75_Out = 0;
  for (byte n = (CellVoltage.getCount() * 3 / 4); n < (CellVoltage.getCount() - 1); n++) {
    if (CellVoltage.get(n) > ( myBMS->Cvolts.p75 + p3IQR)) {
      p75_Out++;
      //Serial.println(ave.get(n));
    }
  }
  myBMS->Cvolts.p25_out_count = p25_Out;
  myBMS->Cvolts.p75_out_count = p75_Out;

  return true;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate actual battery state / data
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryState(BatteryDiag_t *myBMS, boolean debug_verbose) {
  boolean fOK = false;
  uint16_t items;
  uint16_t value;

  this->setCAN_ID(rqID_BMS, respID_BMS);
  items = this->Request_Diagnostics(rqBattState);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 3, 1);
    myBMS->CV_Range.min = (uint16_t) value / 1024.0 * 1000.0;
    this->ReadDiagWord(&value, data, 5, 1);
    myBMS->CV_Range.max = (uint16_t) value / 1024.0 * 1000.0;
    myBMS->CV_Range.mean = (myBMS->CV_Range.max + myBMS->CV_Range.min) / 2;
    this->ReadDiagWord(&value, data, 7, 1);
    myBMS->BattLinkVoltage = value / 64.0;
    this->ReadDiagWord(&value, data, 9, 1);
    myBMS->BattCV_Sum = value / 64.0;
    this->ReadDiagWord(&value, data, 11, 1);
    myBMS->BattPower.voltage = value;
    this->ReadDiagWord(&value, data, 13, 1);
    myBMS->BattPower.current = value;
    myBMS->BattPower.power = myBMS->BattPower.voltage / 64.0 * myBMS->BattPower.current / 32.0 / 1000.0;

    myBMS->HVcontactState = data[15];
    myBMS->HV = myBMS->BattCV_Sum;
    myBMS->EVmode = data[24];
    myBMS->LV = data[25] / 8.0;

    myBMS->Amps = myBMS->BattPower.current;
    myBMS->Amps2 = myBMS->BattPower.current / 32.0;
    CalcPower(myBMS);
    fOK = true;
  } else {
    fOK = false;
  }

  this->setCAN_ID(rqID_EVC, respID_EVC);
  items = this->Request_Diagnostics(rqHV_Energy);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 3, 1);
    myBMS->Energy = value;
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate actual battery SOC and other core data values
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatterySOC(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;
  bool fOK = false;

  this->setCAN_ID(rqID_BMS, respID_BMS);
  items = this->Request_Diagnostics(rqBattSOC);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    //this->ReadDiagWord(&value,data,3,1);
    //myBMS->BattOCV = value;;
    //Serial.println(myBMS->BattOCV / 100.0,3);
    this->ReadDiagWord(&value, data, 5, 1);
    myBMS->realSOC.min = value;
    this->ReadDiagWord(&value, data, 7, 1);
    myBMS->realSOC.max = value;
    //this->ReadDiagWord(&value,data,11,1);
    //myBMS->CapLoss = value;
    this->ReadDiagWord(&value, data, 13, 1);
    myBMS->CapInit = value;
    this->ReadDiagWord(&value, data, 15, 1);
    myBMS->CapEstimate = value;

    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqBattSOCrecal);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 3, 1);
    myBMS->realSOC.mean  = value;
    myBMS->SOCrecalState = data[2];
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate actual battery limiting values
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryLimits(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;

  this->setCAN_ID(rqID_BMS, respID_BMS);
  items = this->Request_Diagnostics(rqBattLimits);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->BattLimit.MaxChargeAmps = ((uint32_t) data[3] * 16777216 + (uint32_t) data[4] * 65535 + (uint16_t) data[5] * 256 + data[6]) / 1024.0;
    myBMS->BattLimit.MaxDischargeAmps = ((int32_t) ((uint32_t) data[7] * 16777216 + (uint32_t) data[8] * 65535 + (uint16_t) data[9] * 256 + data[10])) / 1024.0;
    this->ReadDiagWord(&value, data, 11, 1);
    myBMS->BattLimit.MaxCV = (uint16_t) value / 1024.0 * 1000.0;
    this->ReadDiagWord(&value, data, 13, 1);
    myBMS->BattLimit.MinCV = (uint16_t) value / 1024.0 * 1000.0;
    this->ReadDiagWord(&value, data, 15, 1);
    myBMS->SOC = (float) value / 50.0;
    this->ReadDiagWord(&value, data, 17, 1);
    myBMS->BattLimit.Zcharge = value;
    this->ReadDiagWord(&value, data, 19, 1);
    myBMS->BattLimit.Zdischarge = value;
    myBMS->BattLimit.BattPowerMax = data[21];
    myBMS->BattLimit.BattPowerGen = data[22];
    myBMS->BattLimit.BattPowerCharge = data[23];

    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery health status
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatterySOH(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;

  this->setCAN_ID(rqID_BMS, respID_BMS);
  items = this->Request_Diagnostics(rqBattHealth);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->SOH = data[11] / 2; //SOH value (x/2)
    this->ReadDiagWord(&value, data, 7, 1);
    myBMS->CAPusable_max = value;
    //this->ReadDiagWord(&value,data,18,1);
    //myBMS->CAPusable = value;

    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read total distance / ODO from Dash
//! \brief   When accessing Dash get also ambient temperature
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getODOcount(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value = 0;
  bool fOK = false;

  this->setCAN_ID(rqID_DASH, respID_DASH);

  items = this->Request_Diagnostics(rqDashODO);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->ODO = (unsigned long) data[3] * 65536 + (uint16_t) data[4] * 256 + data[5];
    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqDashTemperature);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 5, 1);
    myBMS->AmbientTemp = (value - 400);
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read available Range / ECO from EVC
//! \brief   When accessing Dash get also ambient temperature
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getRange(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;
  uint16_t value = 0;
  bool fOK = false;

  this->setCAN_ID(rqID_EVC, respID_EVC);

  items = this->Request_Diagnostics(rqEV_Range);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 3, 1);
    myBMS->EVrange = value;
    fOK = true;
  } else {
    fOK = false;
  }

  rqMsg[3] = 0x46;
  items = this->Request_Diagnostics(rqMsg);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->EVeco = data[3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate High Voltage contractor state
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getHVcontactorCount(BatteryDiag_t *myBMS, boolean debug_verbose) {

  uint16_t items;

  this->setCAN_ID(rqID_BMS, respID_BMS);
  items = this->Request_Diagnostics(rqBattHVContactorCycles);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myBMS->HVcontactCyclesLeft = (unsigned long) data[4] * 65536 + (uint16_t) data[5] * 256 + data[6];
    myBMS->HVcontactCyclesMax = (unsigned long) data[8] * 65536 + (uint16_t) data[9] * 256 + data[10];
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Get SOC calibration table based on OCV values
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::printOCVtable(BatteryDiag_t *myBMS, boolean debug_verbose) {
  (void) myBMS;
  uint16_t items;
  uint16_t value;
  bool fOK = false;

  this->setCAN_ID(rqID_BMS, respID_BMS);

  items = this->Request_Diagnostics(rqBattOCV_Cal);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    Serial.println(F("# ; mV"));
    for (byte n = 4; n <= (21 * 4); n += 4) {
      this->ReadDiagWord(&value, data, (n + 1), 1);
      if ((n / 4) < 10) Serial.print('0');
      Serial.print(n / 4);
      PRINT_CSV; Serial.println((uint16_t) (value / 4096.0 * 1000.0));
    }
    Serial.println();
    fOK = true;
  } else {
    fOK = false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Get cell resistance factors, used for capacity measurement
//! \brief   Values are directly printed to serial
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::printRESfactors(BatteryDiag_t *myBMS, boolean debug_verbose) {
  (void) myBMS;
  uint16_t items;
  byte offset = 0;
  uint16_t value;
  bool fOK = false;

  this->setCAN_ID(rqID_BMS, respID_BMS);

  items = this->Request_Diagnostics(rqBattCellResistance_P1);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    Serial.println(F("# ; Ri"));
    for (byte n = 1; n <= (49 * 2); n += 2) {
      this->ReadDiagWord(&value, data, (n + 2), 1);
      if ((n / 2) < 10) Serial.print('0');
      Serial.print(n / 2);
      PRINT_CSV; Serial.println((float) (value / 8192.0), 4);
      offset = n / 2;
    }
    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqBattCellResistance_P2);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    for (byte n = 2; n <= (48 * 2); n += 2) {
      this->ReadDiagWord(&value, data, (n + 1), 1);
      Serial.print(n / 2 + offset);
      PRINT_CSV; Serial.println((float) (value / 8192.0), 4);
    }
    Serial.println();
    fOK = true;
  } else {
    fOK = false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Get mission history data of BMS
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::printBMSlog(BatteryDiag_t *myBMS, boolean debug_verbose) {
  (void) myBMS;
  uint16_t items;
  bool fOK = false;

  this->setCAN_ID(rqID_BMS, respID_BMS);

  for (byte n = 0; n < 5; n++) {
    if (n == 0) {
      items = this->Request_Diagnostics(rqBattLogData_P1);
      if (items > 0) Serial.println(F("# ; Data"));
    } else {
      rqMsg[2] += 1;
      items = this->Request_Diagnostics(rqMsg);
    }
    if (items) {
      if (debug_verbose) {
        PrintReadBuffer(items);
      }
      if (n < 4) {
        this->printBMSlogSet(122);
      } else {
        this->printBMSlogSet(34);
      }
      fOK = true;
    } else {
      fOK = false;
    }
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Print function for mission history data
//! \param   length of data field (byte)
//--------------------------------------------------------------------------------
void canDiag::printBMSlogSet(byte _length) {
  for (byte n = 3; n < _length; n += 8) {
    if (n / 8 < 10) Serial.print('0');
    Serial.print(n / 8); PRINT_CSV;
    Serial.print(data[n] & 0x07); PRINT_CSV;          //Status Balancing?
    Serial.print(data[n + 1], DEC); PRINT_CSV;        //SOC %
    Serial.print(data[n + 2], DEC); PRINT_CSV;        //SOC %
    Serial.print(data[n + 3], HEX); PRINT_CSV;        //???
    Serial.print(data[n + 4], HEX); PRINT_CSV;        //???
    Serial.print((char) data[n + 5], DEC); PRINT_CSV; //Temperature Data degC?
    Serial.print((char) data[n + 6], DEC); PRINT_CSV; //Temperature Data degC?
    Serial.print(data[n + 7], HEX);                   //Status-Flags?
    Serial.println();
  }
}

//--------------------------------------------------------------------------------
//! \brief   Get Charging History Data
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::printCHGlog(boolean debug_verbose) {

  uint16_t items;
  bool fOK = false;

  //Structure for Log-Dataset
  typedef struct {
    byte chgFlag;
    byte chgStatus;
    uint16_t chgTime;
    uint16_t soc;
    uint16_t power;
    long odo;
    byte temp;
  } ChgLog_t;

  byte log_size = 10;
  ChgLog_t *ChgLog_P;

  //Reserve memory
  ChgLog_P = (ChgLog_t *) malloc(sizeof(ChgLog_t) * log_size);

  this->setCAN_ID(rqID_EVC, respID_EVC);

  for (byte n = 0; n < 8; n++) {
    if (n == 0) {
      items = this->Request_Diagnostics(rqEV_ChgLog_P1);
    } else if (n < 6) {
      rqMsg[3] += 1;
      items = this->Request_Diagnostics(rqMsg);
    } else if (n == 6) {
      rqMsg[2] = 0x34; rqMsg[3] = 0xE4;
      items = this->Request_Diagnostics(rqMsg);
    } else if (n == 7) {
      rqMsg[2] = 0x34; rqMsg[3] = 0x12;
      items = this->Request_Diagnostics(rqMsg);
    }
    if (items) {
      if (debug_verbose) {
        PrintReadBuffer(items);
      }
      for (byte i = 0; i < log_size; i++) {
        switch (n) {
          case 0:
            ChgLog_P[i].odo = data[4 + (i * 3)] * 65536 + data[5 + (i * 3)] * 256 + data[6 + (i * 3)];
            break;
          case 1:
            ChgLog_P[i].chgStatus = data[4 + i];
            break;
          case 2:
            ChgLog_P[i].chgFlag = data[4 + i];
            break;
          case 3:
            ChgLog_P[i].soc = data[4 + (i * 2)] * 256 + data[5 + (i * 2)];
            break;
          case 4:
            ChgLog_P[i].temp = data[4 + i];
            break;
          case 5:
            ChgLog_P[i].chgTime = data[4 + (i * 2)] * 256 + data[5 + (i * 2)];
            break;
          case 6:
            ChgLog_P[i].power = data[4 + (i * 2)] * 256 + data[5 + (i * 2)];
            break;
        }
      }
      fOK = true;
    } else {
      fOK = false;
    }
  }

  if (fOK) {
    Serial.println(F("#; km; kW; t/min; SOC%; degC; State; Flag"));
    for (byte i = 0; i < log_size; i++) {
      Serial.print(i); PRINT_CSV;
      Serial.print(ChgLog_P[i].odo); PRINT_CSV;
      Serial.print(ChgLog_P[i].power / 10.0, 1); PRINT_CSV;     
      Serial.print(ChgLog_P[i].chgTime); PRINT_CSV;
      Serial.print(ChgLog_P[i].soc / 5); PRINT_CSV;     
      Serial.print(ChgLog_P[i].temp - 40); PRINT_CSV;
      Serial.println(ChgLog_P[i].chgStatus);
      Serial.print(ChgLog_P[i].chgFlag, HEX); PRINT_CSV;
    }
  }

  //Free memory
  free(ChgLog_P);

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Test if a OBL fastcharger is installed by reading HW-Rev.
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
char canDiag::OBL_7KW_Installed(ChargerDiag_t *myOBL, boolean debug_verbose) {
  (void) myOBL;
  uint16_t items;

  this->setCAN_ID(rqID_OBL, respID_OBL);
  items = this->Request_Diagnostics(rqIDpart);
  
  if (items){
    if (debug_verbose) {
       PrintReadBuffer(items);
    }
    byte n;
    byte comp = 0;
    for (n = 3; n < 7; n++) {
        if (data[n] != ID_7KW[n - 3]) {
          comp++;
        }
    }
    if (comp == 4){
      return true;
    } else {
      return false;
    }
  } else {
    return -1;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Get HW / SW revision (print directly to screen to save memory)
//! \brief   *** Use CAN_IDs set method before call ***
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::printECUrev(boolean debug_verbose, byte _type[]) {

  uint16_t items;
  bool fOK = false;

  items = this->Request_Diagnostics(rqPartNo);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    byte n = 3;
    byte revCount = 0;
    Serial.print(F("HW: "));
    do {
      if (n > 4 && (data[n] == _type[0] && data[n + 1] == _type[1] && data[n + 2] == _type[2])) {
        Serial.print(F(" SW: "));
        revCount++;
        /*if (revCount%2 == 0) {
          Serial.println();
          }*/
      }
      Serial.print((char)data[n]);
    } while ((data[++n] != 0x00) && (n < 23) && (n < items * 7));
    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqIDpart);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    byte n = 17;
    Serial.print(F(","));
    do {
      if (data[n] < 0x10) {
        Serial.print('0');
      }
      Serial.print(data[n], HEX);
    } while ((++n < 21) && (n < items * 7));
    Serial.println();
    
    n = 3;
    Serial.print(F("ID: "));
    do {
      if (data[n] < 0x20) {
        Serial.print(data[n], HEX);
      } else {
        Serial.print((char)data[n]);
      }
    } while ((++n < 17) && (n < items * 7));
    //Serial.println();
    fOK &= true;
  } else {
    fOK &= false;
  }

  rqMsg[2] = 0xF0;
  items = this->Request_Diagnostics(rqMsg);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    byte n = 3;
    Serial.print(F(", "));
    do {
      if (data[n] < 0x20) {
        Serial.print(data[n], HEX);
      } else {
        Serial.print((char)data[n]);
      }
    } while ((++n < 17) && (n < items * 7));
    Serial.println();
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate charger temperatures (values - 40 in deg C)
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getChargerTemperature(ChargerDiag_t *myOBL, boolean debug_verbose) {

  uint16_t items;
  boolean fOK = false;

  this->setCAN_ID(rqID_OBL, respID_OBL);
  items = this->Request_Diagnostics(rqChargerTemperatures);

  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    myOBL->InTemp = data[3];
    myOBL->OutTemp = data[4];
    myOBL->InternalTemp = data[5];
    fOK = true;
  } else {
    fOK = false;
  }

  this->setCAN_ID(rqID_CHGCTRL, respID_CHGCTRL);
  items = this->Request_Diagnostics(rqCoolantTemp);

  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    myOBL->CoolantTemp = data[3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate values from charger-controller for standard OBL
//! \brief   Setpoint for max current, mains coding of SC and cable, max avail.power
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getChargerCtrlValues(ChargerDiag_t *myOBL, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;
  bool fOK = false;

  this->setCAN_ID(rqID_CHGCTRL, respID_CHGCTRL);

  items = this->Request_Diagnostics(rqChargerSelCurrent);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myOBL->Amps_setpoint = data[3] / 4; //Get data (x/4)
    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqMainsMaxCurrent);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myOBL->AmpsChargingpoint = data [3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqConnectorCoding);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 3, 1);
    if (value < 1800) {
      myOBL->AmpsCableCode = value;
    } else {
      myOBL->AmpsCableCode = 0;
    }
    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqMaxPowerAvail);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 3, 1);
    myOBL->CHGpower[2] = value;
    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqChargerPilotState);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myOBL->PilotState = data[3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  this->setCAN_ID(rqID_OBL, respID_OBL);
  items = this->Request_Diagnostics(rqChargerState);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myOBL->ChargerState = data[3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate charger voltages and currents
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getChargerDC(ChargerDiag_t *myOBL, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;

  this->setCAN_ID(rqID_OBL, respID_OBL);
  items = this->Request_Diagnostics(rqChargerDC);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 4, 1);
    if (value < 0xEA00) {  //OBL showing only valid data while charging
      myOBL->DC_HV = value;
    } else {
      myOBL->DC_HV = 0;
    }
    this->ReadDiagWord(&value, data, 10, 1);
    if (value < 0xEA00) {  //OBL showing only valid data while charging
      myOBL->DC_Current = value;
    } else {
      myOBL->DC_Current = 0;
    }

    items = this->Request_Diagnostics(rqChargerLV);
    if (items) {
      if (debug_verbose) {
        this->PrintReadBuffer(items);
      }
      this->ReadDiagWord(&value, data, 3, 1);
      myOBL->LV = value;
    }

    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate charger amps (AC and DC currents)
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getChargerAC(ChargerDiag_t *myOBL, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;

  this->setCAN_ID(rqID_OBL, respID_OBL);
  items = this->Request_Diagnostics(rqChargerAC);

  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    //Get AC Currents (two rails from >= 20A, sum up for total current)
    this->ReadDiagWord(&value, data, 10, 1);
    if (value < 0xEA00) {  //OBL showing only valid data while charging
      myOBL->MainsAmps[0] = value;
    } else {
      myOBL->MainsAmps[0] = 0;
    }
    this->ReadDiagWord(&value, data, 12, 1);
    if (value < 0xEA00) {  //OBL showing only valid data while charging
      myOBL->MainsAmps[1] = value;
    } else {
      myOBL->MainsAmps[1] = 0;
    }
    myOBL->MainsAmps[2] = 0;

    //Get AC Voltages
    this->ReadDiagWord(&value, data, 4, 1);
    if (value < 0xEA00) {  //OBL showing only valid data while charging
      myOBL->MainsVoltage[0] = value;

    } else {
      myOBL->MainsVoltage[0] = 0;
    }
    myOBL->MainsVoltage[1] = 0; myOBL->MainsVoltage[2] = 0;
    if (myOBL->MainsAmps[0] > 0 || myOBL->MainsAmps[1] > 0) {
      myOBL->MainsFreq = data[15];
    } else {
      myOBL->MainsFreq = 0;
    }

    //Get AC Power
    this->ReadDiagWord(&value, data, 16, 1);
    if (value < 0xEA00) {  //OBL showing only valid data while charging
      myOBL->CHGpower[0] = value;

    } else {
      myOBL->CHGpower[0] = 0;
    }
    this->ReadDiagWord(&value, data, 18, 1);
    if (value < 0xEA00) {  //OBL showing only valid data while charging
      myOBL->CHGpower[1] = value;

    } else {
      myOBL->CHGpower[1] = 0;
    }

    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Set charger ac_max current
//! \param   current only in the range of 6 to 20 A (byte)
//! \brief   >>> FOR TEST PURPOSE ONLY! DO NOT USE WHILE CHARGING <<<
//! \brief   >>> DO NOT USE ON US and UK cars, as they work with 32A max<<< 
//! \brief   >>> with entering [-yes] you ACCEPT ALL CONSEQUENCES 
//! \brief   >>> AND THE USAGE IS SOLEY AT YOUR OWN RISK! <<<
//! \brief   >>> LOSS OF WARRANTY, DAMAGE(s), VIOLATION OF REGULATIVE RULES <<<
//! \brief   >>> NO LIABILITY FOR THIS SOFTWARE - SEE LICENSE STATEMENT! <<<
//--------------------------------------------------------------------------------
boolean canDiag::setACmax(ChargerDiag_t *myOBL, boolean debug_verbose) {

  myOBL->newAmps_setpoint = constrain(myOBL->newAmps_setpoint, 6, 20);
  //Serial.println(myOBL->newAmps_setpoint);

  byte _rqMsg[8] = {0x04, 0x2E, 0x61, 0x41, 0x50, 0x00, 0x00, 0x00};
  _rqMsg[4] = myOBL->newAmps_setpoint * 4;  //Define new AC max current

  boolean fOK = false;
  uint16_t items = 0;

  this->setCAN_ID(rqID_CHGCTRL, respID_CHGCTRL);

  items = Request_Diagnostics(rqTesterPresent);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
  }

  delay(10);
  items = Request_Diagnostics(rqDiagSessionExt);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
  }
  
  delay(10);  
  items = this->Request_Diagnostics(_rqMsg);
  if (items) {
    if (debug_verbose) {
      PrintReadBuffer(items);
    }
    if (data[0] == (_rqMsg[1] + 0x40) && data[1] == _rqMsg[2] && data[2] == _rqMsg[3]) {
      fOK = true;
    } else {
      fOK = false;
    }
  } else {
    fOK = false;
  }

  return fOK;
}


//--------------------------------------------------------------------------------
//! \brief   Read and evaluate data of the Tele-Communications Unit
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getTCUdata(TCUdiag_t *myTCU, boolean debug_verbose) {

  uint16_t items;
  uint16_t value;
  boolean fOK = false;
  uint32_t TimeData = 0;

  this->setCAN_ID(rqID_TCU, respID_TCU);

  items = this->Request_Diagnostics(rqTCUtime);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }

    TimeData = data[3] * 16777216 + data[4] * 65536 + data[5] * 256 + data[6];

    //Decode time data set
    myTCU->TCUtime.second = TimeData & 0x3F;
    TimeData = TimeData >> 6;
    myTCU->TCUtime.minute = TimeData & 0x3F;
    TimeData = TimeData >> 6; 
    myTCU->TCUtime.hour = TimeData & 0x1F;
    TimeData = TimeData >> 5;
    myTCU->TCUtime.day = TimeData & 0x1F;
    TimeData = TimeData >> 5;
    myTCU->TCUtime.month = TimeData & 0xF;
    TimeData = TimeData >> 4;
    myTCU->TCUtime.year = TimeData;

    fOK = true;
  } else {
    fOK = false;
  }

  items = this->Request_Diagnostics(rqTCUrssi);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myTCU->Rssi = data[3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqTCUstate);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myTCU->State = data[3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqTCUnetType);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    myTCU->NetType = data[3];
    fOK &= true;
  } else {
    fOK &= false;
  }

  items = this->Request_Diagnostics(rqTCUcounter);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    this->ReadDiagWord(&value, data, 5, 1);
    myTCU->Counter = value;
    fOK &= true;
  } else {
    fOK &= false;
  }

  //Correction for Bug in Ficosa TCU: missing trailing zero for 12:00 to 15:00 UTC
  if (myTCU->TCUtime.hour >= 24) {
    myTCU->TCUtime.hour -= 16;
    myTCU->TCUtime.day += 1;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Read and print network name TCU is connected with
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getTCUnetwork(TCUdiag_t *myTCU, boolean debug_verbose) {
  (void) myTCU;
  uint16_t items;
  boolean fOK = false;

  this->setCAN_ID(rqID_TCU, respID_TCU);

  items = this->Request_Diagnostics(rqTCUnetName);
  if (items) {
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }
    byte n = 4;
    do {
      Serial.print((char)data[n]);
    } while ((data[++n] != 0x20) && (n < items * 7));
    Serial.println();
    fOK = true;
  } else {
    Serial.println(F("-"));
    fOK = false;
  }

  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Calculate power reading
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::CalcPower(BatteryDiag_t *myBMS) {
  myBMS->Power = myBMS->HV * myBMS->Amps2 / 1000.0;
  return true;
}


