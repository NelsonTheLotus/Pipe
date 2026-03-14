pipe creates a .pipe folder where it stores all it's metadata.
Metadata includes:
- File comp time and hash (for dependency evaluation)
- Logs for previous executions
- Stored configuration/pipe file for easy recompilation without having to reconfigure (pipe cache)

Jobs is the combination of currently executing commands. If a thread is running 1 command that 1 job.
If 2 threads are running 2 commands each, that's 4 jobs. Pipe decides the optimal order in which
to issue commands.

within a mapping, if doing wildcard match, allow an option to ignore matched files that have
been processed within the same mapping (default). Or deactivate the option if not. 
Also allow regex file searches.

DO NOT forget to check for dependencies regarding modification of pipefile, using cache.

Add triggers in the config, for example an onClear trigger, run this pipefile rule:
```
config
{
    onClear: clear  // flow called clear
}
```







# TEMP

Copied from chatGPT, generated after a lengthy discussion of what is, what was and what ought to be.

==== BEGIN TRANSCRIPT ====
# Pipe — Conceptual and Behavioral Documentation

This document defines the mental model, user philosophy, structural components, and execution behavior of **Pipe**. It is intended to describe not just how to use Pipe, but how to think in Pipe.

---

# 1. The Mental Model of Pipe

Pipe is a **static dependency graph engine**.

At its core:

* **Nodes** represent artifacts.
* **Actions** represent transformations.
* **Edges are actions.**
* **The graph is frozen before execution.**
* **Execution is target-driven.**

Pipe is not a script runner.
It is not a sequential task list.
It is not a dynamic build interpreter.

It is a deterministic engine that:

1. Constructs a full Directed Acyclic Graph (DAG).
2. Validates it.
3. Resolves external bindings.
4. Prunes unreachable subgraphs based on requested targets.
5. Executes only what is necessary.

If you understand that Pipe builds and executes a **frozen transformation graph**, you understand Pipe.

---

# 2. How Users Should Think When Writing Pipe

Users should think in terms of:

> “What artifacts exist?”
> “What transforms produce them?”
> “What do I want built?”

Not:

> “What commands should run in what order?”

Pipe is artifact-oriented, not command-oriented.

When writing Pipe, you are:

* Declaring transformations (actions).
* Instantiating relationships (pipes).
* Requesting targets.

You are not writing an execution script.

If you want strict sequential scripting with side effects only, Pipe can model it using virtual artifacts — but that is not its primary purpose.

---

# 3. Core Components

## 3.1 Nodes (Artifacts)

Nodes represent artifacts.

They are one of two types:

* **Real artifacts**

  * Correspond to external state (files, environment-bound values).
* **Virtual artifacts**

  * Exist only to enforce ordering or model abstract state.

### Properties

Internally, nodes maintain metadata such as:

* Existence state
* Timestamp or hash
* Producer action (if any)
* Type (REAL or VIRTUAL)

To the user, nodes are simply identifiers.

### Producer Rule

Each artifact may have:

* Exactly one producing action
  or
* No producer (external source)

Multiple producers for the same artifact is an error.

---

## 3.2 Actions

An action represents a transformation.

Internally:

```
Action {
    inputs:  [Artifact]
    outputs: [Artifact]
    command: …
}
```

### Properties

* Actions are atomic scheduling units.
* If any output is required, the entire action executes.
* Actions do not partially execute.
* Actions do not mutate the graph.

### Type Constraints

Actions declare input/output type compatibility:

* Real → Real
* Real → Virtual
* Virtual → Virtual
* etc.

Invalid type combinations are rejected at graph validation time.

---

## 3.3 Pipes

Pipes are **graph construction mechanisms**.

They:

* Mass-connect artifact sets.
* Instantiate actions across many input/output pairs.
* Reduce syntactic verbosity.

Pipes are not runtime entities.

After DAG construction, pipes disappear. Only actions and nodes remain.

---

# 4. Graph Construction Phase

Pipe performs these steps:

1. Parse configuration.
2. Expand patterns deterministically.
3. Instantiate actions via pipes.
4. Construct the complete DAG.
5. Validate graph consistency:

   * No cycles.
   * Single producer rule.
   * Type compatibility.
   * No unresolved explicit artifacts.

At this stage:

* No execution has occurred.
* No filesystem resolution has occurred.
* The graph is purely structural.

---

# 5. External Binding Phase

After the DAG is built and validated:

For each node:

### If REAL:

* Attempt external binding.
* Check existence.
* Record metadata (timestamp, hash, etc.).

### If VIRTUAL:

* Skip external binding.

Failure occurs if:

* A real artifact has no producer and does not exist externally.
* An explicitly declared artifact cannot be resolved.

Pattern-based or abstract artifacts must resolve to concrete producers before execution begins.

No new nodes or edges may appear after this phase.

---

# 6. Target-Driven Execution

Pipe does not build the entire graph automatically.

Execution is driven by requested targets.

Steps:

1. User requests one or more target artifacts.
2. Pipe computes the reachable subgraph.
3. All unreachable actions are discarded.
4. Only the relevant subgraph is evaluated.

