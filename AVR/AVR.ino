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
static float amplitud = 0;
static int frecuencia = 0;
static uint64_t ultimoTiempo = 0;

//
// Para leer paquetes
//
int indice = 0;
int countDatos = 0;
static char paquete[2][32];

///
/// Obtiene la amplitud y la frecuencia deseada por el usuario
///
void serialEvent() {
  // Leer datos del serial
  if (Serial.available() > 0) {
    // Leer caracter
    char c = Serial.read();

    // Actualizar paquete de datos
    switch (c) {
      // Caracter de separacion de datos, incrementar indice y 
      // construir nuevo string
      case ',':
        ++indice;
        countDatos = 0;
        break;

      // Caracter de finalizacion de paquete, actualizar datos
      // y limpiar buffer del paquete
      case ';':
        indice = 0;
        countDatos = 0;
        amplitud = strtod(paquete[0], NULL);
        frecuencia = strtod(paquete[1], NULL);

        for (int i = 0; i < 32; ++i) {
          paquete[0][i] = '\0';
          paquete[1][i] = '\0';
        }
        
        break;

      // Entrada de datos al elemento actual del paquete
      default:
        paquete[indice][countDatos] = c;
        ++countDatos;
        break;
    }

    // Evitar errores si los datos no llegan como deberian
    // de llegar
    if (indice > 1 || countDatos >= 255) {
      indice = 0;
      countDatos = 0;
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

  // Inicializar MPU 6050
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G))
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

  // Actualizar datos de control cada 10 ms
  if (millis() - ultimoTiempo >= 10) {
    ultimoTiempo = millis();
    mandarDatos();
  }
}
