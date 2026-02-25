"""
=============================================================
 Level 3: ReAct Agent（推理 + 行动循环）
=============================================================

🎯 学习目标：
   1. 理解 ReAct 模式（Reasoning + Acting）
   2. 实现思考→行动→观察的循环
   3. 让 Agent 能自主分解复杂任务
   4. 理解 Agent 的"自主性"是怎么实现的

💡 核心概念 — ReAct 循环：
   ┌─────────────────────────────────────┐
   │          ReAct 循环                  │
   │                                     │
   │  Thought → Action → Observation     │
   │  (思考)    (行动)    (观察结果)      │
   │     ↑                    │          │
   │     └────────────────────┘          │
   │    不断循环，直到得出最终答案         │
   └─────────────────────────────────────┘

📝 与 Level 2 的区别:
   Level 2: 用户问 → LLM一步到位选工具 → 返回
   Level 3: 用户问 → LLM思考 → 用工具 → 看结果 → 再思考 → 再用工具 → ... → 最终回答
   
   Level 3 能处理更复杂的、需要多步推理的任务！
"""

import os
import re
import json
import math
from datetime import datetime
from dotenv import load_dotenv

load_dotenv()

# 我使用纯 Python 实现 ReAct，而不依赖 OpenAI 的 function calling
# 这样更好理解 ReAct 的本质！

# ==========================================
# 第一步：定义工具集
# ==========================================

class Tool:
    """工具基类"""
    def __init__(self, name: str, description: str):
        self.name = name
        self.description = description
    
    def run(self, input_str: str) -> str:
        raise NotImplementedError


class CalculatorTool(Tool):
    """计算器工具"""
    def __init__(self):
        super().__init__(
            name="Calculator",
            description="用于数学计算。输入数学表达式，返回计算结果。例如: '2 + 3 * 4' 或 'sqrt(144)'"
        )
    
    def run(self, expression: str) -> str:
        try:
            allowed = {
                "abs": abs, "round": round, "pow": pow,
                "sqrt": math.sqrt, "sin": math.sin, "cos": math.cos,
                "pi": math.pi, "e": math.e, "log": math.log,
            }
            result = eval(expression.strip(), {"__builtins__": {}}, allowed)
            return f"计算结果: {expression} = {result}"
        except Exception as e:
            return f"计算错误: {e}"


class SearchTool(Tool):
    """知识搜索工具"""
    def __init__(self):
        super().__init__(
            name="Search",
            description="搜索知识库获取信息。输入搜索关键词。"
        )
        self.knowledge = {
            "python发明者": "Python 由 Guido van Rossum 于 1989 年底发明，1991 年首次发布。",
            "python版本": "Python 最新稳定版本是 3.12（截至2024年）。Python 2 已于 2020 年停止维护。",
            "机器学习": "机器学习是人工智能的子领域，让计算机通过数据学习模式。常用库：scikit-learn, TensorFlow, PyTorch。",
            "深度学习": "深度学习是机器学习的子集，使用多层神经网络。代表架构：CNN, RNN, Transformer。",
            "transformer": "Transformer 是 2017 年 Google 提出的模型架构，论文'Attention is All You Need'。是 GPT, BERT 等模型的基础。",
            "gpt": "GPT (Generative Pre-trained Transformer) 是 OpenAI 开发的大语言模型系列。GPT-4 于 2023 年发布。",
            "langchain": "LangChain 是 Python 框架，用于构建 LLM 应用。提供 Chain, Agent, Memory 等组件。由 Harrison Chase 创建。",
            "agent": "AI Agent 是能自主执行任务的 AI 系统。核心：感知→决策→行动循环。关键组件：LLM, 工具, 记忆, 规划。",
            "北京人口": "北京市常住人口约 2154 万人（2023年数据）。",
            "上海面积": "上海市总面积约 6340.5 平方公里。",
            "地球太阳距离": "地球到太阳的平均距离约 1.496 亿公里（1个天文单位 AU）。",
        }
    
    def run(self, query: str) -> str:
        results = []
        for key, value in self.knowledge.items():
            if any(word in key for word in query.lower().split()) or \
               any(word in query.lower() for word in key.split()):
                results.append(value)
        
        if results:
            return "搜索结果: " + " | ".join(results)
        return f"未找到'{query}'的相关信息。尝试使用不同的关键词搜索。"


class DateTimeTool(Tool):
    """日期时间工具"""
    def __init__(self):
        super().__init__(
            name="DateTime",
            description="获取当前日期和时间信息。输入'now'获取当前时间，或输入日期格式问题。"
        )
    
    def run(self, input_str: str) -> str:
        now = datetime.now()
        return f"当前时间: {now.strftime('%Y年%m月%d日 %H:%M:%S')}，星期{['一','二','三','四','五','六','日'][now.weekday()]}"


