"""
=============================================================
 Bonus: 使用 LangChain 快速构建 Agent
=============================================================

🎯 前面我们手写了 Agent 的核心逻辑，现在用 LangChain 框架
    几行代码就能实现相同功能！

💡 LangChain 的优势:
   - 封装了复杂的 Agent 逻辑
   - 提供了丰富的内置工具
   - 支持多种 LLM 提供商
   - 社区活跃，生态丰富

⚠️ 运行此文件需要:
   pip install langchain langchain-openai python-dotenv
   并配置好 .env 文件中的 OPENAI_API_KEY
"""

import os
from dotenv import load_dotenv

load_dotenv()


# ==========================================
# 示例 1: 最简单的 LangChain Agent
# ==========================================

def example_1_basic_langchain_agent():
    """
    用 LangChain 快速创建一个带工具的 Agent
    对比手写代码，你会发现 LangChain 极大简化了开发
    """
    from langchain_openai import ChatOpenAI
    from langchain.agents import tool, AgentExecutor, create_react_agent
    from langchain.prompts import PromptTemplate
    
    # 1. 创建 LLM
    llm = ChatOpenAI(
        model="gpt-3.5-turbo",
        temperature=0,
        openai_api_key=os.getenv("OPENAI_API_KEY"),
        openai_api_base=os.getenv("OPENAI_API_BASE"),
    )
    
    # 2. 定义工具 — 用 @tool 装饰器，超级简单！
    @tool
    def calculator(expression: str) -> str:
        """计算数学表达式。输入一个数学表达式字符串，返回计算结果。"""
        try:
            return str(eval(expression))
        except Exception as e:
            return f"计算错误: {e}"
    
    @tool
    def get_word_length(word: str) -> str:
        """获取一个单词或文本的长度。"""
        return str(len(word))
    
    # 3. 创建 Agent
    tools = [calculator, get_word_length]
    
    # ReAct 提示词模板
    prompt = PromptTemplate.from_template("""Answer the following questions as best you can. You have access to the following tools:

{tools}

Use the following format:

Question: the input question you must answer
Thought: you should always think about what to do
Action: the action to take, should be one of [{tool_names}]
Action Input: the input to the action
Observation: the result of the action
... (this Thought/Action/Action Input/Observation can repeat N times)
Thought: I now know the final answer
Final Answer: the final answer to the original input question

Begin!

Question: {input}
Thought:{agent_scratchpad}""")
    
    agent = create_react_agent(llm, tools, prompt)
    agent_executor = AgentExecutor(agent=agent, tools=tools, verbose=True)
    
    # 4. 运行
    print("🤖 LangChain Agent 启动！\n")
    result = agent_executor.invoke({"input": "计算 (15 + 27) * 3 的结果"})
    print(f"\n✅ 最终结果: {result['output']}")


# ==========================================
# 示例 2: 使用 LangChain 的 Tool Calling Agent（更现代的方式）
# ==========================================

def example_2_tool_calling_agent():
    """
    使用 OpenAI Function Calling 的 LangChain Agent
    这是更推荐的现代方式
    """
    from langchain_openai import ChatOpenAI
    from langchain.agents import tool, AgentExecutor, create_tool_calling_agent
    from langchain_core.prompts import ChatPromptTemplate, MessagesPlaceholder
    
    # 1. LLM
    llm = ChatOpenAI(
        model="gpt-3.5-turbo",
        temperature=0,
        openai_api_key=os.getenv("OPENAI_API_KEY"),
        openai_api_base=os.getenv("OPENAI_API_BASE"),
    )
    
    # 2. 工具
    @tool
    def search_info(query: str) -> str:
        """搜索关于编程技术的信息。输入搜索关键词。"""
        info = {
            "python": "Python 是 Guido van Rossum 1991年创建的编程语言，以简洁著称。",
            "javascript": "JavaScript 是 Brendan Eich 1995年创建的脚本语言，是Web开发的核心。",
            "rust": "Rust 是 Mozilla 2010年开发的系统编程语言，以内存安全和高性能著称。",
        }
        for key, value in info.items():
            if key in query.lower():
                return value
        return f"未找到关于 '{query}' 的信息"
    
    @tool
    def calculate(expression: str) -> str:
        """计算数学表达式。输入数学表达式字符串。"""
        try:
            return f"结果: {eval(expression)}"
        except:
            return "计算错误"
    
    tools = [search_info, calculate]
    
    # 3. Prompt 模板
    prompt = ChatPromptTemplate.from_messages([
        ("system", "你是一个有用的AI助手，可以使用工具来回答问题。请用中文回答。"),
        ("user", "{input}"),
        MessagesPlaceholder(variable_name="agent_scratchpad"),
    ])
    
    # 4. 创建并运行 Agent
    agent = create_tool_calling_agent(llm, tools, prompt)
    executor = AgentExecutor(agent=agent, tools=tools, verbose=True)
    
    print("🤖 Tool-Calling Agent 启动！\n")
    result = executor.invoke({"input": "Python 是谁发明的？距今多少年了？（假设现在是2026年）"})
    print(f"\n✅ 最终结果: {result['output']}")


