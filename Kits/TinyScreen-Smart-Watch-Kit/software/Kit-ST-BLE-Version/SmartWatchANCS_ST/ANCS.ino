#if BLE_DEBUG
#include <stdio.h>
char sprintbuff[100];
#define PRINTF(...) {sprintf(sprintbuff,__VA_ARGS__);/*SerialMonitorInterface.print(millis());SerialMonitorInterface.print(' ');*/SerialMonitorInterface.print(sprintbuff);}
#else
#define PRINTF(...)
#endif


#include "ANCSDefs.h"

uint8_t newMessageBuff[20];
uint8_t newTitleBuff[20];
uint8_t negativeActionLabel[10];


uint8_t currentAttributeLength = 0;
uint8_t currentAttributeTotalLength = 0;
uint8_t * currentAttributeLocation = NULL;

uint32_t lastNotificationUUID;
uint8_t lastNotificationCategoryID;
uint32_t canDisplay = 0;
uint32_t needTitle = 0;
uint32_t needMessage = 0;
uint32_t needLabel = 0;
uint8_t newNotification = 0;

uint32_t removeNotification = 0;

uint32_t lastDSActivity = 0;;

uint32_t ANCSBusy = 0;
uint32_t ANCSStayAwake = 0;


void newNSdata(uint8_t * attr_value, uint8_t val_length) {
  PRINTF("NSCharUpdate: ");
  if (attr_value[0] == ANCS_EVT_NOTIFICATION_ADDED) {
    lastNotificationUUID = unpackInt32(&attr_value[4]);
    lastNotificationCategoryID = attr_value[2];
    PRINTF("%08X added\n", lastNotificationUUID);
    needTitle = millis();
    needMessage = millis();
    needLabel = millis();
    ANCSStayAwake = millis();
  }
  if (attr_value[0] == ANCS_EVT_NOTIFICATION_REMOVED) {
    uint32_t removeNotificationUUID = unpackInt32(&attr_value[4]);
    if (removeNotificationUUID == lastNotificationUUID); {
      PRINTF("lastNotification Removed\n");
      removeNotification = millis();
    }
  }
}


void newDSdata(uint8_t * attr_value, uint8_t val_length) {
  PRINTF("DScharUpdate: ");
  lastDSActivity = millis();
  if (!currentAttributeLocation) {
    currentAttributeTotalLength = unpackInt16(&attr_value[6]);
    currentAttributeLength = 0;
    if (attr_value[5] == 1) {
      currentAttributeLocation = newTitleBuff;
      memset(newTitleBuff, 0, sizeof(newTitleBuff));
    }
    if (attr_value[5] == 3) {
      currentAttributeLocation = newMessageBuff;
      memset(newMessageBuff, 0, sizeof(newMessageBuff));
    }
    if (attr_value[5] == 7) {
      currentAttributeLocation = negativeActionLabel;
      memset(negativeActionLabel, 0, sizeof(negativeActionLabel));
    }
    for (int i = 8; i < val_length && currentAttributeLength < currentAttributeTotalLength; i++) {
      currentAttributeLocation[currentAttributeLength] = attr_value[i];
      currentAttributeLength++;
    }
    PRINTF("attr RX: %d:%d\n", currentAttributeLength, currentAttributeTotalLength);
  } else {
    for (int i = 0; i < val_length && currentAttributeLength < currentAttributeTotalLength; i++) {
      currentAttributeLocation[currentAttributeLength] = attr_value[i];
      currentAttributeLength++;
    }
    PRINTF("attr RX: %d:%d\n", currentAttributeLength, currentAttributeTotalLength);
  }
  if (currentAttributeLength == currentAttributeTotalLength) {
    if (currentAttributeLocation == newTitleBuff) {
      needTitle = false;
      ANCSBusy = false;
    }
    if (currentAttributeLocation == newMessageBuff) {
      needMessage = false;
      ANCSBusy = false;
    }
    if (currentAttributeLocation == negativeActionLabel)needLabel = false;
    currentAttributeLocation = NULL;
  }
  PRINTF("\n");
}

