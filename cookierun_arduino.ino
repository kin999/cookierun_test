#include "I2Cdev.h"
#include "Wire.h"
#include "MPU6050.h"

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int key_jump = 66;//key 'b'
int key_slide = 78;//kry 'n'
float base_x_acc, base_y_acc, base_z_acc;
float base_x_gyr, base_y_gyr, base_z_gyr;

int state_jump = 0;
float bound_jump = 12.0;
boolean is_foward = true;

int state_slide = 0;
float bound_slide = 225.0;
float bound_slideX = 12.0;
boolean is_jump = false;

void setup(){
  Wire.begin();
  Keyboard.begin();
  Serial.begin(38400);
  accelgyro.initialize();
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  calibrate_sensors();
}

void loop(){
  //long startTime = micros();
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  double Ax = 2*9.8*ax/32768;
  double Ay = 2*9.8*ay/32768;
  double Az = 2*9.8*az/32768;
  double Gx = (gx-base_x_gyr)/131.0;
  double Gy = (gy-base_y_gyr)/131.0;
  double Gz = (gz-base_z_gyr)/131.0;
  Serial.println(state_jump + " :: "+ state_slide);
  //detect jump
  if(state_jump == 0){
    is_jump = false;
    if(Az > bound_jump && Gx > bound_jump ){
      state_jump = 1;
      is_foward = true;
      //      Serial.println("s0 to s1");
    }
  }
  else if(state_jump == 1){
    if(Az < -bound_jump && Gx < -bound_jump){
      state_jump = 2;
      //      Serial.println("s1 to s2");
    }    
  }
  else if(state_jump == 2){
    if(Az > bound_jump && Gx > bound_jump){
      state_jump = 3;
    }
  }
  else if(state_jump == 3){
    if(Az < bound_jump && Gx < bound_jump){
      state_jump = 0;
      Serial.println("Jump");
      Keyboard.press(key_jump);
      is_foward = true;
      state_slide = 0;
      is_jump = true;
    }
  }

  //detect slide
  //  Serial.println(is_jump);
  if(state_slide == 0){
    if(Gz > bound_slide && Ax > bound_slideX ){
      state_slide = 1;
      //      Serial.println("s0 to s1");
    }
  }
  else if(state_slide == 1){
    if(Gz < base_z_gyr ){
      state_slide = 2;
      //      Serial.println("s1 to s2");
    }
  }
  else if(state_slide == 2){
    Serial.println("slide");
    if(Gz < -bound_slide && Ax < -bound_slideX ){
      state_slide = 3;
      Keyboard.press(key_slide);
      //      Serial.println("s2 to s3");
    }
  }
  else if(state_slide == 3){
    Serial.println("slide");
    if(Gz > base_z_gyr){
      state_slide = 0;
      Keyboard.release(key_slide);
      //      Serial.println("s3 to s0");
    }
  }

  //  Serial.print("a/g:\t");
  //  Serial.print(Ax); Serial.print("\t");
  //  Serial.print(Ay); Serial.print("\t");
  //  Serial.print(Az); Serial.print("\t");
  //  Serial.print(Gx); Serial.print("\t");
  //  Serial.print(Gy); Serial.print("\t");
  //  Serial.println(Gz);
  delay(5);
}

void calibrate_sensors(){
  int    num_readings = 10;
  float  x_acc = 0;
  float  y_acc = 0;
  float  z_acc = 0;
  float  x_gyr = 0;
  float  y_gyr = 0;
  float  z_gyr = 0;

  for(int i=0; i < num_readings; i++){
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    x_acc += ax;
    y_acc += ay;
    z_acc += az;
    x_gyr += gz;
    y_gyr += gy;
    z_gyr += gz;
    delay(100);
  }
  x_acc /= num_readings;
  y_acc /= num_readings;
  z_acc /= num_readings;
  x_gyr /= num_readings;
  y_gyr /= num_readings;
  z_gyr /= num_readings;

  // Store the raw calibration values globally
  base_x_acc = x_acc;
  base_y_acc = y_acc;
  base_z_acc = z_acc;
  base_x_gyr = x_gyr;
  base_y_gyr = y_gyr;
  base_z_gyr = z_gyr;
}
