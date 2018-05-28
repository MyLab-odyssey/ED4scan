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
//! \file    ED4scan_PRN.ino
//! \brief   Functions for serial printing the datasets
//! \date    2018-May
//! \author  MyLab-odyssey
//! \version 0.5.1
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//! \brief   Output a space-line for separation of datasets
//! \brief   with and without titel
//--------------------------------------------------------------------------------
void PrintSPACER() {
  for (byte i = 0; i < 41; i++) Serial.print('-');
  Serial.println();
}

void PrintSPACER(const __FlashStringHelper* titel) {
  size_t len = getLength(titel) + 4;
  byte start = 4; //(42 - len) / 6;

  Serial.println();
  for (byte i = 0; i <= 41; i++) {
    if (i == start) {
      Serial.print(F(">>")); Serial.print(titel); Serial.print(F("<<"));
      i += len;
    } else {
      Serial.print('-');
    }
  }
  Serial.println();
}

//--------------------------------------------------------------------------------
//! \brief   Get length of __FlashStringHelper
//--------------------------------------------------------------------------------
size_t getLength(const __FlashStringHelper *ifsh)
{
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  size_t n = 0;
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    n++;
  }
  return n;
}

//--------------------------------------------------------------------------------
//! \brief   Output header data as welcome screen - wait for CAN-Bus to be ready
//--------------------------------------------------------------------------------
void printWelcomeScreen() {
  byte vLength = strlen(version);
  PrintSPACER();
  Serial.println(F("--- ED4 Smart CAN  --  EV Diagnostics ---"));
  Serial.print(F("--- v")); Serial.print(version);
  for (byte i = 0; i < (41 - 5 - vLength - 3); i++) {
    Serial.print(' ');
  }
  Serial.println(F("---"));
  PrintSPACER();

  Serial.println(F("Connect to OBD port - Waiting for CAN-Bus"));
  do {
    Serial.print('.');
    delay(1000);
  } while (digitalRead(2));
  Serial.println(F("CONNECTED"));
  PrintSPACER();
}

//--------------------------------------------------------------------------------
//! \brief   Output header data
//--------------------------------------------------------------------------------
void printHeaderData() {
  Serial.print(F("ODO  : ")); Serial.print(BMS.ODO); Serial.print(F(" km, Amb.T: "));
  Serial.print(BMS.AmbientTemp / 10.0, 1); Serial.println(F(" degC"));
}

//--------------------------------------------------------------------------------
//! \brief   Output battery production data and battery status SOH flag
//--------------------------------------------------------------------------------
void printBatteryProductionData(boolean fRPT) {
  (void) fRPT;  // Avoid unused param warning
  
  Serial.print(F("SOH  : ")); Serial.print(BMS.SOH / 2.0, 1); Serial.print(F("%, "));
  if (BMS.fSOH == 0xFF) {
    Serial.println(MSG_OK);
  } else if (BMS.fSOH == 0) {
    Serial.println(F("FAULT"));
  } else {
    Serial.print(F("UNDEF")); Serial.print(F(", ")); Serial.println(BMS.fSOH, HEX);
  }
  Serial.print(F("Y/M/D: ")); Serial.print(2000 + BMS.ProdYear); Serial.print('/');
  Serial.print(BMS.ProdMonth); Serial.print('/'); Serial.println(BMS.ProdDay);
  byte type[3] = {0x37, 0x38, 0x39};
  DiagCAN.setCAN_ID(rqID_BMS, respID_BMS);
  DiagCAN.printECUrev(false, type);
}

