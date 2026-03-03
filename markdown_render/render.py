#!/usr/bin/env python3
"""
Markdown to WeChat-style HTML Renderer
=======================================
简易版微信公众号风格 Markdown 渲染器，模仿 Doocs MD 编辑器的输出样式。

支持的 Markdown 特性：
  - 标题 (h1-h6)
  - 段落、粗体、斜体、删除线、行内代码
  - 有序/无序列表（含嵌套）
  - 链接、图片
  - 引用块（含嵌套）
  - 代码块（语法高亮）
  - 表格
  - 分割线
  - LaTeX 公式（行内/块级，渲染为 SVG 需要网络）
  - Mermaid 图表（渲染为 SVG 需要网络）
  - HTML 原样透传

用法：
  python render.py [input.md] [output.html]
    默认: input.md -> input.html
"""

import re
import sys
import html as html_module
import base64

# ============================================================
# 主题配置 - 修改此处可全局更换主题色
# ============================================================
THEME_COLOR = "#FA5151"
TEXT_COLOR = "#3f3f3f"
LINK_COLOR = "#576b95"
CODE_INLINE_COLOR = "#d14"
CODE_BG = "rgba(27, 31, 35, 0.05)"
CODE_BLOCK_BG = "#0d1117"
CODE_BLOCK_TEXT = "#c9d1d9"
BLOCKQUOTE_BG = "#f7f7f7"
FONT_FAMILY = (
    "-apple-system-font, BlinkMacSystemFont, 'Helvetica Neue', "
    "'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei UI', "
    "'Microsoft YaHei', Arial, sans-serif"
)

