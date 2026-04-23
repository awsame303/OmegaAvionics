#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP3XX.h>
#include <Servo.h>
#include <MPU6050_light.h>

#define SERVO_PIN 10
#define SD_CS_PIN 4
#define GROUND_PRESSURE_HPA (30.15 * 33.86389)

#define LOOP_DELAY_MS 50
#define DEPLOY_ALT_MIN_FT 300
#define TRIGGER_APOGEE_FT 750
#define PERSIST_COUNT 5

// Physics Constants for 2.27" 560g Rocket
#define ROCKET_MASS_KG 0.560f
#define AREA_M2 0.00261f
#define RHO 1.225f         // Air density kg/m^3

// --- Aerodynamic Drag States ---
// Both set to 0.559 for Flight 1 (No physical deployment)
#define CD_RETRACTED 0.559f 
#define CD_DEPLOYED 0.559f  

#define TWO_KP (2.0f * 0.5f)
#define TWO_KI (2.0f * 0.0f)

Adafruit_BMP3XX bmp;
MPU6050 mpu(Wire);
Servo airbrake;

float groundAlt = 0;
float altitude_m = 0, velocity_mps = 0, acceleration_mps2 = 0;
float prev_alt = 0;
float tilt[3] = { 0 };
float accel[3] = { 0 };

float est_alt = 0, est_vel = 0;
float P = 1, Q = 0.1, R = 3;
unsigned long last_time = 0;

bool deployed = false;
bool retracted = false;
int above750_count = 0;
int below750_count = 0;
float max_alt = 0;
bool apogee_logged = false;

float ax_offset = 0.0f;
float az_offset = 0.0f;

float integralFBx = 0.0f, integralFBy = 0.0f, integralFBz = 0.0f;
float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;

float kalmanUpdate(float meas, float &est, float &P) {
  P += Q;
  float K = P / (P + R);
  est += K * (meas - est);
  P *= (1 - K);
  return est;
}

void MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az, float dt) {
  float recipNorm;
  float halfvx, halfvy, halfvz;
  float halfex, halfey, halfez;

  gx *= 0.0174533f;
  gy *= 0.0174533f;
  gz *= 0.0174533f;

  recipNorm = 1.0f / sqrtf(ax * ax + ay * ay + az * az);
  ax *= recipNorm;
  ay *= recipNorm;
  az *= recipNorm;

  halfvx = q1 * q3 - q0 * q2;
  halfvy = q0 * q1 + q2 * q3;
  halfvz = q0 * q0 - 0.5f + q3 * q3;

  halfex = (ay * halfvz - az * halfvy);
  halfey = (az * halfvx - ax * halfvz);
  halfez = (ax * halfvy - ay * halfvx);

  if (TWO_KI > 0.0f) {
    integralFBx += TWO_KI * halfex * dt;
    integralFBy += TWO_KI * halfey * dt;
    integralFBz += TWO_KI * halfez * dt;
    gx += integralFBx;
    gy += integralFBy;
    gz += integralFBz;
  } else {
    integralFBx = 0.0f;
    integralFBy = 0.0f;
    integralFBz = 0.0f;
  }

  gx += TWO_KP * halfex;
  gy += TWO_KP * halfey;
  gz += TWO_KP * halfez;
  gx *= (0.5f * dt);
  gy *= (0.5f * dt);
  gz *= (0.5f * dt);

  float qa = q0, qb = q1, qc = q2;
  q0 += (-qb * gx - qc * gy - q3 * gz);
  q1 += (qa * gx + qc * gz - q3 * gy);
  q2 += (qa * gy - qb * gz + q3 * gx);
  q3 += (qa * gz + qb * gy - qc * gx);

  recipNorm = 1.0f / sqrtf(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
}

void clearLogs() {
  if (SD.exists("data.log")) SD.remove("data.log");
  if (SD.exists("apogee.log")) SD.remove("apogee.log");
  File f = SD.open("data.log", FILE_WRITE);
  if (f) {
    f.println(F("time_ms,alt_ft,vel_fps,ax,ay,az,tilt_x_roll,tilt_y_pitch,tilt_z_yaw,pred_apogee_ft"));
    f.close();
  }
}

void setup() {
  Wire.begin();
  delay(10000);
  
  // Force airbrakes completely closed for Flight 1 test
  airbrake.attach(SERVO_PIN);
  airbrake.write(0);
  delay(1000);

  if (!SD.begin(SD_CS_PIN)) {
    while (1);
  }
  clearLogs();

  if (!bmp.begin_I2C(0x77)) {
    while (1);
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);

  mpu.begin();
  delay(100);
  mpu.calcOffsets(true, false);

  for (int i = 0; i < 100; i++) {
    mpu.update();
    ax_offset += mpu.getAccX();
    az_offset += mpu.getAccZ();
    delay(10);
  }
  ax_offset /= 100.0f;
  az_offset /= 100.0f;

  for (int i = 0; i < 20; i++) {
    bmp.performReading();
    delay(10);
  }
  bmp.performReading();
  groundAlt = bmp.readAltitude(GROUND_PRESSURE_HPA);
  last_time = millis();
}

void loop() {
  unsigned long t = millis();
  if (t - last_time < LOOP_DELAY_MS) return;
  float dt = (t - last_time) / 1000.0f;
  last_time = t;

  mpu.update();
  bmp.performReading();

  float alt = bmp.readAltitude(GROUND_PRESSURE_HPA) - groundAlt;
  altitude_m = kalmanUpdate(alt, est_alt, P);
  velocity_mps = (altitude_m - prev_alt) / dt;
  acceleration_mps2 = (velocity_mps - est_vel) / dt;
  est_vel = velocity_mps;
  prev_alt = altitude_m;

  float ax_raw = mpu.getAccX() - ax_offset;
  float ay_raw = mpu.getAccY();
  float az_raw = mpu.getAccZ() - az_offset;

  float gx_raw = mpu.getGyroX();
  float gy_raw = mpu.getGyroY();
  float gz_raw = mpu.getGyroZ();

  accel[0] = ax_raw * 9.81f;
  accel[1] = az_raw * 9.81f;
  accel[2] = -ay_raw * 9.81f;

  MahonyAHRSupdateIMU(gx_raw, gz_raw, -gy_raw, ax_raw, az_raw, -ay_raw, dt);

  float roll = atan2f(2.0f * (q0 * q1 + q2 * q3), 1 - 2.0f * (q1 * q1 + q2 * q2)) * 57.29578f;
  float pitch = asinf(2.0f * (q0 * q2 - q3 * q1)) * 57.29578f;
  float yaw = atan2f(2.0f * (q0 * q3 + q1 * q2), 1 - 2.0f * (q2 * q2 + q3 * q3)) * 57.29578f;

  tilt[0] = roll;
  tilt[1] = pitch;
  tilt[2] = yaw;

  // --- APOGEE PREDICTION MATH (NUMERICAL INTEGRATION) ---
  float current_cd = deployed ? CD_DEPLOYED : CD_RETRACTED;
  
  float sim_alt = altitude_m;
  float sim_vel = velocity_mps;
  float sim_dt = 0.1f; // 100ms time step

  // Simulate flight path until apogee
  while (sim_vel > 0) {
    float sim_v_sq = sim_vel * sim_vel;
    float sim_drag_force = 0.5f * RHO * sim_v_sq * current_cd * AREA_M2;
    float sim_drag_decel = sim_drag_force / ROCKET_MASS_KG;
    
    float total_decel = 9.81f + sim_drag_decel;
    
    sim_alt += (sim_vel * sim_dt);
    sim_vel -= (total_decel * sim_dt);
  }

  float predicted_apogee_m = sim_alt;
  float predicted_apogee_ft = predicted_apogee_m * 3.28084f;
  float current_alt_ft = altitude_m * 3.28084f;
  if (altitude_m > max_alt) max_alt = altitude_m;

  // --- DEPLOYMENT LOGIC ---
  if (!deployed && current_alt_ft > DEPLOY_ALT_MIN_FT) {
    if (predicted_apogee_ft > TRIGGER_APOGEE_FT) {
      above750_count++;
    } else {
      if (above750_count > 0) above750_count--;
    }

    if (above750_count >= PERSIST_COUNT) {
      // Logic triggers, but flaps are disconnected/zeroed for Flight 1 testing
      // airbrake.write(120); 
      deployed = true;
    }
  }

  // --- RETRACTION LOGIC ---
  if (deployed && !retracted) {
    if (predicted_apogee_ft < (TRIGGER_APOGEE_FT - 5)) {  // 5ft Buffer
      below750_count++;
    } else {
      if (below750_count > 0) below750_count--;
    }

    if (below750_count >= 5) {  // Matching the 5 solid readings from the Python script
      // airbrake.write(0);
      retracted = true;
    }
  }

  File f = SD.open("data.log", FILE_WRITE);
  if (f) {
    f.print(t);
    f.print(',');
    f.print(current_alt_ft, 1);
    f.print(',');
    f.print(velocity_mps * 3.28084f, 1);
    f.print(',');
    f.print(accel[0], 2);
    f.print(',');
    f.print(accel[1], 2);
    f.print(',');
    f.print(accel[2], 2);
    f.print(',');
    f.print(tilt[0], 1);
    f.print(',');
    f.print(tilt[1], 1);
    f.print(',');
    f.print(tilt[2], 1);
    f.print(',');
    f.println(predicted_apogee_ft, 1);
    f.close();
  }

  if (!apogee_logged && est_vel < 0 && acceleration_mps2 < -0.2f && max_alt >= 100) {
    File af = SD.open("apogee.log", FILE_WRITE);
    if (af) {
      af.print(F("Apogee(ft): "));
      af.println(max_alt * 3.28084f, 1);
      af.close();
    }
    apogee_logged = true;
  }
}