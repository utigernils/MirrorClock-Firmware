import logging
import voluptuous as vol
from homeassistant import config_entries
from homeassistant.const import CONF_HOST
import aiohttp

from .const import DOMAIN

_LOGGER = logging.getLogger(__name__)

class MirrorClockConfigFlow(config_entries.ConfigFlow, domain=DOMAIN):
    """Handle a config flow for MirrorClock."""

    VERSION = 1

    async def async_step_user(self, user_input=None):
        """Handle the initial step."""
        errors = {}

        if user_input is not None:
            # Simple connection test
            host = user_input[CONF_HOST]
            try:
                async with aiohttp.ClientSession() as session:
                    async with session.get(f"http://{host}/api/status", timeout=5) as response:
                        if response.status == 200:
                            return self.async_create_entry(title=f"MirrorClock ({host})", data=user_input)
                        else:
                            errors["base"] = "cannot_connect"
            except Exception:
                errors["base"] = "cannot_connect"

        return self.async_show_form(
            step_id="user",
            data_schema=vol.Schema(
                {
                    vol.Required(CONF_HOST, default=""): str,
                }
            ),
            errors=errors,
        )
