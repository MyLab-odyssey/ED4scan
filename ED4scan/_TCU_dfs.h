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
//! \file    _TCU_dfs.h
//! \brief   Definitions and structures for the Tele-Communication Unit.
//! \date    2018-April
//! \author  MyLab-odyssey
//! \version 0.4.3
//--------------------------------------------------------------------------------
#ifndef TCU_DFS_H
#define TCU_DFS_H

const char* const PROGMEM TCU_MODE[] ={"OFF", "ON", "?", "CN"};

struct TCUdatetime {
  byte year = 0;
  byte month = 0;
  byte day = 0;
  byte hour = 0;
  byte minute = 0;
  byte second = 0;
};

//TCU data structure
typedef struct {       
  byte Rssi;                 //!< Received Signal Strength Indication
  byte State;                //!< Status of TCU?
  byte NetType;              //!< Network type 2G, 3G, changing?!
  uint16_t Counter;          //!< Counter sum up APP heater activity -> SMS?
  TCUdatetime TCUtime;       //!< variabes to store time and date by TCU
} TCUdiag_t; 

const PROGMEM uint32_t rqID_TCU                   = 0x7CA;
const PROGMEM uint32_t respID_TCU                 = 0x7DA;

const PROGMEM byte rqTCUrssi[3]                   = {0x22, 0xFD, 0x1D};
const PROGMEM byte rqTCUstate[3]                  = {0x22, 0xFE, 0x08};
const PROGMEM byte rqTCUnetType[3]                = {0x22, 0x6D, 0x48};
const PROGMEM byte rqTCUcounter[3]                = {0x22, 0x6D, 0x59}; //SMS counter?
const PROGMEM byte rqTCUnetName[3]                = {0x22, 0xFD, 0x70}; 
const PROGMEM byte rqTCUtime[3]                   = {0x22, 0x6D, 0x46};

#endif // of #ifndef TCU_DFS_H
