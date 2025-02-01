/*
 MQTTBuddy.h - A simple client for MQTT.
  Mou'men Marwan
*/

#ifndef MqttBuddy_h
#define MqttBuddy_h

#include <Arduino.h>
#include "IPAddress.h"
#include "Client.h"
#include "Stream.h"

// Supported MQTT versions
#define MQTT_VERSION_3_1      3
#define MQTT_VERSION_3_1_1    4

// MQTT_VERSION : Pick the version
#ifndef MQTT_VERSION
#define MQTT_VERSION MQTT_VERSION_3_1_1 // default version
#endif

// MQTT_MAX_PACKET_SIZE : Maximum packet size.
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 256
#endif

// MQTT_KEEPALIVE : keepAlive interval in Seconds.
#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE 15
#endif

// MQTT_SOCKET_TIMEOUT: socket timeout interval in Seconds.
#ifndef MQTT_SOCKET_TIMEOUT
#define MQTT_SOCKET_TIMEOUT 15
#endif

// Possible values for the client state
#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5


#define MQTT_CONNECT     1 << 4  // Client request to connect to Server
#define MQTT_CONNACK     2 << 4  // Connect Acknowledgment
#define MQTT_PUBLISH     3 << 4  // Publish message
#define MQTT_PUBACK      4 << 4  // Publish Acknowledgment
#define MQTT_PUBREC      5 << 4  // Publish Received (assured delivery part 1)
#define MQTT_PUBREL      6 << 4  // Publish Release (assured delivery part 2)
#define MQTT_PUBCOMP     7 << 4  // Publish Complete (assured delivery part 3)
#define MQTT_SUBSCRIBE   8 << 4  // Client Subscribe request
#define MQTT_SUBACK      9 << 4  // Subscribe Acknowledgment
#define MQTT_UNSUBSCRIBE 10 << 4 // Client Unsubscribe request
#define MQTT_UNSUBACK    11 << 4 // Unsubscribe Acknowledgment
#define MQTT_PINGREQ     12 << 4 // PING Request
#define MQTT_PINGRESP    13 << 4 // PING Response
#define MQTT_DISCONNECT  14 << 4 // Client is Disconnecting
#define MQTTReserved    15 << 4 // Reserved

#define MQTT_QOS0        (0 << 1)
#define MQTT_QOS1        (1 << 1)
#define MQTT_QOS2        (2 << 1)

// Maximum size of fixed header and variable length size header
#define MQTT_MAX_HEADER_SIZE 5

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif


#define CHECK_STRING_LENGTH(l,s) if (l+2+strnlen(s, this->bufferSize) > this->bufferSize) {_client->stop();return false;}


class MQTTBuddy : public Print {
private:
   Client* _client;
   uint8_t* buffer;
   uint16_t bufferSize;
   uint16_t keepAlive;
   uint16_t socketTimeout;
   uint16_t nextMsgId;
   unsigned long lastOutActivity;
   unsigned long lastInActivity;
   bool pingOutstanding;
   MQTT_CALLBACK_SIGNATURE;
   uint32_t readPacket(uint8_t*);
   boolean readByte(uint8_t * result);
   boolean readByte(uint8_t * result, uint16_t * index);
   boolean write(uint8_t header, uint8_t* buf, uint16_t length);
   uint16_t writeString(const char* string, uint8_t* buf, uint16_t pos);
   // Build up the header ready to send
   // Returns the size of the header
   // Note: the header is built at the end of the first MQTT_MAX_HEADER_SIZE bytes, so will start
   //       (MQTT_MAX_HEADER_SIZE - <returned size>) bytes into the buffer
   size_t buildHeader(uint8_t header, uint8_t* buf, uint16_t length);
   IPAddress ip;
   const char* domain;
   uint16_t port;
   Stream* stream;
   int _state = MQTT_DISCONNECTED;
public:
   MQTTBuddy();
   MQTTBuddy(Client& client);
   MQTTBuddy(IPAddress, uint16_t, Client& client);
   MQTTBuddy(IPAddress, uint16_t, Client& client, Stream&);
   MQTTBuddy(IPAddress, uint16_t, MQTT_CALLBACK_SIGNATURE,Client& client);
   MQTTBuddy(IPAddress, uint16_t, MQTT_CALLBACK_SIGNATURE,Client& client, Stream&);
   MQTTBuddy(uint8_t *, uint16_t, Client& client);
   MQTTBuddy(uint8_t *, uint16_t, Client& client, Stream&);
   MQTTBuddy(uint8_t *, uint16_t, MQTT_CALLBACK_SIGNATURE,Client& client);
   MQTTBuddy(uint8_t *, uint16_t, MQTT_CALLBACK_SIGNATURE,Client& client, Stream&);
   MQTTBuddy(const char*, uint16_t, Client& client);
   MQTTBuddy(const char*, uint16_t, Client& client, Stream&);
   MQTTBuddy(const char*, uint16_t, MQTT_CALLBACK_SIGNATURE,Client& client);
   MQTTBuddy(const char*, uint16_t, MQTT_CALLBACK_SIGNATURE,Client& client, Stream&);

   ~MQTTBuddy();

   MQTTBuddy& setServer(IPAddress ip, uint16_t port);
   MQTTBuddy& setServer(uint8_t * ip, uint16_t port);
   MQTTBuddy& setServer(const char * domain, uint16_t port);
   MQTTBuddy& setCallback(MQTT_CALLBACK_SIGNATURE);
   MQTTBuddy& setClient(Client& client);
   MQTTBuddy& setStream(Stream& stream);
   MQTTBuddy& setKeepAlive(uint16_t keepAlive);
   MQTTBuddy& setSocketTimeout(uint16_t timeout);

   boolean setBufferSize(uint16_t size);
   uint16_t getBufferSize();

   boolean connect(const char* id);
   boolean connect(const char* id, const char* user, const char* pass);
   boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
   boolean connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
   boolean connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage, boolean cleanSession);
   void disconnect();
   boolean publish(const char* topic, const char* payload);
   boolean publish(const char* topic, const char* payload, boolean retained);
   boolean publish(const char* topic, const uint8_t * payload, unsigned int plength);
   boolean publish(const char* topic, const uint8_t * payload, unsigned int plength, boolean retained);
   boolean publish_P(const char* topic, const char* payload, boolean retained);
   boolean publish_P(const char* topic, const uint8_t * payload, unsigned int plength, boolean retained);
   // Start to publish a message.
   // This API:
   //   beginPublish(...)
   //   one or more calls to write(...)
   //   endPublish()
   // Allows for arbitrarily large payloads to be sent without them having to be copied into
   // a new buffer and held in memory at one time
   // Returns 1 if the message was started successfully, 0 if there was an error
   boolean beginPublish(const char* topic, unsigned int plength, boolean retained);
   // Finish off this publish message (started with beginPublish)
   // Returns 1 if the packet was sent successfully, 0 if there was an error
   int endPublish();
   // Write a single byte of payload (only to be used with beginPublish/endPublish)
   virtual size_t write(uint8_t);
   // Write size bytes from buffer into the payload (only to be used with beginPublish/endPublish)
   // Returns the number of bytes written
   virtual size_t write(const uint8_t *buffer, size_t size);
   boolean subscribe(const char* topic);
   boolean subscribe(const char* topic, uint8_t qos);
   boolean unsubscribe(const char* topic);
   boolean loop();
   boolean connected();
   int state();

};


#endif // MqttBuddy_h