# ==========================================
# 第二步：实现 ReAct Agent（纯 Prompt 方式）
# ==========================================

class ReActAgent:
    """
    ReAct Agent — 使用思考-行动-观察循环
    
    这个实现使用 Prompt Engineering 来引导 LLM 进行 ReAct 推理，
    不依赖 OpenAI 的 function calling，更好理解底层原理。
    """
    
    def __init__(self, tools: list[Tool], model: str = "gpt-3.5-turbo", max_steps: int = 5):
        self.tools = {tool.name: tool for tool in tools}
        self.model = model
        self.max_steps = max_steps  # 防止无限循环
        
        # 导入 OpenAI
        from openai import OpenAI
        self.client = OpenAI(
            api_key=os.getenv("OPENAI_API_KEY", "your-api-key"),
            base_url=os.getenv("OPENAI_API_BASE", "https://api.openai.com/v1"),
        )
    
    def _build_system_prompt(self) -> str:
        """构建系统提示词，告诉 LLM ReAct 的格式"""
        
        # 构建工具描述
        tool_descriptions = "\n".join(
            f"  - {name}: {tool.description}" 
            for name, tool in self.tools.items()
        )
        
        return f"""你是一个能够使用工具解决问题的智能助手。

## 可用工具:
{tool_descriptions}

## 你必须严格遵循以下格式进行推理：

Thought: [你的思考过程，分析问题需要什么信息或计算]
Action: [工具名称]
Action Input: [传给工具的输入]

然后你会收到工具的执行结果:
Observation: [工具返回的结果]

你可以继续思考和使用工具:
Thought: [根据观察结果的进一步思考]
Action: [下一个要使用的工具]
Action Input: [输入]

当你得到足够的信息可以回答用户问题时，使用:
Thought: 我现在有足够的信息来回答这个问题了。
Final Answer: [你的最终回答]

## 重要规则:
1. 每次只能使用一个工具
2. 必须等待 Observation 后再继续
3. 如果不需要工具，直接给出 Final Answer
4. 用中文回答
5. 思考过程要详细，展示推理逻辑"""

    def run(self, question: str) -> str:
        """
        运行 ReAct 循环
        
        核心循环:
        1. 让 LLM 思考（Thought）并选择行动（Action）
        2. 执行行动，获得观察结果（Observation）
        3. 将结果告诉 LLM，回到步骤 1
        4. 直到 LLM 给出 Final Answer
        """
        print(f"\n{'='*60}")
        print(f"🎯 问题: {question}")
        print(f"{'='*60}")
        
        messages = [
            {"role": "system", "content": self._build_system_prompt()},
            {"role": "user", "content": question}
        ]
        
        for step in range(self.max_steps):
            print(f"\n--- Step {step + 1} ---")
            
            # 调用 LLM
            response = self.client.chat.completions.create(
                model=self.model,
                messages=messages,
                temperature=0,  # 推理任务用低温度
                stop=["Observation:"],  # 在 Observation 前停止，让我们执行工具
            )
            
            llm_output = response.choices[0].message.content
            print(f"🤖 LLM 输出:\n{llm_output}")
            
            # 检查是否有 Final Answer
            if "Final Answer:" in llm_output:
                final_answer = llm_output.split("Final Answer:")[-1].strip()
                print(f"\n✅ 最终答案: {final_answer}")
                return final_answer
            
            # 解析 Action 和 Action Input
            action_match = re.search(r"Action:\s*(.+?)(?:\n|$)", llm_output)
            action_input_match = re.search(r"Action Input:\s*(.+?)(?:\n|$)", llm_output)
            
            if not action_match or not action_input_match:
                # 没有工具调用，可能直接给了答案
                messages.append({"role": "assistant", "content": llm_output})
                messages.append({"role": "user", "content": "请用 'Final Answer:' 格式给出你的最终回答。"})
                continue
            
            action = action_match.group(1).strip()
            action_input = action_input_match.group(1).strip()
            
            print(f"\n🔧 执行工具: {action}({action_input})")
            
            # 执行工具
            if action in self.tools:
                observation = self.tools[action].run(action_input)
            else:
                observation = f"错误: 工具 '{action}' 不存在。可用工具: {', '.join(self.tools.keys())}"
            
            print(f"👁️ 观察结果: {observation}")
            
            # 将 LLM 输出和工具结果加入消息历史
            messages.append({"role": "assistant", "content": llm_output})
            messages.append({"role": "user", "content": f"Observation: {observation}"})
        
        return "抱歉，我在规定步骤内没有找到答案。请尝试更简单的问题。"


