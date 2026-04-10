import { useState, useEffect } from 'react';
import { Power, Zap } from 'lucide-react';
import type { DeviceState } from '../types/api';
import { api } from '../lib/api';

interface LedControlProps {
  state: DeviceState | null;
  onStateChange: (state: DeviceState) => void;
}

function hexToRgb(hex: string) {
  const r = parseInt(hex.slice(1, 3), 16);
  const g = parseInt(hex.slice(3, 5), 16);
  const b = parseInt(hex.slice(5, 7), 16);
  return { r, g, b };
}

export function LedControl({ state, onStateChange }: LedControlProps) {
  const [color, setColor] = useState(state?.color?.hex ?? '#ffffff');
  const [brightness, setBrightness] = useState(state?.brightness ?? 128);
  const [autoB, setAutoB] = useState(state?.auto_brightness ?? false);
  const [busy, setBusy] = useState(false);

  useEffect(() => {
    if (state) {
      setColor(state.color?.hex ?? '#ffffff');
      setBrightness(state.brightness ?? 128);
      setAutoB(state.auto_brightness ?? false);
    }
  }, [state]);

  async function send(patch: Parameters<typeof api.updateState>[0]) {
    setBusy(true);
    try {
      const res = await api.updateState(patch);
      if (res.success && res.data) onStateChange(res.data);
    } finally {
      setBusy(false);
    }
  }

  async function togglePower() {
    await send({ enabled: !state?.enabled });
  }

  async function applyColor() {
    const rgb = hexToRgb(color);
    await send({ color: { r: rgb.r, g: rgb.g, b: rgb.b } });
  }

  async function applyBrightness() {
    if (autoB) {
      await send({ brightness: 'auto' });
    } else {
      await send({ brightness });
    }
  }

  async function toggleAuto() {
    const next = !autoB;
    setAutoB(next);
    await send({ brightness: next ? 'auto' : brightness });
  }

  const enabled = state?.enabled ?? false;

  return (
    <div className="space-y-4">
      <p className="text-[10px] font-medium tracking-[0.25em] text-zinc-600 uppercase">LED Control</p>

      <div className="bg-zinc-900 border border-zinc-800 p-6">
        <div className="flex items-center justify-between mb-6">
          <div>
            <p className="text-[10px] tracking-[0.2em] uppercase text-zinc-500 mb-1">Power</p>
            <p className="text-2xl font-bold tracking-tight text-white">{enabled ? 'On' : 'Off'}</p>
          </div>
          <button
            onClick={togglePower}
            disabled={busy}
            className={`w-14 h-14 border flex items-center justify-center transition-all duration-200 ${
              enabled
                ? 'border-white bg-white text-black hover:bg-zinc-200'
                : 'border-zinc-700 text-zinc-600 hover:border-zinc-500 hover:text-zinc-400'
            } disabled:opacity-40`}
          >
            <Power size={20} />
          </button>
        </div>

        <div
          className="h-1 mb-6"
          style={{ backgroundColor: enabled ? color : '#27272a' }}
        />

        <div className="space-y-5">
          <div>
            <div className="flex items-center justify-between mb-2">
              <p className="text-[10px] tracking-[0.2em] uppercase text-zinc-500">Color</p>
              <span className="text-xs font-mono text-zinc-400">{color.toUpperCase()}</span>
            </div>
            <div className="flex gap-3 items-center">
              <div className="relative flex-shrink-0">
                <input
                  type="color"
                  value={color}
                  onChange={(e) => setColor(e.target.value)}
                  className="w-10 h-10 cursor-pointer border-0 p-0 bg-transparent"
                  style={{ outline: 'none' }}
                />
              </div>
              <button
                onClick={applyColor}
                disabled={busy || !enabled}
                className="flex-1 py-2 border border-zinc-700 text-zinc-400 text-xs tracking-widest uppercase hover:border-white hover:text-white transition-all duration-200 disabled:opacity-40"
              >
                Apply Color
              </button>
            </div>
          </div>

          <div>
            <div className="flex items-center justify-between mb-3">
              <p className="text-[10px] tracking-[0.2em] uppercase text-zinc-500">Brightness</p>
              <div className="flex items-center gap-3">
                <button
                  onClick={toggleAuto}
                  disabled={busy}
                  className={`flex items-center gap-1.5 px-2.5 py-1 text-[10px] tracking-widest uppercase border transition-all duration-200 ${
                    autoB
                      ? 'border-white text-white bg-white/10'
                      : 'border-zinc-700 text-zinc-500 hover:border-zinc-500'
                  } disabled:opacity-40`}
                >
                  <Zap size={10} />
                  Auto
                </button>
                <span className="text-xs font-mono text-zinc-400 w-8 text-right">{autoB ? 'A' : brightness}</span>
              </div>
            </div>
            <div className="relative">
              <input
                type="range"
                min={0}
                max={255}
                value={brightness}
                onChange={(e) => setBrightness(Number(e.target.value))}
                onMouseUp={applyBrightness}
                onTouchEnd={applyBrightness}
                disabled={autoB || busy || !enabled}
                className="w-full h-1 appearance-none bg-zinc-800 outline-none cursor-pointer disabled:opacity-40 [&::-webkit-slider-thumb]:appearance-none [&::-webkit-slider-thumb]:w-4 [&::-webkit-slider-thumb]:h-4 [&::-webkit-slider-thumb]:bg-white [&::-webkit-slider-thumb]:cursor-pointer"
              />
            </div>
            <div className="flex justify-between mt-1">
              <span className="text-[10px] text-zinc-700">0</span>
              <span className="text-[10px] text-zinc-700">255</span>
            </div>
          </div>
        </div>
      </div>

      {state && (
        <div className="bg-zinc-900 border border-zinc-800 p-4">
          <p className="text-[10px] font-bold tracking-[0.2em] uppercase text-zinc-500 mb-3">Current State</p>
          <div className="grid grid-cols-3 gap-4">
            <StateMetric label="R" value={String(state.color?.r ?? 0)} />
            <StateMetric label="G" value={String(state.color?.g ?? 0)} />
            <StateMetric label="B" value={String(state.color?.b ?? 0)} />
          </div>
        </div>
      )}
    </div>
  );
}

function StateMetric({ label, value }: { label: string; value: string }) {
  return (
    <div>
      <p className="text-[10px] tracking-widest uppercase text-zinc-600">{label}</p>
      <p className="text-lg font-bold font-mono text-white">{value}</p>
    </div>
  );
}
