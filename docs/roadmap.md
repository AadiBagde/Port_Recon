# PortRecon Roadmap

## Current Baseline

PortRecon now has a working sequential TCP connect scanner with:

- IPv4 target selection via `-i`
- Port range selection via `-p start-end`
- Per-port timeout via `-t`
- Verbose mode via `-v`
- Help via `-h`
- Well-known service lookup for open ports
- Passive banner capture for open ports that send an immediate banner

This is the v1.2 foundation. The scanner is intentionally sequential for now;
threading and live metrics should build on this behavior rather than replace it.

## Near-Term Plan

### v1.2 - Banner Grabbing Stabilization

- Keep banner grabbing passive and bounded by the configured timeout.
- Add protocol-specific probes later only behind small helper functions.
- Add local test helpers for banner-producing services.
- Keep `PortResult.banner` empty when no banner is available.

### v1.3 - Multithreaded Scan Engine

- Implement a bounded worker pool using the existing `thread_count` field.
- Preserve the `scan_range()` result contract.
- Add synchronization only around shared counters and work queue state.
- Keep single-threaded scanning available as the fallback path.

### v1.4 - Metrics and Terminal Output

- Track elapsed time, ports per second, and estimated completion.
- Add progress rendering behind a UI module, not inside scanner logic.
- Keep plain output available for scripts and tests.

### v1.5 - Reporting and Logging

- Add JSON and CSV report writers using `PortResult` and `ScanStats`.
- Add scan logs under `logs/`.
- Keep reporting optional via CLI flags.

### v1.6 - Fingerprint Confidence

- Score service identity using port mapping, banner evidence, and optional probes.
- Store score in `PortResult.confidence`.
- Keep confidence at `0` when no evidence is available.

## Later v2.0 Ideas

- CIDR/range target scanning
- IPv6
- UDP scanning
- SYN scanning where supported
- Host discovery
- XML output
- Plugin-style service probes
