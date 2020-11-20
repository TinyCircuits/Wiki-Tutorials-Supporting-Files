#if BLE_DEBUG
#include <stdio.h>
extern char sprintbuff[100];
#define PRINTF(...) {sprintf(sprintbuff,__VA_ARGS__);/*SerialMonitorInterface.print(millis());SerialMonitorInterface.print(' ');*/SerialMonitorInterface.print(sprintbuff);}
#else
#define PRINTF(...)
#endif

BLEConn * primaryConn = NULL;
uint32_t lastProcedureCompleted = 0;

void BLEProcess() {
  while (!HCI_Queue_Empty())HCI_Process();
}

uint8_t BLEsetup( BLEConn* conn, char * localName, void (*connect)(), void (*disconnect)()) {
  primaryConn = conn;
  primaryConn->handle = NULL;
  primaryConn->onConnect = connect;
  primaryConn->onBond = NULL;
  primaryConn->onDisconnect = disconnect;
  primaryConn->isAdvertising = false;
  primaryConn->isConnected = false;
  primaryConn->isBonded = false;
  primaryConn->firstChar = NULL;

  uint8_t ret;
  HCI_Init();
  BNRG_SPI_Init();
  BlueNRG_RST();
  ret = aci_gatt_init();
  if (ret) {
    PRINTF("GATT_Init failed.\n");
  }

  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, strlen(localName), &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (ret) {
    PRINTF("GAP_Init failed.\n");
  }

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, strlen(localName), (uint8_t *)localName);

  if (ret) {
    PRINTF("aci_gatt_update_char_value failed.\n");
  } else {
    PRINTF("BLE Stack Initialized.\n");
  }

  /* +4 dBm output power */
  ret = aci_hal_set_tx_power_level(1, 7);
}


uint8_t clearSecurityDatabase() {
  int ret = aci_gap_clear_security_database();
  if (ret) {
    PRINTF("aci_gap_clear_security_database failed.\n");
  }
  return ret;
}

uint8_t useSecurity(void (*bond)()) {
  uint8_t ret = aci_gap_set_io_capability(IO_CAP_NO_INPUT_NO_OUTPUT);
  if (ret) {
    PRINTF("aci_gap_set_io_capability failed.\n");
  }
  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_NOT_REQUIRED,
                                     OOB_AUTH_DATA_ABSENT,
                                     NULL,
                                     7,  /* Min. encryption key size */
                                     16, /* Max encryption key size */
                                     DONOT_USE_FIXED_PIN_FOR_PAIRING,/* no fixed pin */
                                     0, /* fixed pin not used */
                                     BONDING );
  if (ret) {
    PRINTF("aci_gap_set_auth_requirement failed.\n");
  }
  primaryConn->onBond = bond;
  return ret;
}

uint8_t requestSecurity() {
  tBleStatus ret = aci_gap_slave_security_request(primaryConn->handle, BONDING, MITM_PROTECTION_NOT_REQUIRED);
  if (ret != BLE_STATUS_SUCCESS)
    PRINTF("Slave security request error: %d\n", (uint8_t)ret);
  return ret;
}

uint32_t unpackInt32(uint8_t * src) {
  uint32_t val = src[3]; val <<= 8;
  val = src[2]; val <<= 8;
  val = src[1]; val <<= 8;
  return val | src[0];
}

void packInt32(uint8_t * d, uint32_t val) {
  d[0] = val;
  d[1] = val >> 8;
  d[2] = val >> 16;
  d[3] = val >> 24;
}

uint16_t unpackInt16(uint8_t * src) {
  uint16_t val = src[1]; val <<= 8;
  return val | src[0];
}
void packInt16(uint8_t * d, uint16_t val) {
  d[0] = val;
  d[1] = val >> 8;
}

uint8_t hexToNib(char h) {
  if (h >= '0' && h <= '9') return h - '0';
  else if (h >= 'a' && h <= 'f') return h - 'a' + 10;
  else if (h >= 'A' && h <= 'F') return h - 'A' + 10;
}

void UUIDstrToByte128(char * in, uint8_t * out) {
  uint8_t nibCount = 0;
  uint8_t temp = 0;
  if ( *in == '0' && *(in + 1) == 'x')in += 2;
  while (*in && nibCount < 32) {
    if (isalnum(*in)) {
      if ((nibCount & 1) == 0) {
        temp = (hexToNib(*in) << 4) & 0xF0;
      } else {
        temp |= hexToNib(*in);
        out[15 - (nibCount / 2)] = temp;
      }
      nibCount++;
    }
    in++;
  }
  PRINTF("UUID: ");
  for (int i = 0; i < 16; i++) {
    PRINTF("%02X", out[i]);
  }
  PRINTF("\n");
}

