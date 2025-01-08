#include "config.h"
#include <Adafruit_BME280.h>  // Inclusion de la librairie BME280 d'Adafruit
#include "RTClib.h"

RTC_DS3231 rtc;

// Constantes du programme
#define adresseI2CduBME280 0x76               // Adresse I2C du BME280 (0x76)
#define pressionAuNiveauDeLaMerEnHpa 1024.90  // Pression au niveau de la mer en hPa
#define delaiRafraichissementAffichage 1500   // Délai de rafraîchissement en ms

// Instanciation de la librairie BME280
Adafruit_BME280 bme;

#include <SPI.h>
#include <SD.h>

const int chipSelect = 15;


// Création des flux Adafruit IO
AdafruitIO_Feed *temp = io.feed("temperature");
AdafruitIO_Feed *hum = io.feed("humidite");
AdafruitIO_Feed *press = io.feed("pression");
AdafruitIO_Feed *alt = io.feed("altitude");
AdafruitIO_Feed *inten = io.feed("intensite");


void setup() {
  // Initialisation du port série pour le moniteur
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Programme de test du BME280");

#ifndef ESP8266
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
#endif

  // Initialisation du BME280
  if (!bme.begin(adresseI2CduBME280)) {
    Serial.println(F("--> ÉCHEC…"));
    while (1)
      ;
  } else {
    Serial.println(F("--> RÉUSSIE !"));
  }

  // Connexion à Adafruit IO
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.print("Connecting to Adafruit IO");

  io.connect();
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);

    if (!rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      while (1) delay(10);
    }

    if (rtc.lostPower()) {
      Serial.println("RTC lost power, let's set the time!");
      // When time needs to be set on a new device, or after a power loss, the
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
  }

  Serial.println();
  Serial.println(io.statusText());

  // Afficher les étiquettes en première ligne
  Serial.println("TIME;TEMPERATURE;PRESSION;HUMIDITE;ALTITUDE");

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void loop() {
  io.run();
  DateTime now = rtc.now();
  String dataString = "";

  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (1.0 / 1.023);
  float intensite = voltage*0.1171-1.12;
  // Afficher la tension en volts
  Serial.print("Vmesuré : ");
  Serial.print(voltage);
  Serial.print("Intensité : ");
  Serial.print(intensite);
  Serial.println(" A");

  // Sauvegarde des données sur Adafruit IO
  
  temp->save(bme.readTemperature());
  hum->save(bme.readHumidity());
  press->save(bme.readPressure() / 100.0F);
  alt->save(bme.readAltitude(pressionAuNiveauDeLaMerEnHpa));
  inten->save(intensite);
  
  
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);

  Serial.print(";");
  Serial.print(bme.readTemperature());  // TEMPERATURE
  Serial.print(";");
  Serial.print(bme.readPressure() / 100.0F);  // PRESSION
  Serial.print(";");
  Serial.print(bme.readHumidity());  // HUMIDITE
  Serial.print(";");
  Serial.print(bme.readAltitude(pressionAuNiveauDeLaMerEnHpa));  // ALTITUDE
  Serial.print(";");
  Serial.println(intensite); // Intensite

  dataString = String(now.year(),DEC) + ":" +
                String(now.month(),DEC) + ":" +
                String(now.day(),DEC) + " " +
                String(now.hour(),DEC) + ":" +
                String(now.minute(),DEC) + ":" +
                String(now.second(),DEC) + ";" +
                String(bme.readTemperature()) + ";" +
                String(bme.readPressure() / 100.0F) + ";" +
                String(bme.readHumidity()) + ";" +
                String(bme.readAltitude(pressionAuNiveauDeLaMerEnHpa)) + ";" + 
                String(intensite);

  File dataFile = SD.open("Données.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else { Serial.println("error opening datalog.txt"); }


  delay(5000);  // Pause de 10 secondes
}
