# 1 "/Users/nodebotanist/code/bots/projects/nodebotanist-hub/sample-firmware/esp8266/main.ino"
# 1 "/Users/nodebotanist/code/bots/projects/nodebotanist-hub/sample-firmware/esp8266/main.ino"
# 2 "/Users/nodebotanist/code/bots/projects/nodebotanist-hub/sample-firmware/esp8266/main.ino" 2
# 3 "/Users/nodebotanist/code/bots/projects/nodebotanist-hub/sample-firmware/esp8266/main.ino" 2
# 4 "/Users/nodebotanist/code/bots/projects/nodebotanist-hub/sample-firmware/esp8266/main.ino" 2



const char *mqtt_server = "192.168.1.107";
long lastMsg = 0;

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, 13, ((1 << 6) | (1 << 4) | (0 << 2) | (2)) + 0x0000 /* 800 KHz datastream*/);

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), 16);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      /* Add all subs here */
      client.subscribe("LED");
      client.subscribe("kperch/feeds/twitter");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(", trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void flash(uint16_t red, uint16_t green, uint16_t blue)
{
  uint16_t i, j;
  for (j = 0; j < 5; j++)
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, strip.Color(red, green, blue));
    }
    strip.show();
    delay(500);
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
    delay(500);
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived with topic ");
  Serial.print(topic);
  Serial.print(" : ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, "kperch/feeds/twitter") == 0)
  {
    Serial.println("Tweet!");
    flash(0, 255, 0);
  }
}

void setup()
{
  pinMode(BUILTIN_LED, 0x01);
  Serial.begin(115200);
  Serial.println();

  WiFi.begin("Megaton", "Thepowerofatom!");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

uint16_t j = 0;

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  rainbow();

  long now = millis();
  if (now - lastMsg > 5000)
  {
    lastMsg = now;
    // place timed events and stuff here!
  }
  delay(100);
}

void rainbow()
{
  uint16_t i;
  if (j > 255)
  {
    j = 0;
  }
  for (i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, Wheel((i + j) & 255));
  }
  strip.show();
  j++;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