void UUIDstrToByte16(char * in, uint8_t * out) {
  uint8_t nibCount = 0;
  uint8_t temp = 0;
  if ( *in == '0' && *(in + 1) == 'x')in += 2;
  while (*in && nibCount < 4) {
    if (isalnum(*in)) {
      if ((nibCount & 1) == 0) {
        temp = (hexToNib(*in) << 4) & 0xF0;
      } else {
        temp |= hexToNib(*in);
        out[1 - (nibCount / 2)] = temp;
      }
      nibCount++;
    }
    in++;
  }
  PRINTF("UUID: ");
  for (int i = 0; i < 2; i++) {
    PRINTF("%02X", out[i]);
  }
  PRINTF("\n");
}




BLEServ * Att_Read_CB_service = NULL;

uint8_t discoverService(BLEServ * service, char * servUUID) {
  if(!primaryConn->handle){
    PRINTF("No connection handle\n");
    return -1;
  }
  PRINTF("Starting service discovery..\n");
  service->handle = 0;
  Att_Read_CB_service = service;
  tBleStatus ret;
  unsigned long timer = millis();
  unsigned long previousComplete = lastProcedureCompleted;
  if (strlen(servUUID) < 8) {
    UUIDstrToByte16(servUUID, service->UUID);
    service->UUIDType = UUID_TYPE_16;
    ret = aci_gatt_disc_prim_service_by_uuid( primaryConn->handle, service->UUIDType, service->UUID);
  } else {
    UUIDstrToByte128(servUUID, service->UUID);
    service->UUIDType = UUID_TYPE_128;
    ret = aci_gatt_disc_prim_service_by_uuid( primaryConn->handle, service->UUIDType, service->UUID);
  }
  if (ret) {
    PRINTF("Error while starting discovery of service: %d\n", ret);
  }
  while ((previousComplete == lastProcedureCompleted) && (millis() - timer < 500))HCI_Process();//(!service->handle) could be faster to wait for this condition, then terminate
  Att_Read_CB_service = NULL;
  if (service->handle) {
    PRINTF("Discovered service\n");
    return BLE_STATUS_SUCCESS;
  } else {
    PRINTF("Discover service timeout\n");
    return -1;
  }
}

void Att_Read_CB(uint16_t handle, uint8_t event_data_length, uint8_t attribute_data_length, uint8_t * attribute_data_list) {
  PRINTF("Att_Read_CB ");
  PRINTF("%04X, %d, %d\n", handle, event_data_length, attribute_data_length);
  int i;
  for (i = 0; i < 4; i++) {
    PRINTF("%02X, ", attribute_data_list[i]);
  }
  for (i = attribute_data_length - 1; i > 3; i--) {
    PRINTF("%02X", attribute_data_list[i]);
  }

  if (Att_Read_CB_service) {
    // if (!memcmp(&attribute_data_list[attribute_data_length - 2], Att_Read_CB_service->UUID16, 2) ||
    //     !memcmp(&attribute_data_list[attribute_data_length - 16], Att_Read_CB_service->UUID128, 16) ) {
    PRINTF("Service Found!");
    Att_Read_CB_service->handle = unpackInt16(&attribute_data_list[0]);
    Att_Read_CB_service->handleRangeEnd = unpackInt16(&attribute_data_list[2]);
    // } else {
    //   PRINTF(" No Match");
    // }
  }
  PRINTF("\n");
}

BLEChar * Att_Read_CB_characteristic = NULL;

