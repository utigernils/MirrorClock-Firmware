import fs from 'fs';
import zlib from 'zlib';
import path from 'path';

const distPath = path.join(process.cwd(), 'dist', 'index.html');
if (!fs.existsSync(distPath)) {
  console.error('dist/index.html not found. Check if Vite built successfully.');
  process.exit(1);
}

const html = fs.readFileSync(distPath);
const gzipped = zlib.gzipSync(html);

const outFilePath = path.join(process.cwd(), '..', 'MirrorClock_Firmware', 'web_dashboard.h');

let headerContent = `#pragma once
#include <pgmspace.h>

const uint32_t DASHBOARD_HTML_SIZE = ${gzipped.length};
const uint8_t DASHBOARD_HTML_GZ[] PROGMEM = {
`;

for (let i = 0; i < gzipped.length; i++) {
  headerContent += `0x${gzipped[i].toString(16).padStart(2, '0')}`;
  if (i < gzipped.length - 1) {
    headerContent += ', ';
  }
  if ((i + 1) % 16 === 0) {
    headerContent += '\n  ';
  }
}

headerContent += `
};
`;

fs.writeFileSync(outFilePath, headerContent);
console.log(`Successfully generated ${outFilePath} (${gzipped.length} bytes gzipped)`);
