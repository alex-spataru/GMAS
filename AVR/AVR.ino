#include "MPU6050.h"

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
static float amplitud = 2.5;
static int frecuencia = 0;
static uint64_t ultimoTiempo = 0;

///
/// Obtiene la amplitud y la frecuencia deseada por el usuario
///
static void actualizarDatosDeControl() {
  // Leer datos del serial
  if (Serial.available() > 0) {
    // Obtener datos como string
    String buffer = Serial.readString();
    int len = buffer.length();

    // Paquete de control valido, proceder a actualizar amplitud y frecuencia
    if (buffer.charAt(len - 1) == ';') {
      // Quitar caracter de terminacion
      buffer.remove(len - 1);

      // Interpretar amplitud y frecuencia del paquete
      int index = buffer.indexOf(',');
      if (index > 0) {
        amplitud = buffer.substring(0, index).toFloat();
        frecuencia = buffer.substring(index + 1, len - 1).toFloat();

        Serial.flush();
      }
    }
  }
}

///
/// Actualiza el movimiento del motor para obtener un MAS como el
/// que el usuario especifico
///
static void actualizarMotor() {

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
  Serial.begin(115200);

  // Inicializar MPU 6050
  while (!mpu.begin(MPU6050_SCALE_250DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
}

///
/// Loop principal
///
void loop() {
  // Actualizar datos de control y mover motor
  actualizarMotor();

  // Actualizar datos de control cada 1000 ms
  if (millis() - ultimoTiempo > 100) {
    ultimoTiempo = millis();
    mandarDatos();
    actualizarDatosDeControl();
  }
}

