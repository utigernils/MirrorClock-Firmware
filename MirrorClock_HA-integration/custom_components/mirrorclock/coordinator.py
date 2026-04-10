import logging
from datetime import timedelta
import aiohttp

from homeassistant.core import HomeAssistant
from homeassistant.helpers.update_coordinator import DataUpdateCoordinator, UpdateFailed

from .const import DOMAIN

_LOGGER = logging.getLogger(__name__)

class MirrorClockCoordinator(DataUpdateCoordinator):
    """Class to manage fetching MirrorClock data."""

    def __init__(self, hass: HomeAssistant, host: str):
        """Initialize."""
        self.host = host
        self.base_url = f"http://{host}"
        
        super().__init__(
            hass,
            _LOGGER,
            name=DOMAIN,
            update_interval=timedelta(seconds=5),
        )

    async def _async_update_data(self):
        """Fetch data from API endpoints."""
        try:
            async with aiohttp.ClientSession() as session:
                # Fetch State
                async with session.get(f"{self.base_url}/api/state", timeout=5) as resp_state:
                    state_json = await resp_state.json()
                    
                # Fetch Status
                async with session.get(f"{self.base_url}/api/status", timeout=5) as resp_status:
                    status_json = await resp_status.json()

                # Fetch Config
                async with session.get(f"{self.base_url}/api/config", timeout=5) as resp_config:
                    config_json = await resp_config.json()

                return {
                    "state": state_json.get("data", {}),
                    "status": status_json.get("data", {}),
                    "config": config_json.get("data", {}),
                }

        except Exception as err:
            raise UpdateFailed(f"Error communicating with API: {err}")

    async def async_set_state(self, payload: dict):
        """Update device state."""
        try:
            async with aiohttp.ClientSession() as session:
                async with session.post(
                    f"{self.base_url}/api/state", json=payload, timeout=5
                ) as resp:
                    if resp.status == 200:
                        await self.async_request_refresh()
        except Exception as err:
            _LOGGER.error(f"Failed to set state: {err}")

    async def async_set_config(self, payload: dict):
        """Update device config."""
        try:
            async with aiohttp.ClientSession() as session:
                async with session.post(
                    f"{self.base_url}/api/config", json=payload, timeout=5
                ) as resp:
                    if resp.status == 200:
                        await self.async_request_refresh()
        except Exception as err:
            _LOGGER.error(f"Failed to set config: {err}")

    async def async_reboot(self):
        """Reboot the device."""
        try:
            async with aiohttp.ClientSession() as session:
                async with session.post(f"{self.base_url}/api/system/reboot", timeout=5) as resp:
                    pass
        except Exception as err:
            _LOGGER.error(f"Failed to trigger reboot: {err}")
