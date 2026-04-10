import logging

from homeassistant.components.sensor import SensorEntity, SensorDeviceClass, SensorStateClass
from homeassistant.config_entries import ConfigEntry
from homeassistant.core import HomeAssistant
from homeassistant.helpers.entity_platform import AddEntitiesCallback
from homeassistant.helpers.update_coordinator import CoordinatorEntity
from homeassistant.const import LIGHT_LUX, SIGNAL_STRENGTH_DECIBELS_MILLIWATT, UnitOfTime

from .const import DOMAIN
from .coordinator import MirrorClockCoordinator

_LOGGER = logging.getLogger(__name__)

async def async_setup_entry(
    hass: HomeAssistant,
    config_entry: ConfigEntry,
    async_add_entities: AddEntitiesCallback,
) -> None:
    """Set up MirrorClock sensor platform."""
    coordinator = hass.data[DOMAIN][config_entry.entry_id]

    async_add_entities([
        MirrorClockLuxSensor(coordinator, config_entry),
        MirrorClockCalcBrightnessSensor(coordinator, config_entry),
        MirrorClockWifiSensor(coordinator, config_entry),
        MirrorClockUptimeSensor(coordinator, config_entry),
    ])

class MirrorClockBaseSensor(CoordinatorEntity, SensorEntity):
    """Base Sensor for MirrorClock."""
    
    _attr_has_entity_name = True

    def __init__(self, coordinator: MirrorClockCoordinator, config_entry: ConfigEntry):
        super().__init__(coordinator)
        self.config_entry = config_entry
        self._attr_device_info = {
            "identifiers": {(DOMAIN, config_entry.entry_id)},
            "name": f"MirrorClock ({coordinator.host})",
        }

class MirrorClockLuxSensor(MirrorClockBaseSensor):
    _attr_name = "Illuminance"
    _attr_device_class = SensorDeviceClass.ILLUMINANCE
    _attr_state_class = SensorStateClass.MEASUREMENT
    _attr_native_unit_of_measurement = LIGHT_LUX

    def __init__(self, coordinator, config_entry):
        super().__init__(coordinator, config_entry)
        self._attr_unique_id = f"{config_entry.entry_id}_lux"

    @property
    def native_value(self):
        return self.coordinator.data.get("status", {}).get("sensor", {}).get("lux")

class MirrorClockCalcBrightnessSensor(MirrorClockBaseSensor):
    _attr_name = "Calculated Brightness"
    _attr_state_class = SensorStateClass.MEASUREMENT

    def __init__(self, coordinator, config_entry):
        super().__init__(coordinator, config_entry)
        self._attr_unique_id = f"{config_entry.entry_id}_calc_bright"

    @property
    def native_value(self):
        return self.coordinator.data.get("status", {}).get("sensor", {}).get("calculated_brightness")

class MirrorClockWifiSensor(MirrorClockBaseSensor):
    _attr_name = "WiFi Signal"
    _attr_device_class = SensorDeviceClass.SIGNAL_STRENGTH
    _attr_state_class = SensorStateClass.MEASUREMENT
    _attr_native_unit_of_measurement = SIGNAL_STRENGTH_DECIBELS_MILLIWATT

    def __init__(self, coordinator, config_entry):
        super().__init__(coordinator, config_entry)
        self._attr_unique_id = f"{config_entry.entry_id}_wifi"

    @property
    def native_value(self):
        return self.coordinator.data.get("status", {}).get("wifi", {}).get("rssi")

class MirrorClockUptimeSensor(MirrorClockBaseSensor):
    _attr_name = "Uptime"
    _attr_state_class = SensorStateClass.TOTAL_INCREASING
    _attr_native_unit_of_measurement = UnitOfTime.SECONDS
    _attr_icon = "mdi:timer-outline"

    def __init__(self, coordinator, config_entry):
        super().__init__(coordinator, config_entry)
        self._attr_unique_id = f"{config_entry.entry_id}_uptime"

    @property
    def native_value(self):
        return self.coordinator.data.get("status", {}).get("system", {}).get("uptime")
