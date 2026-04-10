import type { ApiResponse, DeviceConfig, DeviceState, StatusData } from '../types/api';

const BASE = '';

async function request<T>(path: string, options?: RequestInit): Promise<ApiResponse<T>> {
  const res = await fetch(`${BASE}${path}`, {
    headers: { 'Content-Type': 'application/json' },
    ...options,
  });
  if (!res.ok) throw new Error(`HTTP ${res.status}`);
  return res.json();
}

export const api = {
  getStatus: () => request<StatusData>('/api/status'),
  getState: () => request<DeviceState>('/api/state'),
  updateState: (body: Partial<{ enabled: boolean; brightness: number | 'auto'; color: { hex?: string; r?: number; g?: number; b?: number } }>) =>
    request<DeviceState>('/api/state', { method: 'POST', body: JSON.stringify(body) }),
  getConfig: () => request<DeviceConfig>('/api/config'),
  updateConfig: (body: Partial<DeviceConfig>) =>
    request<undefined>('/api/config', { method: 'POST', body: JSON.stringify(body) }),
  reboot: () => request<undefined>('/api/system/reboot', { method: 'POST' }),
};
