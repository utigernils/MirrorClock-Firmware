from homeassistant.components.button import ButtonEntity
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
    """Set up MirrorClock button platform."""
    coordinator = hass.data[DOMAIN][config_entry.entry_id]
    async_add_entities([MirrorClockRebootButton(coordinator, config_entry)])

class MirrorClockRebootButton(CoordinatorEntity, ButtonEntity):
    """Button entity to trigger device reboot."""

    _attr_has_entity_name = True
    _attr_name = "Reboot Device"
    _attr_icon = "mdi:restart"

    def __init__(self, coordinator: MirrorClockCoordinator, config_entry: ConfigEntry):
        """Initialize."""
        super().__init__(coordinator)
        self._attr_unique_id = f"{config_entry.entry_id}_reboot"
        self._attr_device_info = {
            "identifiers": {(DOMAIN, config_entry.entry_id)},
            "name": f"MirrorClock ({coordinator.host})",
        }

    async def async_press(self) -> None:
        """Handle the button press."""
        await self.coordinator.async_reboot()
