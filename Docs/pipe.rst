===========
Pipe Manual
===========

.. |FILENAME| replace:: Pipe Manual
.. |VERSION| replace:: 0.1
.. |STATUS|  replace:: DRAFT
.. |DATE|    replace:: 2025-11-02
.. |PIPE|    replace:: ``Pipe``
.. |PIPE_VERSION| replace:: 1.0

This document specifies the behavior, structure, and semantics of the |PIPE|
file processing utility, which automates large-scale, rule-based file
transformations.

This is version |VERSION|, last updated |DATE|, of the |FILENAME| for
|PIPE| version |PIPE_VERSION|.

.. contents::
   :depth: 2
   :local:


1. Scope
========

This is a normative specification for |PIPE|. It defines the syntax,
evaluation rules, and behavioral guarantees observed when parsing and
executing |PIPE| files.

This document is written to be self-contained and approachable without
prior experience with |PIPE|. However, a conceptual understanding of the
following ideas will be useful:

* **Actions** — reusable command execution templates.
* **Pipes** — mappings from input files to output files using patterns.
* **Flows** — ordered sequences of pipe execution steps.

All examples are illustrative. No example overrides any rule in this document.

Conformance to this document is required for full |PIPE| compatibility.


1.1 Purpose of |PIPE|
---------------------

|PIPE| automates systematic file processing operations. While build systems
such as ``make``, ``CMake``, or ``ninja`` are optimized primarily for C/C++
software compilation, |PIPE| is **operation-agnostic**. It does not assume
any language, build structure, or compilation model.

Its core philosophy is:

**Define what should happen to files, not how to spell out the commands each time.**

|PIPE| is suitable for:

* Compiling software in any language
* Generating derived assets (images, shaders, audio, models)
* Batch conversion and archival tasks
* Large-scale structured data transformation


1.2 Non-Goals and Limitations
-----------------------------

To maintain predictability and avoid implicit behavior, |PIPE| intentionally
**does not** provide:

* **Automatic dependency scanning**  
  The user must specify dependencies explicitly (either directly or via
  ``.d`` files).
* **Implicit tool/command inference**  
  |PIPE| will never guess which compiler or converter you intended.
* **Per-rule conditional execution or dynamic expression logic**  
  Conditional configuration is allowed **only** in the ``config`` section,
  not inside pipes or mappings.

These constraints are foundational to |PIPE|’s goal:

**As simple as possible, as complex as necessary — but never implicit.**



2. Core Terminology and Structural Concepts
===========================================

This chapter defines the core terms used throughout this document. Each term is
normative: the behavior and meaning of |PIPE| rely on these definitions.

2.1 Files and Compilation
-------------------------

In the context of |PIPE|, the term **compile** is used in a broad, generalized
sense. It refers to:

    **Any deterministic transformation of one or more input files into one or
    more output files.**

Examples include, but are not limited to:

* Compiling source code to object files
* Linking binary artifacts
* Converting images, audio, models, or documents
* Asset packing or text preprocessing
* Code generation or transformation passes

The key property is that a compilation step:

* Takes defined input files
* Produces defined output files
* Is reproducible when given the same inputs, parameters, and command


2.2 Actions
-----------

An **Action** is a named, reusable command template. It describes *how* a single
compilation step should be performed, but not *which* files it will apply to.

An action includes:

* The command to execute
* The list of parameters that may be configured
* Optional metadata describing where and when the action is allowed to run

Actions do **not** select input files and do not specify file mappings. They are
pure definitions — inert until used by a pipe.


2.3 Pipes
---------

A **Pipe** defines *file transformation rules*. It determines:

* Which files are **inputs**
* How the corresponding **output paths** are derived
* Which **action** is used to process each input → output pair
* Any additional parameters or dependencies applied on a per-mapping basis

A pipe does **not** execute immediately. It defines a *structured mapping* from
file patterns to compilation steps.

A pipe is evaluated only when invoked within a **flow**.


2.4 Flows
---------

A **Flow** is an ordered sequence of pipe executions. It defines *when* and *in
what order* pipes run.

A flow does **not** define:

* How inputs map to outputs (that is the role of pipes)
* How individual steps are executed (that is the role of actions)

Flows allow projects to specify multi-stage processing pipelines, e.g.:

::

    compile objects  →  link executable  →  generate documentation


2.5 Configuration Stage
-----------------------

Before any pipes or flows are evaluated, |PIPE| executes a **configuration
stage**.

The configuration stage:

* Allows inspecting the environment (e.g., OS, platform, tool availability)
* Allows defining variables and selecting build variants
* May make conditional assignments
* **May not** change pipe structure, mappings, or evaluation rules

The result of the configuration stage is a fully-resolved, static |PIPE| file
that will be executed deterministically.

No conditionals or logic are evaluated **after** the configuration stage.


2.6 Variables and Parameters
----------------------------

A **variable** is a named value that may be referenced within a scope.
Variables may be:

* **Scalar values** (a single string)
* **Lists of values** (zero or more strings)