# ==========================================
# 不需要 API 的概念对比
# ==========================================

def compare_approaches():
    """对比手写 Agent 和使用 LangChain 的代码量"""
    
    print("""
╔══════════════════════════════════════════════════════════════╗
║           手写 Agent vs LangChain 对比                      ║
╠══════════════════════════════════════════════════════════════╣
║                                                              ║
║  📝 手写 Agent (如文件 02, 03):                              ║
║     - 代码量: ~200 行                                        ║
║     - 优点: 完全理解原理，灵活控制                            ║
║     - 缺点: 开发慢，需要处理各种边界情况                      ║
║                                                              ║
║  🔧 LangChain Agent:                                        ║
║     - 代码量: ~30 行                                         ║
║     - 优点: 快速开发，内置大量工具和功能                      ║
║     - 缺点: 学习曲线，抽象层多，不易调试                      ║
║                                                              ║
║  💡 建议学习路径:                                            ║
║     1. 先手写，理解原理 (Level 1-4)                          ║
║     2. 再用框架，提高效率 (LangChain/CrewAI)                 ║
║     3. 遇到问题时，因为理解原理，所以不怕                     ║
║                                                              ║
╠══════════════════════════════════════════════════════════════╣
║                                                              ║
║  🚀 Agent 开发的主流框架:                                    ║
║                                                              ║
║  ┌──────────────┬────────────┬─────────────────────┐        ║
║  │   框架       │ 难度       │ 适用场景            │        ║
║  ├──────────────┼────────────┼─────────────────────┤        ║
║  │ LangChain    │ ⭐⭐⭐    │ 通用LLM应用         │        ║
║  │ LangGraph    │ ⭐⭐⭐⭐  │ 复杂工作流          │        ║
║  │ CrewAI       │ ⭐⭐      │ 多Agent协作         │        ║
║  │ AutoGen      │ ⭐⭐⭐    │ 对话式多Agent       │        ║
║  │ OpenAI SDK   │ ⭐⭐      │ 简单工具调用        │        ║
║  │ Dify         │ ⭐        │ 低代码/可视化       │        ║
║  └──────────────┴────────────┴─────────────────────┘        ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝
""")


# ==========================================
# 主程序
# ==========================================
if __name__ == "__main__":
    print("\n🚀 Bonus: LangChain Agent 快速开发")
    print("=" * 60)
    
    # 先展示概念对比
    compare_approaches()
    
    print("\n请选择要运行的示例（需要 API Key）:")
    print("  1. ReAct Agent（经典方式）")
    print("  2. Tool-Calling Agent（现代方式）")
    print("  0. 退出")
    
    choice = input("\n请选择: ").strip()
    
    try:
        if choice == "1":
            example_1_basic_langchain_agent()
        elif choice == "2":
            example_2_tool_calling_agent()
        else:
            print("👋 退出")
    except ImportError as e:
        print(f"\n❌ 缺少依赖: {e}")
        print("   请运行: pip install langchain langchain-openai python-dotenv")
    except Exception as e:
        print(f"\n❌ 错误: {e}")
        print("   请检查 API Key 是否正确配置")
