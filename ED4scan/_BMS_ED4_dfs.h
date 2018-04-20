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
//! \file    ED4_BMS_dfs.h
//! \brief   Definitions and structures for the BMS module.
//! \date    2018-April
//! \author  MyLab-odyssey
//! \version 0.4.3
//--------------------------------------------------------------------------------
#ifndef BMS_ED4_DFS_H
#define BMS_ED4_DFS_H

//Definitions for BMS
#define DATALENGTH 126
#define CELLCOUNT 96
#define RAW_VOLTAGES 0           //!< Use RAW values or calc with ADC resolution 
#define IQR_FACTOR 1.5           //!< Factor to define Outliners-Range, 1.5 for suspected outliners, 3 for definitive outliners

const char* const PROGMEM EVMODES[] ={"HV OFF", "slow CHG", "fast CHG", "HV ON"};
const char* const PROGMEM CAPMODES[] ={"As/10", "As/100"};

//Data structure for statistics (min, mean, max values, percentiles)
template<typename T>
struct Stats_t{
  int16_t min;                  //!< minimum
  byte p25_out_count;            //!< count of datasets below p25, including mininal value
  int16_t p25;                  //!< 25th percentile
  T mean;                        //!< average 
  int16_t median;               //!< 50th percentile
  int16_t p75;                  //!< 75th percentile
  byte p75_out_count;            //!< count of datasets above p75, including maximum value
  int16_t max;                  //!< maximum
};

//Data structure for value-range (min, mean, max values)
template<typename T>
struct Range_t{
  T min;                         //!< minimum
  T mean;                        //!< average
  T max;                         //!< maximum
};

//Data structure for power calc (voltage, current)
template<typename T>
struct Power_t{
  T voltage;                     //!< voltage value sample (raw), (x/64) for actual value
  T current;                     //!< current value sample (raw), (x/32) for actual value
  float power;                   //!< calculated power of sample in kW
};

//Data structure for limit values (voltage, current, charge, discharge)
typedef struct {
  float MaxChargeAmps;           //!< Maximum allowed charge current
  float MaxDischargeAmps;        //!< Maximum allowed discharge current
  uint16_t MaxCV;                //!< Maximum allowed cell voltage charging
  uint16_t MinCV;                //!< Maximum allowed cell voltage discharging
  uint16_t Zcharge;              //!< Impedance charging mOhm
  uint16_t Zdischarge;           //!< Impedance discharging mOhm  
  byte BattPowerMax;             //!< Maximum available power of battery in kW, (x/2)
  byte BattPowerGen;             //!< Maximum generated power of battery in kW, (x/2)
  byte BattPowerCharge;          //!< Maximum recuperated power of battery in kW, (x/2)
} Limits_t;

//BMS data structure
typedef struct {   
  Range_t<uint16_t> CV_Range;    //!< average cell voltage in mV, no offset

  float BattCV_Sum;              //!< Sum of single cell voltages
  float BattLinkVoltage;         //!< Link voltage to drivetrain inverter

  uint16_t BattBalState;         //!< seems to indicate needed balancing?!
  byte BattBalXOR;               //!< XOR of 96 values => balancing mode of cells all equal?
 
  float HV;                      //!< total voltage of HV system in V
  float LV;                      //!< 12V onboard voltage / LV system
  byte LV_DCDC_amps;             //!< current of DC/DC LV system, not 12V battery!
  byte LV_DCDC_load;             //!< load in % of DC/DC LV system, not 12V battery!
  byte LV_DCDC_power;            //!< power in W (x/10) of DC/DC LV system, not 12V battery!
  byte LV_DCDC_state;            //!< DC/DC state

  Power_t<int16_t> BattPower;    //!< Actual sampled power values
  Limits_t BattLimit;            //!< Limiting values of the battery
  
  Stats_t<uint16_t> Cap_As;      //!< cell capacity statistics from BMS measurement cycle
  float Cap_meas_quality;        //!< some sort of estimation factor??? after measurement cycle
  float Cap_combined_quality;    //!< some sort of estimation factor??? constantly updated
  uint16_t LastMeas_days;        //!< days elapsed since last successful measurement
  
  Stats_t<float> Cvolts;         //!< calculated statistics from individual cell voltage query              
  int16_t CV_min_at;             //!< cell number with voltage mininum in pack
  int16_t CV_max_at;             //!< cell number with voltage maximum in pack
  float Cvolts_stdev;            //!< calculated standard deviation (populated)
  
  Stats_t<float> Ccap_As;        //!< cell capacity statistics calculated from individual cell data
  uint16_t CAP2_mean;             //!< cap mean from impedance track cap measurement
  uint16_t CAPusable_max;         //!< usable Ah by latest charge (As/10)
  uint16_t CAP_min_at;            //!< cell number with capacity mininum in pack
  uint16_t CAP_max_at;            //!< cell number with capacity maximum in pack
  
  uint16_t CapInit;              //!< battery initial capacity (x/16) in As/10 at a certain/defined temperature maybe 45 degC
  uint16_t CapMeas;              //!< battery capacity measured in As/10
  uint16_t CapEstimate;          //!< estimated capacity by BMS (x/16) in As/10
  
  boolean KeyState = 0;          //!< Status of "Ignition-Key"
  byte EVmode = 0;               //!< Mode the EV is actually in: 0 = none, 1 = slow charge, 2 = fast charge, 3 = normal, 4 = fast balance

  byte ProdDay;                  //!< day of battery production
  byte ProdMonth;                //!< month of battery production
  byte ProdYear;                 //!< year of battery production
  
  float SOC;                     //!< State of Charge, as displayed in dash (x/50)
  byte SOCrecalState;            //!< State of SOC recal (experimental, tbd)
  
  byte fSOH;                     //!< Flag showing if degraded cells are found, or battery failiure present 
  byte SOH;                      //!< Battery State of Health in %
  Range_t<uint16_t> realSOC;     //!< The internal SOC value in % (x/16), reported by BMS incl. min, mean, max
    
  int16_t Amps;                  //!< battery current in ampere (x/32) reported by by BMS
  float Amps2;                   //!< battery current in ampere read by live data on CAN or from BMS
  float Power;                   //!< power as product of voltage and amps in kW
  uint16_t Energy;               //!< available energy in kWh (x/200)
  
  unsigned long ODO;             //!< Odometer count
  uint16_t EVrange;              //!< Usable range in km
  boolean EVeco;                 //!< Indicate ECO mode
   
  int16_t Temps[31];             //!< ten temperatures per battery unit (1 to 3)
                                 //!< + max, min, mean and coolant-in temperatures
  int16_t AmbientTemp;           //!< Ambient temperature reported by dash
  uint16_t Isolation;            //!< Isolation in DC path, resistance in kOhm
  byte DCfault;                  //!< Flag to show DC-isolation fault
  
  byte HVcontactState;           //!< contactor state: 0 := OFF, 1 := PRECHARGE, 2 := ON
  long HVcontactCyclesLeft;      //!< counter related to ON/OFF cyles of the car
  long HVcontactCyclesMax;       //!< static, seems to be maxiumum of contactor cycles 
} BatteryDiag_t; 

