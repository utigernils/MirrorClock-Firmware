import { useState, useEffect } from 'react';
import { ChevronDown, ChevronUp, RotateCcw } from 'lucide-react';
import type { DeviceConfig } from '../types/api';
import { api } from '../lib/api';

interface ConfigPanelProps {
  config: DeviceConfig | null;
  onConfigChange: (config: DeviceConfig) => void;
  onReboot: () => void;
}

interface FieldDef {
  key: keyof DeviceConfig;
  label: string;
  type: 'text' | 'number' | 'boolean' | 'password' | 'select';
  options?: { label: string; value: number }[];
}

const FIELDS: FieldDef[] = [
  { 
    key: 'transition_effect', 
    label: 'Transition Effect', 
    type: 'select',
    options: [
      { label: 'None', value: 0 },
      { label: 'Crossfade', value: 1 },
      { label: 'Typewriter', value: 2 },
    ]
  },
  { key: 'transition_duration', label: 'Transition Duration (ms)', type: 'number' },
  { key: 'timezone', label: 'Timezone', type: 'text' },
  { key: 'ntp_server', label: 'NTP Server', type: 'text' },
  { key: 'update_interval', label: 'Update Interval (ms)', type: 'number' },
  { key: 'led_count', label: 'LED Count', type: 'number' },
  { key: 'led_pin', label: 'LED Pin', type: 'number' },
  { key: 'sensor_max_lux', label: 'Sensor Max Lux', type: 'number' },
  { key: 'sensor_min_lux', label: 'Sensor Min Lux', type: 'number' },
  { key: 'sensor_min_brightness', label: 'Min Brightness', type: 'number' },
  { key: 'sensor_max_brightness', label: 'Max Brightness', type: 'number' },
  { key: 'auto_brightness_enabled', label: 'Auto Brightness', type: 'boolean' },
  { key: 'web_server_port', label: 'Web Server Port', type: 'number' },
  { key: 'mqtt_enabled', label: 'MQTT Enabled', type: 'boolean' },
  { key: 'mqtt_broker', label: 'MQTT Broker', type: 'text' },
  { key: 'mqtt_port', label: 'MQTT Port', type: 'number' },
  { key: 'mqtt_user', label: 'MQTT User', type: 'text' },
  { key: 'mqtt_password', label: 'MQTT Password', type: 'password' },
  { key: 'mqtt_topic_prefix', label: 'MQTT Topic Prefix', type: 'text' },
];

