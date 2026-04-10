from typing import Any
from homeassistant.components.switch import SwitchEntity
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
    """Set up MirrorClock switch platform."""
    coordinator = hass.data[DOMAIN][config_entry.entry_id]
    async_add_entities([MirrorClockAutoBrightnessSwitch(coordinator, config_entry)])

class MirrorClockAutoBrightnessSwitch(CoordinatorEntity, SwitchEntity):
    """Switch for Auto Brightness configuration."""

    _attr_has_entity_name = True
    _attr_name = "Auto Brightness"
    _attr_icon = "mdi:brightness-auto"

    def __init__(self, coordinator: MirrorClockCoordinator, config_entry: ConfigEntry):
        """Initialize."""
        super().__init__(coordinator)
        self._attr_unique_id = f"{config_entry.entry_id}_autobrightness"
        self._attr_device_info = {
            "identifiers": {(DOMAIN, config_entry.entry_id)},
            "name": f"MirrorClock ({coordinator.host})",
        }

    @property
    def is_on(self) -> bool:
        """Return True if auto brightness is enabled."""
        val = self.coordinator.data.get("config", {}).get("auto_brightness_enabled", False)
        return bool(val)

    async def async_turn_on(self, **kwargs: Any) -> None:
        """Turn the entity on."""
        await self.coordinator.async_set_config({"auto_brightness_enabled": True})
        # Note: Depending on backend, we might also want to set /api/state {"brightness": "auto"}
        await self.coordinator.async_set_state({"brightness": "auto"})

    async def async_turn_off(self, **kwargs: Any) -> None:
        """Turn the entity off."""
        await self.coordinator.async_set_config({"auto_brightness_enabled": False})
