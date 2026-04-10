from homeassistant.components.number import NumberEntity
from homeassistant.config_entries import ConfigEntry
from homeassistant.core import HomeAssistant
from homeassistant.helpers.entity_platform import AddEntitiesCallback
from homeassistant.helpers.update_coordinator import CoordinatorEntity

from .const import DOMAIN
from .coordinator import MirrorClockCoordinator

async def async_setup_entry(
    hass: HomeAssistant,
    config_entry: ConfigEntry,
    async_add_entities: AddEntitiesCallback,
) -> None:
    """Set up MirrorClock number platform."""
    coordinator = hass.data[DOMAIN][config_entry.entry_id]
    async_add_entities([MirrorClockTransitionDurationNumber(coordinator, config_entry)])

class MirrorClockTransitionDurationNumber(CoordinatorEntity, NumberEntity):
    """Number entity for Transition Duration configuration."""

    _attr_has_entity_name = True
    _attr_name = "Transition Duration"
    _attr_icon = "mdi:timer-outline"
    _attr_native_min_value = 0
    _attr_native_max_value = 10000
    _attr_native_step = 100
    
    def __init__(self, coordinator: MirrorClockCoordinator, config_entry: ConfigEntry):
        """Initialize."""
        super().__init__(coordinator)
        self._attr_unique_id = f"{config_entry.entry_id}_trans_duration"
        self._attr_device_info = {
            "identifiers": {(DOMAIN, config_entry.entry_id)},
            "name": f"MirrorClock ({coordinator.host})",
        }

    @property
    def native_value(self) -> float | None:
        """Return current transition duration."""
        val = self.coordinator.data.get("config", {}).get("transition_duration")
        return float(val) if val is not None else None

    async def async_set_native_value(self, value: float) -> None:
        """Update the transition duration."""
        await self.coordinator.async_set_config({"transition_duration": int(value)})