# ============================================================
# 样式模板
# ============================================================
STYLES = {
    "container": (
        f"font-family: {FONT_FAMILY}; font-size: 16px; "
        f"line-height: 1.75; text-align: left;"
    ),
    "h1": (
        f"display: table; border-bottom: 2px solid {THEME_COLOR}; "
        f"margin: 2em auto 1em; font-weight: bold; text-align: center; "
        f"padding: 0.5em 1em; font-size: 22.4px; "
        f"text-shadow: 1px 1px 3px rgba(0, 0, 0, 0.05); "
        f"color: {THEME_COLOR}; background: transparent;"
    ),
    # "h2": (
    #     f"margin: 4em auto 2em; font-weight: bold; display: block; "
    #     f"text-align: left; background: transparent; padding-bottom: 0.3em; "
    #     f"border-bottom: 2px solid {THEME_COLOR}; color: {THEME_COLOR}; "
    #     f"padding: 0.3em 1.2em; font-size: 20.8px; "
    #     f"border-radius: 8px 24px 8px 24px; "
    #     f"box-shadow: 0 2px 6px rgba(0, 0, 0, 0.06);"
    # ),
    "h2": (
        f"margin: 2em 8px 0.75em 0; color: {TEXT_COLOR}; font-weight: bold; "
        f"padding-left: 12px; font-size: 19.2px; border-radius: 6px; "
        f"line-height: 2.4em; border-left: 4px solid {THEME_COLOR}; "
        f"border-right: 1px solid color-mix(in srgb, {THEME_COLOR} 10%, transparent); "
        f"border-bottom: 1px solid color-mix(in srgb, {THEME_COLOR} 10%, transparent); "
        f"border-top: 1px solid color-mix(in srgb, {THEME_COLOR} 10%, transparent); "
        f"background: color-mix(in srgb, {THEME_COLOR} 8%, transparent);"
    ),
    "h3": (
        f"margin: 2em 8px 0.5em; font-weight: bold; display: block; "
        f"text-align: left; background: transparent; margin-left: 0; "
        f"padding-left: 10px; border-left: 4px solid {THEME_COLOR}; "
        f"color: {THEME_COLOR}; font-size: 17.6px; border-radius: 6px;"
    ),
    "h4": (
        f"margin: 1.5em 8px 0.5em; font-weight: bold; display: block; "
        f"text-align: left; background: transparent; margin-left: 0; "
        f"padding-left: 10px; border-left: 4px solid {THEME_COLOR}; "
        f"color: {THEME_COLOR}; font-size: 16px; border-radius: 6px;"
    ),
    "h5": (
        f"margin: 1.5em 8px 0.5em; display: block; text-align: left; "
        f"background: transparent; margin-left: 0; padding-left: 10px; "
        f"border-left: 4px solid {THEME_COLOR}; color: {THEME_COLOR}; "
        f"font-size: 16px; border-radius: 6px;"
    ),
    "h6": (
        f"margin: 1.5em 8px 0.5em; display: block; text-align: left; "
        f"background: transparent; margin-left: 0; padding-left: 10px; "
        f"border-left: 4px solid {THEME_COLOR}; color: {THEME_COLOR}; "
        f"font-size: 16px; border-radius: 6px;"
    ),
    "p": f"margin: 1.5em 8px; letter-spacing: 0.1em; color: {TEXT_COLOR};",
    "ul": (
        f"margin-left: 0; color: {TEXT_COLOR}; list-style: none; "
        f"padding-left: 1.5em;"
    ),
    "ol": f"margin-left: 0; color: {TEXT_COLOR}; padding-left: 1.5em;",
    "li": f"display: block; color: {TEXT_COLOR}; margin: 0.5em 8px;",
    "strong": (
        f"color: {THEME_COLOR}; font-weight: bold; font-size: inherit;"
    ),
    "em": "font-style: italic; font-size: inherit;",
    "codespan": (
        f"font-size: 90%; color: {CODE_INLINE_COLOR}; "
        f"background: {CODE_BG}; padding: 3px 5px; border-radius: 4px;"
    ),
    "code_block": (
        f"color: {CODE_BLOCK_TEXT}; background: {CODE_BLOCK_BG}; "
        f"font-size: 90%; overflow-x: auto; border-radius: 8px; "
        f"line-height: 1.5; margin: 10px 8px; "
        f"border: 1px solid rgba(0, 0, 0, 0.04); padding: 0 !important;"
    ),
    "code_inner": (
        f"font-size: 90%; border-radius: 4px; display: -webkit-box; "
        f"padding: 0.5em 1em 1em; overflow-x: auto; text-indent: 0; "
        f"color: inherit; background: none; white-space: pre; margin: 0; "
        f"font-family: 'Fira Code', Menlo, Operator Mono, Consolas, Monaco, monospace;"
    ),
    "blockquote": (
        f"border-left: 4px solid {THEME_COLOR}; border-radius: 6px; "
        f"background: {BLOCKQUOTE_BG}; margin-bottom: 1em; "
        f"font-style: italic; padding: 1em 1em 1em 2em; "
        f"color: rgba(0, 0, 0, 0.6); "
        f"border-bottom: 0.2px solid rgba(0, 0, 0, 0.04); "
        f"border-top: 0.2px solid rgba(0, 0, 0, 0.04); "
        f"border-right: 0.2px solid rgba(0, 0, 0, 0.04);"
    ),
    "blockquote_p": (
        f"display: block; font-size: 1em; letter-spacing: 0.1em; "
        f"color: {TEXT_COLOR}; margin: 0;"
    ),
    "link": f"color: {LINK_COLOR}; text-decoration: none;",
    "img": (
        "display: block; max-width: 100%; margin: 0.1em auto 0.5em; "
        "border-radius: 8px; border: 1px solid rgba(0, 0, 0, 0.04);"
    ),
    "figcaption": (
        "text-align: center; color: #888; font-size: 0.8em;"
    ),
    "hr": (
        "height: 1px; border: none; margin: 2em 0; "
        "background: linear-gradient(to right, rgba(0,0,0,0), "
        "rgba(0,0,0,0.1), rgba(0,0,0,0));"
    ),
    "table": f"color: {TEXT_COLOR}; margin-top: 0 !important;",
    "th": (
        f"border: 1px solid #dfdfdf; padding: 0.25em 0.5em; "
        f"color: {TEXT_COLOR}; word-break: keep-all; "
        f"background: rgba(0, 0, 0, 0.05);"
    ),
    "td": (
        f"border: 1px solid #dfdfdf; padding: 0.25em 0.5em; "
        f"color: {TEXT_COLOR}; word-break: keep-all;"
    ),
}