If no target is specified:

* Either a default target must exist.
* Or execution fails.

Pipe does not guess.

---

# 7. Invalidation and Rebuild Logic

Execution is artifact-driven but action-scheduled.

For each action:

It executes if:

* Any input is newer than any output.
* Any output is missing.
* Any output is invalid.
* Policy forces rebuild.

Nodes determine invalidation.
Actions are the execution units.

---

# 8. Execution Model

Execution is:

* Deterministic.
* Topologically ordered.
* Parallel where dependencies allow.

Key properties:

* Actions are atomic.
* Multiple-output actions execute fully if any output is needed.
* No action may modify the graph.
* No dynamic edge discovery occurs during execution.

---

# 9. Virtual Artifacts

Virtual artifacts:

* Represent abstract states.
* Enforce ordering.
* Enable modeling of side-effect-only commands.

They:

* Are produced by exactly one action.
* May depend on other artifacts.
* Never bind to filesystem.

They replace the ambiguity found in systems like Make where targets may or may not correspond to files.

---

# 10. Patterns

Patterns are compile-time graph generators.

Rules:

* Expanded before execution.
* Deterministic.
* Frozen before scheduling.

Patterns may not introduce runtime graph mutation.

If a pattern expands ambiguously to multiple producers of the same artifact → error.

---

# 11. Error Conditions

Pipe fails immediately on:

* Cycles in DAG.
* Multiple producers for one artifact.
* Type mismatches.
* Missing explicit real artifacts.
* Unresolvable patterns.
* Runtime failure of required producer chain.

Pipe prioritizes explicit failure over silent fallback.

---

# 12. Interaction With External Environment

Pipe interacts with external state only through:

* Real artifact binding.
* Action command execution.

Pipe does not:

* Discover dependencies during execution.
* Infer edges from command output.
* Modify graph structure dynamically.

External changes are detected only through artifact invalidation logic.

---

# 13. Subtleties and Design Philosophy

## 13.1 Static Graph Guarantee

Once built, the graph is immutable.

This simplifies:

* Scheduling
* Parallelism
* Reasoning
* Debugging

Systems that allow runtime graph mutation (e.g., highly dynamic configurations in Bazel-style ecosystems) gain power but lose simplicity.

Pipe intentionally favors simplicity.

---

## 13.2 Atomic Actions

Even if only one output of an action is required:

* The full action runs.

Pipe does not attempt partial command extraction.

Optimization exists at the graph level, not the shell level.

---

## 13.3 Strictness Over Implicitness

Pipe rejects:

* Ambiguous producers
* Silent fallback to external files
* Implicit phony behavior

This avoids the long-term ambiguity problems seen in legacy build systems.

---

## 13.4 Separation of Phases

Pipe strictly separates:

1. Graph construction
2. Graph validation
3. External binding
4. Target pruning
5. Invalidation analysis
6. Execution

Blurring these phases introduces nondeterminism and complexity.

---

# 14. What Pipe Is Not

Pipe is not:

* A general-purpose scripting language.
* A dynamic dependency discovery system.
* A runtime graph mutator.
* A speculative execution engine.

Pipe is a deterministic transformation engine over a static DAG.

---

# 15. The Guiding Principle

When designing or using Pipe, always ask:

> “Is this behavior visible in the graph?”

If the answer is no, the design is wrong.

The graph must explain everything.

If the graph explains everything, the system remains simple, scalable, and debuggable.

==== END TRANSCRIPT ====



And another design transcript:
==== BEGIN TRANSCRIPT ====
# Pipe — Strategic Vision and Design Rationale

This document summarizes the broader vision, motivations, and design philosophy behind **Pipe**, as well as reflections on its potential role in the ecosystem of build and automation tools. It captures the reasoning behind the architecture and the long-term direction for the project.

The goal is not only to document how Pipe works, but also **why it exists and what problems it aims to solve**.

---

# 1. The Problem Space

Build systems and automation tools are a foundational part of software development. Despite decades of innovation, many developers still experience significant friction when dealing with them.

Common complaints include:

* Fragile dependency management
* Difficult debugging of build behavior
* Poor cross-platform ergonomics
* Intrusive project configuration
* Complex and unintuitive configuration languages

Widely used systems such as:

* Make
* CMake
* Ninja
* Bazel
* Meson

each address parts of the problem, but none completely solve the developer experience around **automation, reproducibility, and cross-platform orchestration**.

As a result, many projects accumulate layers of scripts, configuration files, and platform-specific logic that become increasingly difficult to reason about over time.

Pipe is an attempt to rethink the **core automation engine** while maintaining a philosophy of simplicity and determinism.

---

# 2. Pipe’s Core Idea

At its heart, Pipe is a **static dependency graph engine**.

Instead of thinking about automation as a sequence of commands, Pipe encourages thinking in terms of **artifacts and transformations**.

Developers describe:

* what artifacts exist
* how artifacts are produced
* which artifacts are desired