export function ConfigPanel({ config, onConfigChange, onReboot }: ConfigPanelProps) {
  const [open, setOpen] = useState(false);
  const [form, setForm] = useState<Partial<DeviceConfig>>({});
  const [busy, setBusy] = useState(false);
  const [rebooting, setRebooting] = useState(false);
  const [saved, setSaved] = useState(false);
  const [confirmReboot, setConfirmReboot] = useState(false);

  useEffect(() => {
    if (config) setForm(config);
  }, [config]);

  function handleChange(key: keyof DeviceConfig, value: string | boolean | number) {
    setForm((prev) => ({ ...prev, [key]: value }));
  }

  async function handleSave() {
    setBusy(true);
    setSaved(false);
    try {
      const res = await api.updateConfig(form as Partial<DeviceConfig>);
      if (res.success) {
        setSaved(true);
        if (config) onConfigChange({ ...config, ...form } as DeviceConfig);
        setTimeout(() => setSaved(false), 3000);
      }
    } finally {
      setBusy(false);
    }
  }

  async function handleReboot() {
    if (!confirmReboot) {
      setConfirmReboot(true);
      setTimeout(() => setConfirmReboot(false), 4000);
      return;
    }
    setRebooting(true);
    try {
      await api.reboot();
      onReboot();
    } finally {
      setRebooting(false);
      setConfirmReboot(false);
    }
  }

  return (
    <div className="space-y-4">
      <div className="flex items-center justify-between">
        <p className="text-[10px] font-medium tracking-[0.25em] text-zinc-600 uppercase">Configuration</p>
        <div className="flex gap-2">
          <button
            onClick={handleReboot}
            disabled={rebooting || busy}
            className={`flex items-center gap-2 px-4 py-2 text-xs tracking-widest uppercase border transition-all duration-200 disabled:opacity-40 ${
              confirmReboot
                ? 'border-red-500 text-red-400 bg-red-500/10'
                : 'border-zinc-700 text-zinc-500 hover:border-red-500 hover:text-red-400'
            }`}
          >
            <RotateCcw size={11} className={rebooting ? 'animate-spin' : ''} />
            {confirmReboot ? 'Confirm Reboot' : 'Reboot'}
          </button>
        </div>
      </div>

      <div className="bg-zinc-900 border border-zinc-800">
        <button
          onClick={() => setOpen(!open)}
          className="w-full flex items-center justify-between px-4 py-3 text-left hover:bg-zinc-800/50 transition-colors"
        >
          <span className="text-xs tracking-widest uppercase text-zinc-400">
            {open ? 'Hide Settings' : 'Show Settings'}
          </span>
          {open ? <ChevronUp size={14} className="text-zinc-600" /> : <ChevronDown size={14} className="text-zinc-600" />}
        </button>

        {open && (
          <div className="border-t border-zinc-800 p-4">
            <div className="grid grid-cols-1 gap-3 sm:grid-cols-2">
              {FIELDS.map((field) => (
                <ConfigField
                  key={field.key}
                  field={field}
                  value={form[field.key]}
                  onChange={handleChange}
                />
              ))}
            </div>
            <div className="mt-4 flex items-center justify-between border-t border-zinc-800 pt-4">
              <p className="text-xs text-zinc-600">Some changes require reboot to take effect.</p>
              <button
                onClick={handleSave}
                disabled={busy}
                className={`px-6 py-2 text-xs tracking-widest uppercase border transition-all duration-200 disabled:opacity-40 ${
                  saved
                    ? 'border-emerald-500 text-emerald-400 bg-emerald-500/10'
                    : 'border-white text-white hover:bg-white hover:text-black'
                }`}
              >
                {saved ? 'Saved' : busy ? 'Saving...' : 'Save'}
              </button>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}

interface ConfigFieldProps {
  field: FieldDef;
  value: string | number | boolean | undefined;
  onChange: (key: keyof DeviceConfig, value: string | boolean | number) => void;
}

function ConfigField({ field, value, onChange }: ConfigFieldProps) {
  if (field.type === 'boolean') {
    return (
      <div className="flex items-center justify-between py-2 border-b border-zinc-800">
        <label className="text-[10px] tracking-widest uppercase text-zinc-500">{field.label}</label>
        <button
          onClick={() => onChange(field.key, !value)}
          className={`w-10 h-5 relative border transition-all duration-200 ${
            value ? 'border-white bg-white' : 'border-zinc-700 bg-transparent'
          }`}
        >
          <span
            className={`absolute top-0.5 h-3 w-3 transition-all duration-200 ${
              value ? 'left-6 bg-black' : 'left-0.5 bg-zinc-600'
            }`}
          />
        </button>
      </div>
    );
  }

  if (field.type === 'select' && field.options) {
    return (
      <div className="py-1">
        <label className="text-[10px] tracking-widest uppercase text-zinc-600 block mb-1">{field.label}</label>
        <select
          value={value as number}
          onChange={(e) => onChange(field.key, Number(e.target.value))}
          className="w-full bg-zinc-800 border border-zinc-700 text-white text-xs font-mono px-3 py-2 focus:outline-none focus:border-zinc-500"
        >
          {field.options.map((opt) => (
            <option key={opt.value} value={opt.value}>
              {opt.label}
            </option>
          ))}
        </select>
      </div>
    );
  }

  return (
    <div className="py-1">
      <label className="text-[10px] tracking-widest uppercase text-zinc-600 block mb-1">{field.label}</label>
      <input
        type={field.type === 'password' ? 'password' : field.type === 'number' ? 'number' : 'text'}
        value={value as string | number ?? ''}
        onChange={(e) =>
          onChange(
            field.key,
            field.type === 'number' ? Number(e.target.value) : e.target.value
          )
        }
        className="w-full bg-zinc-800 border border-zinc-700 text-white text-xs font-mono px-3 py-2 focus:outline-none focus:border-zinc-500 placeholder-zinc-600"
      />
    </div>
  );
}
