from __future__ import annotations

import argparse
import datetime as dt
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "review_reports"


def git(*args: str) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or f"git {' '.join(args)} failed")
    return result.stdout.strip()


def get_scope_payload(scope: str) -> tuple[str, str, str]:
    if scope == "staged":
        return (
            git("diff", "--staged", "--name-only"),
            git("diff", "--staged", "--stat"),
            git("diff", "--staged"),
        )
    if scope == "unstaged":
        return (
            git("diff", "--name-only"),
            git("diff", "--stat"),
            git("diff"),
        )

    staged_names = git("diff", "--staged", "--name-only")
    unstaged_names = git("diff", "--name-only")
    names = "\n".join([p for p in [staged_names, unstaged_names] if p]).strip()
    staged_stat = git("diff", "--staged", "--stat")
    unstaged_stat = git("diff", "--stat")
    stat = "\n".join([p for p in ["## Staged", staged_stat, "", "## Unstaged", unstaged_stat] if p]).strip()
    diff = "\n\n".join(
        [
            part
            for part in [
                "## Staged\n" + git("diff", "--staged"),
                "## Unstaged\n" + git("diff"),
            ]
            if part.strip()
        ]
    )
    return names, stat, diff


def main() -> None:
    parser = argparse.ArgumentParser(description="Export PR review snapshot")
    parser.add_argument("--scope", choices=["all", "staged", "unstaged"], default="all")
    parser.add_argument("--output", help="Output markdown path")
    parser.add_argument("--diff-lines", type=int, default=400, help="Maximum diff lines to include")
    args = parser.parse_args()

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    timestamp = dt.datetime.now().strftime("%Y%m%d_%H%M%S")
    output = Path(args.output) if args.output else REPORT_DIR / f"pr_snapshot_{args.scope}_{timestamp}.md"
    if not output.is_absolute():
        output = ROOT / output

    branch = git("branch", "--show-current")
    status = git("status", "--short")
    recent = git("log", "--oneline", "-5")
    names, stat, diff = get_scope_payload(args.scope)
    diff_lines = diff.splitlines()
    if len(diff_lines) > args.diff_lines:
        diff = "\n".join(diff_lines[: args.diff_lines]) + "\n\n... diff truncated ..."

    content = f"""# PR Review Snapshot

- Generated: {dt.datetime.now().isoformat(timespec='seconds')}
- Branch: {branch}
- Scope: {args.scope}

## Git Status

```text
{status or '(clean)'}
```

## Recent Commits

```text
{recent}
```

## Changed Files

```text
{names or '(no changed files in selected scope)'}
```

## Diff Stat

```text
{stat or '(no diff stat available)'}
```

## Diff Excerpt

```diff
{diff or '(no diff available)'}
```
"""
    output.write_text(content, encoding="utf-8")
    print(output)


if __name__ == "__main__":
    main()