//text to escape
// E2, 80, 98/99, -> '
// E2, 80, A6, -> ...
void cleanString(char * s){
  uint8_t skip=0;
  while(*s){
    if((*s)==0x0A)(*s)='|';
    if((*s)<' ')(*s)=' ';
    if((*s)==0xE2){
      if(*(s+1)==0x80 && (*(s+2)&0xFE)==0x98){
        (*s)='\'';strcpy((s+1),(s+3));
      }
      if(*(s+1)==0x80 && *(s+2)==0xA6){
        (*s)='.';*(s+1)='.';*(s+2)='.';
      }
    }
    s++;
  }
}

void ANCSProcess() {

  if (needTitle) {
    if (millis() - needTitle > 100 && millis() - lastDSActivity > 100) {
      canDisplay = false;
      ANCSBusy = true;
      ANCSStayAwake = true;
      getNotificationAttribute(ANCS_NOTIFICATION_ATTRIBUTE_TITLE);
      needTitle = millis();
    }
  } else if (needMessage) {
    if (millis() - needMessage > 100 && millis() - lastDSActivity > 100) {
      ANCSBusy = true;
      getNotificationAttribute(ANCS_NOTIFICATION_ATTRIBUTE_MESSAGE);
      needMessage = millis();
    }
  } else if (needLabel) {
    if (millis() - needLabel > 100 && millis() - lastDSActivity > 100) {
      ANCSBusy = true;
      getNotificationAttribute(ANCS_NOTIFICATION_ATTRIBUTE_NEGATIVE_ACTION_LABEL);
      needLabel = millis();
    }
  } else if (ANCSBusy) {
    ANCSStayAwake = false;
    ANCSBusy = false;
    canDisplay = true;
    newNotification = true;
    cleanString((char*)newMessageBuff);
    PRINTF("%c",ANCSNotificationTitle());
    PRINTF("%c",ANCSNotificationMessage());
    PRINTF("%c",ANCSNotificationNegativeAction());
  }

  if (removeNotification) {
    canDisplay = 0;
    removeNotification = 0;
  }
}

uint8_t ANCSNewNotification() {
  if (newNotification) {
    newNotification = false;
    return true;
  }
  return false;
}

void ANCSReset() {
  ANCSBusy = false;
  currentAttributeLocation = NULL;
  lastNotificationUUID = 0;
  needTitle = false;
  needMessage = false;
  needLabel = false;
}

uint8_t ANCSRequestStayAwake() {
  return ANCSStayAwake;
}

uint8_t ANCSIsBusy() {
  return ANCSBusy;
}

uint8_t ANCSNotificationCount() {
  if (canDisplay)return 1;
  return 0;
}

char * ANCSNotificationTitle() {
  return (char *)newTitleBuff;
}

char * ANCSNotificationMessage() {
  return (char *)newMessageBuff;
}

char * ANCSNotificationNegativeAction() {
  return (char *)negativeActionLabel;
}
void ANCSPerformNotificationNegativeAction() {
  performNotificationAction(1);
  canDisplay = 0;
}


uint8_t performNotificationAction(uint8_t action) {
  uint8_t data[6];
  uint8_t length = 6;

  data[0] = 0x02;
  packInt32(&data[1], lastNotificationUUID);
  data[5] = action;

  PRINTF("Performing action %d for %08X\n", action, lastNotificationUUID);
  tBleStatus ret =  writeCharacteristic(&CPchar, data, length);
  return ret;
}


uint8_t getNotificationAttribute(uint8_t typeofatt) {
  currentAttributeLocation = NULL;

  uint16_t maxlen = 18;
  uint8_t data[8];
  uint8_t length = 6;

  data[0] = 0x00;
  packInt32(&data[1], lastNotificationUUID);
  data[5] = typeofatt;
  packInt16(&data[6], maxlen);

  if (typeofatt >= 1 && typeofatt <= 3) {
    length = 8;
  }

  PRINTF("Requesting %d for %08X\n", typeofatt, lastNotificationUUID);
  tBleStatus ret = writeCharacteristic(&CPchar, data, length);
  return ret;
}

