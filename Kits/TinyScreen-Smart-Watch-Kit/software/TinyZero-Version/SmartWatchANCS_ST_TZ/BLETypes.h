#ifndef BLETypes_h
#define BLETypes_h


typedef struct _BLEConn
{
  uint16_t handle;
  uint8_t isAdvertising;
  uint8_t isConnected;
  uint8_t isBonded;
  uint8_t connectedAddr[6];
  //uint8_t advertisingUUIDType;
  //uint8_t advertisingUUID[16];
  void (* onConnect)();
  void (* onBond)();
  void (* onDisconnect)();
  void * firstChar;
} BLEConn;

//UUID_TYPE_16
typedef struct _BLEServ
{
  uint8_t UUIDType;
  uint8_t UUID[16];
  uint16_t handle;
  uint16_t handleRangeEnd;
} BLEServ;

typedef struct _BLEChar
{
  uint8_t UUIDType;
  uint8_t UUID[16];
  uint16_t serviceHandle;
  uint16_t handle;
  uint16_t valueHandle;
  uint8_t properties;
  void (* onUpdate)(uint8_t *, uint8_t);
  void * nextCharacteristic;
} BLEChar;

uint8_t BLEsetup( BLEConn* conn, char * localName, void (*connect)(), void (*disconnect)());

uint8_t discoverService(BLEServ * service, char * servUUID);

uint8_t discoverCharacteristic(BLEServ * service, BLEChar * characteristic, char * charUUID);

uint8_t enableNotifications(BLEChar * characteristic, void (* onUpdate)(uint8_t *, uint8_t));

uint8_t writeCharacteristic(BLEChar * characteristic, uint8_t * val, uint8_t length);

uint8_t readCharacteristic(BLEChar * characteristic, uint8_t * val, uint8_t length);

#endif //BLETypes_h
