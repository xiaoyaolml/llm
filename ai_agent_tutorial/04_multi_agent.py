"""
=============================================================
 Level 4: 多 Agent 协作系统
=============================================================

🎯 学习目标：
   1. 理解多 Agent 协作的架构设计
   2. 实现 Agent 之间的通信和任务分配
   3. 学会设计"管理者-执行者"模式
   4. 构建一个完整的多 Agent 工作流

💡 核心概念 — 多 Agent 架构：
   
   ┌─────────────────────────────────────────┐
   │          Orchestrator Agent              │
   │          (协调者/管理者)                  │
   │                                         │
   │   分析任务 → 分配给专业 Agent → 汇总结果  │
   └────────┬──────────┬──────────┬──────────┘
            ↓          ↓          ↓
   ┌────────────┐ ┌─────────┐ ┌──────────┐
   │ Researcher │ │ Coder   │ │ Reviewer │
   │ Agent      │ │ Agent   │ │ Agent    │
   │ (研究员)   │ │ (程序员) │ │ (审查员) │
   └────────────┘ └─────────┘ └──────────┘

📝 实际应用场景：
   - 软件开发团队：产品经理Agent + 开发者Agent + 测试Agent
   - 内容创作：研究Agent + 写作Agent + 编辑Agent
   - 数据分析：采集Agent + 分析Agent + 可视化Agent
"""

import os
import json
from dataclasses import dataclass, field
from typing import Optional
from dotenv import load_dotenv

load_dotenv()


# ==========================================
# 第一步：定义消息和任务系统
# ==========================================

@dataclass
class Message:
    """Agent 之间传递的消息"""
    sender: str      # 发送者
    receiver: str     # 接收者
    content: str      # 消息内容
    msg_type: str = "task"  # 消息类型: task(任务), result(结果), feedback(反馈)

@dataclass
class Task:
    """任务定义"""
    task_id: int
    description: str
    assigned_to: str = ""
    status: str = "pending"  # pending, in_progress, completed, failed
    result: str = ""


# ==========================================
# 第二步：定义专业 Agent
# ==========================================

class SpecialistAgent:
    """
    专业 Agent 基类
    每个专业 Agent 有自己的角色和能力
    """
    
    def __init__(self, name: str, role: str, expertise: str):
        self.name = name
        self.role = role
        self.expertise = expertise
        self.message_history: list[Message] = []
    
    def receive_message(self, message: Message) -> str:
        """接收消息并处理"""
        self.message_history.append(message)
        return self.process(message.content)
    
    def process(self, task_content: str) -> str:
        """处理任务 — 子类实现"""
        raise NotImplementedError
    
    def __repr__(self):
        return f"🤖 {self.name} ({self.role})"


class ResearcherAgent(SpecialistAgent):
    """研究员 Agent — 负责信息收集和分析"""
    
    def __init__(self):
        super().__init__(
            name="Researcher",
            role="研究员",
            expertise="信息收集、文献调研、技术分析"
        )
    
    def process(self, task_content: str) -> str:
        """
        实际项目中，这里会调用 LLM API
        为了演示，我们用模拟的方式展示 Agent 的输出
        """
        # 模拟研究结果
        research_db = {
            "web框架": {
                "findings": [
                    "Flask: 轻量级，适合小项目，学习曲线低",
                    "Django: 全功能，适合大项目，自带ORM和admin",
                    "FastAPI: 高性能，自动生成API文档，支持异步",
                ],
                "recommendation": "建议初学者从 Flask 开始，然后学习 FastAPI",
                "sources": ["Python官方文档", "GitHub Stars统计", "StackOverflow调查"]
            },
            "数据库": {
                "findings": [
                    "SQLite: 轻量，无需服务器，适合原型开发",
                    "PostgreSQL: 功能强大，ACID事务，适合生产环境",
                    "MongoDB: NoSQL，灵活Schema，适合JSON数据",
                ],
                "recommendation": "开发阶段用SQLite，生产环境用PostgreSQL",
                "sources": ["DB-Engines排名", "各大云厂商推荐"]
            },
        }
        
        # 简单关键词匹配
        for key, data in research_db.items():
            if key in task_content:
                return json.dumps(data, ensure_ascii=False, indent=2)
        
        return json.dumps({
            "findings": [f"关于'{task_content}'的研究需要更多时间"],
            "recommendation": "需要进一步调研",
            "sources": ["待定"]
        }, ensure_ascii=False, indent=2)