uint8_t discoverCharacteristic(BLEServ * service, BLEChar * characteristic, char * charUUID) {
  if(!primaryConn->handle){
    PRINTF("No connection handle\n");
    return -1;
  }
  PRINTF("Starting characteristic discovery..\n");
  characteristic->serviceHandle = service->handle;
  characteristic->handle = NULL;
  characteristic->nextCharacteristic = NULL;
  characteristic->onUpdate = NULL;
  Att_Read_CB_characteristic = characteristic;
  tBleStatus ret;
  unsigned long timer = millis();
  unsigned long previousComplete = lastProcedureCompleted;
  if (strlen(charUUID) < 8) {
    UUIDstrToByte16(charUUID, characteristic->UUID);
    characteristic->UUIDType = UUID_TYPE_16;
    ret = aci_gatt_disc_charac_by_uuid( primaryConn->handle, service->handle, service->handleRangeEnd, characteristic->UUIDType, characteristic->UUID);
  } else {
    UUIDstrToByte128(charUUID, characteristic->UUID);
    characteristic->UUIDType = UUID_TYPE_128;
    ret = aci_gatt_disc_charac_by_uuid( primaryConn->handle, service->handle, service->handleRangeEnd, characteristic->UUIDType, characteristic->UUID);
  }
  if (ret) {
    PRINTF("Error while starting discovery of service: %d\n", ret);
  }
  while (previousComplete == lastProcedureCompleted && millis() - timer < 500)HCI_Process();//(!service->handle) could be faster to wait for this condition, then terminate
  Att_Read_CB_characteristic = NULL;
  if (characteristic->handle) {
    PRINTF("Discovered characteristic\n");
    return BLE_STATUS_SUCCESS;
  } else {
    PRINTF("Discover characteristic timeout\n");
    return -1;
  }
}

void Gatt_Disc_By_UUID_CB(uint16_t handle, uint8_t data_length, uint16_t attr_handle, uint8_t * data) {
  PRINTF("Gatt_Disc_By_UUID_CB ");
  PRINTF("%04X, %04X, %d\n", handle, attr_handle, data_length);
  int i;
  for (i = 0; i < data_length - 2; i++) {
    PRINTF("%02X, ", data[i]);
  }

  if (Att_Read_CB_characteristic) {
    uint8_t UUIDBytes = 2;
    if (Att_Read_CB_characteristic->UUIDType == UUID_TYPE_128)
      UUIDBytes = 16;
    if (!memcmp(&data[data_length - UUIDBytes - 2], Att_Read_CB_characteristic->UUID, UUIDBytes)) {
      Att_Read_CB_characteristic->properties = data[0];
      Att_Read_CB_characteristic->handle = attr_handle;
      Att_Read_CB_characteristic->valueHandle = unpackInt16(&data[1]);
      PRINTF("Characteristic Found! %02X, %04X, %04X", Att_Read_CB_characteristic->properties, Att_Read_CB_characteristic->handle, Att_Read_CB_characteristic->valueHandle);
    } else {
      PRINTF(" No Match");
    }
  }
  PRINTF("\n");
}

uint8_t writeCharacteristic(BLEChar * characteristic, uint8_t * val, uint8_t length) {
  if (!primaryConn->handle) {
    PRINTF("No connection handle\n");
    return -1;
  }
  unsigned long timer = millis();
  unsigned long previousComplete = lastProcedureCompleted;
  tBleStatus ret = aci_gatt_write_charac_value(primaryConn->handle, characteristic->valueHandle, length, val);
  if (ret) {
    PRINTF("Error while writing characteristic: %d\n", ret);
    return ret;
  }
  while (previousComplete == lastProcedureCompleted && millis() - timer < 500)HCI_Process();//(!service->handle) could be faster to wait for this condition, then terminate
  return BLE_STATUS_SUCCESS;
}

uint8_t enableNotifications(BLEChar * characteristic, void (* updateHandler)(uint8_t *, uint8_t)) {
  PRINTF("Enabling notifications for handle %04X\n", characteristic->handle);
  if (!primaryConn->handle) {
    PRINTF("No connection handle\n");
    return -1;
  }
  unsigned long timer = millis();
  unsigned long previousComplete = lastProcedureCompleted;
  uint8_t data[2] = {0x01, 0x00};
  tBleStatus ret = aci_gatt_write_charac_descriptor(primaryConn->handle, characteristic->handle + 2, 2, data);
  if (ret) {
    PRINTF("Error while enabling notifications: %d", ret);
    return ret;
  }
  while (previousComplete == lastProcedureCompleted && millis() - timer < 500)HCI_Process();//(!service->handle) could be faster to wait for this condition, then terminate
  //add to updateable char list
  characteristic->onUpdate = updateHandler;
  if (!primaryConn->firstChar) {
    primaryConn->firstChar = (void *)characteristic;
    PRINTF("First char update handler in list\n");
  } else if (primaryConn->firstChar == (void *)characteristic) {
    PRINTF("Char update handler already in list\n");
  } else {
    BLEChar * nextChar = (BLEChar *)primaryConn->firstChar;
    while (nextChar->nextCharacteristic) {
      if (nextChar->nextCharacteristic == (void *)characteristic) {
        PRINTF("Char update handler already in list\n");
        break;
      } else {
        nextChar = (BLEChar *)nextChar->nextCharacteristic;
      }
    }
    if (!nextChar->nextCharacteristic) {
      nextChar->nextCharacteristic = (void *)characteristic;
      PRINTF("Char update handler added to end of list\n");
    }
  }
  return BLE_STATUS_SUCCESS;
}

