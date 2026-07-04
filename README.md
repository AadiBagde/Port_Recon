# PortRecon

A modular, professionally architected TCP port scanner built in C — developed
in disciplined, spec-driven phases rather than ad-hoc feature additions.

Each milestone is implemented from a formal **Software Requirement
Specification (SRS)** and tagged as a Git release, so the project's history
reads like an actual engineering changelog instead of a series of unrelated
commits.

---

## Project Status

**Current phase:** Service Database & Detection (🚧 in progress)

| Phase | Module                          | Status |
|-------|----------------------------------|--------|
| SRS-001 | Networking Foundation (Winsock init/cleanup, TCP socket creation) | ✅ Done |
| SRS-002 | TCP Connection Engine (`build_socket_address`, `connect_to_target`) | ✅ Done |
| SRS-003 | Single Port Scanner (`scan_port` → `PortResult`) | ✅ Done |
| SRS-004 | Port Range Scanner (`scan_range` → `PortResult*`, `ScanStats`) | ✅ Done |
| SRS-005 | Timeout Engine (non-blocking `connect_with_timeout` + `select()`) | ✅ Done |
| SRS-006 | Service Database (static port→service lookup table) | 🚧 In Progress |
| SRS-007 | Basic Service Detection (wiring lookup into `PortResult.service`) | 🚧 In Progress |
| SRS-008+ | CLI Argument Parsing, Scan Profiles, Error Handling | ⬜ Not Started |

> The Service Database / Detection phase was interrupted mid-implementation.
> `services.h` / `services.c` and the `scan_port()` wiring may be partially
> present — verify against the acceptance criteria in `docs/roadmap.md`
> before assuming this phase is complete.

---

## What PortRecon Can Do Right Now

- Scan a single TCP port on a target IP and get back a structured result
  (`PortResult`), not just a boolean.
- Scan an entire port range in one call (`scan_range`), with aggregate
  statistics (`ScanStats`: total / scanned / open / closed ports).
- Connect with a **bounded timeout** — filtered or unreachable hosts return
  in a configurable window (e.g. ~300ms) instead of hanging on the OS
  default TCP timeout (which can be 20–30+ seconds per port).
- (In progress) Resolve well-known ports to service names (e.g. `22 → SSH`,
  `80 → HTTP`) via a static lookup table.

## What It Deliberately Doesn't Do Yet

By design, the following are **out of scope** until their own SRS phase:

- Multithreaded scanning
- Banner grabbing
- Service fingerprint confidence scoring
- CLI argument parsing (target/ports/threads are currently hardcoded in `main.c`)
- Logging, JSON/CSV reporting
- Distinguishing `FILTERED` from `CLOSED` (requires ICMP/RST inspection)

See `docs/roadmap.md` for the full phase-by-phase plan through v2.0.

---

## Architecture

```
Application
      │
      ▼
Scanner Engine (scan_range → scan_port)
      │
      ├──▶ connect_with_timeout()
      │         │
      │         ▼
      │    Socket Utilities (non-blocking connect + select())
      │
      └──▶ lookup_service_name()   [in progress]
                │
                ▼
           Service Database (static table)
```

`main.c` is intentionally a thin orchestrator — it contains no raw socket
calls, no `select()`, and no networking logic. All of that lives in the
scanner and socket-utility layers.

## Project Structure

```
PortRecon/
├── src/
│   ├── main.c
│   └── scanner/
│       ├── scanner.c
│       ├── socket_utils.c
│       └── services.c        (in progress)
│
├── include/
│   ├── utils/
│   │   └── types.h
│   └── scanner/
│       ├── scanner.h
│       ├── socket_utils.h
│       └── services.h        (in progress)
│
├── reports/
├── logs/
├── screenshots/
├── docs/
│   ├── architecture.md
│   └── roadmap.md
├── tests/
├── Makefile
├── README.md
├── LICENSE
└── .gitignore
```

## Key Data Structures

```c
typedef struct
{
    char target_ip[MAX_IP_LENGTH];
    int  start_port;
    int  end_port;
    int  timeout_ms;      /* ms to wait for a TCP handshake, e.g. 50–5000 */
    int  thread_count;    /* reserved for future multithreading phase */
    bool verbose;
    char output_file[MAX_FILENAME];
} ScanConfig;

typedef struct
{
    int  port;
    bool is_open;
    char service[MAX_SERVICE_NAME];   /* "unknown" until Service Detection lands */
    char banner[MAX_BANNER_LENGTH];   /* reserved for future Banner Grabbing phase */
    int  confidence;                  /* reserved for future Confidence Scoring phase */
} PortResult;

typedef struct
{
    int    total_ports;
    int    scanned_ports;
    int    open_ports;
    int    closed_ports;
    double elapsed_time;
} ScanStats;
```

## Core Functions

| Function | Responsibility |
|---|---|
| `initialize_network()` / `cleanup_network()` | Winsock lifecycle (no-op on POSIX) |
| `create_tcp_socket()` | Create an IPv4 TCP socket |
| `build_socket_address()` | Populate a `sockaddr_in` from IP + port |
| `connect_with_timeout()` | Non-blocking connect bounded by `select()` |
| `scan_port()` | Scan one port, return a populated `PortResult` |
| `scan_range()` | Scan a full port range, return `PortResult*` + fill `ScanStats` |
| `lookup_service_name()` *(in progress)* | Resolve a port number to a known service name |

## Building

```bash
make
```

## Running

Target IP, port range, and timeout are currently hardcoded in `src/main.c`
(CLI argument parsing arrives in a later phase). Edit the values in `main()`
and rebuild to point the scanner at a different target.

```bash
./portrecon
```

Example output:

```
Starting scan on 127.0.0.1 (75-85) with timeout 300 ms...

PORT      STATUS
75        CLOSED
76        CLOSED
...
80        OPEN

Open Ports   : 1
Closed Ports : 10
Total Ports  : 11

------------------------------------------------------------

Testing timeout behavior against 10.255.255.1:80 (timeout=300ms)...
Result: CLOSED/FILTERED (took ~303 ms)
```

## Design Principles

This project follows a strict set of engineering conventions, enforced
across every phase:

- **Single Responsibility Principle** — each function does one thing.
- **No networking logic in `main.c`** — it only orchestrates and prints.
- **Caller owns resources** — every socket is closed exactly once, on
  every code path.
- **No global mutable state.**
- **Defensive programming** — every public function validates its
  parameters and never crashes, prints, or calls `exit()` from inside a
  library-layer function.
- **No magic numbers** — timeouts, buffer sizes, and defaults are named
  constants.

## Roadmap

Full phase breakdown (Timeout Engine → Service Detection → CLI → Banner
Grabbing → Multithreading → Live Metrics → Reporting → Fingerprint
Confidence Scoring → v2.0 features) lives in [`docs/roadmap.md`](docs/roadmap.md).

## License

See [`LICENSE`](LICENSE).