class CoderAgent(SpecialistAgent):
    """程序员 Agent — 负责代码编写"""
    
    def __init__(self):
        super().__init__(
            name="Coder",
            role="程序员",
            expertise="代码编写、架构设计、技术实现"
        )
    
    def process(self, task_content: str) -> str:
        """生成代码"""
        
        code_templates = {
            "flask": '''
# Flask Web 应用示例
from flask import Flask, jsonify, request

app = Flask(__name__)

# 内存中的数据存储
todos = []

@app.route('/api/todos', methods=['GET'])
def get_todos():
    """获取所有待办事项"""
    return jsonify(todos)

@app.route('/api/todos', methods=['POST'])
def add_todo():
    """添加新的待办事项"""
    data = request.json
    todo = {
        "id": len(todos) + 1,
        "title": data.get("title", ""),
        "completed": False,
    }
    todos.append(todo)
    return jsonify(todo), 201

@app.route('/api/todos/<int:todo_id>', methods=['PUT'])
def update_todo(todo_id):
    """更新待办事项"""
    todo = next((t for t in todos if t["id"] == todo_id), None)
    if not todo:
        return jsonify({"error": "Not found"}), 404
    data = request.json
    todo.update(data)
    return jsonify(todo)

if __name__ == '__main__':
    app.run(debug=True, port=5000)
''',
            "fastapi": '''
# FastAPI Web 应用示例
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI(title="Todo API")

class TodoCreate(BaseModel):
    title: str
    completed: bool = False

class Todo(TodoCreate):
    id: int

todos: list[Todo] = []

@app.get("/api/todos", response_model=list[Todo])
async def get_todos():
    return todos

@app.post("/api/todos", response_model=Todo, status_code=201)
async def add_todo(todo_data: TodoCreate):
    todo = Todo(id=len(todos) + 1, **todo_data.model_dump())
    todos.append(todo)
    return todo

@app.put("/api/todos/{todo_id}", response_model=Todo)
async def update_todo(todo_id: int, todo_data: TodoCreate):
    todo = next((t for t in todos if t.id == todo_id), None)
    if not todo:
        raise HTTPException(status_code=404, detail="Not found")
    todo.title = todo_data.title
    todo.completed = todo_data.completed
    return todo

# 运行: uvicorn main:app --reload
''',
        }
        
        for key, code in code_templates.items():
            if key in task_content.lower():
                return f"```python\n{code}\n```"
        
        return f"# 需要根据需求编写 {task_content} 相关代码\n# TODO: 实现具体功能"


class ReviewerAgent(SpecialistAgent):
    """代码审查 Agent — 负责质量把关"""
    
    def __init__(self):
        super().__init__(
            name="Reviewer",
            role="代码审查员",
            expertise="代码审查、安全检查、最佳实践建议"
        )
    
    def process(self, task_content: str) -> str:
        """审查代码或方案"""
        review = {
            "score": 7,
            "issues": [
                "⚠️ 数据存储在内存中，重启后数据丢失，建议添加持久化",
                "⚠️ 缺少输入验证和错误处理",
                "⚠️ 没有添加认证和鉴权机制",
            ],
            "suggestions": [
                "✅ 添加 SQLite/PostgreSQL 数据库支持",
                "✅ 添加请求参数校验",
                "✅ 实现 JWT Token 认证",
                "✅ 添加日志记录",
                "✅ 编写单元测试",
            ],
            "verdict": "基本功能完成，但需要增强安全性和健壮性后才能部署到生产环境。"
        }
        return json.dumps(review, ensure_ascii=False, indent=2)


