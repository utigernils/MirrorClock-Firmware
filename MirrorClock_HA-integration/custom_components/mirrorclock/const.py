from homeassistant.const import Platform

DOMAIN = "mirrorclock"
CONF_HOST = "host"

PLATFORMS: list[Platform] = [
    Platform.LIGHT,
    Platform.SENSOR,
    Platform.SWITCH,
    Platform.NUMBER,
    Platform.BUTTON,
]