//--------------------------------------------------------------------------------
//! \brief   Output standard dataset
//--------------------------------------------------------------------------------
void printStandardDataset() {
  Serial.print(F("SOC  : ")); Serial.print(BMS.SOC,1); Serial.print(F(" % = "));
  Serial.print(BMS.Energy / 200.0, 2); Serial.print(F(" kWh = "));
  if (BMS.EVrange < 0x3FF) {
    Serial.print(BMS.EVrange);
  } else {
    Serial.print('-');
  }
  Serial.print(F(" km"));
  if (BMS.EVeco) {
    Serial.println(F(" ECO"));
  } else {
    Serial.println();
  }
  Serial.print(F("rSOC : ")); Serial.print((float) BMS.realSOC.min / 16.0, 1); Serial.print(F(", "));
  Serial.print(BMS.realSOC.mean / 16.0, 1); Serial.print(F(", ")); Serial.print(BMS.realSOC.max / 16.0, 1); Serial.println(F(" %"));
  Serial.print(F("HV   : ")); Serial.print(BMS.HV,1); Serial.print(F(" V, "));
  Serial.print((float) BMS.Amps2, 2); Serial.print(F(" A, "));
  if (BMS.Power != 0) {
    Serial.print((float) BMS.Power, 2);
  } else {
    Serial.print(F("0.00"));
  }
  Serial.println(F(" kW"));   
  Serial.print(F("LV   : ")); Serial.print(BMS.LV, 1); Serial.print(F(" V, "));
  Serial.print(BMS.LV_DCDC_amps / 10.0, 1); Serial.print(F(" A, "));
  Serial.print(BMS.LV_DCDC_power,1); Serial.print(F(" W, "));
  Serial.print(BMS.LV_DCDC_load / 256.0 * 100.0,0); Serial.println(F(" %"));
  Serial.print(F("EV   : "));
  Serial.print((char *) pgm_read_word(ON_OFF + (BMS.KeyState))); Serial.print(F(", ")); 
  if (BMS.EVmode <= 3 || BMS.EVmode == 5) {
    Serial.print((char *) pgm_read_word(EVMODES + BMS.EVmode));
  } else {
    Serial.print(BMS.EVmode, HEX);
  }
  Serial.print(F(", "));
  Serial.print(BMS.BattLimit.BattPowerMax / 2.0, 1); Serial.print(F(", "));
  Serial.print(BMS.BattLimit.BattPowerGen / 2.0, 1); Serial.print(F(", "));
  Serial.print(BMS.BattLimit.BattPowerCharge / 2.0, 1); Serial.println(F(" kW"));
}

//--------------------------------------------------------------------------------
//! \brief   Output BMS cell voltages
//--------------------------------------------------------------------------------
void printBMS_CellVoltages() {
  Serial.print(F("CV mean  : ")); Serial.print(BMS.CV_Range.mean); Serial.print(F(" mV"));
  Serial.print(F(", dV = ")); Serial.print(BMS.CV_Range.max - BMS.CV_Range.min); Serial.println(F(" mV"));
  Serial.print(F("CV min   : ")); Serial.print(BMS.CV_Range.min); Serial.print(F(" mV, LIM: "));
  Serial.print(BMS.BattLimit.MinCV); Serial.println(F(" mV"));
  Serial.print(F("CV max   : ")); Serial.print(BMS.CV_Range.max); Serial.print(F(" mV, LIM: "));
  Serial.print(BMS.BattLimit.MaxCV); Serial.println(F(" mV"));
  Serial.print(F("Ri in,out: ")); Serial.print(BMS.BattLimit.Zcharge); Serial.print(F(", "));
  Serial.print(BMS.BattLimit.Zdischarge); Serial.println(F(" mOhm"));
  Serial.print(F("Balancing: ")); Serial.print(BMS.BattBalState, HEX);
  Serial.print(F(", XOR: ")); Serial.println(BMS.BattBalXOR, HEX);
}

//--------------------------------------------------------------------------------
//! \brief   Output BMS capacity estimation
//--------------------------------------------------------------------------------
void printBMS_CapacityEstimate() {
  Serial.print(F("Measured : ")); Serial.print(BMS.CAP2_mean / 360.0, 3); Serial.print(F(", "));
  Serial.print(BMS.CapMeas / 360.0, 3); Serial.println(F(" (dSOC), "));
  Serial.print(F("Estimate : ")); Serial.print(BMS.Cap_combined_quality,3); Serial.print(F(" of ")); 
  Serial.print(BMS.Cap_meas_quality,3); Serial.print(F(", "));
  Serial.print(BMS.LastMeas_days); Serial.println(F(" day(s)"));
  Serial.print(F("Capacity : ")); Serial.print(BMS.CapInit / 360.0, 3); Serial.print(F(" (LIM), "));
  Serial.print(BMS.CapEstimate / 360.0, 3); Serial.println(F(" (EST)"));
}

//--------------------------------------------------------------------------------
//! \brief   Output SOH state and capacity estimation
//--------------------------------------------------------------------------------
void printBMS_SOHstate() {
  Serial.print(F("SOH : ")); Serial.print(BMS.SOH / 2.0, 1); Serial.print(F(" %, "));
  Serial.print(BMS.CAPusable_max / 360.0, 3); Serial.println(F(" Ah"));
}