# ==========================================
# 第三步：实现协调者 (Orchestrator)
# ==========================================

class OrchestratorAgent:
    """
    协调者 Agent — 多 Agent 系统的"管理者"
    
    职责：
    1. 接收用户需求
    2. 分解为子任务
    3. 将子任务分配给合适的专业 Agent
    4. 收集结果并汇总
    5. 返回最终回答
    """
    
    def __init__(self):
        # 注册所有专业 Agent
        self.agents = {
            "researcher": ResearcherAgent(),
            "coder": CoderAgent(),
            "reviewer": ReviewerAgent(),
        }
        self.task_history: list[Task] = []
        self.message_log: list[Message] = []
    
    def decompose_task(self, user_request: str) -> list[Task]:
        """
        将用户需求分解为子任务
        
        实际项目中这一步由 LLM 完成
        这里为了演示，用规则来模拟
        """
        tasks = []
        task_id = 1
        
        # 简单的任务分解逻辑
        if any(word in user_request for word in ["开发", "创建", "搭建", "构建", "做一个"]):
            tasks.append(Task(
                task_id=task_id,
                description=f"技术调研: 调研{user_request}的最佳实践和技术选型",
                assigned_to="researcher"
            ))
            task_id += 1
            
            tasks.append(Task(
                task_id=task_id,
                description=f"代码实现: 根据需求编写代码 - {user_request}",
                assigned_to="coder"
            ))
            task_id += 1
            
            tasks.append(Task(
                task_id=task_id,
                description=f"代码审查: 审查实现的代码质量和安全性",
                assigned_to="reviewer"
            ))
        else:
            tasks.append(Task(
                task_id=task_id,
                description=user_request,
                assigned_to="researcher"
            ))
        
        return tasks
    
    def assign_and_execute(self, task: Task) -> str:
        """分配并执行任务"""
        agent = self.agents.get(task.assigned_to)
        if not agent:
            return f"错误: 找不到 Agent '{task.assigned_to}'"
        
        # 创建消息
        message = Message(
            sender="Orchestrator",
            receiver=agent.name,
            content=task.description,
            msg_type="task"
        )
        self.message_log.append(message)
        
        # 执行任务
        task.status = "in_progress"
        print(f"  📨 分配给 {agent}: {task.description[:50]}...")
        
        result = agent.receive_message(message)
        
        task.status = "completed"
        task.result = result
        
        # 记录结果消息
        result_message = Message(
            sender=agent.name,
            receiver="Orchestrator",
            content=result,
            msg_type="result"
        )
        self.message_log.append(result_message)
        
        return result
    
    def run(self, user_request: str) -> str:
        """
        运行完整的多 Agent 工作流
        """
        print(f"\n{'='*60}")
        print(f"🎯 用户需求: {user_request}")
        print(f"{'='*60}")
        
        # Step 1: 分解任务
        print(f"\n📋 Step 1: 分解任务")
        tasks = self.decompose_task(user_request)
        for t in tasks:
            print(f"  📌 任务 {t.task_id}: {t.description}")
            print(f"     分配给: {t.assigned_to}")
        
        # Step 2: 逐个执行任务
        print(f"\n⚡ Step 2: 执行任务")
        results = {}
        for task in tasks:
            print(f"\n  --- 任务 {task.task_id} ---")
            result = self.assign_and_execute(task)
            results[task.assigned_to] = result
            self.task_history.append(task)
        
        # Step 3: 汇总结果
        print(f"\n📊 Step 3: 汇总结果")
        summary = self._summarize_results(user_request, tasks, results)
        
        return summary
    
    def _summarize_results(self, request: str, tasks: list[Task], results: dict) -> str:
        """汇总所有结果"""
        
        report = f"""
{'='*60}
📋 多 Agent 协作报告
{'='*60}

🎯 原始需求: {request}

📊 任务执行情况:
"""
        for task in tasks:
            status_emoji = "✅" if task.status == "completed" else "❌"
            report += f"\n  {status_emoji} 任务 {task.task_id} [{task.assigned_to}]: {task.description[:40]}"
        
        report += f"\n\n{'─'*40}"
        
        for agent_name, result in results.items():
            agent = self.agents[agent_name]
            report += f"\n\n📦 {agent.name} ({agent.role}) 的结果:"
            report += f"\n{result}"
        
        report += f"\n\n{'='*60}"
        report += f"\n💬 协调者总结:"
        report += f"\n  任务已全部完成。"
        report += f"\n  参与 Agent: {', '.join(r.name for r in self.agents.values())}"
        report += f"\n  消息总数: {len(self.message_log)}"
        report += f"\n{'='*60}"
        
        return report
    
    def show_communication_log(self):
        """显示 Agent 之间的通信日志"""
        print(f"\n📬 Agent 通信日志:")
        print("─" * 50)
        for i, msg in enumerate(self.message_log, 1):
            direction = "→" if msg.msg_type == "task" else "←"
            print(f"  {i}. [{msg.msg_type}] {msg.sender} {direction} {msg.receiver}")
            print(f"     内容: {msg.content[:60]}...")
        print("─" * 50)