const PROGMEM uint32_t rqID_BMS                   = 0x79B;
const PROGMEM uint32_t respID_BMS                 = 0x7BB; 
const PROGMEM byte rqPartNo[2]                    = {0x21, 0xEF};
const PROGMEM byte rqIDpart[2]                    = {0x21, 0x80};
const PROGMEM byte rqBattHealth[2]                = {0x21, 0x61}; 
const PROGMEM byte rqBattLimits[2]                = {0x21, 0x01}; 
const PROGMEM byte rqBattHVContactorCycles[2]     = {0x21, 0x02}; 
const PROGMEM byte rqBattState[2]                 = {0x21, 0x07}; 
const PROGMEM byte rqBattSOC[2]                   = {0x21, 0x08}; 
const PROGMEM byte rqBattSOCrecal[2]              = {0x21, 0x25}; 
const PROGMEM byte rqBattTemperatures[2]          = {0x21, 0x04}; 
const PROGMEM byte rqBattVoltages_P1[2]           = {0x21, 0x41}; 
const PROGMEM byte rqBattVoltages_P2[2]           = {0x21, 0x42}; 
const PROGMEM byte rqBattOCV_Cal[2]               = {0x21, 0x05};
const PROGMEM byte rqBattCellResistance_P1[2]     = {0x21, 0x10};
const PROGMEM byte rqBattCellResistance_P2[2]     = {0x21, 0x11};
const PROGMEM byte rqBattIsolation[2]             = {0x21, 0x29}; 
const PROGMEM byte rqBattMeas_Capacity[2]         = {0x21, 0x0B};
const PROGMEM byte rqBattProdDate[3]              = {0x22, 0x03, 0x04}; 
const PROGMEM byte rqBattCapInit[3]               = {0x22, 0x03, 0x05}; 
const PROGMEM byte rqBattSN[2]                    = {0x21, 0xA0};
const PROGMEM byte rqBattCapacity_dSOC_P1[2]      = {0x21, 0x12}; 
const PROGMEM byte rqBattCapacity_dSOC_P2[2]      = {0x21, 0x13}; 
const PROGMEM byte rqBattCapacity_P1[2]           = {0x21, 0x14};
const PROGMEM byte rqBattCapacity_P2[2]           = {0x21, 0x15};
const PROGMEM byte rqBattBalancing[2]             = {0x21, 0x16}; 
const PROGMEM byte rqBattLogData_P1[2]            = {0x21, 0x30};

const PROGMEM uint32_t rqID_DASH                  = 0x743;
const PROGMEM uint32_t respID_DASH                = 0x763; 
const PROGMEM byte rqDashODO[3]                   = {0x22, 0x02, 0x07};
const PROGMEM byte rqDashTemperature[3]           = {0x22, 0x20, 0x0C};

const PROGMEM uint32_t rqID_EVC                   = 0x7E4;
const PROGMEM uint32_t respID_EVC                 = 0x7EC;
const PROGMEM byte rqKeyState[3]                  = {0x22, 0x20, 0x0E};
const PROGMEM byte rqDCDC_State[3]                = {0x22, 0x30, 0x2A};
const PROGMEM byte rqDCDC_Load[3]                 = {0x22, 0x34, 0x95};
const PROGMEM byte rqDCDC_Amps[3]                 = {0x22, 0x30, 0x25};
const PROGMEM byte rqDCDC_Power[3]                = {0x22, 0x34, 0x94};
const PROGMEM byte rqHV_Energy[3]                 = {0x22, 0x32, 0x0C}; //usable energy kWh
const PROGMEM byte rqEV_ChgLog_P1[3]              = {0x22, 0x33, 0xD4};
const PROGMEM byte rqEV_Range[3]                  = {0x22, 0x34, 0x51};


#endif // of #ifndef BMS_ED4_DFS_H