//--------------------------------------------------------------------------------
//! \brief   Output HV contactor state and DC isolation
//--------------------------------------------------------------------------------
void printHVcontactorState() {
  Serial.print(F("HV contr.: "));
  if (BMS.HVcontactState == 0x02) {
    Serial.println(F("ON"));
  } else if (BMS.HVcontactState == 0x00) {
    Serial.println(F("OFF"));
  }
  Serial.print(F("Cycles   : ")); Serial.print(BMS.HVcontactCyclesLeft);
  Serial.print(F(" of ")); Serial.println(BMS.HVcontactCyclesMax);
  Serial.print(F("DC iso.  : >")); Serial.print(BMS.Isolation); Serial.print(F(" kOhm, MF:"));
  Serial.println(BMS.DCfault, BIN); //Print Flag-Bits for isolation measurement
}

//--------------------------------------------------------------------------------
//! \brief   Output BMS temperatures
//--------------------------------------------------------------------------------
void printBMStemperatures() {
  for (byte n = 3; n < 28; n = n + 9) {
    Serial.print(F("module ")); Serial.print((n / 9) + 1); Serial.print(F(": "));
    for (byte i = 0; i < 9; i++) {
      float temp = BMS.Temps[n + i] / 64.0;
      if (temp >= 0) Serial.print(' ');
      Serial.print((float) temp, 0);
      if ( i < 8) {
        Serial.print(',');
      } else {
        Serial.println();
      }
    }
  }
  Serial.print(F("   mean : ")); Serial.print((float) BMS.Temps[2] / 64.0, 0);
  Serial.print(F(", min : ")); Serial.print((float) BMS.Temps[1] / 64.0, 0);
  Serial.print(F(", max : ")); Serial.println((float) BMS.Temps[0] / 64.0, 0);
}

//--------------------------------------------------------------------------------
//! \brief   Output individual cell data and statistics
//--------------------------------------------------------------------------------
void printIndividualCellData() {
  Serial.print(F("# ;mV  ;As/10"));
  if (myDevice.CapMeasMode == 1) {
    Serial.println(F(" dSOC"));
  } else {
    Serial.println();
  }
  for(int16_t n = 0; n < CELLCOUNT; n++){
    if (n < 9) Serial.print('0');
    Serial.print(n+1); Serial.print(F(";")); Serial.print(DiagCAN.getCellVoltage(n));
    Serial.print(F(";")); Serial.print(DiagCAN.getCellCapacity(n) * BMS.CAP_factor);
    Serial.println();
  }
  PrintSPACER(F("Cell Statistics"));
  Serial.print(F("CV mean : ")); Serial.print(BMS.Cvolts.mean,0); Serial.print(F(" mV"));
  Serial.print(F(", dV= ")); Serial.print(BMS.Cvolts.max - BMS.Cvolts.min); Serial.print(F(" mV"));
  Serial.print(F(", s= ")); Serial.print(BMS.Cvolts_stdev); Serial.println(F(" mV"));
  Serial.print(F("CV min  : ")); Serial.print(BMS.Cvolts.min); Serial.print(F(" mV, # ")); Serial.println(BMS.CV_min_at + 1);
  Serial.print(F("CV max  : ")); Serial.print(BMS.Cvolts.max); Serial.print(F(" mV, # ")); Serial.println(BMS.CV_max_at + 1);
  PrintSPACER();
  Serial.print(F("CAP mean: ")); Serial.print(BMS.Ccap_As.mean, 0); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.mean / 360.0,1); Serial.println(F(" Ah"));
  Serial.print(F("CAP min : ")); Serial.print(BMS.Ccap_As.min); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.min / 360.0,1); Serial.print(F(" Ah, # ")); Serial.println(BMS.CAP_min_at + 1);
  Serial.print(F("CAP max : ")); Serial.print(BMS.Ccap_As.max); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.max / 360.0,1); Serial.print(F(" Ah, # ")); Serial.println(BMS.CAP_max_at + 1);
}

