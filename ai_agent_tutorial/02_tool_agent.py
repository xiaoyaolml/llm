"""
=============================================================
 Level 2: 带工具调用的 AI Agent (Function Calling)
=============================================================

🎯 学习目标：
   1. 理解什么是"工具调用"(Tool/Function Calling)
   2. 学会定义工具函数
   3. 让 Agent 自动选择并调用合适的工具
   4. 处理工具返回的结果

💡 核心概念：
   - Tool (工具): Agent 可以调用的外部函数
   - Function Calling: LLM 决定"要不要调用函数、调用哪个、参数是什么"
   - 工作流: 用户提问 → LLM 分析 → 选择工具 → 执行工具 → 返回结果

📝 为什么需要工具？
   LLM 本身只能生成文本，不能：
   - 做精确计算（大数运算容易出错）
   - 获取实时信息（训练数据有截止日期）
   - 操作外部系统（无法发邮件、查数据库）
   所以我们给 Agent 配备"工具箱"！
"""

import os
import json
import math
from datetime import datetime
from openai import OpenAI
from dotenv import load_dotenv

load_dotenv()

client = OpenAI(
    api_key=os.getenv("OPENAI_API_KEY", "your-api-key"),
    base_url=os.getenv("OPENAI_API_BASE", "https://api.openai.com/v1"),
)


# ==========================================
# 第一步：定义工具函数
# ==========================================
# 这些是 Agent 可以调用的"工具"
# 每个工具就是一个普通的 Python 函数

def calculate(expression: str) -> str:
    """
    计算数学表达式
    这是 Agent 的 "计算器工具"
    """
    try:
        # 安全地计算数学表达式
        # 注意：实际项目中需要更严格的安全检查
        allowed_names = {
            "abs": abs, "round": round,
            "min": min, "max": max,
            "sum": sum, "len": len,
            "pow": pow, "sqrt": math.sqrt,
            "sin": math.sin, "cos": math.cos,
            "tan": math.tan, "pi": math.pi,
            "e": math.e, "log": math.log,
        }
        result = eval(expression, {"__builtins__": {}}, allowed_names)
        return json.dumps({"result": result, "expression": expression})
    except Exception as e:
        return json.dumps({"error": str(e), "expression": expression})


def get_current_time(timezone: str = "UTC") -> str:
    """
    获取当前时间
    这是 Agent 的 "时钟工具"
    """
    now = datetime.now()
    return json.dumps({
        "current_time": now.strftime("%Y-%m-%d %H:%M:%S"),
        "timezone": timezone,
        "day_of_week": now.strftime("%A"),
    })


def search_knowledge_base(query: str) -> str:
    """
    搜索知识库（模拟）
    这是 Agent 的 "搜索工具"
    实际项目中可以连接数据库、向量数据库、搜索引擎等
    """
    # 模拟知识库
    knowledge = {
        "python": "Python 是一种高级编程语言，由 Guido van Rossum 创建于 1991 年。特点：简洁、易读、功能强大。",
        "agent": "AI Agent 是能够自主感知环境、做出决策并采取行动的智能实体。核心组件：LLM + 工具 + 记忆 + 规划。",
        "langchain": "LangChain 是一个用于开发 LLM 应用的框架，提供了链、Agent、记忆等抽象。",
        "react": "ReAct 是一种 Agent 决策模式：Reasoning（推理）+ Acting（行动），让 Agent 交替进行思考和工具调用。",
        "rag": "RAG (Retrieval-Augmented Generation) 是检索增强生成，先检索相关文档，再用 LLM 生成回答。",
    }
    
    # 简单的关键词匹配搜索
    results = []
    for key, value in knowledge.items():
        if query.lower() in key.lower() or key.lower() in query.lower():
            results.append({"topic": key, "content": value})
    
    if not results:
        return json.dumps({"message": "未找到相关信息", "query": query})
    
    return json.dumps({"results": results, "query": query})


def analyze_text(text: str, analysis_type: str = "summary") -> str:
    """
    文本分析工具
    支持：字数统计、关键词提取（模拟）
    """
    result = {
        "char_count": len(text),
        "word_count": len(text.split()),
        "line_count": text.count('\n') + 1,
        "analysis_type": analysis_type,
    }
    
    if analysis_type == "statistics":
        # 统计中文字符数
        chinese_chars = sum(1 for c in text if '\u4e00' <= c <= '\u9fff')
        result["chinese_char_count"] = chinese_chars
        result["english_word_count"] = result["word_count"] - chinese_chars
    
    return json.dumps(result)


