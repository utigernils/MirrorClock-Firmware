export interface SystemStatus {
  uptime: number;
  free_heap: number;
  chip_id: number;
  cpu_freq: number;
}

export interface WifiStatus {
  connected: boolean;
  ssid: string;
  ip: string;
  rssi: number;
}

export interface TimeStatus {
  epoch: number;
  formatted: string;
}

export interface SensorStatus {
  lux: number;
  calculated_brightness: number;
}

export interface StatusData {
  system: SystemStatus;
  wifi: WifiStatus;
  time: TimeStatus;
  sensor: SensorStatus;
}

export interface ColorState {
  r: number;
  g: number;
  b: number;
  hex: string;
}

export interface DeviceState {
  enabled: boolean;
  brightness: number;
  auto_brightness: boolean;
  color: ColorState;
}

export interface DeviceConfig {
  led_pin: number;
  led_count: number;
  timezone: string;
  ntp_server: string;
  sensor_sda: number;
  sensor_scl: number;
  sensor_max_lux: number;
  sensor_min_lux: number;
  sensor_cal_max: number;
  sensor_min_brightness: number;
  sensor_max_brightness: number;
  update_interval: number;
  auto_brightness_enabled: boolean;
  web_server_port: number;
  mqtt_enabled: boolean;
  mqtt_broker: string;
  mqtt_port: number;
  mqtt_user: string;
  mqtt_password: string;
  mqtt_topic_prefix: string;
}

export interface ApiResponse<T = undefined> {
  success: boolean;
  timestamp: number;
  message?: string;
  data?: T;
}