Pipe then constructs a **deterministic Directed Acyclic Graph (DAG)** of these transformations and executes only what is necessary.

The system is built on several key principles:

* **Deterministic execution**
* **Static graph construction**
* **Explicit dependencies**
* **Single producer per artifact**
* **Target-driven execution**

These principles make builds easier to reason about and easier to debug.

---

# 3. What Pipe Is Not Trying to Do

Pipe does not attempt to:

* be a general scripting language
* replace programming languages
* dynamically discover dependencies during execution
* hide platform differences behind overly abstract interfaces

Many build systems attempt to become **universal orchestration languages**, which often leads to excessive complexity.

Pipe deliberately avoids this path.

Instead, Pipe focuses on being a **clean and reliable transformation engine**.

---

# 4. The Cross-Platform Philosophy

One of the most common frustrations in build systems is cross-platform support.

Tools often attempt to provide a single interface that magically works everywhere. In practice, this rarely succeeds because platforms differ significantly in compilers, shells, file systems, and toolchains.

Pipe takes a different approach.

Instead of attempting to provide a universal abstraction layer for all platforms, Pipe follows the philosophy:

> **Write once, run everywhere.**

This differs from the idea of writing a single instruction that magically works on all systems.

Instead:

* The user writes a configuration once.
* The configuration may include platform-specific logic.
* Pipe provides **clean, structured system detection** so this logic can be expressed clearly.

The goal is to empower developers to write portable automation without forcing them into awkward shell hacks or fragile environment checks.

For example, platform detection should be explicit and structured rather than relying on ad-hoc shell behavior.

---

# 5. System Detection as a First-Class Concept

A major pain point in existing systems is environment detection.

Many tools rely on fragile techniques such as:

* parsing shell output
* environment variable heuristics
* custom probing scripts
* platform-specific conditionals

Pipe aims to make **system information structured and accessible**.

Instead of relying on shell behavior, configuration can reference environment facts directly, such as:

* operating system
* architecture
* compiler availability
* tool versions
* supported features

This enables developers to express platform logic clearly while keeping the configuration portable.

---

# 6. Modularity and Future Ecosystem

Pipe’s core engine is intentionally **language agnostic**.

It does not attempt to encode knowledge about:

* C/C++ compilation
* Python packaging
* Rust builds
* Java ecosystems

Instead, higher-level functionality can be implemented through **modules**.

For example, a module might provide support for:

* compiling C/C++ projects
* managing Python environments
* building container images

These modules can encode platform-specific knowledge while relying on Pipe’s deterministic execution engine.

Over time, an ecosystem of modules could emerge where developers reuse solutions to common problems.

This separation ensures that the core engine remains simple while the ecosystem grows organically.

---

# 7. Simplicity and Project Structure

Another design goal of Pipe is to minimize intrusion into project layouts.

Many build systems require configuration files to be scattered across the project tree.

For example, large projects using CMake often contain many configuration files spread across multiple directories.

Pipe aims to keep configuration:

* centralized
* readable
* minimal

This improves maintainability and lowers the barrier to entry for new contributors.

---

# 8. Debuggability as a Core Feature

One of the most important goals for Pipe is **transparency**.

Developers frequently struggle with build systems because they cannot easily answer questions like:

* Why did this rebuild?
* Why did this not rebuild?
* What will run before I start the build?

Pipe’s strict DAG model makes it possible to answer these questions precisely.

Future tooling may provide commands such as:

* explaining why a target was rebuilt
* displaying the dependency chain for an artifact
* showing what actions will execute before running a build

Because the graph is static and explicit, Pipe can always explain its behavior.

This aligns with Pipe’s core design principle:

> **If the graph explains everything, the system remains understandable.**

---

# 9. Potential Impact

Pipe does not attempt to revolutionize software development.

Instead, it aims to provide a **modern, principled alternative** to traditional automation tools.

If successful, Pipe could serve as:

* a cleaner replacement for traditional Make-style automation
* a flexible foundation for cross-language build workflows
* a reliable engine for deterministic pipelines

The long-term success of Pipe will depend not only on its architecture but also on:

* developer experience
* clarity of configuration
* ease of adoption
* ecosystem growth

Many successful build systems gained adoption not purely through technical superiority, but through strong tooling and community support.

---

# 10. Guiding Principle

The philosophy guiding Pipe can be summarized simply:

> **The graph must explain everything.**

When every transformation and dependency is visible in the graph:

* builds become predictable
* debugging becomes straightforward
* automation becomes easier to reason about

Pipe is built around the belief that **clarity and determinism are the foundation of reliable automation**.

---

# 11. Final Perspective

Pipe is not intended to be a theoretical experiment or an overly ambitious universal system.

It is a practical attempt to design an automation engine that is:

* deterministic
* transparent
* extensible
* developer-friendly

While the space of build systems is already crowded, many existing tools suffer from historical complexity and accumulated design compromises.

Pipe’s goal is to explore what a **modern, principled automation system** could look like if designed with clarity and determinism as first-class priorities.

==== END TRANSCRIPT ====