# ==========================================
# 第二步：定义工具的描述（告诉 LLM 有哪些工具可用）
# ==========================================
# 这是 OpenAI Function Calling 的标准格式
# LLM 根据这些描述来决定使用哪个工具

TOOLS = [
    {
        "type": "function",
        "function": {
            "name": "calculate",
            "description": "计算数学表达式。当用户需要进行数学计算时使用此工具。支持基本运算(+,-,*,/)和数学函数(sqrt, sin, cos, log等)。",
            "parameters": {
                "type": "object",
                "properties": {
                    "expression": {
                        "type": "string",
                        "description": "要计算的数学表达式，例如: '2 + 3 * 4' 或 'sqrt(144)' 或 'pow(2, 10)'"
                    }
                },
                "required": ["expression"]
            }
        }
    },
    {
        "type": "function",
        "function": {
            "name": "get_current_time",
            "description": "获取当前日期和时间。当用户询问现在几点、今天日期等时间相关问题时使用。",
            "parameters": {
                "type": "object",
                "properties": {
                    "timezone": {
                        "type": "string",
                        "description": "时区，例如: 'UTC', 'Asia/Shanghai', 'US/Eastern'",
                        "default": "UTC"
                    }
                },
                "required": []
            }
        }
    },
    {
        "type": "function",
        "function": {
            "name": "search_knowledge_base",
            "description": "搜索知识库获取技术信息。当用户询问技术概念、编程知识时使用。",
            "parameters": {
                "type": "object",
                "properties": {
                    "query": {
                        "type": "string",
                        "description": "搜索关键词，例如: 'Python', 'Agent', 'LangChain'"
                    }
                },
                "required": ["query"]
            }
        }
    },
    {
        "type": "function",
        "function": {
            "name": "analyze_text",
            "description": "分析文本内容，提供字数统计等信息。",
            "parameters": {
                "type": "object",
                "properties": {
                    "text": {
                        "type": "string",
                        "description": "要分析的文本内容"
                    },
                    "analysis_type": {
                        "type": "string",
                        "description": "分析类型: 'summary'(概要) 或 'statistics'(详细统计)",
                        "enum": ["summary", "statistics"],
                        "default": "summary"
                    }
                },
                "required": ["text"]
            }
        }
    },
]

# 工具名称 → 函数的映射
TOOL_FUNCTIONS = {
    "calculate": calculate,
    "get_current_time": get_current_time,
    "search_knowledge_base": search_knowledge_base,
    "analyze_text": analyze_text,
}


# ==========================================
# 第三步：实现 Tool Agent
# ==========================================

class ToolAgent:
    """
    带工具调用能力的 AI Agent
    
    工作流程：
    1. 接收用户输入
    2. 发送给 LLM（附带工具列表）
    3. LLM 决定是否需要调用工具
       - 如果是：执行工具 → 将结果发回 LLM → LLM 生成最终回答
       - 如果否：LLM 直接回答
    """
    
    def __init__(self, model: str = "gpt-3.5-turbo"):
        self.model = model
        self.messages = [
            {
                "role": "system",
                "content": """你是一个能力强大的 AI 助手，你拥有以下工具：
1. 计算器 - 进行精确的数学计算
2. 时钟 - 获取当前时间
3. 知识库搜索 - 查找技术知识
4. 文本分析 - 分析文本内容

请根据用户的问题，选择合适的工具来回答。如果不需要工具，直接回答即可。
回答使用中文。"""
            }
        ]
    
    def chat(self, user_input: str) -> str:
        """与 Agent 对话，Agent 会自动决定是否使用工具"""
        
        self.messages.append({"role": "user", "content": user_input})
        
        print(f"\n🔄 正在思考...")
        
        # 第一次调用 LLM：让它决定要不要用工具
        response = client.chat.completions.create(
            model=self.model,
            messages=self.messages,
            tools=TOOLS,           # ← 告诉 LLM 有哪些工具
            tool_choice="auto",    # ← "auto" = LLM 自己决定要不要用工具
        )
        
        response_message = response.choices[0].message
        
        # 检查 LLM 是否想调用工具
        if response_message.tool_calls:
            # LLM 决定要用工具！
            print(f"🔧 Agent 决定使用工具！")
            
            # 将 LLM 的回复（包含工具调用信息）加入消息历史
            self.messages.append(response_message)
            
            # 执行每个工具调用
            for tool_call in response_message.tool_calls:
                function_name = tool_call.function.name
                function_args = json.loads(tool_call.function.arguments)
                
                print(f"   📌 调用工具: {function_name}")
                print(f"   📎 参数: {function_args}")
                
                # 执行工具函数
                if function_name in TOOL_FUNCTIONS:
                    function_result = TOOL_FUNCTIONS[function_name](**function_args)
                else:
                    function_result = json.dumps({"error": f"未知工具: {function_name}"})
                
                print(f"   ✅ 工具结果: {function_result[:100]}...")
                
                # 将工具结果加入消息历史
                self.messages.append({
                    "tool_call_id": tool_call.id,
                    "role": "tool",
                    "name": function_name,
                    "content": function_result,
                })
            
            # 第二次调用 LLM：让它根据工具结果生成最终回答
            print(f"🔄 Agent 正在根据工具结果生成回答...")
            second_response = client.chat.completions.create(
                model=self.model,
                messages=self.messages,
            )
            
            final_answer = second_response.choices[0].message.content
            self.messages.append({"role": "assistant", "content": final_answer})
            return final_answer
        
        else:
            # LLM 决定不用工具，直接回答
            print(f"💬 Agent 直接回答（不需要工具）")
            answer = response_message.content
            self.messages.append({"role": "assistant", "content": answer})
            return answer