# Mac 风格代码块装饰
MAC_DOTS_SVG = (
    '<span style="display: flex; padding: 10px 14px 0;">'
    '<svg xmlns="http://www.w3.org/2000/svg" width="45px" height="13px" viewBox="0 0 450 130">'
    '<ellipse cx="50" cy="65" rx="50" ry="52" stroke="rgb(220,60,54)" '
    'stroke-width="2" fill="rgb(237,108,96)"></ellipse>'
    '<ellipse cx="225" cy="65" rx="50" ry="52" stroke="rgb(218,151,33)" '
    'stroke-width="2" fill="rgb(247,193,81)"></ellipse>'
    '<ellipse cx="400" cy="65" rx="50" ry="52" stroke="rgb(27,161,37)" '
    'stroke-width="2" fill="rgb(100,200,86)"></ellipse>'
    '</svg></span>'
)


# ============================================================
# 辅助函数
# ============================================================
def escape(text: str) -> str:
    """HTML 转义，但保留已有的 HTML 标签不转义。"""
    return html_module.escape(text)


def render_inline(text: str) -> str:
    """处理行内 Markdown 元素：粗体、斜体、删除线、行内代码、链接、图片、行内公式。"""
    # 行内代码（最先处理，避免内容被其他规则干扰）
    def replace_code(m):
        code = escape(m.group(1))
        code = code.replace('$', '&#36;')  # 防止 $ 泄漏到 LaTeX 正则
        return f'<code style="{STYLES["codespan"]}">{code}</code>'
    text = re.sub(r'`([^`]+)`', replace_code, text)

    # 图片
    def replace_img(m):
        alt, src = m.group(1), m.group(2)
        return (
            f'<figure style="margin: 1.5em 8px; color: {TEXT_COLOR};">'
            f'<img src="{src}" alt="{escape(alt)}" style="{STYLES["img"]}">'
            f'<figcaption style="{STYLES["figcaption"]}">{escape(alt)}</figcaption>'
            f'</figure>'
        )
    text = re.sub(r'!\[([^\]]*)\]\(([^)]+)\)', replace_img, text)

    # 链接
    def replace_link(m):
        label, href = m.group(1), m.group(2)
        return (
            f'<a href="{href}" title="{escape(label)}" '
            f'style="{STYLES["link"]}">{label}</a>'
        )
    text = re.sub(r'\[([^\]]+)\]\(([^)]+)\)', replace_link, text)

    # 行内 LaTeX 公式 $...$
    def replace_inline_math(m):
        formula = m.group(1)
        encoded = base64.b64encode(formula.encode('utf-8')).decode('ascii')
        display = escape(formula)
        return (
            f'<span class="katex-inline-render" data-formula-b64="{encoded}" '
            f'style="max-width: 100%; overflow-x: auto;">'
            f'<code style="font-style: italic; color: {THEME_COLOR};">{display}</code>'
            f'</span>'
        )
    text = re.sub(r'(?<!\$)\$([^\$]+?)\$(?!\$)', replace_inline_math, text)

    # 粗斜体 ***text*** 或 ___text___
    text = re.sub(
        r'\*\*\*(.+?)\*\*\*|___(.+?)___',
        lambda m: (
            f'<strong style="{STYLES["strong"]}">'
            f'<em style="{STYLES["em"]}">{m.group(1) or m.group(2)}</em></strong>'
        ),
        text,
    )

    # 粗体 **text** 或 __text__
    text = re.sub(
        r'\*\*(.+?)\*\*|__(.+?)__',
        lambda m: (
            f'<strong style="{STYLES["strong"]}">{m.group(1) or m.group(2)}</strong>'
        ),
        text,
    )

    # 斜体 *text* 或 _text_
    text = re.sub(
        r'(?<!\*)\*([^\*]+?)\*(?!\*)|(?<!_)_([^_]+?)_(?!_)',
        lambda m: (
            f'<em style="{STYLES["em"]}">{m.group(1) or m.group(2)}</em>'
        ),
        text,
    )

    # 删除线 ~~text~~
    text = re.sub(r'~~(.+?)~~', r'<del>\1</del>', text)

    return text