void advertise(char * advName, char * advUUID) {
  const uint16_t advMinMS = 100;
  const uint16_t advMaxMS = 200;
  const uint16_t connMinMS = 50;
  const uint16_t connMaxMS = 100;

  uint8_t UUID[18];
  if (strlen(advUUID) < 8) {
    UUID[0] = 3;
    UUID[1] = AD_TYPE_SERV_SOLICIT_16_BIT_UUID_LIST;
    UUIDstrToByte16(advUUID, UUID + 2);
  } else {
    UUID[0] = 17;
    UUID[1] = AD_TYPE_SERV_SOLICIT_128_BIT_UUID_LIST;
    UUIDstrToByte128(advUUID, UUID + 2);
  }

  tBleStatus ret = hci_le_set_scan_resp_data(UUID[0] + 1, UUID);

  if (ret != BLE_STATUS_SUCCESS)
    PRINTF("Set scan resp error: %d\n", (uint8_t)ret);

  char local_name[16] = {AD_TYPE_COMPLETE_LOCAL_NAME};
  strcpy(local_name + 1, advName);
  uint8_t nameLen = strlen(local_name + 1) + 1;

  ret = aci_gap_set_discoverable(ADV_IND,
                                 ((uint32_t)advMinMS * 1000ul) / 625ul, ((uint32_t)advMaxMS * 1000ul) / 625ul,
                                 STATIC_RANDOM_ADDR, NO_WHITE_LIST_USE,
                                 nameLen, local_name,
                                 0, NULL, //UUID List
                                 ((uint32_t)connMinMS * 1000ul) / 625ul, ((uint32_t)connMaxMS * 1000ul) / 625ul);

  if (ret == BLE_STATUS_SUCCESS) {
    PRINTF("General Discoverable Mode.\n");
  } else {
    PRINTF("gen disc err %d\n", (uint8_t)ret);
  }
  primaryConn->isAdvertising = true;
}

void Read_Request_CB(uint16_t handle)
{
  //handle==char.valueHandle
  PRINTF("Read Request CB\n");
  if (primaryConn->handle != 0)
    aci_gatt_allow_read(primaryConn->handle);
}

BLEChar * Att_Read_Resp_CB_characteristic = NULL;
uint8_t * Att_Read_Resp_CB_characteristic_val = NULL;
uint8_t Att_Read_Resp_CB_characteristic_len = NULL;

uint8_t readCharacteristic(BLEChar * characteristic, uint8_t * val, uint8_t length) {
  PRINTF("Starting read characteristic..\n");
  if (!primaryConn->handle) {
    PRINTF("No connection handle\n");
    return -1;
  }
  Att_Read_Resp_CB_characteristic = characteristic;
  Att_Read_Resp_CB_characteristic_val = val;
  Att_Read_Resp_CB_characteristic_len = length;
  unsigned long timer = millis();
  unsigned long previousComplete = lastProcedureCompleted;
  tBleStatus ret = aci_gatt_read_charac_val(primaryConn->handle, characteristic->valueHandle);
  if (ret) {
    PRINTF("Error while starting read characteristic: %d\n", ret);
  }
  while ((previousComplete == lastProcedureCompleted) && (millis() - timer < 500))HCI_Process();//(!service->handle) could be faster to wait for this condition, then terminate
  Att_Read_Resp_CB_characteristic = NULL;
  if (Att_Read_Resp_CB_characteristic_val != val) {
    PRINTF("Read characteristic complete\n");
    return BLE_STATUS_SUCCESS;
  } else {
    PRINTF("Read characteristic timeout\n");
    return -1;
  }
}


