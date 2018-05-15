//--------------------------------------------------------------------------------
// ED BMSdiag, v0.4.4
// Retrieve battery diagnostic data from your smart electric drive EV.
//
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
//! \file    ED4scan.ino
//! \brief   Retrieve battery diagnostic data from your smart electric drive EV.
//! \brief   Only usable for fourth gen. model 453 series, build >= 2017
//! \brief   Build a diagnostic tool with the MCP2515 CAN controller and Arduino
//! \brief   compatible hardware.
//! \date    2018-May
//! \author  MyLab-odyssey
//! \version 0.4.4
//--------------------------------------------------------------------------------
#include "ED4scan.h"

//--------------------------------------------------------------------------------
//! \brief   SETUP()
//--------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial); // while the serial stream is not open, do nothing

  pinMode(CS, OUTPUT);
  pinMode(CS_SD, OUTPUT);
  digitalWrite(CS_SD, HIGH);

  //Initialize MCP2515 and clear filters
  DiagCAN.begin(&CAN0, &CAN_Timeout);
  DiagCAN.clearCAN_Filter();

  digitalWrite(CS, HIGH);

  //MCP2515 read buffer: setting pin 2 for input, LOW if CAN messages are received
  pinMode(2, INPUT);

  //Serial.println(getFreeRam());

  //Print Welcome Screen and wait for CAN-Bus
  printWelcomeScreen();
  delay(1000);
  
  //Get basic BMS data and test installed charger
  DiagCAN.setCAPmode(CAP_MODE);
  OBL.OBL7KW = DiagCAN.OBL_7KW_Installed(&OBL, false);

  Serial.print(F("Reading data")); myDevice.progress = true;
  show_splash(0, 0L); myDevice.progress = false;
  
  //Setup CLI, display prompt and local echo
  setupMenu();
  init_cmd_prompt();
  cmd_display();
  set_local_echo(ECHO);
  
}

//--------------------------------------------------------------------------------
//! \brief   LOOP()
//--------------------------------------------------------------------------------
void loop() {
   if (CLI_Timeout.Expired(true)) {
      cmdPoll();                                   //Poll CLI status
   }
   if (myDevice.logging && LOG_Timeout.Expired(true)){
      logdata();
   }
}

//--------------------------------------------------------------------------------
//! \brief   Memory available between Heap and Stack
//--------------------------------------------------------------------------------
int getFreeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

//--------------------------------------------------------------------------------
//! \brief   Wait for serial data to be avaiable.
//--------------------------------------------------------------------------------
void WaitforSerial() {
  Serial.println(F("Press ENTER to start query:"));
  while (!Serial.available()) {}                  // Wait for serial input to start
}

//--------------------------------------------------------------------------------
//! \brief   Read all queued charachers to clear input buffer.
//--------------------------------------------------------------------------------
void clearSerialBuffer() {
  do {                                            // Clear serial input buffer
      delay(10);
  } while (Serial.read() >= 0);
}

//--------------------------------------------------------------------------------
//! \brief   Get status for BMS relevant core data
//! \param   selected items for task (byte array), length of array
//--------------------------------------------------------------------------------
void getState_BMS(byte *selected, byte len) {
  boolean fOK = false;

  //Read CAN-messages
  byte testStep = 0;
  do {
    switch (selected[testStep]) {
      case BMSstate:
         fOK = DiagCAN.getBatteryState(&BMS, false);
         break;
      case BMSsoc:
         fOK = DiagCAN.getBatterySOC(&BMS, false);
         break;
      case BMSlimit:
         fOK = DiagCAN.getBatteryLimits(&BMS, false);
         break;
      case BMSbal:
         fOK = DiagCAN.getBalancingStatus(&BMS, false);
         break;
      case EVkey:
         fOK = DiagCAN.getKeyState(&BMS, false);
         break;
      case EVdcdc:
         fOK = DiagCAN.getDCDC_State(&BMS, false);
         break;
      case EVodo:
         fOK = DiagCAN.getODOcount(&BMS, false);
         break;
      case EVrange:
         fOK = DiagCAN.getRange(&BMS, false);
         break;
    }
    if (!myDevice.logging && testStep < BMSCOUNT) {
      if (fOK) {
        if (myDevice.progress) Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print('#'); Serial.print(selected[testStep]);
      }
    }
    testStep++;
  } while (testStep < len);
  myDevice.progress = false;
}

//--------------------------------------------------------------------------------
//! \brief   Get BMS datasets
//! \param   selected items for task (byte array), length of array
//--------------------------------------------------------------------------------
boolean getBMSdata(byte *selected, byte len) {
  boolean fOK = false;
  
  //Get diagnostics data
  DiagCAN.setCAN_ID(rqID_BMS, respID_BMS);

  byte testStep = 0;
  do {
    switch (selected[testStep]) {
      case 0:
         fOK = DiagCAN.getBatteryVoltage(&BMS, false);
         break;
      case 1:
         fOK = DiagCAN.getBatteryCapacity(&BMS, false);
         break;
      case 2:
         fOK = DiagCAN.getBalancingStatus(&BMS, false);
         break;
      case 5:
         fOK = DiagCAN.getBatterySOH(&BMS, false);
         break;
      case 6:
         fOK = DiagCAN.getBatteryDate(&BMS, false);
         break;
      case 8:
         fOK = DiagCAN.getBatteryTemperature(&BMS, false);
         break;
      case 9:
         fOK = DiagCAN.getHVcontactorCount(&BMS, false);
         break;
      case 10:
         fOK = DiagCAN.getODOcount(&BMS, false);
         break;
      case 11:
         fOK = DiagCAN.getIsolationValue(&BMS, false);
         break;
    }
    if (!myDevice.logging && testStep < 12) {
      if (fOK) {
        if (myDevice.progress) Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print('#'); Serial.print(selected[testStep]);
      }
    }
    testStep++;
  } while (fOK && testStep < len);
  
  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Get OBL datasets
//--------------------------------------------------------------------------------
boolean getOBLdata() {
  boolean fOK = false;
  
  //Get diagnostics data
  DiagCAN.setCAN_ID(rqID_OBL, respID_OBL);

  byte testStep = 0;
  do {
    switch (testStep) {
      case OBLdc:
         fOK = DiagCAN.getChargerDC(&OBL, false);
         break;
      case OBLac:
         fOK = DiagCAN.getChargerAC(&OBL, false);
         break;
      case OBLctrl:
         fOK = DiagCAN.getChargerCtrlValues(&OBL, false);
         break;
      case OBLt:
         fOK = DiagCAN.getChargerTemperature(&OBL, false);
         break;
    }
    if (!myDevice.logging && testStep < OBLCOUNT) {
      if (fOK) {
        if (myDevice.progress) Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print('#'); Serial.print(testStep);
      }
    }
    fOK = true;
    testStep++;
  } while (fOK && testStep < OBLCOUNT);
  
  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Get TCU datasets
//--------------------------------------------------------------------------------
boolean getTCUdata() {
  boolean fOK = false;
  
  //Get diagnostics data
  DiagCAN.setCAN_ID(rqID_TCU, respID_TCU);

  fOK = DiagCAN.getTCUdata(&TCU, false);   

  if (!myDevice.logging) {
    if (fOK) {
      if (myDevice.progress) Serial.print(MSG_DOT);
    } else {
      Serial.print(MSG_FAIL);Serial.print(F("#0"));
    }
  }
  return fOK;
}
