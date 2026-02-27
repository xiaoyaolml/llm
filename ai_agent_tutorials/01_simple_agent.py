"""
=============================================================
 Level 1: 最简单的 AI Agent — 带记忆的对话 Agent
=============================================================

🎯 学习目标：
   1. 理解 Agent 的最基本结构
   2. 学会使用 OpenAI API
   3. 理解"系统提示词"的作用
   4. 实现对话记忆功能

💡 核心概念：
   - System Prompt (系统提示词): 告诉 AI "你是谁"
   - Message History (消息历史): 让 AI 记住之前的对话
   - Temperature (温度): 控制回答的随机性 (0=确定性, 1=创造性)
"""

import os
from openai import OpenAI
from dotenv import load_dotenv

# ==========================================
# 第一步：加载配置
# ==========================================
load_dotenv()  # 从 .env 文件加载环境变量

# 创建 OpenAI 客户端
# 如果使用其他兼容 API（DeepSeek、智谱等），修改 base_url
client = OpenAI(
    api_key=os.getenv("OPENAI_API_KEY", "your-api-key"),
    base_url=os.getenv("OPENAI_API_BASE", "https://api.openai.com/v1"),
)

# ==========================================
# 第二步：定义 Agent 类
# ==========================================
class SimpleAgent:
    """
    最简单的 AI Agent
    功能：对话 + 记忆
    """
    
    def __init__(self, system_prompt: str, model: str = "gpt-3.5-turbo"):
        """
        初始化 Agent
        
        参数:
            system_prompt: 系统提示词，定义 Agent 的角色和行为
            model: 使用的模型名称
        """
        self.model = model
        self.system_prompt = system_prompt
        
        # 消息历史 = Agent 的"记忆"
        # 第一条消息永远是系统提示词
        self.messages = [
            {"role": "system", "content": system_prompt}
        ]
    
    def chat(self, user_input: str) -> str:
        """
        与 Agent 对话
        
        参数:
            user_input: 用户输入的消息
            
        返回:
            Agent 的回复
        """
        # 1. 将用户消息加入历史
        self.messages.append({"role": "user", "content": user_input})
        
        # 2. 调用 LLM API
        response = client.chat.completions.create(
            model=self.model,
            messages=self.messages,
            temperature=0.7,  # 0=确定性回答, 1=创造性回答
        )
        
        # 3. 提取 AI 的回复
        assistant_message = response.choices[0].message.content
        
        # 4. 将 AI 回复也加入历史（这就是"记忆"！）
        self.messages.append({"role": "assistant", "content": assistant_message})
        
        return assistant_message
    
    def reset_memory(self):
        """清除对话记忆，只保留系统提示词"""
        self.messages = [
            {"role": "system", "content": self.system_prompt}
        ]
        print("✅ 记忆已清除")
    
    def show_memory(self):
        """显示当前的对话记忆"""
        print("\n📝 当前对话记忆：")
        print("=" * 50)
        for msg in self.messages:
            role = msg["role"]
            content = msg["content"][:100]  # 只显示前100个字符
            emoji = {"system": "⚙️", "user": "👤", "assistant": "🤖"}.get(role, "❓")
            print(f"  {emoji} [{role}]: {content}...")
        print("=" * 50)


# ==========================================
# 第三步：创建不同角色的 Agent
# ==========================================

# Agent 1: Python 编程助手
python_tutor = SimpleAgent(
    system_prompt="""你是一个友好的 Python 编程教师。
    
你的特点：
- 用简单易懂的语言解释编程概念
- 每次回答都提供代码示例
- 鼓励学生，不批评错误
- 回答简洁，不超过200字
- 用中文回答"""
)

# Agent 2: 翻译助手
translator = SimpleAgent(
    system_prompt="""你是一个专业的中英翻译助手。

规则：
- 如果用户输入中文，翻译为英文
- 如果用户输入英文，翻译为中文
- 翻译要自然流畅，不要机械翻译
- 同时给出直译和意译两个版本"""
)

# Agent 3: 代码审查助手
code_reviewer = SimpleAgent(
    system_prompt="""你是一个严格的代码审查专家。

你的工作：
- 分析用户提交的代码
- 找出潜在的 bug 和安全问题
- 给出改进建议
- 评估代码质量（1-10分）
- 用中文回答"""
)