void Att_Read_Resp_CB(uint16_t handle, uint8_t data_length, uint8_t * attr_value) {
  PRINTF("Att_Read_Resp_CB %04X, %d\n", handle, data_length);
  for (int i = 0; i < data_length; i++) {
    PRINTF("%02X, ", attr_value[i]);
  } PRINTF("\n");
  for (int i = 0; i < data_length; i++) {
    PRINTF("%c", attr_value[i]);
  } PRINTF("\n");

  if (Att_Read_Resp_CB_characteristic) {
    if (Att_Read_Resp_CB_characteristic_len > data_length) {
      Att_Read_Resp_CB_characteristic_len = data_length;
    }
    for (int i = 0; i < data_length && i < Att_Read_Resp_CB_characteristic_len; i++) {
      *Att_Read_Resp_CB_characteristic_val = attr_value[i];
      Att_Read_Resp_CB_characteristic_val++;
    }
  }
}

void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle) {
  PRINTF("Connected to device:");
  for (int i = 5; i > 0; i--) {
    PRINTF("%02X-", addr[i]);
  }
  PRINTF("%02X\r\n", addr[0]);

  primaryConn->handle = handle;
  primaryConn->isAdvertising = false;
  for (int i = 0; i < 5; i++) {
    primaryConn->connectedAddr[i] = addr[5 - i];
  }
  if (primaryConn->onConnect) {
    primaryConn->onConnect();
  }
}

void GAP_DisconnectionComplete_CB(void) {
  PRINTF("Disconnected\n");

  primaryConn->handle = NULL;
  primaryConn->isAdvertising = false;
  primaryConn->isConnected = false;
  primaryConn->isBonded = false;
  if (primaryConn->onDisconnect) {
    primaryConn->onDisconnect();
  }
}


void Att_Read_By_Type_CB(uint16_t handle, uint8_t data_length, uint8_t * data) {
  PRINTF("Att_Read_By_Type_CB ");
  PRINTF("%04X, %d\n", handle, data_length);
  int i;
  for (i = 0; i < 2; i++) {
    PRINTF("%02X, ", data[i]);
  }

  for (i = data_length - 1; i > 1; i--) {
    PRINTF("%02X", data[i]);
  }

  PRINTF("\n");
}

void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  //handle==char.valueHandle
  PRINTF("attr modified %04X\n", handle);
  int i;
  for (i = 0; i < data_length; i++) {
    // ble_rx_buffer[i] = att_data[i];
    PRINTF("%02X-", att_data[i]);
  }
  PRINTF("\n");
}

void Attribute_Notification_CB(uint16_t handle, uint8_t data_length, uint16_t attr_handle, uint8_t * attr_value) {
  PRINTF("Attribute_Notification_CB %04X, %04X, %d\n", handle, attr_handle, data_length);
  for (int i = 0; i < data_length - 2; i++) {
    PRINTF("%02X, ", attr_value[i]);
  } PRINTF("\n");
  for (int i = 0; i < data_length - 2; i++) {
    PRINTF("%c", attr_value[i]);
  } PRINTF("\n");

  BLEChar * testChar = (BLEChar *)primaryConn->firstChar;
  while (testChar) {
    if (attr_handle == testChar->valueHandle) {
      if (testChar->onUpdate) {
        testChar->onUpdate(attr_value, data_length - 2);
        return;
      }
    }
    testChar = (BLEChar *)testChar->nextCharacteristic;
  }
  PRINTF("Unhandled update!\n");

  //atrributeUpdateHandler(attr_handle, attr_value, data_length - 2);
}


/*
  //todo
  uint8_t lib_aci_send_data(uint8_t ignore, uint8_t* sendBuffer, uint8_t sendLength) {
  return !Write_UART_TX((char*)sendBuffer, sendLength);
  }

  uint8_t Write_UART_TX(char* TXdata, uint8_t datasize)
  {
  tBleStatus ret;

  // ret = aci_gatt_update_char_value(UARTServHandle, UARTRXCharHandle, 0, datasize, (uint8_t *)TXdata);

  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while updating UART characteristic.\n") ;
    return BLE_STATUS_ERROR ;
  }
  return BLE_STATUS_SUCCESS;

  }
*/

