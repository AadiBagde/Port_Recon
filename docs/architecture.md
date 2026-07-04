# PortRecon Architecture

PortRecon is organized as small modules with clear ownership:

```text
main.c
  |
  v
CLI parser -> ScanConfig
  |
  v
Scanner engine -> PortResult[] + ScanStats
  |
  +-- socket_utils: socket creation, address building, timed TCP connect
  +-- services: static well-known port lookup
  +-- banner: passive banner receive
```

`main.c` is the orchestration and presentation layer. It should not contain
raw socket calls, `select()`, or scanner internals.

Library modules should validate inputs, avoid process exits, avoid printing
scan results, and leave ownership of dynamically allocated results with the
caller.