//--------------------------------------------------------------------------------
//! \brief   Visualize voltage distribution of cell data and statistics
//--------------------------------------------------------------------------------
void printVoltageDistribution() {
  uint16_t CVmin = BMS.Cvolts.min;
  uint16_t CVmax = BMS.Cvolts.max;
  uint16_t CVp25 = BMS.Cvolts.p25;
  uint16_t CVp50 = BMS.Cvolts.median;
  uint16_t CVp75 = BMS.Cvolts.p75;
  uint16_t CVp3IQR = (BMS.Cvolts.p75 - BMS.Cvolts.p25) * IQR_FACTOR;
  
  byte bp_p25 = map(CVp25, CVmin, CVmax, 0, 40);
  byte bp_p50 = map(CVp50, CVmin, CVmax, 0, 40);
  byte bp_p75 = map(CVp75, CVmin, CVmax, 0, 40);
  byte bp_p3IQR_low = map(CVp25 - CVp3IQR, CVmin, CVmax, 0, 40);
  byte bp_p3IQR_high = map(CVp75 + CVp3IQR, CVmin, CVmax, 0, 40);
 
  Serial.print(F("Voltage Distribution (dV= ")); Serial.print(CVmax - CVmin); Serial.println(F(" mV)"));

  Serial.print('*');
  for (byte n = 1; n < 40; n++) {
    if (n < bp_p25) {
      if (n == bp_p3IQR_low) {
        Serial.print('>');
      } else {
        Serial.print('-');
      }
    } else if (n == bp_p25) {
      Serial.print('[');
    } else if (n < bp_p50) {
      Serial.print('=');
    } else if (n == bp_p50) {
      Serial.print('|');
    } else if (n > bp_p50 && n < bp_p75) {
      Serial.print('=');
    } else if (n == bp_p75) {
      Serial.print(']');
    } else {
      if (n == bp_p3IQR_high) {
        Serial.print('<');
      } else {
        Serial.print('-');
      }
    }
  }
  Serial.println('*');

  Serial.print(CVmin);
  Serial.print(F("   "));
  if (BMS.Cvolts.p25_out_count < 10) Serial.print('0');
  Serial.print(BMS.Cvolts.p25_out_count); Serial.print(F(" > "));
  Serial.print('[');
  Serial.print(CVp25); Serial.print(F("; "));
  Serial.print(CVp50); Serial.print(F("; "));
  Serial.print(CVp75); Serial.print(']');
  Serial.print(F(" < "));
  if (BMS.Cvolts.p75_out_count < 10) Serial.print('0');
  Serial.print(BMS.Cvolts.p75_out_count); Serial.print(' ');
  Serial.print(' '); Serial.print(CVmax);

  Serial.println();
  Serial.print(F("min"));
  for (byte n = 0; n <= 8; n++) Serial.print(' ');
  Serial.print(F("[p25; median; p75]"));
  for (byte n = 0; n <= 7; n++) Serial.print(' ');
  Serial.println(F("max"));
}