void HCI_Event_CB(void *pckt)
{
  hci_uart_pckt *hci_pckt = (hci_uart_pckt *)pckt;
  hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;
  if (hci_pckt->type != HCI_EVENT_PKT)
    return;
  PRINTF("%d PCKT START: ", millis());
  switch (event_pckt->evt) {
    case EVT_DISCONN_COMPLETE:
      {
        //evt_disconn_complete *evt = (void *)event_pckt->data;
        GAP_DisconnectionComplete_CB();
      }
      break;
    case EVT_LE_META_EVENT:
      {
        evt_le_meta_event *evt = (evt_le_meta_event *)event_pckt->data;
        switch (evt->subevent) {
          case EVT_LE_CONN_COMPLETE:
            {
              evt_le_connection_complete *cc = (evt_le_connection_complete *)evt->data;
              GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
            }
            break;
          default:
            {
              PRINTF("EVT_LE_META_EVENT: %04X\n", evt->subevent);
            }
        }
      }
      break;
    case EVT_VENDOR:
      {
        evt_blue_aci *blue_evt = (evt_blue_aci *)event_pckt->data;
        switch (blue_evt->ecode) {
          case EVT_BLUE_ATT_READ_BY_GROUP_TYPE_RESP:
            {
              evt_att_read_by_group_resp *gr = (evt_att_read_by_group_resp *)blue_evt->data;
              Att_Read_CB(gr->conn_handle, gr->event_data_length, gr->attribute_data_length, gr->attribute_data_list);
            }
            break;
          case EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP:
            {
              evt_att_find_by_type_val_resp *tv = (evt_att_find_by_type_val_resp *)blue_evt->data;
              Att_Read_CB(tv->conn_handle, tv->event_data_length, tv->event_data_length, tv->handles_info_list);
            }
            break;
          case EVT_BLUE_ATT_READ_RESP:
            {
              evt_att_read_resp *rr = (evt_att_read_resp *)blue_evt->data;
              Att_Read_Resp_CB(rr->conn_handle, rr->event_data_length, rr->attribute_value);
            }
            break;
          case EVT_BLUE_ATT_READ_BY_TYPE_RESP:
            {
              evt_att_read_by_type_resp *tr = (evt_att_read_by_type_resp *)blue_evt->data;
              Att_Read_By_Type_CB(tr->conn_handle, tr->handle_value_pair_length, tr->handle_value_pair);
            }
            break;
          case EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP:
            {
              evt_gatt_disc_read_char_by_uuid_resp *ur = (evt_gatt_disc_read_char_by_uuid_resp *)blue_evt->data;
              Gatt_Disc_By_UUID_CB(ur->conn_handle, ur->event_data_length, ur->attr_handle, ur->attr_value);
            }
            break;
          case EVT_BLUE_GATT_READ_PERMIT_REQ:
            {
              evt_gatt_read_permit_req *pr = (evt_gatt_read_permit_req *)blue_evt->data;
              Read_Request_CB(pr->attr_handle);
            }
            break;
          case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
            {
              evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*)blue_evt->data;
              Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data);
            }
            break;
          case EVT_BLUE_GATT_NOTIFICATION:
            {
              evt_gatt_attr_notification *n = (evt_gatt_attr_notification*)blue_evt->data;
              Attribute_Notification_CB(n->conn_handle, n->event_data_length, n->attr_handle, n->attr_value);
            }
            break;
          case EVT_BLUE_GATT_PROCEDURE_COMPLETE:
            {
              PRINTF("GATT Procedure complete\n");
              lastProcedureCompleted = millis();
            }
            break;
          case EVT_BLUE_GAP_PAIRING_CMPLT:
            {
              evt_gap_pairing_cmplt *pc = (evt_gap_pairing_cmplt *)blue_evt->data;
              PRINTF("Pairing complete: %02X\n", pc->status);
              if (pc->status == 0) {
                primaryConn->isBonded = true;
                if (primaryConn->onBond) {
                  primaryConn->onBond();
                }
              }
            }
            break;
          case EVT_BLUE_GATT_ERROR_RESP:
            {
              evt_gatt_error_resp *er = (evt_gatt_error_resp *)blue_evt->data;
              PRINTF("err: %02X, %04X, %02X\n", er->req_opcode, er->attr_handle, er->error_code);
            }
            break;
          case EVT_BLUE_GAP_BOND_LOST:
            {
              PRINTF("bond lost, allowing rebond\n")
              aci_gap_allow_rebond_IDB05A1(primaryConn->handle);
            }
            break;
          default:
            {
              PRINTF("EVT_BLUE: %04X\n", blue_evt->ecode);
            }
        }
      }
      break;
    default:
      {
        PRINTF("gen evt: %04X\n", event_pckt->evt);
      }
  }
}

