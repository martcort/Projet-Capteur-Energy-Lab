/************************ Configuration Adafruit IO *******************************/

// Rendez-vous sur io.adafruit.com si vous devez créer un compte
// ou si vous avez besoin de votre clé Adafruit IO.
#define IO_USERNAME  "username"  // Nom d'utilisateur Adafruit IO
#define IO_KEY       "mp"  // Clé Adafruit IO

/******************************* WIFI *********************************************/

// Le client AdafruitIO_WiFi fonctionnera avec les cartes suivantes :
//   - HUZZAH ESP8266 Breakout -> https://www.adafruit.com/products/2471
//   - Feather HUZZAH ESP8266 -> https://www.adafruit.com/products/2821
//   - Feather HUZZAH ESP32 -> https://www.adafruit.com/product/3405
//   - Feather M0 WiFi -> https://www.adafruit.com/products/3010
//   - Feather WICED -> https://www.adafruit.com/products/3056
//   - Adafruit PyPortal -> https://www.adafruit.com/product/4116
//   - Adafruit Metro M4 Express AirLift Lite -> https://www.adafruit.com/product/4000
//   - Adafruit AirLift Breakout -> https://www.adafruit.com/product/4201
//   - Adafruit AirLift Shield -> https://www.adafruit.com/product/4285
//   - Adafruit AirLift FeatherWing -> https://www.adafruit.com/product/4264

#define WIFI_SSID "wifi"  // Nom du réseau WiFi
#define WIFI_PASS "mp"  // Mot de passe du réseau WiFi

// Décommentez la ligne suivante si vous utilisez AirLift
// #define USE_AIRLIFT

// Décommentez la ligne suivante si vous utilisez winc1500
// #define USE_WINC1500

// Décommentez la ligne suivante si vous utilisez une carte MKR1010 ou Nano 33 IoT
// #define ARDUINO_SAMD_MKR1010

// Commentez les lignes suivantes si vous utilisez FONA ou Ethernet
#include "AdafruitIO_WiFi.h"

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) ||         \
    defined(ADAFRUIT_PYPORTAL)
// Configuration des broches utilisées pour la connexion ESP32
#if !defined(SPIWIFI_SS) // Si la définition WiFi n'est pas incluse dans la variante de la carte
// Ne changez pas les noms de ces #define, ils correspondent aux noms des variantes
#define SPIWIFI SPI         // Interface SPI
#define SPIWIFI_SS 10       // Broche de sélection du périphérique (chip select)
#define NINA_ACK 9          // Broche BUSY ou READY
#define NINA_RESETN 6       // Broche de réinitialisation
#define NINA_GPIO0 -1       // Non connectée
#endif
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS,
                   NINA_ACK, NINA_RESETN, NINA_GPIO0, &SPIWIFI);
#else
// Initialisation standard pour Adafruit IO via WiFi
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
#endif

/******************************* FONA ********************************************/

// Le client AdafruitIO_FONA fonctionnera avec les cartes suivantes :
//   - Feather 32u4 FONA -> https://www.adafruit.com/product/3027

// Décommentez les deux lignes suivantes pour Feather 32u4 FONA,
// et commentez le client AdafruitIO_WiFi dans la section WiFi
// #include "AdafruitIO_FONA.h"
// AdafruitIO_FONA io(IO_USERNAME, IO_KEY);

/**************************** ETHERNET *******************************************/

// Le client AdafruitIO_Ethernet fonctionnera avec les cartes suivantes :
//   - Ethernet FeatherWing -> https://www.adafruit.com/products/3201

// Décommentez les deux lignes suivantes pour Ethernet,
// et commentez le client AdafruitIO_WiFi dans la section WiFi
// #include "AdafruitIO_Ethernet.h"
// AdafruitIO_Ethernet io(IO_USERNAME, IO_KEY);
