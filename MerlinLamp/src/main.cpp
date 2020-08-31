#define configUSE_TRACE_FACILITY 1

#include <Arduino.h>
#include <arduinonvs.h>
#include <stdio.h>
#include <stdlib.h>

#include <Preferences.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

#include "WiFiManager.h"

#include "DimEnvironment.h"

#include <UdpLogger.h>

#include <ArduinoNvs.h>
#include <Settings.h>
#include <Supervisor.h>

#include "MyWebServer.h"

LedDeviceCreator *_pLedDeviceCreator;
ILedDevice *_pLedDevice;
LedManager *_pLedManager;
ButtonCreator *_pButtonCreator;

Supervisor *_pSupervisor;
Settings *_pSettings;

MyWebServer *pMyWebServer;

IButton *_pButton;

void Callback()
{
  //Serial.println("Callback");
  //pMyWebServer->HandleClient();

  //TrackMemory();

  //delay(10);
}

void RunDim(void *parameter)
{
  Serial.println("RunDim Start");
  StackWatcher::Init();
  //StackWatcher::Enable();

  StackWatcher::Log("Task Start");
  _pSupervisor->ExecuteLoop();
}

void HandleWebClient(void *parameter)
{
  Serial.println("HandleWedClient Start");
  while (true)
  {
    pMyWebServer->HandleClient();
    delay(25);
  }
}

void setup()
{

  Serial.begin(115200);
  Serial.println("setup");
  Serial.flush();
  StackWatcher::Log("setup");
  //StackWatcher::Enable();
  _pSupervisor = new Supervisor();
  _pSettings = new Settings();
  WiFi.setHostname(_pSupervisor->GetNodeName());

  //_pLedPwm = new LedPwmEsp32();
  //_pLedManager = new LedManager(_pLedPwm, 16);

  _pLedDeviceCreator = new LedDeviceCreator();
  //_pLedDevice = new LedRGB(33, 13);

  WiFiManager wifiManager;

  wifiManager.autoConnect("SequenceController", "12345678");
  //wifiManager.startConfigPortal("SequenceController", "12345678");
  Serial.print("after autoconnect: ");
  Serial.println(WiFi.localIP());

  pMyWebServer = new MyWebServer(_pSupervisor, WiFi.localIP());

  _pSettings->Init();
  _pLedManager = new LedManager(_pLedDeviceCreator);
  _pButtonCreator = new ButtonCreator();
  _pSupervisor->Init(_pLedManager, _pSettings, Callback, _pButtonCreator);

  xTaskCreatePinnedToCore(
      RunDim, /* Task function. */
      "Dim",  /* String with name of task. */
      50000,  /* Stack size in bytes. */
      NULL,   /* Parameter passed as input of the task */
      5,      /* Priority of the task. */
      NULL,
      1); /* Task handle. */

  xTaskCreate(
      HandleWebClient,   /* Task function. */
      "HandleWebClient", /* String with name of task. */
      10000,             /* Stack size in bytes. */
      NULL,              /* Parameter passed as input of the task */
      5,                 /* Priority of the task. */
      NULL);             /* Task handle. */

  Serial.println("Setup completed");
}

int iterations = 0;

void TrackMemory()
{
  if (iterations % 100 == 0)
  {
    Serial.print(iterations);
    Serial.print(" = ");
    Serial.println(ESP.getFreeHeap());
  }
  iterations++;
}

void loop()
{
  //pMyWebServer->HandleClient();
  vTaskDelay(portMAX_DELAY);

  //Serial.print("Touch: ");
  //Serial.println(touchRead(32));

  //StackWatcher::Log("loop");
  //_pSupervisor->ExecuteLoop();

  //pMyWebServer->HandleClient();

  //supervisor.Execute();

  //TrackMemory();

  //delay(10);
}

#if fred
#include <Arduino.h>

#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *_pStrip;
int _pixelCount = 33;
int _pixelPin = 13;

void setup()
{
  Serial.begin(115200);

  _pStrip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(_pixelCount, _pixelPin);
  // put your setup code here, to run once:

  _pStrip->Begin();
  for (int i = 0; i < 33; i++)
  {
    _pStrip->SetPixelColor(i, RgbColor(0, 0, 0));
  }
  _pStrip->Show();
}

void loop()
{
  Serial.println("hello");
  _pStrip->SetPixelColor(0, RgbColor(255, 0, 0));
  _pStrip->SetPixelColor(1, RgbColor(0, 255, 0));
  _pStrip->SetPixelColor(2, RgbColor(0, 0, 255));
  _pStrip->Show();

  // put your main code here, to run repeatedly:
}

#endif