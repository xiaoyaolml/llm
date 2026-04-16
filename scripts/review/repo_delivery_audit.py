from __future__ import annotations

import argparse
import datetime as dt
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "review_reports"

CHECKS = [
    ("AGENTS.md", True),
    (".cursor/rules", True),
    (".clang-format", True),
    (".clang-tidy", True),
    ("CMakePresets.json", True),
    (".vscode/tasks.json", True),
    ("cursor_notes/playbooks", True),
    ("scripts/review", True),
    ("review_reports", True),
    ("README.md", True),
    ("docs", False),
    ("tests", False),
    ("benchmarks", False),
    ("CMakeLists.txt", False),
]


def main() -> None:
    parser = argparse.ArgumentParser(description="Audit repository delivery readiness")
    parser.add_argument("--output", help="Output markdown path")
    args = parser.parse_args()

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    timestamp = dt.datetime.now().strftime("%Y%m%d_%H%M%S")
    output = Path(args.output) if args.output else REPORT_DIR / f"repo_delivery_audit_{timestamp}.md"
    if not output.is_absolute():
        output = ROOT / output

    passed_required = []
    missing_required = []
    optional_present = []
    optional_missing = []

    for rel, required in CHECKS:
        path = ROOT / rel
        exists = path.exists()
        if required and exists:
            passed_required.append(rel)
        elif required and not exists:
            missing_required.append(rel)
        elif not required and exists:
            optional_present.append(rel)
        else:
            optional_missing.append(rel)

    status = "PASS" if not missing_required else "WARN"
    content = f"# Repository Delivery Audit\n\n- Generated: {dt.datetime.now().isoformat(timespec='seconds')}\n- Status: {status}\n\n## Required: Present\n\n" + "\n".join(f"- [x] {item}" for item in passed_required) + "\n\n## Required: Missing\n\n" + ("\n".join(f"- [ ] {item}" for item in missing_required) or "- (none)") + "\n\n## Optional: Present\n\n" + ("\n".join(f"- [x] {item}" for item in optional_present) or "- (none)") + "\n\n## Optional: Missing\n\n" + ("\n".join(f"- [ ] {item}" for item in optional_missing) or "- (none)") + "\n\n## Suggested Next Steps\n\n"

    if missing_required:
        content += "\n".join(f"- Add {item}" for item in missing_required)
    else:
        content += "- Required baseline files are present. Continue with tests, docs, and benchmark coverage."

    output.write_text(content, encoding="utf-8")
    print(output)


if __name__ == "__main__":
    main()
