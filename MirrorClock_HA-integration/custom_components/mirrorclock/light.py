import logging
from typing import Any

from homeassistant.components.light import (
    LightEntity,
    ColorMode,
    LightEntityFeature,
    ATTR_BRIGHTNESS,
    ATTR_RGB_COLOR,
    ATTR_EFFECT,
)
from homeassistant.config_entries import ConfigEntry
from homeassistant.core import HomeAssistant
from homeassistant.helpers.entity_platform import AddEntitiesCallback
from homeassistant.helpers.update_coordinator import CoordinatorEntity

from .const import DOMAIN
from .coordinator import MirrorClockCoordinator

_LOGGER = logging.getLogger(__name__)

EFFECT_MAP = {
    "None": 0,
    "Crossfade": 1,
    "Typewriter": 2,
}
REVERSE_EFFECT_MAP = {v: k for k, v in EFFECT_MAP.items()}

async def async_setup_entry(
    hass: HomeAssistant,
    config_entry: ConfigEntry,
    async_add_entities: AddEntitiesCallback,
) -> None:
    """Set up MirrorClock light platform."""
    coordinator = hass.data[DOMAIN][config_entry.entry_id]
    async_add_entities([MirrorClockLight(coordinator, config_entry)])

class MirrorClockLight(CoordinatorEntity, LightEntity):
    """Representation of the MirrorClock Light."""

    _attr_has_entity_name = True
    _attr_name = "Light"
    _attr_supported_color_modes = {ColorMode.RGB}
    _attr_color_mode = ColorMode.RGB
    _attr_supported_features = LightEntityFeature.EFFECT

    def __init__(self, coordinator: MirrorClockCoordinator, config_entry: ConfigEntry):
        """Initialize."""
        super().__init__(coordinator)
        self._attr_unique_id = f"{config_entry.entry_id}_light"
        self._attr_device_info = {
            "identifiers": {(DOMAIN, config_entry.entry_id)},
            "name": f"MirrorClock ({coordinator.host})",
            "manufacturer": "Custom Firmware",
            "model": "ESP8266 MirrorClock",
        }

    @property
    def is_on(self) -> bool:
        """Return True if entity is on."""
        return self.coordinator.data.get("state", {}).get("enabled", False)

    @property
    def brightness(self) -> int | None:
        """Return the brightness of this light between 0..255."""
        val = self.coordinator.data.get("state", {}).get("brightness")
        if isinstance(val, int):
            return val
        # If it's "auto", standard Home Assistant Light entities don't have an "auto" state.
        # Fallback to calculated brightness from status
        status = self.coordinator.data.get("status", {}).get("sensor", {})
        return status.get("calculated_brightness", 255)

    @property
    def rgb_color(self) -> tuple[int, int, int] | None:
        """Return the rgb color value."""
        color = self.coordinator.data.get("state", {}).get("color", {})
        return (color.get("r", 255), color.get("g", 255), color.get("b", 255))

    @property
    def effect_list(self) -> list[str]:
        """Return the list of supported effects."""
        return list(EFFECT_MAP.keys())

    @property
    def effect(self) -> str | None:
        """Return the current effect."""
        eff_value = self.coordinator.data.get("config", {}).get("transition_effect", 0)
        return REVERSE_EFFECT_MAP.get(eff_value, "None")

    async def async_turn_on(self, **kwargs: Any) -> None:
        """Turn the light on."""
        state_payload = {"enabled": True}
        config_payload = {}

        if ATTR_BRIGHTNESS in kwargs:
            state_payload["brightness"] = kwargs[ATTR_BRIGHTNESS]
            # Override auto_brightness to false if manually changed
            config_payload["auto_brightness_enabled"] = False

        if ATTR_RGB_COLOR in kwargs:
            rgb = kwargs[ATTR_RGB_COLOR]
            state_payload["color"] = {"r": rgb[0], "g": rgb[1], "b": rgb[2]}

        if ATTR_EFFECT in kwargs:
            effect_name = kwargs[ATTR_EFFECT]
            if effect_name in EFFECT_MAP:
                config_payload["transition_effect"] = EFFECT_MAP[effect_name]

        # Dispatch updates
        await self.coordinator.async_set_state(state_payload)
        
        if config_payload:
            await self.coordinator.async_set_config(config_payload)

    async def async_turn_off(self, **kwargs: Any) -> None:
        """Turn the light off."""
        await self.coordinator.async_set_state({"enabled": False})
