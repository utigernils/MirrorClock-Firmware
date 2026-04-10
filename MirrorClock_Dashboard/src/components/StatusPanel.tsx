import { Activity, Cpu, MemoryStick, Signal, Sun } from 'lucide-react';
import type { StatusData } from '../types/api';

interface StatusPanelProps {
  status: StatusData | null;
}

function formatUptime(seconds: number): string {
  const h = Math.floor(seconds / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;
  if (h > 0) return `${h}h ${m}m`;
  if (m > 0) return `${m}m ${s}s`;
  return `${s}s`;
}

function formatBytes(bytes: number): string {
  return `${(bytes / 1024).toFixed(1)} KB`;
}

function RssiBar({ rssi }: { rssi: number }) {
  const quality = rssi > -50 ? 4 : rssi > -60 ? 3 : rssi > -70 ? 2 : 1;
  return (
    <div className="flex items-end gap-0.5 h-4">
      {[1, 2, 3, 4].map((bar) => (
        <div
          key={bar}
          style={{ height: `${bar * 25}%` }}
          className={`w-1.5 rounded-sm ${bar <= quality ? 'bg-white' : 'bg-zinc-700'}`}
        />
      ))}
    </div>
  );
}

function Metric({ label, value, icon: Icon }: { label: string; value: string; icon: React.ElementType }) {
  return (
    <div className="flex items-center justify-between py-3 border-b border-zinc-800 last:border-0">
      <div className="flex items-center gap-3">
        <Icon size={13} className="text-zinc-600" />
        <span className="text-xs tracking-widest uppercase text-zinc-500">{label}</span>
      </div>
      <span className="text-sm font-mono font-medium text-white">{value}</span>
    </div>
  );
}

export function StatusPanel({ status }: StatusPanelProps) {
  if (!status) {
    return (
      <div className="space-y-4">
        <SectionLabel>System Status</SectionLabel>
        <Card>
          <div className="py-8 text-center text-zinc-600 text-sm tracking-widest uppercase">No Data</div>
        </Card>
      </div>
    );
  }

  return (
    <div className="space-y-4">
      <SectionLabel>System Status</SectionLabel>

      <Card>
        <CardTitle>System</CardTitle>
        <Metric label="Uptime" value={formatUptime(status.system.uptime)} icon={Activity} />
        <Metric label="Free Heap" value={formatBytes(status.system.free_heap)} icon={MemoryStick} />
        <Metric label="CPU Freq" value={`${status.system.cpu_freq} MHz`} icon={Cpu} />
        <Metric label="Chip ID" value={String(status.system.chip_id)} icon={Cpu} />
      </Card>

      <Card>
        <CardTitle>Network</CardTitle>
        <div className="flex items-center justify-between py-3 border-b border-zinc-800">
          <div className="flex items-center gap-3">
            <Signal size={13} className="text-zinc-600" />
            <span className="text-xs tracking-widest uppercase text-zinc-500">Signal</span>
          </div>
          <div className="flex items-center gap-3">
            <span className="text-sm font-mono text-white">{status.wifi.rssi} dBm</span>
            <RssiBar rssi={status.wifi.rssi} />
          </div>
        </div>
        <Metric label="SSID" value={status.wifi.ssid || '—'} icon={Signal} />
        <Metric label="IP Address" value={status.wifi.ip || '—'} icon={Signal} />
      </Card>

      <Card>
        <CardTitle>Sensor</CardTitle>
        <Metric label="Lux" value={`${status.sensor.lux.toFixed(1)} lx`} icon={Sun} />
        <Metric label="Calc. Brightness" value={String(status.sensor.calculated_brightness)} icon={Sun} />
      </Card>
    </div>
  );
}

function SectionLabel({ children }: { children: React.ReactNode }) {
  return (
    <p className="text-[10px] font-medium tracking-[0.25em] text-zinc-600 uppercase">{children}</p>
  );
}

function Card({ children }: { children: React.ReactNode }) {
  return (
    <div className="bg-zinc-900 border border-zinc-800 p-4">{children}</div>
  );
}

function CardTitle({ children }: { children: React.ReactNode }) {
  return (
    <p className="text-[10px] font-bold tracking-[0.2em] uppercase text-zinc-500 mb-2">{children}</p>
  );
}
