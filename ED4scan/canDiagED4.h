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
//! \file    canDiagED4.h
//! \brief   Library module for retrieving diagnostic data. 
//! \date    2018-April
//! \author  MyLab-odyssey
//! \version 0.4.3
//--------------------------------------------------------------------------------
#ifndef CANDIAG_H
#define CANDIAG_H

//#define DO_DEBUG_UPDATE           //!< Uncomment to show DEBUG output

#ifndef DO_DEBUG_UPDATE
#define DEBUG_UPDATE(...)
#else
#define DEBUG_UPDATE(...) Serial.print(__VA_ARGS__)
#endif

#define CSV ';'

#ifndef PRINT_CSV
#define PRINT_CSV Serial.print(CSV); Serial.print(' ')
#endif

#define VERBOSE_ENABLE 1         //!< Local verbose mode enable to allow output of CAN messages
#define CAP_MODE 2               //!< Select used default capacity readout: 1 = dSOC in As/10, 2 = impedance meas. in As/100

#include <mcp_can.h>
#include <Timeout.h>
#include <AvgNew.h>
#include "_BMS_ED4_dfs.h"
#include "_OBL_dfs.h"
#include "_TCU_dfs.h"

class canDiag { 
 
private:
    MCP_CAN *myCAN0;
    CTimeout *myCAN_Timeout;

    byte *data;
    Average CellVoltage;
    Average CellCapacity;
    Average CellCap;

    int _getFreeRam();

    //CAN-Bus declarations
    unsigned long rxID;
    byte len = 0;
    byte rxLength = 0;
    byte rxBuf[8];
    byte rqMsg[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    byte rqFlowControl[8] = {0x30, 0x08, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00};
    byte rqFC_length = 8;   //!< Interval to send flow control messages (rqFC) 

    uint32_t rqID;
    uint32_t respID;
    uint16_t SkipStart;
    uint16_t SkipEnd;
    boolean SkipEnable = false;

    byte CapMode = CAP_MODE;
        
    uint16_t Request_Diagnostics(const byte* rqQuery);
    uint16_t Request_Diagnostics(byte* _rqQuery);
    uint16_t Get_RequestResponse();
    boolean Read_FC_Response(int16_t items);
    void PrintReadBuffer(uint16_t lines);

    void ReadBatteryTemperatures(BatteryDiag_t *myBMS, byte data_in[], uint16_t highOffset, uint16_t length);
    void ReadCellCapacity(byte data_in[], uint16_t highOffset, uint16_t length);
    void ReadCellVoltage(byte data_in[], uint16_t highOffset, uint16_t length);
    void ReadDiagWord(uint16_t data_out[], byte data_in[], uint16_t highOffset, uint16_t length);
  
public:  
    canDiag();
    ~canDiag(); 

    void reserveMem_CellVoltage();
    void reserveMem_CellCapacity();
    void freeMem_CellVoltage();
    void freeMem_CellCapacity();

    boolean WakeUp();
    
//--------------------------------------------------------------------------------
//! \brief   General functions MCP2515 controller
//--------------------------------------------------------------------------------
    void begin(MCP_CAN *myCAN0, CTimeout *myCAN_TimeoutObj);  
    void clearCAN_Filter();
    boolean ClearReadBuffer();
    void setCAN_Filter(unsigned long filter);
    void setCAN_ID(unsigned long _respID);
    void setCAN_ID(unsigned long _rqID, unsigned long _respID);

//--------------------------------------------------------------------------------
//! \brief   Get methods for BMS data
//--------------------------------------------------------------------------------
    boolean getKeyState(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getDCDC_State(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryTemperature(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryDate(BatteryDiag_t *myBMS, boolean debug_verbose);
    //boolean getBatteryVIN(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBalancingStatus(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getIsolationValue(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryCapacity(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryVoltage(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryVoltageDist(BatteryDiag_t *myBMS);
    boolean getBatteryState(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatterySOC(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryLimits(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatterySOH(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getODOcount(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getRange(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getHVcontactorCount(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean printOCVtable(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean printRESfactors(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean printBMSlog(BatteryDiag_t *myBMS, boolean debug_verbose);
    void    printBMSlogSet(byte _length);
    boolean printCHGlog(boolean debug_verbose);
    boolean getBatteryExperimentalData(BatteryDiag_t *myBMS, boolean debug_verbose);

    uint16_t getCellVoltage(byte n);
    uint16_t getCellCapacity(byte n);

//--------------------------------------------------------------------------------
//! \brief   Get methods for OBL data (charger)
//--------------------------------------------------------------------------------
    char    OBL_7KW_Installed(ChargerDiag_t *myOBL, boolean debug_verbose);
    boolean getChargerTemperature(ChargerDiag_t *myOBL, boolean debug_verbose);
    boolean getChargerCtrlValues(ChargerDiag_t *myOBL, boolean debug_verbose);
    boolean getChargerDC(ChargerDiag_t *myOBL, boolean debug_verbose);
    boolean getChargerAC(ChargerDiag_t *myOBL, boolean debug_verbose);
    boolean setACmax(ChargerDiag_t *myOBL, boolean debug_verbose);

//--------------------------------------------------------------------------------
//! \brief   Get methods for Tele-Communication Unit
//--------------------------------------------------------------------------------
    boolean getTCUdata(TCUdiag_t *myTCU, boolean debug_verbose);
    boolean getTCUnetwork(TCUdiag_t *myTCU, boolean debug_verbose);
    
//--------------------------------------------------------------------------------
//! \brief   Evaluate values and general functions
//--------------------------------------------------------------------------------
    void setCAPmode(byte _mode);
    boolean CalcPower(BatteryDiag_t *myBMS);
    boolean printECUrev(boolean debug_verbose, byte _type[]);
};

#endif // of #ifndef CANDIAG_H