# ============================================================
# 块级解析器
# ============================================================
class MarkdownRenderer:
    """简易 Markdown -> 内联样式 HTML 渲染器"""

    def __init__(self):
        self.lines: list[str] = []
        self.pos = 0
        self.output: list[str] = []

    def render(self, md_text: str) -> str:
        self.lines = md_text.split('\n')
        self.pos = 0
        self.output = []
        self._parse_blocks()
        body = '\n'.join(self.output)

        return (
            f'<section class="container" style="{STYLES["container"]}">\n'
            f'{body}\n'
            f'</section>'
        )

    def _peek(self) -> str | None:
        if self.pos < len(self.lines):
            return self.lines[self.pos]
        return None

    def _advance(self) -> str:
        line = self.lines[self.pos]
        self.pos += 1
        return line

    def _parse_blocks(self):
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None:
                break

            # 空行
            if line.strip() == '':
                self._advance()
                continue

            # 首个 h1 去除 margin-top
            # 标题
            if re.match(r'^#{1,6}\s', line):
                self._parse_heading()
                continue

            # 代码块
            if line.strip().startswith('```'):
                self._parse_code_block()
                continue

            # 块级公式 $$
            if line.strip().startswith('$$'):
                self._parse_math_block()
                continue

            # 表格 (|开头)
            if line.strip().startswith('|'):
                self._parse_table()
                continue

            # 引用
            if line.strip().startswith('>'):
                self._parse_blockquote()
                continue

            # 无序列表
            if re.match(r'^(\s*)[-*+]\s', line):
                self._parse_unordered_list()
                continue

            # 有序列表
            if re.match(r'^(\s*)\d+\.\s', line):
                self._parse_ordered_list()
                continue

            # 分割线
            if re.match(r'^(-{3,}|\*{3,}|_{3,})\s*$', line.strip()):
                self._advance()
                self.output.append(f'<hr style="{STYLES["hr"]}">')
                continue

            # HTML 透传（以 < 开头且不是 markdown 语法）
            if line.strip().startswith('<') and not line.strip().startswith('<!['):
                self._parse_html_block()
                continue

            # 普通段落
            self._parse_paragraph()

    # ---------- 标题 ----------
    def _parse_heading(self):
        line = self._advance()
        m = re.match(r'^(#{1,6})\s+(.*)', line)
        if not m:
            return
        level = len(m.group(1))
        content = render_inline(m.group(2))
        tag = f'h{level}'
        style = STYLES.get(tag, STYLES["h6"])
        # 第一个 h1 去除 margin-top
        if tag == 'h1' and not any('class="h1"' in o for o in self.output):
            style += " margin-top: 0 !important;"
        self.output.append(f'<{tag} style="{style}">{content}</{tag}>')

    # ---------- 代码块 ----------
    def _parse_code_block(self):
        line = self._advance()
        m = re.match(r'^```(\w*)', line.strip())
        lang = m.group(1) if m else ''
        code_lines = []

        while self.pos < len(self.lines):
            line = self._advance()
            if line.strip() == '```':
                break
            code_lines.append(line)

        code_text = '\n'.join(code_lines)

        # Mermaid 图表特殊处理
        if lang == 'mermaid':
            self._render_mermaid(code_text)
            return

        escaped_code = escape(code_text)
        # 将空格替换为 &nbsp; 以保持缩进（微信兼容）
        escaped_code = escaped_code.replace('  ', '&nbsp;&nbsp;')

        self.output.append(
            f'<pre style="{STYLES["code_block"]}">'
            f'{MAC_DOTS_SVG}'
            f'<code class="language-{lang}" style="{STYLES["code_inner"]}">'
            f'{escaped_code}'
            f'</code></pre>'
        )

    # ---------- Mermaid ----------
    def _render_mermaid(self, code: str):
        """渲染 Mermaid 为 pre 代码块（纯文本降级），
        如需 SVG 渲染，建议使用 mermaid CLI 或在线 API。
        """
        # 降级：用一个提示块 + 代码展示
        escaped = escape(code)
        self.output.append(
            f'<div class="mermaid-placeholder" style="'
            f'background: #f6f8fa; border: 1px solid #e1e4e8; '
            f'border-radius: 8px; padding: 16px; margin: 10px 8px; '
            f'text-align: center; color: #586069;">'
            f'<p style="margin: 0 0 8px; font-weight: bold; '
            f'color: {THEME_COLOR};">📊 Mermaid 图表</p>'
            f'<pre style="text-align: left; font-size: 13px; '
            f'color: {TEXT_COLOR}; background: none; margin: 0; '
            f'white-space: pre-wrap;">{escaped}</pre>'
            f'<p style="margin: 8px 0 0; font-size: 12px; color: #888;">'
            f'提示：在浏览器中打开可启用 Mermaid.js 渲染</p>'
            f'</div>'
        )

    # ---------- 块级公式 ----------
    def _parse_math_block(self):
        self._advance()  # skip opening $$
        formula_lines = []
        while self.pos < len(self.lines):
            line = self._peek()
            if line is not None and line.strip() == '$$':
                self._advance()
                break
            formula_lines.append(self._advance())

        formula = '\n'.join(formula_lines).strip()
        if not formula:
            return

        encoded = base64.b64encode(formula.encode('utf-8')).decode('ascii')
        escaped = escape(formula)
        self.output.append(
            f'<section class="katex-block-render" data-formula-b64="{encoded}" style="'
            f'font-family: {FONT_FAMILY}; font-size: 16px; line-height: 1.75; '
            f'max-width: 100%; overflow-x: auto; padding: 0.5em 0; '
            f'text-align: center;">'
            f'<code style="font-size: 14px; color: {TEXT_COLOR}; '
            f'background: rgba(27, 31, 35, 0.05); padding: 8px 16px; '
            f'border-radius: 4px; display: inline-block; '
            f'white-space: pre-wrap; text-align: left;">'
            f'{escaped}</code></section>'
        )

    # ---------- 表格 ----------
    def _parse_table(self):
        rows: list[list[str]] = []
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None or not line.strip().startswith('|'):
                break
            self._advance()
            # 跳过分隔行
            if re.match(r'^\|[\s\-:|]+\|$', line.strip()):
                continue
            cells = [c.strip() for c in line.strip().strip('|').split('|')]
            rows.append(cells)

        if not rows:
            return

        table_style = (
            f"font-family: {FONT_FAMILY}; font-size: 16px; line-height: 1.75; "
            f"text-align: left; max-width: 100%; overflow: auto;"
        )

        html_parts = [
            f'<section style="{table_style}">',
            f'<table style="{STYLES["table"]}">'
        ]

        # 表头
        html_parts.append(f'<thead style="font-weight: bold; color: {TEXT_COLOR};">')
        html_parts.append('<tr>')
        for cell in rows[0]:
            html_parts.append(f'<th style="{STYLES["th"]}">{render_inline(cell)}</th>')
        html_parts.append('</tr></thead>')

        # 表体
        html_parts.append('<tbody>')
        for row in rows[1:]:
            html_parts.append('<tr>')
            for cell in row:
                html_parts.append(f'<td style="{STYLES["td"]}">{render_inline(cell)}</td>')
            html_parts.append('</tr>')
        html_parts.append('</tbody></table></section>')

        self.output.append('\n'.join(html_parts))

    # ---------- 引用 ----------
    def _parse_blockquote(self):
        bq_lines = []
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None:
                break
            if line.strip().startswith('>'):
                self._advance()
                # 去掉首个 >
                content = re.sub(r'^>\s?', '', line, count=1)
                bq_lines.append(content)
            elif line.strip() == '':
                # 空行可能结束引用或属于引用内部
                if self.pos + 1 < len(self.lines) and self.lines[self.pos + 1].strip().startswith('>'):
                    self._advance()
                    bq_lines.append('')
                else:
                    break
            else:
                break

        # 检测嵌套引用
        inner_text = '\n'.join(bq_lines)
        if re.search(r'^>', inner_text, re.MULTILINE):
            # 递归渲染嵌套引用
            sub_renderer = MarkdownRenderer()
            inner_html = sub_renderer.render(inner_text)
            # 去掉外层 container wrapper
            inner_html = re.sub(
                r'^<section[^>]*>|</section>$', '', inner_html.strip()
            ).strip()
        else:
            inner_html = f'<p style="{STYLES["blockquote_p"]}">{render_inline(inner_text.strip())}</p>'

        self.output.append(
            f'<blockquote style="{STYLES["blockquote"]}">{inner_html}</blockquote>'
        )

    # ---------- 无序列表 ----------
    def _parse_unordered_list(self, indent=0):
        items = []
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None or line.strip() == '':
                # 空行可能结束列表
                if (self.pos + 1 < len(self.lines) and
                    re.match(r'^(\s*)[-*+]\s', self.lines[self.pos + 1]) and
                    len(re.match(r'^(\s*)', self.lines[self.pos + 1]).group(1)) >= indent):
                    self._advance()
                    continue
                break

            m = re.match(r'^(\s*)[-*+]\s+(.*)', line)
            if m:
                cur_indent = len(m.group(1))
                if cur_indent < indent:
                    break
                if cur_indent > indent:
                    # 嵌套 - 检查是有序还是无序
                    nested_m = re.match(r'^(\s*)\d+\.\s', line)
                    if nested_m:
                        sub_html = self._collect_ordered_list(cur_indent)
                        items.append(('nested_ol', sub_html))
                    else:
                        sub_html = self._collect_unordered_list(cur_indent)
                        items.append(('nested_ul', sub_html))
                    continue
                self._advance()
                items.append(('item', m.group(2)))
            elif re.match(r'^(\s*)\d+\.\s', line):
                m2 = re.match(r'^(\s*)', line)
                cur_indent = len(m2.group(1))
                if cur_indent > indent:
                    sub_html = self._collect_ordered_list(cur_indent)
                    items.append(('nested_ol', sub_html))
                    continue
                break
            else:
                break

        html_parts = [f'<ul style="{STYLES["ul"]}">']
        for kind, content in items:
            if kind == 'item':
                html_parts.append(
                    f'<li style="{STYLES["li"]}">• {render_inline(content)}</li>'
                )
            elif kind == 'nested_ul':
                html_parts.append(content)
            elif kind == 'nested_ol':
                html_parts.append(content)
        html_parts.append('</ul>')
        self.output.append('\n'.join(html_parts))

    def _collect_unordered_list(self, indent):
        items = []
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None or line.strip() == '':
                break
            m = re.match(r'^(\s*)[-*+]\s+(.*)', line)
            if m and len(m.group(1)) >= indent:
                self._advance()
                items.append(m.group(2))
            else:
                break
        html_parts = [f'<ul style="{STYLES["ul"]}">']
        for item in items:
            html_parts.append(
                f'<li style="{STYLES["li"]}">• {render_inline(item)}</li>'
            )
        html_parts.append('</ul>')
        return '\n'.join(html_parts)

    # ---------- 有序列表 ----------
    def _parse_ordered_list(self, indent=0):
        items = []
        counter = 1
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None or line.strip() == '':
                break
            m = re.match(r'^(\s*)\d+\.\s+(.*)', line)
            if m:
                cur_indent = len(m.group(1))
                if cur_indent < indent:
                    break
                if cur_indent > indent:
                    sub_html = self._collect_ordered_list(cur_indent)
                    items.append(('nested', sub_html))
                    continue
                self._advance()
                items.append(('item', m.group(2), counter))
                counter += 1
            else:
                break

        html_parts = [f'<ol style="{STYLES["ol"]}">']
        for entry in items:
            if entry[0] == 'item':
                _, content, num = entry
                html_parts.append(
                    f'<li style="{STYLES["li"]}">{num}. {render_inline(content)}</li>'
                )
            elif entry[0] == 'nested':
                html_parts.append(entry[1])
        html_parts.append('</ol>')
        self.output.append('\n'.join(html_parts))

    def _collect_ordered_list(self, indent):
        items = []
        counter = 1
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None or line.strip() == '':
                break
            m = re.match(r'^(\s*)\d+\.\s+(.*)', line)
            if m and len(m.group(1)) >= indent:
                self._advance()
                items.append((m.group(2), counter))
                counter += 1
            else:
                break
        html_parts = [f'<ol style="{STYLES["ol"]}">']
        for content, num in items:
            html_parts.append(
                f'<li style="{STYLES["li"]}">{num}. {render_inline(content)}</li>'
            )
        html_parts.append('</ol>')
        return '\n'.join(html_parts)

    # ---------- HTML 透传 ----------
    def _parse_html_block(self):
        html_lines = []
        # 收集连续的 HTML 行
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None:
                break
            if line.strip() == '' and html_lines:
                # 检查是否还有 HTML 接续
                if (self.pos + 1 < len(self.lines) and
                    self.lines[self.pos + 1].strip().startswith('<')):
                    html_lines.append(self._advance())
                    continue
                break
            if line.strip().startswith('<') or (html_lines and line.strip()):
                html_lines.append(self._advance())
            else:
                break
        self.output.append('\n'.join(html_lines))

    # ---------- 段落 ----------
    def _parse_paragraph(self):
        para_lines = []
        while self.pos < len(self.lines):
            line = self._peek()
            if line is None or line.strip() == '':
                break
            # 遇到块级元素标志时停止
            if (re.match(r'^#{1,6}\s', line) or
                line.strip().startswith('```') or
                line.strip().startswith('$$') or
                line.strip().startswith('>') or
                line.strip().startswith('|') or
                re.match(r'^(\s*)[-*+]\s', line) or
                re.match(r'^(\s*)\d+\.\s', line) or
                re.match(r'^(-{3,}|\*{3,}|_{3,})\s*$', line.strip()) or
                (line.strip().startswith('<') and not line.strip().startswith('<!'))):
                break
            para_lines.append(self._advance())

        if para_lines:
            content = render_inline(' '.join(para_lines))
            self.output.append(f'<p style="{STYLES["p"]}">{content}</p>')


