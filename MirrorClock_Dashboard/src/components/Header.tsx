import { RefreshCw, Wifi, WifiOff } from 'lucide-react';
import type { StatusData } from '../types/api';

interface HeaderProps {
  status: StatusData | null;
  loading: boolean;
  onRefresh: () => void;
}

export function Header({ status, loading, onRefresh }: HeaderProps) {
  const connected = status?.wifi?.connected ?? false;
  const time = status?.time?.formatted ?? '--:--';

  return (
    <header className="border-b border-zinc-800 bg-black">
      <div className="max-w-screen-xl mx-auto px-6 py-4 flex items-center justify-between">
        <div className="flex items-center gap-6">
          <div>
            <p className="text-[10px] font-medium tracking-[0.25em] text-zinc-500 uppercase">Device Controller</p>
            <h1 className="text-xl font-bold tracking-tight text-white leading-tight">MirrorClock</h1>
          </div>
          <div className="h-8 w-px bg-zinc-800" />
          <div className="flex items-center gap-2">
            {connected ? (
              <Wifi size={14} className="text-emerald-400" />
            ) : (
              <WifiOff size={14} className="text-zinc-600" />
            )}
            <span className="text-sm text-zinc-400 font-mono">
              {connected ? status?.wifi?.ip : 'Disconnected'}
            </span>
          </div>
        </div>

        <div className="flex items-center gap-6">
          <div className="text-right">
            <p className="text-[10px] tracking-[0.2em] text-zinc-600 uppercase">Current Time</p>
            <p className="text-2xl font-bold tabular-nums tracking-tight text-white font-mono">{time}</p>
          </div>
          <button
            onClick={onRefresh}
            disabled={loading}
            className="flex items-center gap-2 px-4 py-2 border border-zinc-700 text-zinc-400 text-xs tracking-widest uppercase hover:border-white hover:text-white transition-all duration-200 disabled:opacity-40"
          >
            <RefreshCw size={12} className={loading ? 'animate-spin' : ''} />
            Refresh
          </button>
        </div>
      </div>
    </header>
  );
}
