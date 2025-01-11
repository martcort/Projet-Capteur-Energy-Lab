#include "config.h"
#include <Adafruit_BME280.h>  // Inclusion de la bibliothèque BME280 d'Adafruit
#include "RTClib.h"  // Inclusion de la bibliothèque pour la gestion du module RTC

RTC_DS3231 rtc;

// Constantes du programme
#define adresseI2CduBME280 0x76               // Adresse I2C du capteur BME280 (0x76)
#define pressionAuNiveauDeLaMerEnHpa 1024.90  // Pression standard au niveau de la mer en hPa
#define delaiRafraichissementAffichage 1500   // Délai de rafraîchissement de l'affichage en millisecondes

// Instanciation de la bibliothèque BME280
Adafruit_BME280 bme;

#include <SPI.h>
#include <SD.h>

const int chipSelect = 15;  // Broche de sélection de la carte SD

// Création des flux Adafruit IO pour envoyer les données à la plateforme
AdafruitIO_Feed *temp = io.feed("temperature");  // Flux pour la température
AdafruitIO_Feed *hum = io.feed("humidite");      // Flux pour l'humidité
AdafruitIO_Feed *press = io.feed("pression");   // Flux pour la pression
AdafruitIO_Feed *alt = io.feed("altitude");     // Flux pour l'altitude
AdafruitIO_Feed *inten = io.feed("intensite");  // Flux pour l'intensité

void setup() {
  // Initialisation du port série pour le moniteur série
  Serial.begin(9600);
  while (!Serial);  // Attente de la connexion série
  Serial.println("Programme de test du BME280");

#ifndef ESP8266
  while (!Serial);  // Nécessaire pour certains modules USB natifs
#endif

  // Initialisation du capteur BME280
  if (!bme.begin(adresseI2CduBME280)) {
    Serial.println(F("--> ÉCHEC de l'initialisation du BME280..."));
    while (1);  // Blocage si le capteur n'est pas détecté
  } else {
    Serial.println(F("--> Initialisation du BME280 réussie !"));
  }

  // Connexion à la plateforme Adafruit IO
  Serial.begin(115200);
  while (!Serial);  // Attente de la connexion série
  Serial.print("Connexion à Adafruit IO");

  io.connect();  // Connexion au service
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);

    // Initialisation du module RTC
    if (!rtc.begin()) {
      Serial.println("Module RTC non détecté !");
      Serial.flush();
      while (1) delay(10);
    }

    // Vérification de la perte d'alimentation du RTC
    if (rtc.lostPower()) {
      Serial.println("Le RTC a perdu son alimentation. Réinitialisation...");
      // Réglage de l'heure à la date de compilation du programme
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // Exemple pour un réglage manuel : rtc.adjust(DateTime(2024, 1, 21, 3, 0, 0));
    }
  }

  Serial.println();
  Serial.println(io.statusText());

  // Affichage des étiquettes en première ligne du moniteur série
  Serial.println("TIME;TEMPERATURE;PRESSION;HUMIDITE;ALTITUDE");

  Serial.print("Initialisation de la carte SD...");

  // Vérification de la présence et de l'initialisation de la carte SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Échec de l'initialisation ou carte absente");
    return;  // Fin du programme en cas d'échec
  }
  Serial.println("Carte SD initialisée.");
}

void loop() {
  io.run();  // Gestion des flux Adafruit IO
  DateTime now = rtc.now();  // Lecture de l'heure actuelle via le RTC
  String dataString = "";

  // Lecture de la tension sur la broche analogique A0
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (1.0 / 1.023);  // Conversion en tension (mV) (correspond à VMax/2)
  float voltageeff = voltage * 2 / 1.414;       // Calcul de la tension efficace (Veff = Vmax/sqrt(2))
  float intensite = voltageeff * 0.0258 - 0.1902;  // Calcul de l'intensité (avec la linéarisation réalisée sur Excel)

  // Affichage des mesures de tension et d'intensité
  Serial.print("Vmesuré : ");
  Serial.print(voltage);
  Serial.print("mV, Intensité : ");
  Serial.print(intensite);
  Serial.println(" A");

  // Sauvegarde des données dans les flux Adafruit IO
  temp->save(bme.readTemperature());
  hum->save(bme.readHumidity());
  press->save(bme.readPressure() / 100.0F);
  alt->save(bme.readAltitude(pressionAuNiveauDeLaMerEnHpa));
  inten->save(intensite);

  // Affichage des données sur le moniteur série
  // Date (année, mois, jour, heure, minute et seconde)
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
  Serial.print(bme.readTemperature());  // Température
  Serial.print(";");
  Serial.print(bme.readPressure() / 100.0F);  // Pression
  Serial.print(";");
  Serial.print(bme.readHumidity());  // Humidité
  Serial.print(";");
  Serial.print(bme.readAltitude(pressionAuNiveauDeLaMerEnHpa));  // Altitude
  Serial.print(";");
  Serial.println(intensite);  // Intensité

  // Préparation de la chaîne de données pour la carte SD
  dataString = String(now.year(), DEC) + ":" +
               String(now.month(), DEC) + ":" +
               String(now.day(), DEC) + " " +
               String(now.hour(), DEC) + ":" +
               String(now.minute(), DEC) + ":" +
               String(now.second(), DEC) + ";" +
               String(bme.readTemperature()) + ";" +
               String(bme.readPressure() / 100.0F) + ";" +
               String(bme.readHumidity()) + ";" +
               String(bme.readAltitude(pressionAuNiveauDeLaMerEnHpa)) + ";" + 
               String(intensite);

  // Enregistrement des données sur la carte SD
  File dataFile = SD.open("Données.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);  // Écriture des données
    dataFile.close();  // Fermeture du fichier
    Serial.println(dataString);  // Affichage des données sur le moniteur série
  } else {
    Serial.println("Erreur lors de l'ouverture du fichier Données.txt");
  }

  delay(10000);  // Pause de 10 secondes entre chaque boucle pour ne pas surcharger AdafruitIO
}