# ============================================================
# 完整 HTML 页面模板（含 Mermaid.js CDN 支持）
# ============================================================
def wrap_full_html(body: str) -> str:
    return f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Markdown Rendered</title>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css">
<script src="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js"></script>
<style>
  body {{
    max-width: 800px;
    margin: 0 auto;
    padding: 20px;
    background: #fff;
  }}
  .mermaid-placeholder pre {{
    margin: 0;
  }}
  /* 复制按钮样式 */
  #copy-btn {{
    position: fixed;
    top: 20px;
    right: 20px;
    z-index: 9999;
    padding: 10px 20px;
    background: {THEME_COLOR};
    color: #fff;
    border: none;
    border-radius: 8px;
    font-size: 14px;
    cursor: pointer;
    box-shadow: 0 2px 8px rgba(0,0,0,0.15);
    transition: all 0.2s;
  }}
  #copy-btn:hover {{
    opacity: 0.85;
    transform: translateY(-1px);
    box-shadow: 0 4px 12px rgba(0,0,0,0.2);
  }}
  #copy-btn.copied {{
    background: #07c160;
  }}
  @media print {{
    #copy-btn {{ display: none; }}
  }}
</style>
</head>
<body>
<button id="copy-btn" onclick="copyRichText()">⏳ 渲染中...</button>
<p style="font-size: 0px; line-height: 0; margin: 0px;">&nbsp;</p>
{body}
<p style="font-size: 0px; line-height: 0; margin: 0px;">&nbsp;</p>
<!-- KaTeX 渲染（同步，页面加载后立即执行） -->
<script>
(function() {{
  if (typeof katex === 'undefined') {{ console.warn('KaTeX not loaded'); return; }}
  document.querySelectorAll('.katex-inline-render').forEach(function(el) {{
    try {{
      var formula = atob(el.getAttribute('data-formula-b64'));
      katex.render(formula, el, {{ throwOnError: false, displayMode: false }});
    }} catch(e) {{ console.warn('KaTeX inline error:', e); }}
  }});
  document.querySelectorAll('.katex-block-render').forEach(function(el) {{
    try {{
      var formula = atob(el.getAttribute('data-formula-b64'));
      katex.render(formula, el, {{ throwOnError: false, displayMode: true }});
    }} catch(e) {{ console.warn('KaTeX block error:', e); }}
  }});
  var btn = document.getElementById('copy-btn');
  if (btn) btn.textContent = '\U0001f4cb \u590d\u5236\u5bcc\u6587\u672c';
}})();
</script>
<!-- Mermaid 渲染（异步模块，独立于 KaTeX，失败不影响公式） -->
<script type="module">
try {{
  const m = await import('https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs');
  const mermaid = m.default;
  mermaid.initialize({{ startOnLoad: false, theme: 'default' }});
  var placeholders = document.querySelectorAll('.mermaid-placeholder');
  if (placeholders.length > 0) {{
    placeholders.forEach(function(el) {{
      var code = el.querySelector('pre').textContent;
      var div = document.createElement('div');
      div.className = 'mermaid';
      div.textContent = code;
      el.replaceWith(div);
    }});
    await mermaid.run();
  }}
}} catch(e) {{
  console.warn('Mermaid render failed:', e);
}}
</script>
<!-- 复制功能 -->
<script>
function copyRichText() {{
  var container = document.querySelector('.container');
  if (!container) return;
  // 主要方法：选区复制（保留渲染后的视觉样式，富文本兼容性最佳）
  var range = document.createRange();
  range.selectNodeContents(container);
  var sel = window.getSelection();
  sel.removeAllRanges();
  sel.addRange(range);
  try {{
    var ok = document.execCommand('copy');
    sel.removeAllRanges();
    if (ok) {{ showCopied(); return; }}
  }} catch(e) {{}}
  sel.removeAllRanges();
  // 备用方法：Clipboard API
  if (navigator.clipboard && window.ClipboardItem) {{
    var blob = new Blob([container.outerHTML], {{ type: 'text/html' }});
    var item = new ClipboardItem({{ 'text/html': blob }});
    navigator.clipboard.write([item]).then(showCopied).catch(function() {{
      alert('\u590d\u5236\u5931\u8d25\uff0c\u8bf7\u624b\u52a8 Ctrl+A \u5168\u9009\u540e Ctrl+C \u590d\u5236');
    }});
  }} else {{
    alert('\u590d\u5236\u5931\u8d25\uff0c\u8bf7\u624b\u52a8 Ctrl+A \u5168\u9009\u540e Ctrl+C \u590d\u5236');
  }}
}}
function showCopied() {{
  var btn = document.getElementById('copy-btn');
  btn.textContent = '\u2705 \u5df2\u590d\u5236';
  btn.classList.add('copied');
  setTimeout(function() {{
    btn.textContent = '\U0001f4cb \u590d\u5236\u5bcc\u6587\u672c';
    btn.classList.remove('copied');
  }}, 2000);
}}
</script>
</body>
</html>"""


# ============================================================
# CLI 入口
# ============================================================
def main():
    input_file = sys.argv[1] if len(sys.argv) > 1 else 'input.md'
    if len(sys.argv) > 2:
        output_file = sys.argv[2]
    else:
        output_stem = input_file.rsplit('.', 1)[0] if '.' in input_file else input_file
        output_file = f'{output_stem}.html'

    with open(input_file, 'r', encoding='utf-8') as f:
        md_text = f.read()

    renderer = MarkdownRenderer()
    body_html = renderer.render(md_text)
    full_html = wrap_full_html(body_html)

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(full_html)

    print(f"✅ 渲染完成: {input_file} -> {output_file}")
    print(f"   请在浏览器中打开 {output_file} 查看效果")
    print(f"   (Mermaid 图表需要联网加载 JS)")


if __name__ == '__main__':
    main()
