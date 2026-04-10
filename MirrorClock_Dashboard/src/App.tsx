import { useState, useEffect, useCallback } from 'react';
import { AlertCircle } from 'lucide-react';
import { api } from './lib/api';
import type { DeviceConfig, DeviceState, StatusData } from './types/api';
import { Header } from './components/Header';
import { StatusPanel } from './components/StatusPanel';
import { LedControl } from './components/LedControl';
import { ConfigPanel } from './components/ConfigPanel';

export default function App() {
  const [status, setStatus] = useState<StatusData | null>(null);
  const [state, setState] = useState<DeviceState | null>(null);
  const [config, setConfig] = useState<DeviceConfig | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [rebootMessage, setRebootMessage] = useState<string | null>(null);

  const fetchAll = useCallback(async () => {
    setLoading(true);
    setError(null);
    try {
      const [statusRes, stateRes, configRes] = await Promise.all([
        api.getStatus(),
        api.getState(),
        api.getConfig(),
      ]);
      if (statusRes.data) setStatus(statusRes.data);
      if (stateRes.data) setState(stateRes.data);
      if (configRes.data) setConfig(configRes.data);
    } catch {
      setError('Failed to connect to device. Make sure you are on the same network.');
    } finally {
      setLoading(false);
    }
  }, []);

  useEffect(() => {
    fetchAll();
    const interval = setInterval(fetchAll, 10000);
    return () => clearInterval(interval);
  }, [fetchAll]);

  function handleReboot() {
    setRebootMessage('Device is rebooting. Reconnecting in 10s...');
    setTimeout(() => {
      setRebootMessage(null);
      fetchAll();
    }, 10000);
  }

  return (
    <div className="min-h-screen bg-black text-white" style={{ fontFamily: "'Inter', 'Helvetica Neue', Helvetica, Arial, sans-serif" }}>
      <Header status={status} loading={loading} onRefresh={fetchAll} />

      <main className="max-w-screen-xl mx-auto px-6 py-8">
        {error && (
          <div className="mb-6 flex items-center gap-3 border border-red-800 bg-red-950/40 px-4 py-3 text-sm text-red-400">
            <AlertCircle size={14} className="flex-shrink-0" />
            {error}
          </div>
        )}

        {rebootMessage && (
          <div className="mb-6 flex items-center gap-3 border border-amber-700 bg-amber-950/40 px-4 py-3 text-sm text-amber-400">
            <AlertCircle size={14} className="flex-shrink-0" />
            {rebootMessage}
          </div>
        )}

        <div className="grid grid-cols-1 lg:grid-cols-12 gap-6">
          <div className="lg:col-span-4">
            <StatusPanel status={status} />
          </div>

          <div className="lg:col-span-4">
            <LedControl state={state} onStateChange={setState} />
          </div>

          <div className="lg:col-span-4">
            <ConfigPanel
              config={config}
              onConfigChange={setConfig}
              onReboot={handleReboot}
            />
          </div>
        </div>

        <footer className="mt-12 border-t border-zinc-900 pt-6 flex items-center justify-between">
          <p className="text-[10px] tracking-[0.2em] uppercase text-zinc-700">MirrorClock Dashboard</p>
          <p className="text-[10px] font-mono text-zinc-800">
            {status ? `Epoch ${status.time.epoch}` : '—'}
          </p>
        </footer>
      </main>
    </div>
  );
}