//--------------------------------------------------------------------------------
//! \brief   Output On-Board Charger voltages and currents AC and DC
//--------------------------------------------------------------------------------
void printOBL_Status() {
  Serial.print(F("EVSE status: ")); Serial.print(OBL.AmpsChargingpoint); Serial.print(F(" A, State: "));
  if (OBL.PilotState <= 4) {
    Serial.println((char *) pgm_read_word(PILOT_STATE + (OBL.PilotState)));
  } else {
    Serial.println(OBL.PilotState, HEX);
  }
  if (!FASTCHG) {
    Serial.print(F("Cable code : ")); 
    if (OBL.AmpsCableCode > 0) {
      Serial.print(OBL.AmpsCableCode); Serial.println(F(" Ohm"));
    } else {
      Serial.println('-');
    }
    Serial.print(F("Charger max: ")); Serial.print(OBL.Amps_setpoint); Serial.print(F(" A, State: "));
    if (OBL.ChargerState <= 4) {
      Serial.println((char *) pgm_read_word(OBL_STATE + (OBL.ChargerState / 2)));
    } else {
      Serial.println(OBL.ChargerState, HEX);
    }
    Serial.print(F("AC L1: ")); Serial.print(OBL.MainsVoltage[0] / 10.0, 1); Serial.print(F(" V, "));
    Serial.print((OBL.MainsAmps[0] + OBL.MainsAmps[1]) / 10.0, 1); Serial.print(F(" A, "));
    Serial.print(OBL.MainsFreq, 1); Serial.println(F(" Hz"));
    Serial.print(F("AC R1: ")); Serial.print(OBL.CHGpower[0] / 2000.0, 2); Serial.print(F(" kW, R2: "));
    Serial.print(OBL.CHGpower[1] / 2000.0, 2); Serial.print(F(" kW, max: "));
    Serial.print(OBL.CHGpower[2] / 64.0, 2); Serial.println(F(" kW"));
  } else {
    Serial.print(F("Charger max: ")); Serial.print(OBL.Amps_setpoint); Serial.print(F(" A, State: "));
    Serial.println((char *) pgm_read_word(OBL_STATE + (OBL.ChargerState)));
    Serial.print(F("AC L1-2-3: ")); Serial.print(OBL.MainsVoltage[0] / 2.0, 1); Serial.print(F(", "));
    Serial.print(OBL.MainsVoltage[1] / 2.0, 1); Serial.print(F(", "));
    Serial.print(OBL.MainsVoltage[2] / 2.0, 1); Serial.println(F(" V"));
    Serial.print(F("           ")); Serial.print(OBL.MainsAmps[0] / 10.0, 1); Serial.print(F(", "));
    Serial.print(OBL.MainsAmps[1] / 10.0, 1); Serial.print(F(", "));
    Serial.print(OBL.MainsAmps[2] / 10.0, 1); Serial.print(F(" A; "));
    Serial.print(OBL.CHGpower[0] / 1000.0, 3); Serial.println(F(" kW"));
  }
  Serial.print(F("DC HV: ")); Serial.print(OBL.DC_HV / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(OBL.DC_Current / 10.0, 2); Serial.println(F(" A"));
  if (!FASTCHG) {
    Serial.print(F("DC LV: ")); Serial.print(OBL.LV / 100.0, 1); Serial.println(F(" V"));
  } else {
    Serial.print(F("DC LV: ")); Serial.print(OBL.LV / 64.0, 1); Serial.println(F(" V"));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Output OBL charger temperatures
//--------------------------------------------------------------------------------
void printOBLtemperatures() {
  if (!FASTCHG) {
    Serial.print(F("In     : ")); 
    if (OBL.InTemp < 0xFF) {
      Serial.println(OBL.InTemp - TEMP_OFFSET, DEC);
    } else {
      Serial.println('-');
    }
  } else {
    Serial.print(F("System : ")); Serial.print(OBL.SysTemp, DEC); Serial.println(F(" %"));
  }
  if (!FASTCHG) {
    Serial.print(F("Intern.: ")); 
    if (OBL.InternalTemp < 0xFF) {
      Serial.println(OBL.InternalTemp - TEMP_OFFSET, DEC);
    } else {
      Serial.println('-');
    }
    Serial.print(F("Out    : ")); 
    if (OBL.OutTemp < 0xFF) {
      Serial.println(OBL.OutTemp - TEMP_OFFSET, DEC);
    } else {
      Serial.println('-');
    }
  } else {
    Serial.print(F("Hotspot: ")); 
    if (OBL.InternalTemp < 0xFF) {
      Serial.println(OBL.InternalTemp - TEMP_OFFSET + 10, DEC);
    }
  }
  Serial.print(F("Coolant: ")); 
  if (OBL.OutTemp < 0xFF) {
    Serial.println(OBL.CoolantTemp - TEMP_OFFSET + 10, DEC);
  } else {
    Serial.println('-');
  }
}

//--------------------------------------------------------------------------------
//! \brief   Output OBL charger HW/SW revisions
//--------------------------------------------------------------------------------
void printOBLrevision() {
  byte type[3] = {0x34,0x35,0x33};
  DiagCAN.setCAN_ID(rqID_OBL, respID_OBL);
  DiagCAN.printECUrev(false, type); //get HW, SW revisons and send to serial
}

//--------------------------------------------------------------------------------
//! \brief   Output TCU data
//--------------------------------------------------------------------------------
void printTCU_Status() {
  Serial.print(F("Date/Time : "));
  Serial.print(1990 + TCU.TCUtime.year); Serial.print('/'); Serial.print(TCU.TCUtime.month); Serial.print('/'); Serial.print(TCU.TCUtime.day); Serial.print(' ');
  if (TCU.TCUtime.hour <= 9) Serial.print('0'); Serial.print(TCU.TCUtime.hour); Serial.print(':');
  if (TCU.TCUtime.minute <= 9) Serial.print('0'); Serial.print(TCU.TCUtime.minute); Serial.print(':'); 
  if (TCU.TCUtime.second <= 9) Serial.print('0'); Serial.print(TCU.TCUtime.second); Serial.println(F(" UTC"));
  Serial.print(F("RSSI      : ")); Serial.print((TCU.Rssi * 2.0) - 111.0, 0); Serial.print(F(" dBm, "));
  Serial.print(((TCU.Rssi * 4.0)), 0); Serial.println('%');
  Serial.print(F("State     : ")); Serial.println((char *) pgm_read_word(TCU_MODE + (TCU.State)));
  Serial.print(F("NetType   : ")); 
  if (TCU.NetType > 0) {
    Serial.print(TCU.NetType + 1); Serial.println('G');
  } else {
    Serial.println(F("offline"));
  }
  Serial.print(F("Network   : ")); DiagCAN.getTCUnetwork(&TCU, false);
  Serial.print(F("SMS count : ")); Serial.println(TCU.Counter, DEC);
}

//--------------------------------------------------------------------------------
//! \brief   Output status data as splash screen
//--------------------------------------------------------------------------------
void printSplashScreen() {
  Serial.println(); PrintSPACER();
  printHeaderData();
  PrintSPACER();
  printStandardDataset();
  PrintSPACER();
  Serial.println(F("ENTER command (? for help)"));
}

//--------------------------------------------------------------------------------
//! \brief   Output BMS dataset
//--------------------------------------------------------------------------------
void printBMSdata() {
  Serial.println(MSG_OK);
  digitalWrite(CS, HIGH);
  PrintSPACER(F("BMS Status"));
  printHeaderData();
  PrintSPACER();
  printStandardDataset();
  PrintSPACER();
  printBatteryProductionData(false);
  PrintSPACER();
  printBMS_CellVoltages();
  PrintSPACER();
  printHVcontactorState();
  PrintSPACER(F("Battery T/degC"));
  printBMStemperatures();
  PrintSPACER(F("Battery C/Ah"));
  printBMS_CapacityEstimate();
  PrintSPACER();
  if (myDevice.verbose) {
    printIndividualCellData();
    PrintSPACER();
  }
  if (BOXPLOT) {
    DiagCAN.getBatteryVoltageDist(&BMS);  //Sort cell voltages rising up and calc. quartiles
                                          //!!! after sorting track of individual cells is lost -> redo ".getBatteryVoltages" !!!
    printVoltageDistribution();           //Print statistic data as boxplot
    PrintSPACER();
  }
}

//--------------------------------------------------------------------------------
//! \brief   Output OBL dataset
//--------------------------------------------------------------------------------
void printOBLdata() {
  Serial.println(MSG_OK);
  digitalWrite(CS, HIGH);
  PrintSPACER(F("OBC Status"));
  printOBL_Status();
  PrintSPACER(F("OBC T/degC"));
  printOBLtemperatures();
  PrintSPACER();
}

//--------------------------------------------------------------------------------
//! \brief   Output Cooling- and Subsystem dataset
//--------------------------------------------------------------------------------
void printTCUdata() {
  Serial.println(MSG_OK);
  digitalWrite(CS, HIGH);
  PrintSPACER(F("TCU Status"));
  printTCU_Status();
  PrintSPACER();
}

//--------------------------------------------------------------------------------
//! \brief   Get all BMS data and output them
//! \brief   Dynamic memory allocation for CellVoltages and -Capacities
//! \brief   The allocated memory will be released after the data output
//--------------------------------------------------------------------------------
void printBMSall() {
  byte selected[12];   //hold list for selected tasks
  
  //Read all CAN-Bus IDs related to BMS
  for (byte i = 0; i < BMSCOUNT; i++) {
    selected[i] = i;
  }
  Serial.print(F("Reading data"));
  myDevice.progress = true;
  getState_BMS(selected, BMSCOUNT);
  
  //Reserve memory
  DiagCAN.reserveMem_CellVoltage();
  DiagCAN.reserveMem_CellCapacity();
  
  //Get all diagnostics data of BMS
  for (byte i = 0; i < 12; i++) {
    selected[i] = i;
  }
  if (getBMSdata(selected, 12)) {
    printBMSdata();
  } else {
    Serial.println();
    Serial.println(FAILURE);
  }
  myDevice.progress = false;
    
  //Free allocated memory
  DiagCAN.freeMem_CellVoltage();
  DiagCAN.freeMem_CellCapacity();
}

//--------------------------------------------------------------------------------
//! \brief   Get all OBL data and output them
//--------------------------------------------------------------------------------
void printOBLall() {
  Serial.print(F("Reading data"));
  if (getOBLdata()) {
    printOBLdata();
  } else {
    Serial.println();
    Serial.println(FAILURE);
  }
}

//--------------------------------------------------------------------------------
//! \brief   Get all Cooling- and Subsystem data and output them
//--------------------------------------------------------------------------------
void printTCUall() {
  Serial.print(F("Reading data"));
  if (getTCUdata()) {
    printTCUdata();
  } else {
    Serial.println();
    Serial.println(FAILURE);
  }
}