A **parameter** is a variable used as part of an action command, and is
**always treated as a list**, even when it contains only a single element.

List coercion occurs **by context only**. A scalar becomes a list only when:

* Used with list formatting directives
* Used as part of a multi-file mapping
* Inserted into a parameter that expects multiple values

2.6.1. Evaluation rules
~~~~~~~~~~~~~~~~~~~~~~~

Variable and parameter values are **resolved deterministically**. |PIPE| does
**not** perform recursive or lazy expansion. Once a variable is resolved at the
moment it is needed, its value does not change unless explicitly reassigned by
the user.

Evaluation timing follows two categories:

1. **Configuration-Time Evaluation**
   Pure and system directives, list operations, and conditionals are resolved
   during the configuration stage.

2. **Execution-Time Reflection (File Matching Only)**
   File matching expressions (such as ``match("src/**/*.c")`` or explicit glob
   patterns in mappings) are treated as **environment reflection, not logic**.
   They are allowed to evaluate during execution because they produce **data**
   but do **not** affect control flow or conditional branching.

   This allows |PIPE| to observe which files exist **without influencing
   program logic**, preserving determinism.

   Example::

      # Allowed, reflection only
      sources = match("src/**/*.c")

   But the following is forbidden::

      # Forbidden, because file existence affects control flow
      if match("src/**/*.c") is empty { ... }

This distinction ensures:

* Deterministic control flow
* Environment-dependent input discovery remains safe
* Parallel execution and atomic planning remain mathematically tractable


2.7 Scopes
----------

A **scope** is a bounded evaluation environment in which variables, parameters, directives
and definitions exist and may be evaluated. A scope determines **where** names are visible and
**when** evaluation occurs.

The following entities form scopes:

* The **configuration stage** (root scope)
* Each **flow**
* Each **pipe**
* Each **action**

Scopes are **hierarchical** and **nested**:

    Configuration → Flow → Pipe → Action

Variables defined in an outer scope are **visible** to inner scopes unless
explicitly shadowed.

Variables do **not** propagate upward: an inner scope cannot modify or define
variables in an outer scope.

Evaluation always proceeds from the outermost (configuration) scope inward,
resulting in a fully-resolved, static execution model


2.8 File Matching and Filesets
------------------------------

Files and directory trees may be matched using:

* `*` — wildcard within a directory
* `**` — recursive directory match

Example::

    src/**/*.c

This expands to a **fileset**, resolved when the pipe executes.
A fileset is a list. Any operations that can be performed on lists
can be performed on filesets.


File matching is **environment reflection**, not logic, and is permitted
during execution. File globs specified in a pipe are evaluated **immediately
before the pipe begins execution**, allowing the pipe to operate as a single
atomic processing unit.

Therefore:

* All input files that a pipe operates on must already exist
* A pipe may not rely on files produced by another concurrently executing pipe
* A pipe's input files may not be created during the execution of that pipe

This preserves determinism and enables parallel execution safely because
file discovery is data acquisition, not control flow

2.8.1 Match Policies
~~~~~~~~~~~~~~~~~~~~

Every `*` or `**` match uses one of two match policies:

* **require-one** (default): at least one file must match
* **allow-empty**: zero matches is permitted

The policy is controlled syntactically:

* Must match >= 1 file

    src/\*\*/\*.c

* May match 0 or more files without error:

    src/\*\*/\*.c?

If a pattern using *require-one* matches zero files, the pipe **fails**.

2.8.2 Explicit Filenames
~~~~~~~~~~~~~~~~~~~~~~~~

If an input path does **not** contain `*` or `**`, it is treated as
an explicit file reference.

**Explicit inputs must exist.**
If they do not, Pipe halts and reports an error.

This behavior is not adjustable.


2.9 Directives
--------------

A **directive** is a built-in operation function that may be applied to variables
or lists.

Directives fall into three classes:

**Pure Directives**
    *No side effects.*
    Operate on strings, lists, and paths.
    May be used anywhere.

    Example: ``format()``

**System Directives**
    May inspect or query the environment.
    May **interact with the user** or perform I/O.
    **Restricted to the configuration stage only.**

**Logging Directives**
    **Logging Directives**
    May write to stdout or to a file designated during configuration.  
    Output targets must be selected at configuration time.  
    Logging directives are allowed during execution, but:
    - They may not introduce branching behavior
    - They may not modify variables or state

During pipe and flow execution:

* No system interaction occurs
* No new variables are defined
* No decision-making happens

All branching must be resolved during the configuration stage.
Execution is purely mechanical and deterministic.


2.9 Atomic Execution and Parallelism
------------------------------------

A **pipe** is the smallest executable processing unit. A pipe may be
declared **atomic**, indicating that:

* It does not depend on outputs of any other pipe
* It does not produce outputs required by any other pipe in the same flow

An atomic pipe may be executed at any time, in any order, and on any
available execution core.

A **flow** is a sequence of pipes. A flow may itself be marked **atomic**.
An atomic flow must execute its internal pipes strictly in order, but the
flow as a whole may be scheduled alongside other flows.

Parallel execution rules:

* A flow may execute multiple **atomic pipes** in parallel.
* A flow may only parallelize itself if **all** contained operations are atomic.
* If any pipe in a flow is non-atomic, the entire flow becomes sequentially ordered.

Pipe does **not** infer dependencies.  
If the user requires ordering constraints, the user must express them explicitly
by marking operations as non-atomic.



3. Configuration Stage
======================

The configuration stage establishes all global, user-controlled settings
before any pipes or flows are executed. It determines the **execution environment**
but does **not** perform any file-processing actions. Configuration is declared
in a dedicated ``config { ... }`` block located anywhere in the file, though
conventionally at the top.

::

   config {
       ...
   }

The configuration stage, if not cached, is evaluated **once** before any execution or planning
phase. Its output is a **finalized global variable context**.


3.1 Syntax Overview
-------------------

Configuration statements use a **block** enclosed in braces:

::

   config {
       variable_name  operator  value
       ...
   }

Values may be:

* Strings
* Lists (comma-separated)
* Directive outputs

Whitespace is not semantically meaningful except to separate tokens.


3.2 Assignment Operators
------------------------

There are **four** assignment operators:

+-------------+-----------------------------------------------+
| Operator    | Meaning                                       |
+=============+===============================================+
| ``=:`       | Define or replace the variable value.          |
+-------------+-----------------------------------------------+
| ``+:``      | Append one or more elements to a list value.  |
+-------------+-----------------------------------------------+
| ``-:``      | Remove one or more elements from a list value.|
+-------------+-----------------------------------------------+
| ``:``       | Same as ``=:``                                |
+-------------+-----------------------------------------------+

Examples::

   config {
       mode =: debug
       include_paths +: src/include, vendor/include
       features -: experimental
   }


3.3 Variables and Defaults
--------------------------

Variables defined in the configuration stage form a **global namespace**.

Pipes, flows, and actions may also define variables. A variable declared using
``default`` acts as a **fallback**: it is only used if no instance of an identically
named varaible exists in a higher-order scope.

Example::

   config {
       platform =: windows
   }

   pipe compile {
       default platform =: linux
   }

Within ``compile``, the value of ``platform`` is ``windows`` because config
overrides defaults.

If a variables is declared  **without** ``default``, that assignment takes
precedence over config::

   pipe compile {
       platform =: linux   # explicit override
   }

|PIPE| always resolves variables using the following precedence:

1. Local explicit assignment
1. higher-order scope explicit assignments
2. Config assignments
3. Local default assignment
4. higher-order scope default assignments

Note that accessing the value of a non-default paramater from a higher order scope is possible, 
but must be stated explcitly using the keyword ``use``.


3.4 Conditionals
----------------

Conditionals are allowed within the configuration stage only. They may use:

* Equality comparison (``==`` and ``!=``)
* Boolean operators: ``and``, ``or``, ``not``
* Parentheses for grouping

Example::

   config {
       platform =: detect_platform()

       if (platform == windows) {
           cflags +: -DWINDOWS
           linker_flags +: /MACHINE:X64
       }

       if (platform == linux and mode == debug) {
           cflags +: -DLINUX_DEBUG
           linker_flags +: -pthread
       }
   }

Conditionals must evaluate to a **deterministic result** at configuration time.
No runtime logic may appear inside conditional
expressions.


3.5 Directives in Configuration
-------------------------------

Directives are deterministic functions that operate on literal or already-
defined variable values. They are classified into three types:

* **Pure directives**  
  Have no side effects; always return the same result for the same input.

* **System directives**  
  Read environment information (e.g., OS, toolchain version). Side effects are
  limited to **inspection only**.

* **Logging directives**  
  Produce user-visible output for debugging or introspection during config.

Examples::

   platform =: detect_platform()      # System directive
   out_dir =: normalize_path(build/)  # Pure directive
   log(using polatform: $(platform))  # Logging directive


3.6 File Matching in Configuration
----------------------------------

File matching patterns (e.g., ``src/**/*.c``) are **environment reflection**.
An environnment reflection is allowed to be evaluated at run-time, but may 
not influence any form of control logic outside of the config stage.

File matching results may be converted to variables, and comparisons on these
lists are allowed **during configuration**, enabling deterministic decisions:

::

   config {
       sources = match("src/**/*.c")
       if sources is empty {
           log("Warning: no source files found.")
       }
   }

Late branching or conditionals based on file existence **inside a pipe or flow**
is forbidden.


3.7 Atomicity and Parallel Execution Planning
---------------------------------------------

The configuration stage also determines **atomic execution boundaries**.

A construct may declare itself ``atomic``::

   pipe compile : atomic {
       ...
   }

``atomic`` asserts:

* The construct does **not** depend on artifacts created by other constructs.
* It may be executed in parallel or reordered freely.

Flows may also be declared ``atomic``; in that case, all operations inside the
flow must also be atomic. Non-atomic flows execute their internal pipes
**in the declared sequence**.

This enables full parallel execution planning **without requiring dependency
graphs**, preserving determinism and user control.