# ==========================================
# 第四步：运行 Agent
# ==========================================
def run_interactive_agent(agent: SimpleAgent, agent_name: str):
    """
    启动交互式对话
    
    输入 'quit' 退出
    输入 'memory' 查看记忆
    输入 'reset' 清除记忆
    """
    print(f"\n{'='*60}")
    print(f"🤖 {agent_name} 已启动！")
    print(f"   输入 'quit' 退出 | 'memory' 查看记忆 | 'reset' 清除记忆")
    print(f"{'='*60}\n")
    
    while True:
        user_input = input("👤 你: ").strip()
        
        if not user_input:
            continue
        
        if user_input.lower() == 'quit':
            print("👋 再见！")
            break
        
        if user_input.lower() == 'memory':
            agent.show_memory()
            continue
        
        if user_input.lower() == 'reset':
            agent.reset_memory()
            continue
        
        try:
            response = agent.chat(user_input)
            print(f"\n🤖 Agent: {response}\n")
        except Exception as e:
            print(f"\n❌ 错误: {e}")
            print("   请检查 API Key 是否正确配置\n")


# ==========================================
# 第五步：演示（不需要 API Key 也能理解）
# ==========================================
def demo_without_api():
    """
    不需要 API Key 的演示
    展示 Agent 的核心逻辑
    """
    print("\n" + "="*60)
    print("📖 Agent 核心逻辑演示（不需要 API Key）")
    print("="*60)
    
    # 模拟消息历史
    messages = []
    
    # Step 1: 设置系统提示词
    messages.append({
        "role": "system", 
        "content": "你是一个Python编程助手"
    })
    print("\n1️⃣ 设置系统提示词:")
    print(f"   messages = {messages}")
    
    # Step 2: 用户提问
    messages.append({
        "role": "user", 
        "content": "什么是列表推导式？"
    })
    print("\n2️⃣ 用户提问后:")
    print(f"   messages 长度 = {len(messages)}")
    
    # Step 3: AI 回答（模拟）
    ai_response = "列表推导式是Python的简洁语法: [x**2 for x in range(10)]"
    messages.append({
        "role": "assistant", 
        "content": ai_response
    })
    print(f"\n3️⃣ AI 回答: {ai_response}")
    
    # Step 4: 用户追问（Agent 有记忆！）
    messages.append({
        "role": "user", 
        "content": "能给个更复杂的例子吗？"
    })
    print("\n4️⃣ 用户追问: '能给个更复杂的例子吗？'")
    print(f"   此时 messages 有 {len(messages)} 条消息")
    print("   → AI 能看到之前的对话，所以知道你在问列表推导式的例子！")
    print("   → 这就是'记忆'的本质：把历史消息都发给 AI")
    
    print("\n" + "="*60)
    print("🎓 关键收获:")
    print("   1. Agent 的'记忆' = 消息列表 (messages)")
    print("   2. 每次调用 API 都发送完整的消息历史")
    print("   3. 系统提示词决定了 Agent 的'人格'")
    print("   4. 核心循环: 用户输入 → 加入历史 → 调用AI → 回复加入历史")
    print("="*60)


# ==========================================
# 主程序入口
# ==========================================
if __name__ == "__main__":
    print("\n🚀 AI Agent 教程 — Level 1: 简单对话 Agent")
    print("=" * 60)
    
    # 先运行不需要 API 的演示
    demo_without_api()
    
    # 询问是否启动交互模式
    print("\n\n是否启动交互式 Agent？(需要配置 API Key)")
    print("  1. Python 编程助手")
    print("  2. 中英翻译助手")
    print("  3. 代码审查助手")
    print("  0. 退出")
    
    choice = input("\n请选择 (0-3): ").strip()
    
    agents = {
        "1": (python_tutor, "Python 编程助手"),
        "2": (translator, "中英翻译助手"),
        "3": (code_reviewer, "代码审查助手"),
    }
    
    if choice in agents:
        agent, name = agents[choice]
        run_interactive_agent(agent, name)
    else:
        print("👋 教程结束，请继续学习 02_tool_agent.py！")