# ==========================================
# 第四步：演示多 Agent 系统
# ==========================================

def demo_multi_agent():
    """
    演示多 Agent 协作处理一个完整项目需求
    """
    print("\n" + "="*60)
    print("📖 多 Agent 协作系统演示")
    print("="*60)
    
    # 创建协调者
    orchestrator = OrchestratorAgent()
    
    # 展示 Agent 团队
    print("\n👥 Agent 团队成员:")
    for key, agent in orchestrator.agents.items():
        print(f"   {agent} — 擅长: {agent.expertise}")
    
    # 执行任务
    print("\n" + "─"*60)
    result = orchestrator.run("开发一个 Flask web框架的 Todo API 应用")
    print(result)
    
    # 显示通信日志
    orchestrator.show_communication_log()
    
    # 教学总结
    print(f"""
🎓 关键收获:
  
  1. 多 Agent 系统的核心是 "分工协作"
     - Orchestrator 负责任务分解和协调
     - 专业 Agent 各司其职

  2. Agent 之间通过 消息(Message) 通信
     - 任务消息: Orchestrator → 专业Agent
     - 结果消息: 专业Agent → Orchestrator

  3. 工作流程:
     用户需求 → 任务分解 → 分配执行 → 结果汇总

  4. 实际项目中的增强方向:
     - 每个 Agent 都调用 LLM API（本示例用模拟代替）
     - 添加 Agent 之间的反馈循环（审查员可以要求程序员修改）
     - 使用框架: CrewAI, AutoGen, LangGraph
     - 添加共享记忆系统

📚 推荐的多 Agent 框架:
  - CrewAI:   https://github.com/joaomdmoura/crewAI
  - AutoGen:  https://github.com/microsoft/autogen
  - LangGraph: https://github.com/langchain-ai/langgraph
""")


# ==========================================
# 主程序入口
# ==========================================
if __name__ == "__main__":
    print("\n🚀 AI Agent 教程 — Level 4: 多 Agent 协作系统")
    print("=" * 60)
    
    demo_multi_agent()
    
    # 交互模式
    print("\n是否尝试自定义需求？")
    print("  1. 输入自定义需求")
    print("  0. 退出")
    
    choice = input("\n请选择: ").strip()
    
    if choice == "1":
        orchestrator = OrchestratorAgent()
        print("\n输入你的项目需求（输入 'quit' 退出）：")
        
        while True:
            user_input = input("\n👤 需求: ").strip()
            if user_input.lower() == 'quit':
                break
            if user_input:
                result = orchestrator.run(user_input)
                print(result)
    
    print("\n🎉 恭喜！你已经完成了 AI Agent 教程的所有 4 个级别！")
    print("   接下来可以尝试用 LangChain 或 CrewAI 构建真实项目！")
