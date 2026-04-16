from __future__ import annotations

import argparse
import datetime as dt
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_DIR = ROOT / "review_reports"

TEMPLATES = {
    "cpp-pr": """先不要修改代码。\n请作为 C++ 服务端 PR 审查者，审查当前改动，并按以下结构输出：\n1. 正确性与边界条件\n2. 并发与生命周期风险\n3. 性能与尾延迟风险\n4. 协议 / API / 配置兼容性影响\n5. 日志、监控、trace 是否足够\n6. 测试缺口\n7. 必须阻塞合并的问题\n""",
    "concurrency": """先不要修改代码。\n请重点从并发与线程安全角度审查当前 PR：\n1. 共享状态读写点\n2. 锁顺序与死锁风险\n3. 回调生命周期\n4. 原子与内存序\n5. shutdown / close / cancel 路径\n6. 竞态、ABA、use-after-free 风险\n7. 回归测试建议\n""",
    "performance": """先不要修改代码。\n请重点从性能和低延迟角度审查当前 PR：\n1. 热路径和额外工作量\n2. 分配、拷贝、格式化开销\n3. 锁争用、队列积压、背压\n4. 网络 / I/O / 数据库放大效应\n5. P99/P999 风险\n6. benchmark / profiling 验证建议\n7. 是否值得阻塞合并\n""",
    "security": """先不要修改代码。\n请重点从安全和合规角度审查当前 PR：\n1. 输入验证与注入风险\n2. 认证 / 授权边界变化\n3. 敏感信息、日志、审计风险\n4. 配置默认值与暴露面\n5. 合规和审计链路影响\n6. 修复优先级\n""",
    "risk-protocol": """先不要修改代码。\n请重点从风险、数据、协议和 API 角度审查当前 PR：\n1. 数值精度、单位、边界条件\n2. 数据完整性、一致性、时效性\n3. 协议兼容性和错误语义\n4. API 调用方影响\n5. 回归测试缺口\n6. 上线风险\n""",
}


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate review prompt for Cursor")
    parser.add_argument("--mode", choices=sorted(TEMPLATES.keys()), default="cpp-pr")
    parser.add_argument("--target", default="当前改动", help="Optional target description")
    parser.add_argument("--output", help="Output file path")
    args = parser.parse_args()

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    timestamp = dt.datetime.now().strftime("%Y%m%d_%H%M%S")
    output = Path(args.output) if args.output else REPORT_DIR / f"review_prompt_{args.mode}_{timestamp}.md"
    if not output.is_absolute():
        output = ROOT / output

    prompt = TEMPLATES[args.mode].replace("当前 PR", f"{args.target} PR").replace("当前改动", args.target)
    content = f"# Review Prompt: {args.mode}\n\n```text\n{prompt}\n```\n"
    output.write_text(content, encoding="utf-8")
    print(output)


if __name__ == "__main__":
    main()
