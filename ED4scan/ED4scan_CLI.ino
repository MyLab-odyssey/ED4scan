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
//! \file    ED4scan_CLI.ino
//! \brief   Functions for the Command Line Interface (CLI) menu system
//! \date    2018-June
//! \author  MyLab-odyssey
//! \version 0.5.2
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//! \brief   Setup menu items
//--------------------------------------------------------------------------------
void setupMenu() {
  cmdInit();

  if (HELP) {  
    cmdAdd("?", help);
  } 
  cmdAdd("..", main_menu);
  cmdAdd("#", show_splash);
  cmdAdd("t", get_temperatures);
  cmdAdd("v", get_voltages);
  cmdAdd("bms", bms_sub);
  cmdAdd("tcu", tcu_sub);
  if (OBL.OBL7KW ^ FASTCHG) cmdAdd("obc", obl_sub);
  cmdAdd("all", get_all);
  cmdAdd("log", set_logging);
  cmdAdd("info", show_info);
  cmdAdd("set", set_cmd);
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get all datasets depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_all (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  switch (myDevice.menu) {
    case subBMS:
      printBMSall();
      break;
    case subOBL:
      printOBLall();
      break;
    case subTCU:
      printTCUall();
      break;
    case MAIN:
      printBMSall();
      printOBLall();
      printTCUall();
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get temperature values depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_temperatures (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  switch (myDevice.menu) {
    case subBMS:
      if (DiagCAN.getBatteryTemperature(&BMS, false)){
        printBMStemperatures();
      }
      break;
    case subOBL:
      if (DiagCAN.getChargerTemperature(&OBL, false)){
        printOBLtemperatures();
      }
      break;
    case subTCU:
      break;
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get voltages depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_voltages (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  byte bms_sel[] = {BMSstate, BMSlimit, BMSbal};
  
  switch (myDevice.menu) {
    case subBMS:
      getState_BMS(bms_sel, sizeof(bms_sel));
      Serial.println();
      printBMS_CellVoltages();
      break;
    case subOBL:
      if (getOBLdata()){
        Serial.println();
        printOBL_Status();
      }
      break;
    case subTCU:
      break;
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get OCV calibration table of BMS
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_OCVtable (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  switch (myDevice.menu) {
    case subBMS:
        PrintSPACER(F("OCV Lookup"));
        DiagCAN.printOCVtable(&BMS, false);
      break;
    case subOBL:
    case subTCU:
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get Cell Resistance factor of BMS
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_RESfactors (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  switch (myDevice.menu) {
    case subBMS:
        PrintSPACER(F("Cell Resistance"));
        DiagCAN.printRESfactors(&BMS, false);
      break;
    case subOBL:
    case subTCU:
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get Log data of BMS
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_BMSlog (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  switch (myDevice.menu) {
    case subBMS:
        PrintSPACER(F("BMS Log"));
        DiagCAN.printBMSlog(&BMS, false);
      break;
    case subOBL:
    case subTCU:
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get SOH state of the battery
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_SOHstate (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  byte bms_sel[] = {5};
  switch (myDevice.menu) {
    case subBMS:
      PrintSPACER(F("Battery SOH"));
      getBMSdata(bms_sel, sizeof(bms_sel));
      printBMS_SOHstate();
      break;
    case subOBL:
    case subTCU:
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get Charge-Log data of BMS
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void get_CHGlog (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  switch (myDevice.menu) {
    case subBMS:
    case subOBL:
        PrintSPACER(F("OBC Log"));
        DiagCAN.printCHGlog(false);
      break;
    case subTCU:
    case MAIN:
      break;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to show a help page depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
#ifdef HELP
void help(uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  switch (myDevice.menu) {
    case MAIN:
      Serial.println(F("Main:"));
      Serial.println(F(" BMS, OBC, TCU"));
      Serial.println(F(" all"));
      Serial.println(F(" info show config"));
      Serial.println(F(" log  on|off [t]"));
      Serial.println(F(" set  edit config"));
      Serial.println(F(" #    EV status"));
      break;
    case subBMS:
      Serial.println(F("BMS:"));
      Serial.println(MSG_ALL);
      Serial.println(MSG_V);
      Serial.println(MSG_T);
      break;
    case subOBL:
      Serial.println(F("OBC:"));
      Serial.println(MSG_ALL);
      Serial.print(MSG_V); Serial.println(MSG_A_STATUS);
      Serial.println(MSG_T);
      break;
    case subTCU:
      Serial.println(F("TCU:"));
      Serial.println(MSG_ALL);
      break;
  }   
  if ( myDevice.menu != MAIN) Serial.println(MSG_BACK);
}
#endif

//--------------------------------------------------------------------------------
//! \brief   Callback to show a splash screen for startup or by command
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void show_splash(uint8_t arg_cnt, char **args) {
  boolean fOK;
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  byte selected[] = {BMSstate, BMSsoc, BMSlimit, EVkey, EVdcdc, EVodo, EVrange};
  fOK = getState_BMS(selected, sizeof(selected)); 
  if (myDevice.progress & fOK) Serial.println(MSG_OK);
  printSplashScreen();
}

//--------------------------------------------------------------------------------
//! \brief   Callback to get all datasets depending on the active menu
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void show_info(uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  
  //Serial.print(F("Usable Memory: ")); Serial.println(getFreeRam());
  //Serial.print(F("Menu: ")); Serial.println(myDevice.menu);
  Serial.print(F("OBC     : ")); 
  if (OBL.OBL7KW > -1) {   
    if (OBL.OBL7KW ^ FASTCHG) { 
      Serial.print((char *) pgm_read_word(OBL_ID + (OBL.OBL7KW)));
      Serial.println(F(" inst."));
    } else {
      Serial.print(F("Recompile for "));
      Serial.println((char *) pgm_read_word(OBL_ID + (!OBL.OBL7KW)));
    }
  } else {
    Serial.println(FAILURE);
  }
  Serial.print(F("CV outp.: ")); Serial.println((char *) pgm_read_word(ON_OFF + (myDevice.verbose)));
  Serial.print(F("ECU_list: ")); Serial.println((char *) pgm_read_word(ON_OFF + (myDevice.ecu_list)));
  Serial.print(F("CAP_mode: ")); Serial.println((char *) pgm_read_word(CAPMODES + (myDevice.CapMeasMode-1)));
  Serial.print(F("Log dt/s: ")); Serial.print(myDevice.timer, DEC); Serial.print(F(", "));
  Serial.println((char *) pgm_read_word(ON_OFF + (myDevice.logging)));
  if (myDevice.ecu_list) {
    Serial.println();
    Serial.println(F("*BMS"));
    byte type[3] = {0x37, 0x38, 0x39};
    DiagCAN.setCAN_ID(rqID_BMS, respID_BMS);
    DiagCAN.printECUrev(false, type);
    Serial.println(F("*OBC"));
    type[0] = 0x34; type[1] = 0x35; type[2] = 0x33;
    DiagCAN.setCAN_ID(rqID_OBL, respID_OBL);
    DiagCAN.printECUrev(false, type);
    Serial.println(F("*OBC-CTRL"));
    DiagCAN.setCAN_ID(rqID_CHGCTRL, respID_CHGCTRL);
    DiagCAN.printECUrev(false, type);
    Serial.println(F("*TCU"));
    DiagCAN.setCAN_ID(rqID_TCU, respID_TCU);
    DiagCAN.printECUrev(false, type);
    Serial.println(F("*DASH"));
    DiagCAN.setCAN_ID(rqID_DASH, respID_DASH);
    DiagCAN.printECUrev(false, type);
    Serial.println(F("*EVC"));
    DiagCAN.setCAN_ID(rqID_EVC, respID_EVC);
    DiagCAN.printECUrev(false, type);
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to start logging and / or set parameters
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void set_logging(uint8_t arg_cnt, char **args) {
  if (arg_cnt > 2) {
    myDevice.timer = (unsigned int) cmdStr2Num(args[2], 10);
  } 
  if (arg_cnt > 1) {
    if (strcmp(args[1], "on") == 0) {
      myDevice.logging = true;
      LOG_Timeout.Reset(myDevice.timer * 1000);
      myDevice.logCount = 0;
    }
    if (strcmp(args[1], "off") == 0) {
      myDevice.logging = false;
    }
  } else {
    if (arg_cnt == 1) {
      show_info(arg_cnt, args);
    }
  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to set diagnostic parameters
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void set_cmd(uint8_t arg_cnt, char **args) {
  int8_t cmdOK = -1;
  if (arg_cnt == 3) {
    if (strcmp(args[1], "cap_mode") == 0) {
      myDevice.CapMeasMode = constrain((byte) cmdStr2Num(args[2], 10), 1, 2);
      DiagCAN.setCAPmode(myDevice.CapMeasMode);
      cmdOK = 1;
    } else if (strcmp(args[1], "cv") == 0 && strcmp(args[2], "on") == 0) {
      myDevice.verbose = 1;
      cmdOK = 1;
    } else if (strcmp(args[1], "cv") == 0 && strcmp(args[2], "off") == 0) {
      myDevice.verbose = 0;
      cmdOK = 1;
    } else if (strcmp(args[1], "ecu_list") == 0 && strcmp(args[2], "on") == 0) {
      myDevice.ecu_list = 1;
      cmdOK = 1;
    } else if (strcmp(args[1], "ecu_list") == 0 && strcmp(args[2], "off") == 0) {
      myDevice.ecu_list = 0;
      cmdOK = 1;
    }
  } else if (arg_cnt > 3) {
    //** >>> FOR TEST PURPOSE ONLY! DO NOT USE WHILE CHARGING! DO NOT USE ON US and UK cars, as they already work with 32A max<<< **
    //** >>> with entering [-yes] you ACCEPT ALL CONSEQUENCES AND THE USAGE IS SOLEY AT YOUR OWN RISK! <<< **
    //** >>> LOSS OF WARRANTY, DAMAGE(s), VIOLATION OF REGULATIVE RULES, NO LIABILITY FOR THIS SOFTWARE - SEE LICENSE STATEMENT! <<< **
    if (OBL.OBL7KW && (BMS.KeyState == 0) && strcmp(args[1], "ac_max") == 0 && strcmp(args[3], "-yes") == 0) {
      OBL.newAmps_setpoint = constrain((byte) cmdStr2Num(args[2], 10), 6, 32);
      if (DiagCAN.setACmax(&OBL, false)) {
        if (DiagCAN.getChargerCtrlValues(&OBL, false)) {
          if (OBL.Amps_setpoint == OBL.newAmps_setpoint) {
            Serial.print(F("OK: AC_max= ")); Serial.print(OBL.Amps_setpoint);
            Serial.println(F(" A"));
            cmdOK = 1;
          } else {
            cmdOK = 0;
          }
        } else {
          cmdOK = 0;
        }
      } else{
        cmdOK = 0;
      }
    }
  }
  if (cmdOK == 0) Serial.println(F("CMD: failed"));
  if (cmdOK < 0) Serial.println(F("CMD: wrong format"));

}

void init_cmd_prompt() {
  set_cmd_display("");            //reset command prompt to "CMD >>" 
}

//--------------------------------------------------------------------------------
//! \brief   Callback to activate main menu
//! \brief   reset command promt
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void main_menu (uint8_t arg_cnt, char **args) {
  (void) arg_cnt, (void) args;  // Avoid unused param warning
  myDevice.menu = MAIN;
  init_cmd_prompt();
}

//--------------------------------------------------------------------------------
//! \brief   Callback to switch to the BMS sub-menu and / or evaluate commands
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void bms_sub (uint8_t arg_cnt, char **args) {
  myDevice.menu = subBMS;
  set_cmd_display("BMS >>");
  if (arg_cnt == 2) {
    if (strcmp(args[1], "all") == 0) {
      get_all(arg_cnt, args);
    }
    if (strcmp(args[1], "t") == 0) {
      get_temperatures(arg_cnt, args);
    }
    if (strcmp(args[1], "v") == 0) {
      get_voltages(arg_cnt, args);
    }
    if (strcmp(args[1], "ocv") == 0) {
      get_OCVtable(arg_cnt, args);
    }
    if (strcmp(args[1], "res") == 0) {
      get_RESfactors(arg_cnt, args);
    }
    if (strcmp(args[1], "log") == 0) {
      get_BMSlog(arg_cnt, args);
    }
    if (strcmp(args[1], "soh") == 0) {
      get_SOHstate(arg_cnt, args);
    }  
  } else {

  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to switch to the OBL (Onboard Loader / std. charger)
//! \brief   sub-menu and / or evaluate commands
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void obl_sub (uint8_t arg_cnt, char **args) {
  myDevice.menu = subOBL;
  set_cmd_display("OBC >>");
  if (arg_cnt == 2) {
    if (strcmp(args[1], "all") == 0) {
      get_all(arg_cnt, args);
    }
    if (strcmp(args[1], "t") == 0) {
      get_temperatures(arg_cnt, args);
    }
    if (strcmp(args[1], "v") == 0) {
      get_voltages(arg_cnt, args);
    }
    if (strcmp(args[1], "log") == 0) {
      get_CHGlog(arg_cnt, args);
    }
  } else {

  }
}

//--------------------------------------------------------------------------------
//! \brief   Callback to switch to the Cooling sub-menu and / or evaluate commands
//! \param   Argument count (int) and argument-list (char*) from Cmd.h
//--------------------------------------------------------------------------------
void tcu_sub (uint8_t arg_cnt, char **args) {
  myDevice.menu = subTCU;
  set_cmd_display("TCU >>");
  if (arg_cnt == 2) {
    if (strcmp(args[1], "all") == 0) get_all(arg_cnt, args);
  } else {

  }
}

//--------------------------------------------------------------------------------
//! \brief   Logging data. Call queryfunctions and output the data
//--------------------------------------------------------------------------------
void logdata(){
  byte bms_sel[] = {BMSstate, BMSsoc, BMSlimit, BMSbal};
  getState_BMS(bms_sel, sizeof(bms_sel));
  getOBLdata();

  if (myDevice.logCount == 0) {
    //Print Header
    Serial.println();
    Serial.print(F("dt/s: "));Serial.println(myDevice.timer);
    Serial.print(F("#;SOC;rSOC,av;min;max;kWh;A;kW;V;Vc,min;max;Bal;"));
    if (!FASTCHG) {
      Serial.println(F("L1/V;L1/A;HV/V;HV/A;R1/kW;R2/kW;Ti/C;Tint/C;To/C;Tc/C"));
    } else {
      Serial.println(F("L1/V;L1/A;L2/V;L2/A;L3/V;L3/A;Pi/kW;HV/V;HV/A;Ts/%;Th/C;Tc/C"));
    }
  }
  myDevice.logCount++;
  //Print logged values
  Serial.print(myDevice.logCount); Serial.print(CSV);
  Serial.print(BMS.SOC,1); Serial.print(CSV);
  Serial.print((float) BMS.realSOC.mean / 16.0, 1); Serial.print(CSV);
  Serial.print((float) BMS.realSOC.min / 16.0, 1); Serial.print(CSV);
  Serial.print((float) BMS.realSOC.max / 16.0, 1); Serial.print(CSV);
  Serial.print(BMS.Energy / 200.0, 2); Serial.print(CSV);
  Serial.print((float) BMS.Amps2, 2); Serial.print(CSV);
  if (BMS.Power != 0) {
    Serial.print((float) BMS.Power, 2); Serial.print(CSV);
  } else {
    Serial.print(F("0.00")); Serial.print(CSV);
  }
  Serial.print(BMS.HV,1); Serial.print(CSV);
  Serial.print(BMS.CV_Range.min); Serial.print(CSV);
  Serial.print(BMS.CV_Range.max); Serial.print(CSV);
  Serial.print(BMS.BattBalState, HEX); Serial.print(CSV);
  if (!FASTCHG) {
    Serial.print(OBL.MainsVoltage[0] / 10.0, 1); Serial.print(CSV);
    Serial.print((OBL.MainsAmps[0] + OBL.MainsAmps[1]) / 10.0, 1); Serial.print(CSV);
    Serial.print(OBL.DC_HV / 10.0, 1); Serial.print(CSV);
    Serial.print(OBL.DC_Current / 10.0, 1); Serial.print(CSV);
    Serial.print(OBL.CHGpower[0] / 2000.0, 3); Serial.print(CSV);
    Serial.print(OBL.CHGpower[1] / 2000.0, 3); Serial.print(CSV);
    Serial.print(OBL.InTemp - TEMP_OFFSET, DEC); Serial.print(CSV);
    Serial.print(OBL.InternalTemp - TEMP_OFFSET, DEC); Serial.print(CSV);
    Serial.print(OBL.OutTemp - TEMP_OFFSET, DEC); Serial.print(CSV);
    Serial.print(OBL.CoolantTemp - TEMP_OFFSET + 10, DEC);
  } else {
    Serial.print(OBL.MainsVoltage[0] / 2.0, 1); Serial.print(CSV);
    Serial.print(OBL.MainsAmps[0] / 10.0, 1); Serial.print(CSV);
    Serial.print(OBL.MainsVoltage[1] / 2.0, 1); Serial.print(CSV);
    Serial.print(OBL.MainsAmps[1] / 10.0, 1); Serial.print(CSV);
    Serial.print(OBL.MainsVoltage[2] / 2.0, 1); Serial.print(CSV);
    Serial.print(OBL.MainsAmps[2] / 10.0, 1); Serial.print(CSV);
    Serial.print(OBL.CHGpower[0] / 1000.0, 3); Serial.print(CSV);
    Serial.print(OBL.DC_HV / 10.0, 1); Serial.print(CSV);
    Serial.print(OBL.DC_Current / 10.0, 2); Serial.print(CSV);
    Serial.print(OBL.SysTemp, DEC); Serial.print(CSV);
    Serial.print(OBL.InternalTemp - TEMP_OFFSET + 10, DEC); Serial.print(CSV);
    Serial.print(OBL.CoolantTemp - TEMP_OFFSET + 10, DEC);
  }
  Serial.println();
}

