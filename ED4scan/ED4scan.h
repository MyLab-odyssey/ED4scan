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
//! \file    ED4scan.h
//! \brief   Definitions and structures for the main program ED4scan.ino
//! \date    2018-May
//! \author  MyLab-odyssey
//! \version 0.5.1
//--------------------------------------------------------------------------------

#define VERBOSE 1                //!< Set default VERBOSE mode to output individual cell data
#define BOXPLOT 1                //!< Visualize cell statistics as boxplot
#define HELP 1                   //!< HELP menu active
#define ECHO 1                   //!< local ECHO of CLI 

#include <mcp_can.h>
#include <Timeout.h>
#include <Cmd.h>
#include "canDiagED4.h"

//Global definitions
const char* const PROGMEM version = "0.5.1";

#define FAILURE F("* FAIL *")
#define MSG_OK F("OK")
#define MSG_FAIL 'F'
#define MSG_DOT '.'
#define MSG_ALL F(" all   Full dataset")
#define MSG_V F(" v     Voltages")
#define MSG_A_STATUS F(", amps & status")
#define MSG_T F(" t     Temperatures")

const char* const PROGMEM ON_OFF[] ={"OFF", "ON"};

#define CS     10                //!< chip select pin of MCP2515 CAN-Controller
#define CS_SD  8                 //!< CS for SD card, if you plan to use a logger...
MCP_CAN CAN0(CS);                //!< Set CS pin

canDiag DiagCAN;                 //!< Diagnostics library object
BatteryDiag_t BMS;               //!< BMS (Battery Management System) dataset object
ChargerDiag_t OBL;               //!< OBL (On-Board Loader / Charger) dataset object
TCUdiag_t TCU;                   //!< TCU (Tele-Communications Unit) dataset object

CTimeout CAN_Timeout(5000);      //!< Timeout value for CAN response in millis
CTimeout CLI_Timeout(500);       //!< Timeout value for CLI polling in millis
CTimeout LOG_Timeout(30000);     //!< Timeout value for LOG activity in millis

//Menu levels
typedef enum {MAIN, subBMS, subOBL, subTCU} submenu_t;

//Test-Scheduler enum
enum BMStask {BMSstate, BMSsoc, BMSlimit, BMSbal, EVkey, EVdcdc, EVodo, EVrange, BMSCOUNT};
enum OBLtask {OBLdc, OBLac, OBLctrl, OBLt, OBLCOUNT};

//deviceStatus struct to store menu settings
typedef struct {
  submenu_t menu = MAIN;
  uint16_t timer = 30;           //!< timer interval for logging in seconds
  bool logging = false;          //!< flag for logging activity
  uint16_t logCount = 0;         //!< log entry counter
  byte CapMeasMode = CAP_MODE;   //!< selected capacity readout/display mode As/10 (1) or As/100 (2)
  bool verbose = VERBOSE;        //!< flag for individual cell voltages output
  bool ecu_list = 0;             //!< flag for listing ECU revisions under 'info' cmd
  bool progress = false;         //!< show task progress while reading data
} deviceStatus_t;

deviceStatus_t myDevice;
