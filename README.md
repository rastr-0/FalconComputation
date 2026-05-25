# FalconComputation

A TCP server that accepts math and variable commands over a simple text protocol.
Built in C++ as a learning project covering sockets, OOP, and session state.

---

## How it works

A client connects over TCP and sends commands as plain text lines. The server responds with `OK ...` or `ERROR reason`, one response per line.

```
SET x 5 3 8 1 2    → OK
SUM $x             → OK 19
STATS $x           → OK MIN=1 MAX=8 SUM=19 AVG=3.80 MEDIAN=3
QUIT               → BYE
```

Variables are per-connection — each client gets its own isolated session.

---

## Architecture

```
main_server.cpp          registers all commands, starts the server
    │
    ▼
Server                   TCP accept loop (networking only, knows nothing about commands)
    │  one per connection
    ▼
ProtocolHandler          parses a raw line, dispatches to a command, serialises the result
    ├── Session          per-connection variable store
    └── CommandRegistry  maps command names → command objects (shared across all connections)
```

Each layer has one job. The server does sockets. The handler does protocol. Commands do math.

---

## Commands

**Compute** — work on literal values or `$variables`

| Command | Example | Result |
|---------|---------|--------|
| `SUM` | `SUM 1 2 3` | `OK 6` |
| `MIN` / `MAX` | `MAX $x` | `OK 8` |
| `SORT` | `SORT 3 1 2` | `OK 1 2 3` |
| `AVG` | `AVG 1 2 3` | `OK 2.00` |
| `MEDIAN` | `MEDIAN 1 2 3 4` | `OK 2.50` |
| `COUNT` | `COUNT $x $y` | `OK 9` |
| `STATS` | `STATS $x` | `OK MIN=... MAX=... SUM=... AVG=... MEDIAN=...` |

**Variable**

| Command | Example | Result |
|---------|---------|--------|
| `SET` | `SET x 1 2 3` | `OK` |
| `SET` (capture) | `SET s <- SORT $x` | `OK` |
| `GET` | `GET $x` | `OK 1 2 3` |
| `APPEND` | `APPEND $x 4 5` | `OK` |
| `COPY` | `COPY $x $backup` | `OK` |
| `DEL` | `DEL $x $y` | `OK` |
| `LIST` | `LIST` | `OK x=[1 2 3] y=[4 5]` |
| `RESET` | `RESET` | `OK` |

**Control:** `HELP`, `HELP <command>`, `QUIT`

---

## Key design decisions

**String args through the parser** — the parser only tokenises; type conversion happens inside each command. This allows `$variable` references and string args like `HELP SUM` without the parser needing to know about them.

**Stateless command objects** — commands are singletons stored in the registry. All per-call state (args, session) is passed into `execute()`, not stored in the constructor. This makes the registry simple and commands easy to test.

**`$variable` expansion** — any token starting with `$` is looked up in the session's variable store and its values are spliced into the argument list. This happens inside `ComputeCommand::resolveArgs()`, so all compute commands get it for free.

**Capture form** — `SET x <- SORT $y` runs any registered command and stores its output as a variable. It goes through the registry, so it works with any command including ones added later.

---

## Building and running

```bash
# Build
make

# Start the server
./server

# Connect (in another terminal)
nc 127.0.0.1 5678
```

---

## Project structure

```
server/
  parser.h / .cpp          tokeniser
  command.h / .cpp         base classes + compute commands
  VariableCommands.h / .cpp SET GET DEL LIST RESET APPEND COPY
  VariableStore.h / .cpp   per-session variable storage
  CommandRegistry.h / .cpp command name → object map
  Session.h / .cpp         bundles store + registry for a connection
  ProtocolHandler.h / .cpp parse → dispatch → serialise
  server.h / .cpp          TCP socket loop
  main_server.cpp          entry point
client/
  client.h / client.cpp
```
