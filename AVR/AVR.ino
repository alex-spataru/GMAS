/*
 * Copyright (c) 2019 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#include "MPU6050.h"

//
// -------------------------------------
// Configuracion de cables/pines del MPU
// -------------------------------------
// Blanco/Verde     INT       D2
// Verde            XCL       --
// Naranja          XDA       --
// Cafe             VCC       5V
// Blanco/Cafe      SDA       A4
// Azul             SCL       A5
// Blanco/Azul      GND       GND
// Blanco/Naranja   AD0       --
// -------------------------------------
//

//
// Controlador del MPU 6050
//
static MPU6050 mpu;

//
// Datos de acelerometro
//
static float aX, aY, aZ;

//
// Datos de giroscopio
//
static float gX, gY, gZ;

//
// Datos de control
//
static float velocidad = 0;
static uint64_t ultimoTiempo = 0;

//
// Para leer paquetes
//
static int countDatos = 0;
static char paquete[255];

///
/// Obtiene la amplitud y la frecuencia deseada por el usuario
///
void actualizarSerial() {
  // Leer datos del serial
  while (Serial.available() > 0) {
    // Leer caracter
    char c = Serial.read();

    // Actualizar paquete de datos
    switch (c) {
      // Caracter de finalizacion de paquete, actualizar datos
      // y limpiar buffer del paquete
      case ';':
        countDatos = 0;
        velocidad = strtod(paquete, NULL);
        break;

      // Entrada de datos al elemento actual del paquete
      default:
        paquete[countDatos] = c;
        ++countDatos;
        break;
    }

    // Evitar errores si los datos no llegan como deberian
    // de llegar
    if (countDatos >= 255)
      countDatos = 0;
  }
}

///
/// Actualiza el movimiento del motor para obtener un MAS como el
/// que el usuario especifico
///
static void actualizarMotor() {
  analogWrite(6, (int) (velocidad * 2.5));
}

///
/// Manda los datos del MPU 6050 al software de control para procesamiento
///
static void mandarDatos() {
  // Obtener valores del acelerometro
  Vector normAccel = mpu.readNormalizeAccel();
  aX = normAccel.XAxis;
  aY = normAccel.YAxis;
  aZ = normAccel.ZAxis;

  // Obtener valores del giroscopio
  Vector normGiro = mpu.readNormalizeGyro();
  gX = normGiro.XAxis;
  gY = normGiro.YAxis;
  gZ = normGiro.ZAxis;

  // Mandar secuencia de inicio
  Serial.print("{");

  // Mandar datos de acelerometro
  Serial.print(aX); Serial.print(',');
  Serial.print(aY); Serial.print(',');
  Serial.print(aZ); Serial.print(',');

  // Mandar datos de giroscopio
  Serial.print(gX); Serial.print(',');
  Serial.print(gY); Serial.print(',');
  Serial.print(gZ); Serial.print('}');

  // Mandar secuencia de terminacion
  Serial.print(";");
}

///
/// Funcion de configuracion del Arduino
///
void setup() {
  // Inicializar serial
  Serial.begin(1000000);

  // Inicializar pines del motor
  pinMode(6, OUTPUT);

  // Inicializar MPU 6050
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  // Magia
  mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);
  mpu.setIntFreeFallEnabled(false);  
  mpu.setIntZeroMotionEnabled(false);
  mpu.setIntMotionEnabled(false);
  mpu.setDHPFMode(MPU6050_DHPF_5HZ);
  mpu.setMotionDetectionThreshold(2);
  mpu.setMotionDetectionDuration(5);
  mpu.setZeroMotionDetectionThreshold(4);
  mpu.setZeroMotionDetectionDuration(2);  
}

///
/// Loop principal
///
void loop() {
  // Actualizar datos de control y mover motor
  actualizarMotor();
  actualizarSerial();

  // Actualizar datos de control cada 20 ms
  if (millis() - ultimoTiempo >= 20) {
    ultimoTiempo = millis();
    mandarDatos();
  }
}
