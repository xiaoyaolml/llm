# AGENTS.md

## Project Identity

- This repository is treated as a C++ server-backend workspace.
- Prioritize stability, debuggability, predictable latency, and low-risk iteration.
- Common concerns include networking, RPC, concurrency, timeouts, cancellation, logging, metrics, tracing, and performance.

## Working Style

- Prefer minimal, local, low-risk changes.
- Understand the existing implementation before editing it.
- Reuse existing patterns and utilities before introducing new abstractions.
- Do not perform broad refactors unless explicitly requested.
- Keep naming, file organization, layering, and error handling consistent with nearby code.
- If requirements are ambiguous, choose the smallest correct solution and state assumptions.

## Architecture

- Keep transport, protocol, business logic, storage, and utilities separated where the existing codebase allows.
- Avoid unnecessary cross-module coupling.
- Prefer explicit ownership and dependency injection over hidden global mutable state.
- If adding a new component, explain where it belongs in the architecture and why.

## Concurrency and Lifetime

- Before changing shared-state code, explain thread-safety impact.
- Be explicit about ownership, destruction order, callback lifetime, and shutdown behavior.
- Call out whether code runs on IO threads, worker threads, timer threads, or arbitrary threads.
- Avoid detached threads unless explicitly requested.
- Avoid capturing raw pointers in async callbacks unless lifetime is clearly guaranteed.
- Explain lock ordering when multiple locks may be involved.
- Explain memory ordering when atomics are used or modified.
- Highlight risks such as deadlock, race conditions, ABA, use-after-free, double-close, or re-entrancy.

## Performance

- Treat request processing, parsing, serialization, scheduling, and hot-path logging as performance-sensitive.
- Avoid unnecessary allocations, string copies, and format conversions on hot paths.
- Avoid expensive work while holding locks.
- Mention complexity changes when touching loops, maps, queues, parsers, or batching logic.
- If a change may affect throughput or tail latency, say so explicitly.
- Prefer measurable optimizations over speculative rewrites.
- When performance is the goal, propose benchmark or profiling validation.

## Networking and RPC

- Preserve protocol and RPC compatibility unless a breaking change is explicitly requested.
- Be explicit about framing, partial packets, retries, deadlines, cancellation, and backpressure.
- Do not block IO threads with long-running work.
- When changing protocol parsing or serialization, consider malformed input, truncated packets, and boundary cases.
- For retry logic, mention idempotency assumptions.
- For connection lifecycle changes, explain behavior on normal close, timeout, and error paths.

## Error Handling

- Do not swallow errors silently.
- Preserve existing error-propagation conventions unless there is a strong reason to improve them.
- Include actionable context in logs and error returns.
- If adding a new failure path, consider whether metrics, tracing, or alerts should also be updated.

## Logging, Metrics, and Tracing

- Prefer structured logging.
- Include stable identifiers when useful, such as request_id, session_id, peer, shard_id, and error_code.
- Do not log secrets, tokens, credentials, or sensitive payloads.
- For hard-to-reproduce issues, prefer the smallest observability improvement that improves diagnosis.

## Testing and Verification

- Add or update tests when behavior changes.
- Follow existing test patterns, naming, and fixture style.
- Do not weaken tests just to make the implementation pass.
- For bug fixes, add a regression test when practical.
- For parser or protocol changes, include malformed, truncated, and boundary-condition cases.
- For concurrency changes, mention whether stress tests, repeated runs, sanitizers, or benchmarks are needed.
- Suggest validation steps such as build, unit tests, integration tests, benchmarks, and static analysis.

## Change Scope Discipline

- Keep unrelated cleanup out of functional changes unless explicitly requested.
- Avoid renaming symbols unless necessary for correctness or clarity.
- Avoid changing public interfaces unless required.
- If a task becomes much larger than expected, stop and summarize the expanded scope before continuing.

## Documentation

- For non-trivial changes, summarize what changed, why, risks, and how to verify.
- When useful, produce concise design notes, flow summaries, rollout notes, or regression checklists.
- Prefer concrete explanations over generic prose.

## Preferred Workflow

- First understand the existing implementation.
- Then propose a plan for non-trivial changes.
- Then implement in small verifiable steps.
- Then run or suggest validation.
- Then summarize diffs, risks, and next steps.