# ==========================================
# 第三步：不需要 API 的 ReAct 演示
# ==========================================

def demo_react_pattern():
    """
    演示 ReAct 模式的完整推理过程
    展示 Agent 如何一步步解决复杂问题
    """
    print("\n" + "="*60)
    print("📖 ReAct 模式完整演示")
    print("="*60)
    
    print("""
🎯 复杂问题: "北京人口密度是多少？(人/平方公里)"

这个问题 Agent 不能一步回答，需要:
1. 查询北京人口
2. 查询北京面积  
3. 计算人口密度 = 人口 / 面积

让我们看 ReAct Agent 如何一步步解决：

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🔄 ReAct 循环 - Step 1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

💭 Thought: 用户问北京的人口密度。我需要知道北京的人口和面积。
             让我先搜索北京的人口。
             
🎬 Action: Search
📥 Action Input: 北京人口
""")
    
    # 实际执行搜索
    search_tool = SearchTool()
    result1 = search_tool.run("北京人口")
    print(f"👁️ Observation: {result1}")
    
    print(f"""
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🔄 ReAct 循环 - Step 2
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

💭 Thought: 我知道了北京人口约 2154 万人。
             现在还需要知道北京的面积。但知识库里可能没有。
             让我搜索一下。
             
🎬 Action: Search
📥 Action Input: 北京面积
""")
    
    result2 = search_tool.run("北京面积")
    print(f"👁️ Observation: {result2}")
    print("   (假设搜索到: 北京面积约 16410 平方公里)")
    
    print(f"""
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🔄 ReAct 循环 - Step 3
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

💭 Thought: 现在我有了两个数据:
             - 北京人口: 2154 万人 = 21540000 人
             - 北京面积: 16410 平方公里
             需要计算人口密度 = 人口 / 面积
             
🎬 Action: Calculator
📥 Action Input: 21540000 / 16410
""")
    
    calc_tool = CalculatorTool()
    result3 = calc_tool.run("21540000 / 16410")
    print(f"👁️ Observation: {result3}")
    
    print(f"""
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🔄 ReAct 循环 - Step 4 (最终)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

💭 Thought: 我现在有足够的信息来回答这个问题了。
             北京人口约 2154 万，面积约 16410 平方公里，
             人口密度约 1312.6 人/平方公里。

✅ Final Answer: 北京的人口密度约为 1,313 人/平方公里。
   （根据数据：人口约 2,154 万人，面积约 16,410 平方公里，
     计算: 21,540,000 ÷ 16,410 ≈ 1,312.6）

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
""")
    
    print("🎓 关键收获:")
    print("  1. ReAct Agent 不会一次性回答复杂问题")
    print("  2. 它会分解问题，逐步搜索和计算")
    print("  3. 思考(Thought) → 行动(Action) → 观察(Observation) 不断循环")
    print("  4. 直到收集够信息，给出 Final Answer")
    print("  5. 这就是 Agent 的'自主性' — 自己决定下一步做什么！")
    print()
    
    print("📊 Level 2 vs Level 3 对比:")
    print("  Level 2 (工具调用): 用户问 → 用1个工具 → 回答 (一步到位)")
    print("  Level 3 (ReAct):    用户问 → 思考 → 用工具 → 思考 → 用工具 → ... → 回答 (多步推理)")
    print("  适用场景: 需要多步推理、信息综合的复杂问题")


# ==========================================
# 主程序入口
# ==========================================
if __name__ == "__main__":
    print("\n🚀 AI Agent 教程 — Level 3: ReAct Agent")
    print("=" * 60)
    
    # 运行演示
    demo_react_pattern()
    
    # 交互模式
    print("\n是否启动交互式 ReAct Agent？(需要 API Key)")
    print("  1. 启动")
    print("  0. 退出")
    
    choice = input("\n请选择: ").strip()
    
    if choice == "1":
        tools = [CalculatorTool(), SearchTool(), DateTimeTool()]
        agent = ReActAgent(tools=tools)
        
        print("\n🤖 ReAct Agent 已启动！")
        print("   试试复杂问题：")
        print("   - 'Python是谁发明的？那一年他多大了？（假设他1956年出生）'")
        print("   - 'GPT的基础架构是什么？它是哪年提出的？'")
        print("   - '计算圆周率的前5位乘以自然常数e的值'")
        print("   输入 'quit' 退出\n")
        
        while True:
            user_input = input("👤 你: ").strip()
            if user_input.lower() == 'quit':
                break
            if user_input:
                try:
                    response = agent.run(user_input)
                    print(f"\n🤖 最终回答: {response}\n")
                except Exception as e:
                    print(f"❌ 错误: {e}\n")
    
    print("👋 教程结束，请继续学习 04_multi_agent.py！")
