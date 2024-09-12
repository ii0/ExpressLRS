/************************************************************************************
Credits:
  This software is based on and uses software published by Richard Amiss 2023,
  QLiteOSD, which is based on work by Paul Kurucz (pkuruz):opentelem_to_bst_bridge 
  as well as software from d3ngit : djihdfpv_mavlink_to_msp_V2 and 
  crashsalot : VOT_to_DJIFPV

THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, WHETHER EXPRESS, 
IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE 
COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR 
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
************************************************************************************/

#include "SerialDisplayport.h"
#include "crsf_protocol.h"

void SerialDisplayport::send(uint8_t messageID, void * payload, uint8_t size, Stream * _stream)
{
  _stream->write('$');
  _stream->write('M');
  _stream->write('<');
  _stream->write(size);
  _stream->write(messageID);
  uint8_t checksum = size ^ messageID;
  uint8_t * payloadPtr = (uint8_t*)payload;
  for (uint8_t i = 0; i < size; ++i) {
    uint8_t b = *(payloadPtr++);
    checksum ^= b;
    _stream->write(b);
  }
  _stream->write(checksum);
}

uint32_t SerialDisplayport::sendRCFrame(bool frameAvailable, bool frameMissed, uint32_t *channelData)
{
    if (millis() > m_lastSentMSP + 100)
    {
        m_lastSentMSP = millis();
    }
    else
    {
        return DURATION_IMMEDIATELY;
    }

    // msp_fc_version_t fc_version = { 0 };
    // msp_fc_variant_t fc_variant = { 0 };
    // char fcVariant[5] = "BTFL";

    // memcpy(fc_variant.flightControlIdentifier, fcVariant, sizeof(fcVariant));
    // send(MSP_FC_VARIANT, &fc_variant, sizeof(fc_variant), _outputPort);

    // fc_version.versionMajor = 4;
    // fc_version.versionMinor = 1;
    // fc_version.versionPatchLevel = 1;
    // send(MSP_FC_VERSION, &fc_version, sizeof(fc_version), _outputPort);
    
    bool armed = channelData[4] > CRSF_CHANNEL_VALUE_MID;

    msp_status_DJI_t status_DJI;
    status_DJI.cycleTime = 0x0080;
    status_DJI.i2cErrorCounter = 0;
    status_DJI.sensor = 0x23;
    status_DJI.flightModeFlags = armed ? 0x3 : 0x2;
    status_DJI.configProfileIndex = 0;
    status_DJI.averageSystemLoadPercent = 7;
    status_DJI.accCalibrationAxisFlags = 0;
    status_DJI.DJI_ARMING_DISABLE_FLAGS_COUNT = 20;
    status_DJI.djiPackArmingDisabledFlags = (1 << 24);
    status_DJI.armingFlags = 0x0303;
    send(MSP_STATUS_EX, &status_DJI, sizeof(status_DJI), _outputPort);

    status_DJI.armingFlags = 0x0000;
    send(MSP_STATUS, &status_DJI, sizeof(status_DJI), _outputPort);

    return DURATION_IMMEDIATELY;
}