# ==========================================
# 第四步：演示工具调用的核心逻辑（不需要 API）
# ==========================================

def demo_tool_calling():
    """
    演示工具调用的完整流程（不需要 API Key）
    """
    print("\n" + "="*60)
    print("📖 工具调用核心逻辑演示")
    print("="*60)
    
    print("""
🔍 场景：用户问 "2的10次方等于多少？"

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Step 1: 用户提问
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  用户: "2的10次方等于多少？"
  
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Step 2: 发送给 LLM（附带工具列表）
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  → 发送 messages + tools 给 LLM
  → LLM 分析问题，发现需要精确计算
  → LLM 返回: "我要调用 calculate 工具"
  
  LLM 的返回:
  {
    "tool_calls": [{
      "function": {
        "name": "calculate",        ← LLM 选择了计算器工具
        "arguments": '{"expression": "pow(2, 10)"}'  ← LLM 自动构造参数
      }
    }]
  }
  
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Step 3: 执行工具函数
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━""")
    
    # 实际执行计算
    result = calculate("pow(2, 10)")
    print(f"  执行: calculate(expression='pow(2, 10)')")
    print(f"  结果: {result}")
    
    print(f"""
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Step 4: 将结果发回 LLM
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  → 将工具结果加入 messages
  → 再次调用 LLM
  → LLM 根据结果生成自然语言回答:
  
  "2的10次方等于1024。"

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
""")
    
    print("🎓 关键收获:")
    print("  1. LLM 不直接执行工具，而是'告诉我们'要用什么工具和参数")
    print("  2. 我们的代码负责实际执行工具")
    print("  3. 工具结果发回 LLM，LLM 生成最终回答")
    print("  4. 整个过程调用了 2 次 LLM API")
    print()
    
    # 演示其他工具
    print("📦 工具演示：")
    print(f"  时钟: {get_current_time()}")
    print(f"  搜索: {search_knowledge_base('agent')}")
    print(f"  分析: {analyze_text('Hello 你好世界', 'statistics')}")


# ==========================================
# 主程序入口
# ==========================================
if __name__ == "__main__":
    print("\n🚀 AI Agent 教程 — Level 2: 工具调用 Agent")
    print("=" * 60)
    
    # 运行演示
    demo_tool_calling()
    
    # 交互模式
    print("\n是否启动交互式工具 Agent？(需要 API Key)")
    print("  1. 启动")
    print("  0. 退出")
    
    choice = input("\n请选择: ").strip()
    
    if choice == "1":
        agent = ToolAgent()
        print("\n🤖 工具 Agent 已启动！")
        print("   试试：'计算 sqrt(144) + 2^10'")
        print("   试试：'现在几点了？'")
        print("   试试：'帮我查一下什么是 RAG'")
        print("   输入 'quit' 退出\n")
        
        while True:
            user_input = input("👤 你: ").strip()
            if user_input.lower() == 'quit':
                break
            if user_input:
                try:
                    response = agent.chat(user_input)
                    print(f"\n🤖 Agent: {response}\n")
                except Exception as e:
                    print(f"❌ 错误: {e}\n")
    
    print("👋 教程结束，请继续学习 03_react_agent.py！")
