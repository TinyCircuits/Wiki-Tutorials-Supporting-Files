

int saberLight = 0;
int saberLightOtherWay = 0;
int saberCount = 0;
int saberVals[LEDCount];

void saberModeInit() {
  saberLight = 0;
  saberLightOtherWay = 0;
  saberCount = 0;
  memset(saberVals, 0, sizeof(saberVals));
}

void saberModeLoop(int maxReading, int ledToLight) {
  if (accel.doubleTap) {
    saberColor++;
    if (saberColor > 2)saberColor = 0;
    delay(50);
    accel.clearInterrupt();
  }

  int j;

  int touchCal = 20;
  //A = avgX - 55;
  //if (A < 0)A = 5;
  int A = 5;
  if (maxReading > touchCal) {
    int light = ledToLight;
    if (saberLight < light)saberLight = light;
    if (saberLight > (LEDCount - 1))saberLight = light;

    if (saberLightOtherWay > light)saberLightOtherWay = light;
    if (saberLightOtherWay < 0)saberLightOtherWay = light;
    if (saberLightOtherWay > (LEDCount - 2))saberLightOtherWay = LEDCount - 2;

    A = 200;
  }


  int aMult = A / 30;
  if (aMult < 1)aMult = 1;
  for (j = 0; j < LEDCount; j++) {
    int lightVal = saberVals[j];
    if (j == saberLight) {
      if (lightVal < 255) {
        lightVal += (8 * aMult);
        if (lightVal > 255)lightVal = 255;
      } else {
        saberLight += 1;
      }
    } else if (j == saberLightOtherWay) {

      if (lightVal < 255) {
        lightVal += (8 * aMult);
        if (lightVal > 255)lightVal = 255;
      } else {
        saberLightOtherWay -= 1;
      }

    } else {
      if (lightVal > 0) {
        lightVal -= (2 * aMult);
        if (lightVal < 0)lightVal = 0;
      }
    }
    saberVals[j] = lightVal;
    lightVal = A + lightVal;
    int extra = 0;
    if (lightVal > 255) {
      extra = (lightVal - 255) / 2;
      lightVal = 255;
      if (extra > 255)
        extra = 255;
    }
    if(saberColor==0){
      led[j].r = lightVal;
    }else{
      led[j].r = extra;
    }
    if(saberColor==1){
      led[j].g = lightVal;
    }else{
      led[j].g = extra;
    }
    if(saberColor==2){
      led[j].b = lightVal;
    }else{
      led[j].b = extra;
    }
  }

  if (saberLight > (LEDCount-1))
    saberLight++;

  if (saberLight > 250)
    saberLight = 0;

  //if(saberLightOtherWay>=0)
  //  saberLightOtherWay--
}
