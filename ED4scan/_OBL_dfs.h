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
//! \file    _OBL_dfs.h
//! \brief   Definitions and structures for the On-Board-Loader (Charger) module.
//! \date    2018-May
//! \author  MyLab-odyssey
//! \version 0.5.1
//--------------------------------------------------------------------------------
#ifndef OBL_DFS_H
#define OBL_DFS_H

//Definitions for OBL
#define TEMP_OFFSET 50
boolean const FASTCHG = false; //!< Set to true if you have the 22kW fast charger 
                               //!< to compile for this variant
                               
const char PROGMEM ID_7KW[] = {'2','2','3','9'};
const char PROGMEM ID_22KW[] = {'0','1','3','2'};
const char* const PROGMEM OBL_ID[] ={"22KW", "7KW"};
const char* const PROGMEM OBL_STATE[] ={"CHG", "ON", "STBY"};
const char* const PROGMEM PILOT_STATE[] ={"-", "B", "C", "D", "E"};

//OBL data structure
typedef struct {       
  char OBL7KW = -1;               //!< Flag set if standard (TRUE) or 22kW charger (FALSE) is detected in system, -1 indicate fail
  uint16_t MainsAmps[3];          //!< AC current of L1, L2, L3 (x/10)
  uint16_t MainsVoltage[3];       //!< AC voltage of L1, L2, L3 (x/10)
  uint16_t CHGpower[3];           //!< Power of rail1, rail2 W (x/2) & max available kw (x/64)
  uint16_t MainsFreq;             //!< AC input frequency
  byte Amps_setpoint;             //!< AC charging current set as maximum limit
  byte newAmps_setpoint = 20;     //!< AC charging current setpoint
  uint16_t AmpsCableCode;         //!< Maximum current cable (resistor coded in Ohm)
  byte AmpsChargingpoint;         //!< Maxiumum current of chargingpoint
  byte PilotState;                //!< State of charging station signaled by pilot level
  byte ChargerState;              //!< State of charger: charging / standby
  uint16_t DC_Current;            //!< DC current measured by charger
  uint16_t DC_HV;                 //!< DC HV measured by charger
  uint16_t LV;                    //!< 12V onboard voltage of Charger DC/DC
  byte InTemp;                    //!< water inlet temperature
  byte OutTemp;                   //!< water outlet temperature
  byte InternalTemp;              //!< internal temperature, reported by charger
  byte CoolantTemp;               //!< Coolant temperature degC (x-40)
  byte SysTemp;                   //!< System temperature in %
} ChargerDiag_t; 

const PROGMEM uint32_t rqID_OBL                   = 0x792;
const PROGMEM uint32_t respID_OBL                 = 0x793;
const PROGMEM byte rqChargerLV[3]                 = {0x22, 0x73, 0x01};
const PROGMEM byte rqChargerAC[3]                 = {0x22, 0x73, 0x03};
const PROGMEM byte rqChargerDC[3]                 = {0x22, 0x73, 0x04};
const PROGMEM byte rqChargerTemperatures[3]       = {0x22, 0x73, 0x00};
const PROGMEM byte rqChargerState[3]              = {0x22, 0x30, 0x08};
const PROGMEM byte rqJB2LV[3]                     = {0x22, 0x50, 0x6C};
const PROGMEM byte rqJB2AC_Ph1_RMS_A[3]           = {0x22, 0x20, 0x01};
const PROGMEM byte rqJB2AC_Ph2_RMS_A[3]           = {0x22, 0x50, 0x3A};
const PROGMEM byte rqJB2AC_Ph3_RMS_A[3]           = {0x22, 0x50, 0x3B};
const PROGMEM byte rqJB2AC_Ph12_RMS_V[3]          = {0x22, 0x50, 0x3F};
const PROGMEM byte rqJB2AC_Ph23_RMS_V[3]          = {0x22, 0x50, 0x41};
const PROGMEM byte rqJB2AC_Ph31_RMS_V[3]          = {0x22, 0x50, 0x42};
const PROGMEM byte rqJB2AC_Power[3]               = {0x22, 0x50, 0x4A};
const PROGMEM byte rqJB2DC_A[3]                   = {0x22, 0x50, 0x4D};
const PROGMEM byte rqJB2DC_V[3]                   = {0x22, 0x50, 0x4E};
const PROGMEM byte rqJB2Temp_SYS[3]               = {0x22, 0x20, 0x04};
const PROGMEM byte rqJB2Temp_COOL[3]              = {0x22, 0x20, 0x05};
const PROGMEM byte rqJB2Temp_HOT[3]               = {0x22, 0x50, 0x54};
const PROGMEM byte rqJB2State[3]                  = {0x22, 0x20, 0x03};
const PROGMEM byte rqJB2Pilot_DUTY[3]             = {0x22, 0x50, 0x26};
const PROGMEM byte rqJB2Pilot_V[3]                = {0x22, 0x50, 0x27};
const PROGMEM byte rqJB2SelCurrent[3]             = {0x22, 0x50, 0x70};

const PROGMEM uint32_t rqID_CHGCTRL               = 0x719;
const PROGMEM uint32_t respID_CHGCTRL             = 0x739; 
const PROGMEM byte rqTesterPresent[3]             = {0x3E, 0x01, 0x00};
const PROGMEM byte rqDiagSessionExt[3]            = {0x10, 0xC0, 0x00};
const PROGMEM byte rqChargerSelCurrent[3]         = {0x22, 0x61, 0x41};
const PROGMEM byte rqChargerPilotState[3]         = {0x22, 0x61, 0x55};
const PROGMEM byte rqMainsMaxCurrent[3]           = {0x22, 0x60, 0x2A};
const PROGMEM byte rqMaxPowerAvail[3]             = {0x22, 0x60, 0x2D};
const PROGMEM byte rqCoolantTemp[3]               = {0x22, 0x60, 0x32};
const PROGMEM byte rqConnectorCoding[3]           = {0x22, 0x20, 0x03};

#endif // of #ifndef OBL_DFS_H
