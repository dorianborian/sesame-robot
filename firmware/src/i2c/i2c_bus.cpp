#include "i2c_bus.h"
#include <Wire.h>

void initI2C() {
  Wire.begin(I2C_SDA, I2C_SCL);
}
