# MirrorClock API Reference

This document provides a comprehensive description of the RESTful API endpoints available in the MirrorClock firmware. The API is intended to be used by frontends or other clients to monitor status, retrieve configuration, and control the clock.

## Base URL
All endpoints are relative to the IP address of the ESP8266 running the firmware.  
Example: `http://<ESP_IP>`

## Cross-Origin Resource Sharing (CORS)
The API supports CORS and will respond to `OPTIONS` preflight requests with the appropriate headers (`Access-Control-Allow-Origin: *`, `Access-Control-Allow-Methods: GET, POST, OPTIONS`, `Access-Control-Allow-Headers: Content-Type`).

## Response Format
Every endpoint returns a unified JSON response structure:
```json
{
  "success": true,               // boolean indicating if the request was successful
  "timestamp": 1618317042,       // auto-generated epoch timestamp from the device
  "message": "Status retrieved", // optional informational message
  "data": { ... }                // optional payload depending on the endpoint
}
```

---

## Endpoints

### 1. Get System Status
Retrieves current device metrics, WiFi status, time, and sensor readings.

**Endpoint:** `GET /api/status`

**Response Example:**
```json
{
  "success": true,
  "timestamp": 1672535000,
  "message": "Status retrieved",
  "data": {
    "system": {
      "uptime": 120543,
      "free_heap": 42104,
      "chip_id": 1234567,
      "cpu_freq": 80
    },
    "wifi": {
      "connected": true,
      "ssid": "MyWiFiNetwork",
      "ip": "192.168.1.100",
      "rssi": -65
    },
    "time": {
      "epoch": 1672535000,
      "formatted": "12:34"
    },
    "sensor": {
      "lux": 150.5,
      "calculated_brightness": 128
    }
  }
}
```

---

### 2. Get Current State
Retrieves the current running state of the LEDs, including brightness and color settings.

**Endpoint:** `GET /api/state`

**Response Example:**
```json
{
  "success": true,
  "timestamp": 1672535000,
  "message": "State retrieved",
  "data": {
    "enabled": true,
    "brightness": 255,
    "auto_brightness": false,
    "color": {
      "r": 255,
      "g": 100,
      "b": 50,
      "hex": "#FF6432"
    }
  }
}
```

---

### 3. Update Current State
Updates the running state of the LEDs (brightness, color, and power).

**Endpoint:** `POST /api/state`  
**Content-Type:** `application/json`

**Request Body Details (All fields are optional):**
- `enabled` (boolean): Turn the LEDs on or off.
- `brightness` (integer or string): Can be a numeric value (0-255) to set manual brightness, or the string `"auto"` to enable auto-brightness based on the light sensor.
- `color` (object): Defines the LED color. Can be expressed as `hex` or RGB (`r`, `g`, `b`). 

**Request Example:**
```json
{
  "enabled": true,
  "brightness": "auto",
  "color": {
    "hex": "#00FF00"
  }
}
```

*Alternatively, sending RGB:*
```json
{
  "color": {
    "r": 0,
    "g": 255,
    "b": 0
  }
}
```

**Response:** Returns the updated state object in the `data` field (identical to `GET /api/state`).

---

### 4. Get Configuration
Retrieves hardware and software configuration values saved on the device.

**Endpoint:** `GET /api/config`

**Response Example:**
```json
{
  "success": true,
  "timestamp": 1672535000,
  "message": "Config retrieved",
  "data": {
    "led_pin": 4,
    "led_count": 60,
    "timezone": "CET-1CEST,M3.5.0,M10.5.0/3",
    "ntp_server": "pool.ntp.org",
    "sensor_sda": 4,
    "sensor_scl": 5,
    "sensor_max_lux": 1000,
    "sensor_min_lux": 0,
    "sensor_cal_max": 255,
    "sensor_min_brightness": 10,
    "sensor_max_brightness": 255,
    "update_interval": 1000,
    "auto_brightness_enabled": true,
    "web_server_port": 80,
    "mqtt_enabled": false,
    "mqtt_broker": "192.168.1.10",
    "mqtt_port": 1883,
    "mqtt_user": "mqtt_user",
    "mqtt_password": "", 
    "mqtt_topic_prefix": "mirrorclock/"
  }
}
```
*Note:* For security reasons, the `mqtt_password` is never sent in the response.

---

### 5. Update Configuration
Updates the configuration values. Settings are persisted to memory automatically.  
*(Note: Some changes like pin assignments, MQTT broker, and web server port will not take effect until the device is rebooted.)*

**Endpoint:** `POST /api/config`  
**Content-Type:** `application/json`

**Request Body Details (All fields are optional):**
- Any field retrieved from `GET /api/config` can be sent to be updated. Pass only the keys you want to modify. If `mqtt_password` is provided and is not empty, it will be updated.

**Request Example:**
```json
{
  "timezone": "EST5EDT",
  "ntp_server": "time.google.com",
  "sensor_max_brightness": 200
}
```

**Response Example:**
```json
{
  "success": true,
  "timestamp": 1672535500,
  "message": "Config updated. Please restart device for some changes to take effect."
}
```

---

### 6. Reboot System
Triggers a programmatic reboot of the ESP. Let the client wait a few seconds before attempting to reconnect.

**Endpoint:** `POST /api/system/reboot`

**Response Example:**
```json
{
  "success": true,
  "timestamp": 1672535600,
  "message": "Rebooting..."
}
```