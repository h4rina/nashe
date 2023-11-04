#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

const char *ssid = "Wifiprota";
const char *password = "prota123";
const char *mqtt_server = "broker.hivemq.com";

#define DHTPIN 16
#define DHTTYPE DHT22

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MESSAGE_INTERVAL 3000

DHT dht(DHTPIN, DHTTYPE);

void setup_wifi()
{
    delay(10);
    Serial.begin(115200);
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Esperando conexion MQTT...");
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str()))
        {
            Serial.println("Conectado");
            client.subscribe("/ET28/6TO/inTopic");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" intenta de nuevo en 5 segundos");
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    dht.begin();
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    unsigned long now = millis();
    if (now - lastMsg > MESSAGE_INTERVAL)
    {
        lastMsg = now;

        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();

        if (!isnan(humidity))
        {

            String humidityValue = String(humidity);
            Serial.print("Publicando el mensaje: ");
            Serial.println(humidityValue);
            client.publish("/ET28/PROYECTO/HUMEDAD", humidityValue.c_str());
        }
        else
        {
            Serial.println("No recibe data del DHT");
        }
        if (!isnan(temperature))
        {

            String temperatureValue = String(temperature);
            Serial.print("Publicando mensaje: ");
            Serial.println(temperatureValue);
            client.publish("/ET28/PROYECTO/TEMPERATURA", temperatureValue.c_str());
        }
        else
        {
            Serial.println("No recibe data del DHT");
        }
    }
}