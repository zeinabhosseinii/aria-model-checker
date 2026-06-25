# ARIA: Actor-based Reduction via Independence Analysis

Static priority assignment for Rebeca actor models via the Essential Rebeca Dependency Graph (ERDG). Produces `@priority`-annotated models that Afra consumes directly for priority-guided state-space exploration.

Based on: *Causal-Aware Partial-Order Reduction for Actor Models* — Hosseini, Ghassemi, Hojjat (2026).

---

## How it works

ARIA runs a five-step pipeline:

1. Parse the Rebeca model with an ANTLR4 grammar and extract per-instance state variable accesses and send statements.
2. Build the ERDG — four node sets (rebecs, message servers, activations, variables) and six edge types encoding causal and data-flow structure.
3. Detect **SDI** (Shared Destination Interference): two rebecs are dependent iff they send causally-independent messages to a shared destination whose handlers have **SVI** (Shared Variable Interference).
4. Partition rebecs into interference groups, build the Hierarchical Rebec Graph (HRG), and assign global actor priorities via topological sort.
5. Assign intra-rebec message server priorities from the SVI partition, then write `@priority` annotations into the output model.

State-space reductions of **33–64%** across nine benchmarks, with all property violations preserved (deadlock, queue overflow, assertion correctness).

---

## Requirements

- Java 17+, Maven 3.8+, Graphviz

```bash
brew install openjdk@17 maven graphviz
```

---

## Usage

```bash
mvn clean compile
./run.sh file benchmarks/BankTransfer.rebeca   # run a single model
./run.sh run BankTransfer                       # run a registered benchmark
./run.sh run-all                                # run all benchmarks
./run.sh list                                   # list registered benchmarks
```

Output goes to `output/<ModelName>/`:

| File | Description |
|------|-------------|
| `<ModelName>_annotated.rebeca` | priority-annotated model for Afra |
| `ERDG.png` | Essential Rebeca Dependency Graph |
| `RIG.png` | Rebec Interference Graph |
| `HRG.png` | Hierarchical Rebec Graph |

---

## Benchmarks

| Benchmark | Full | Ours | Reduction | Bug |
|-----------|:----:|:----:|:---------:|-----|
| TicketService (2 customers) | 34 | 18 | 33% | Deadlock |
| TicketService (3 customers) | 126 | 45 | 64% | Deadlock |
| BankTransfer | 5,026 | 2,381 | 52% | Deadlock |
| ResourceManager | 143,978 | 46,759 | 67% | — |
| LoadBalancer | 992 | 362 | 63% | — |
| SlotGuard | 3,521,765 | 919,634 | 74% | — |
| GradientSync | 1,313 | 584 | 55% | Queue Overflow |
| ImageProcessor | 1,144 | 660 | 42% | Queue Overflow |
| RequestPipeline | 2,214 | 1,091 | 50% | Assertion Violation |
| AsyncStreamBench | 7,588 | 4,820 | 36% | Assertion Violation |

---

## Adding a benchmark

Place a `.rebeca` file in `benchmarks/` and run it directly with `./run.sh file`. To register it for `./run.sh run <name>`:

```bash
python3 << 'PYEOF'
import json
with open('benchmarks/benchmarks.json', 'r') as f:
    data = json.load(f)
with open('benchmarks/MyModel.rebeca', 'r') as f:
    model = f.read()
data['benchmarks'].append({"name": "MyModel", "description": "...", "model": model})
with open('benchmarks/benchmarks.json', 'w') as f:
    json.dump(data, f, indent=2)
PYEOF
```

---

## Full verification workflow

```bash
# 1. generate priority assignment
./run.sh file benchmarks/MyModel.rebeca
# → output/MyModel/MyModel_annotated.rebeca

# 2. generate C++ state-space explorer via RMC
cd ~/Desktop/org.rebecalang.rmc
export JAVA_HOME=/opt/homebrew/Cellar/openjdk@17/.../Contents/Home
export PATH="$JAVA_HOME/bin:$PATH"
./run-rmc.sh -s /path/to/MyModel_annotated.rebeca -e TIMED_REBECA -v 2.3 -x -o ./tmp/MyModel

# 3. compile and explore
g++ -std=c++11 -pthread -O2 ./tmp/MyModel/*.cpp -o execute
./execute -x statespace.xml
```

---

## Supported syntax

Standard Core Rebeca plus: `abstract reactiveclass`/`extends`, `abstract msgsrv`, `?(a,b)` nondeterminism, `(Type)cast`, `sender`/`instanceof`, `int[5]` arrays, `for`/`while`/`break`/`continue`, `assertion(...)`, `env` declarations, `after`/`deadline` (Timed Rebeca), `//` and `/* */` comments.

---

## License

MIT
