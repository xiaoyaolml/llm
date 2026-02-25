# Ollama 从零开始完整使用教程

> 本教程涵盖 Ollama 的安装、配置、模型管理、API 调用、高级用法等全部内容，帮助你在本地快速搭建和运行大语言模型。

---

## 目录

1. [Ollama 是什么](#1-ollama-是什么)
2. [安装 Ollama](#2-安装-ollama)
3. [基础命令速查](#3-基础命令速查)
4. [模型管理](#4-模型管理)
5. [交互式对话](#5-交互式对话)
6. [REST API 调用](#6-rest-api-调用)
7. [自定义模型（Modelfile）](#7-自定义模型modelfile)
8. [Python / JavaScript 集成](#8-python--javascript-集成)
9. [GPU 与性能调优](#9-gpu-与性能调优)
10. [Docker 部署](#10-docker-部署)
11. [搭配 Open WebUI 图形界面](#11-搭配-open-webui-图形界面)
12. [常见问题与排错](#12-常见问题与排错)
13. [实战案例](#13-实战案例)

---

## 1. Ollama 是什么

Ollama 是一个开源的本地大语言模型运行框架，让你可以在自己的机器上一键运行 Llama 3、Qwen、DeepSeek、Gemma、Mistral、Phi 等主流开源模型。

**核心优势：**

- **极简部署**：一条命令即可安装并运行模型
- **跨平台**：支持 macOS / Linux / Windows
- **GPU 加速**：自动检测 NVIDIA / AMD GPU 并利用
- **兼容 OpenAI API**：可直接替换 OpenAI 接口
- **模型自定义**：通过 Modelfile 创建个性化模型
- **隐私安全**：所有数据在本地处理，不上传云端

---

## 2. 安装 Ollama

### 2.1 Linux（推荐）

```bash
# 一键安装（自动检测 GPU 驱动）
curl -fsSL https://ollama.com/install.sh | sh
```

安装完成后，Ollama 会自动以 systemd 服务启动，默认监听端口 `11434`。

```bash
# 验证安装
ollama -v

# 验证服务运行
curl http://127.0.0.1:11434
# 返回: Ollama is running

# 查看服务状态
systemctl status ollama

# 查看日志
journalctl -u ollama -f
```

### 2.2 macOS

前往 [https://ollama.com/download](https://ollama.com/download) 下载 `.dmg` 安装包，双击安装即可。

或使用 Homebrew：

```bash
brew install ollama
```

### 2.3 Windows

前往 [https://ollama.com/download](https://ollama.com/download) 下载 `.exe` 安装程序，按提示安装。

安装完成后，Ollama 会在系统托盘运行。打开 PowerShell 或 CMD 验证：

```powershell
ollama -v
```

### 2.4 升级 Ollama

```bash
# Linux：重新运行安装脚本即自动升级
curl -fsSL https://ollama.com/install.sh | sh

# macOS：重新下载安装包覆盖安装 或 brew upgrade ollama
# Windows：重新下载安装包覆盖安装
```

---

## 3. 基础命令速查

| 命令                      | 说明                                 | 示例                           |
| ------------------------- | ------------------------------------ | ------------------------------ |
| `ollama run <模型>`     | 下载（如未缓存）并运行模型，进入对话 | `ollama run qwen3:8b`        |
| `ollama pull <模型>`    | 仅下载模型，不运行                   | `ollama pull qwen3:8b`       |
| `ollama list`           | 列出已下载的所有模型                 | `ollama list`                |
| `ollama show <模型>`    | 显示模型详细信息                     | `ollama show qwen3:8b`       |
| `ollama rm <模型>`      | 删除已下载的模型                     | `ollama rm qwen3:8b`         |
| `ollama cp <源> <目标>` | 复制模型                             | `ollama cp qwen3:8b my-qwen` |
| `ollama ps`             | 查看当前加载在内存中的模型           | `ollama ps`                  |
| `ollama stop <模型>`    | 从内存中卸载模型                     | `ollama stop qwen3:8b`       |
| `ollama serve`          | 手动启动 Ollama 服务                 | `ollama serve`               |
| `ollama -h`             | 查看帮助                             | `ollama -h`                  |
| `ollama -v`             | 查看版本                             | `ollama -v`                  |

---


# Ollama 运行大模型

只要一行命令
```bash
ollama run qwen3:0.6b
```

## 4. 模型管理

### 4.1 模型仓库

Ollama 官方模型库：[https://ollama.com/library](https://ollama.com/library)

### 4.2 推荐模型一览

| 模型                 | 参数量      | 最低内存   | 适用场景             | 命令                            |
| -------------------- | ----------- | ---------- | -------------------- | ------------------------------- |
| `qwen2.5`          | 0.5B-72B    | 1GB-48GB   | 中文对话、代码、推理 | `ollama run qwen2.5`          |
| `deepseek-r1`      | 1.5B-671B   | 2GB-400GB+ | 深度推理、数学       | `ollama run deepseek-r1:8b`   |
| `llama3.1`         | 8B/70B/405B | 6GB-250GB  | 英文通用、代码       | `ollama run llama3.1`         |
| `gemma2`           | 2B/9B/27B   | 2GB-18GB   | 轻量级通用           | `ollama run gemma2`           |
| `mistral`          | 7B          | 6GB        | 英文通用             | `ollama run mistral`          |
| `phi3`             | 3.8B/14B    | 3GB-10GB   | 轻量推理             | `ollama run phi3`             |
| `codellama`        | 7B/13B/34B  | 6GB-24GB   | 代码生成             | `ollama run codellama`        |
| `nomic-embed-text` | 137M        | 1GB        | 文本嵌入/RAG         | `ollama run nomic-embed-text` |
| `llava`            | 7B/13B      | 6GB-10GB   | 多模态（图文）       | `ollama run llava`            |





### 4.3 模型标签（Tag）

模型名称格式为 `<模型名>:<标签>`，标签表示不同的变体：

```bash
# 默认标签（通常是最小可用版本）
ollama run qwen2.5

# 指定参数量
ollama run qwen2.5:7b
ollama run qwen2.5:14b
ollama run qwen2.5:72b

# 指定量化精度
ollama run llama3.1:8b-q4_0     # 4位量化，速度快，精度略低
ollama run llama3.1:8b-q8_0     # 8位量化，精度更高
ollama run llama3.1:8b-fp16     # 半精度浮点，最高精度
```

**量化说明：**

- `q4_0` / `q4_K_M`：4位量化，内存占用最小，推理最快
- `q5_0` / `q5_K_M`：5位量化，精度与速度的平衡
- `q8_0`：8位量化，精度高但内存占用翻倍
- `fp16`：半精度，完整精度但内存需求最大

### 4.4 下载与删除

```bash
# 下载模型（后台拉取，不进入对话）
ollama pull qwen2.5:7b

# 查看已下载模型
ollama list

# 输出示例：
# NAME              ID            SIZE    MODIFIED
# qwen2.5:7b        abc123def     4.7 GB  2 hours ago
# llama3.1:latest   def456abc     4.9 GB  1 day ago

# 查看模型详细信息
ollama show qwen2.5:7b

# 删除模型（释放磁盘空间）
ollama rm qwen2.5:7b
```

---

## 5. 交互式对话

### 5.1 基本对话

```bash
ollama run qwen2.5:7b
```

进入交互模式后直接输入问题：

```
>>> 你好，请用三句话介绍一下你自己
我是一个由阿里云开发的大型语言模型，名为通义千问。我能够理解和生成中英文文本，
帮助用户完成各种语言相关任务。我的目标是为用户提供准确、有用的信息和建议。

>>> 帮我写一个 Python 快速排序
```

### 5.2 对话中的命令

在交互模式中，以 `/` 开头的是特殊命令：

| 命令                               | 说明                 |
| ---------------------------------- | -------------------- |
| `/bye`                           | 退出对话             |
| `/clear`                         | 清除对话上下文       |
| `/set system <提示词>`           | 设置系统提示词       |
| `/set parameter temperature 0.8` | 调整温度参数         |
| `/set parameter num_ctx 4096`    | 设置上下文窗口大小   |
| `/show info`                     | 显示当前模型信息     |
| `/show system`                   | 显示当前系统提示词   |
| `/save <名称>`                   | 保存当前会话为新模型 |
| `/load <名称>`                   | 加载已保存的模型     |

### 5.3 设置系统提示词

```bash
# 方式一：命令行参数
ollama run qwen2.5:7b --system "你是一个专业的Python程序员，回答问题时优先使用Python代码示例"

# 方式二：交互模式中设置
>>> /set system 你是一个翻译助手，将用户输入的中文翻译成英文
```

### 5.4 多模态对话（图片输入）

```bash
ollama run llava

>>> 描述一下这张图片 /path/to/image.jpg
```

### 5.5 单次调用（非交互）

```bash
# 管道方式，适合脚本调用
echo "什么是快速排序？" | ollama run qwen2.5:7b

# 处理文件内容
cat code.py | ollama run codellama "请审查这段代码并给出改进建议"
```

---

## 6. REST API 调用

Ollama 提供完整的 REST API，默认地址 `http://localhost:11434`。

### 6.1 生成补全（Generate）

```bash
curl http://localhost:11434/api/generate -d '{
  "model": "qwen2.5:7b",
  "prompt": "什么是机器学习？",
  "stream": false
}'
```

**流式响应（默认）：**

```bash
curl http://localhost:11434/api/generate -d '{
  "model": "qwen2.5:7b",
  "prompt": "写一首关于春天的诗"
}'
# 每行返回一个 JSON 对象，逐词输出
```

### 6.2 对话补全（Chat）

```bash
curl http://localhost:11434/api/chat -d '{
  "model": "qwen2.5:7b",
  "messages": [
    {"role": "system", "content": "你是一个有帮助的AI助手"},
    {"role": "user", "content": "帮我解释什么是递归"}
  ],
  "stream": false
}'
```

### 6.3 文本嵌入（Embeddings）

```bash
curl http://localhost:11434/api/embed -d '{
  "model": "nomic-embed-text",
  "input": "Ollama 是一个本地大模型运行框架"
}'
```

### 6.4 兼容 OpenAI API 格式

Ollama 内置兼容 OpenAI 的 API 端点，可以直接替代 OpenAI：

```bash
# 对话补全（兼容 OpenAI 格式）
curl http://localhost:11434/v1/chat/completions -d '{
  "model": "qwen2.5:7b",
  "messages": [
    {"role": "user", "content": "你好"}
  ]
}'

# 文本嵌入（兼容 OpenAI 格式）
curl http://localhost:11434/v1/embeddings -d '{
  "model": "nomic-embed-text",
  "input": "Hello world"
}'

# 模型列表
curl http://localhost:11434/v1/models
```

### 6.5 API 参数说明

```json
{
  "model": "qwen2.5:7b",
  "prompt": "你的问题",
  "stream": false,
  "options": {
    "temperature": 0.7,       // 创造性 (0.0-2.0)，越高越随机
    "top_p": 0.9,             // 核采样阈值
    "top_k": 40,              // 每步考虑的 token 数
    "num_predict": 1024,      // 最大生成 token 数
    "num_ctx": 4096,          // 上下文窗口大小
    "repeat_penalty": 1.1,    // 重复惩罚
    "seed": 42,               // 随机种子（可复现）
    "stop": ["\n", "用户："]   // 停止词
  }
}
```

---

## 7. 自定义模型（Modelfile）

Modelfile 是 Ollama 的模型定义文件，类似 Dockerfile，支持自定义系统提示、参数、模板等。

### 7.1 基本结构

创建文件 `Modelfile`：

```dockerfile
# 基础模型
FROM qwen2.5:7b

# 系统提示词
SYSTEM """
你是一个资深的 Python 开发工程师，擅长代码审查和性能优化。
回答问题时：
1. 优先给出代码示例
2. 解释关键设计决策
3. 指出潜在的性能问题
4. 使用中文回答
"""

# 参数设置
PARAMETER temperature 0.3
PARAMETER top_p 0.9
PARAMETER num_ctx 8192
PARAMETER stop "<|im_end|>"
```

### 7.2 创建并运行自定义模型

```bash
# 从 Modelfile 创建模型
ollama create my-python-expert -f ./Modelfile

# 运行自定义模型
ollama run my-python-expert

# 查看模型信息
ollama show my-python-expert
```

### 7.3 实用 Modelfile 示例

**中英翻译助手：**

```dockerfile
FROM qwen2.5:7b

SYSTEM """
你是一个专业的中英翻译助手。
- 如果用户输入中文，翻译成英文
- 如果用户输入英文，翻译成中文
- 提供多种翻译版本（直译、意译、口语化）
- 对专业术语给出注释
"""

PARAMETER temperature 0.2
PARAMETER num_ctx 4096
```

**代码审查专家：**

```dockerfile
FROM codellama:13b

SYSTEM """
You are a senior code reviewer. For every code snippet:
1. Identify bugs and potential issues
2. Suggest performance improvements
3. Check for security vulnerabilities
4. Recommend best practices
5. Rate the code quality (1-10)
Output in Chinese.
"""

PARAMETER temperature 0.1
PARAMETER num_ctx 8192
```

### 7.4 使用自定义模板（TEMPLATE）

```dockerfile
FROM qwen2.5:7b

TEMPLATE """
{{- if .System }}
<|im_start|>system
{{ .System }}<|im_end|>
{{- end }}
<|im_start|>user
{{ .Prompt }}<|im_end|>
<|im_start|>assistant
"""

SYSTEM "你是一个有帮助的AI助手"
PARAMETER stop "<|im_end|>"
```

### 7.5 导入 GGUF 模型

如果你有从 HuggingFace 下载的 GGUF 格式模型：

```dockerfile
# Modelfile
FROM ./my-model-q4_K_M.gguf

SYSTEM "你是一个有帮助的AI助手"
PARAMETER temperature 0.7
```

```bash
ollama create my-custom-model -f Modelfile
ollama run my-custom-model
```

---

## 8. Python / JavaScript 集成

### 8.1 Python 官方库

```bash
pip install ollama
```

**基本对话：**

```python
import ollama

# 单轮对话
response = ollama.chat(
    model='qwen2.5:7b',
    messages=[
        {'role': 'system', 'content': '你是一个有帮助的AI助手'},
        {'role': 'user', 'content': '什么是快速排序？'},
    ]
)
print(response['message']['content'])
```

**流式输出：**

```python
import ollama

stream = ollama.chat(
    model='qwen2.5:7b',
    messages=[{'role': 'user', 'content': '写一个Python装饰器教程'}],
    stream=True
)

for chunk in stream:
    print(chunk['message']['content'], end='', flush=True)
```

**多轮对话：**

```python
import ollama

messages = [
    {'role': 'system', 'content': '你是一个Python教学助手'}
]

while True:
    user_input = input('\n你: ')
    if user_input.lower() in ['quit', 'exit', 'bye']:
        break
  
    messages.append({'role': 'user', 'content': user_input})
  
    response = ollama.chat(model='qwen2.5:7b', messages=messages)
    assistant_msg = response['message']['content']
  
    print(f'\n助手: {assistant_msg}')
    messages.append({'role': 'assistant', 'content': assistant_msg})
```

**文本嵌入：**

```python
import ollama

response = ollama.embed(
    model='nomic-embed-text',
    input='Ollama 是一个本地大模型框架'
)
print(f"嵌入向量维度: {len(response['embeddings'][0])}")
```

**异步调用：**

```python
import asyncio
import ollama

async def main():
    client = ollama.AsyncClient()
    response = await client.chat(
        model='qwen2.5:7b',
        messages=[{'role': 'user', 'content': '你好'}]
    )
    print(response['message']['content'])

asyncio.run(main())
```

### 8.2 使用 OpenAI SDK 调用 Ollama

```bash
pip install openai
```

```python
from openai import OpenAI

# 指向本地 Ollama 服务
client = OpenAI(
    base_url='http://localhost:11434/v1',
    api_key='ollama'  # 任意值即可
)

response = client.chat.completions.create(
    model='qwen2.5:7b',
    messages=[
        {'role': 'system', 'content': '你是一个有帮助的AI助手'},
        {'role': 'user', 'content': '解释一下什么是 RAG'}
    ],
    temperature=0.7,
    max_tokens=1024
)

print(response.choices[0].message.content)
```

### 8.3 使用 LangChain 集成

```bash
pip install langchain langchain-ollama
```

```python
from langchain_ollama import ChatOllama

llm = ChatOllama(
    model="qwen2.5:7b",
    temperature=0.7,
    base_url="http://localhost:11434"
)

# 简单调用
response = llm.invoke("什么是向量数据库？")
print(response.content)

# 带系统提示的链式调用
from langchain_core.prompts import ChatPromptTemplate

prompt = ChatPromptTemplate.from_messages([
    ("system", "你是一个{role}，请用简洁的语言回答问题"),
    ("user", "{question}")
])

chain = prompt | llm
response = chain.invoke({"role": "数据库专家", "question": "MySQL 和 PostgreSQL 有什么区别？"})
print(response.content)
```

### 8.4 JavaScript / TypeScript

```bash
npm install ollama
```

```javascript
import { Ollama } from 'ollama';

const ollama = new Ollama({ host: 'http://localhost:11434' });

// 基本对话
const response = await ollama.chat({
  model: 'qwen2.5:7b',
  messages: [{ role: 'user', content: '你好' }],
});
console.log(response.message.content);

// 流式输出
const stream = await ollama.chat({
  model: 'qwen2.5:7b',
  messages: [{ role: 'user', content: '写一个 Node.js 服务器' }],
  stream: true,
});

for await (const chunk of stream) {
  process.stdout.write(chunk.message.content);
}
```

---

## 9. GPU 与性能调优

### 9.1 GPU 支持

| GPU 类型      | 支持情况    | 说明                      |
| ------------- | ----------- | ------------------------- |
| NVIDIA (CUDA) | ✅ 完整支持 | 需安装 NVIDIA 驱动和 CUDA |
| AMD (ROCm)    | ✅ 支持     | Linux 下 ROCm 支持        |
| Apple Silicon | ✅ 原生支持 | Metal 加速，M1/M2/M3/M4   |
| Intel ARC     | ⚠️ 实验性 | 部分支持                  |
| 纯CPU         | ✅ 支持     | 速度较慢但可用            |

### 9.2 检查 GPU 使用情况

```bash
# 查看 NVIDIA GPU 使用
nvidia-smi

# 查看 Ollama 加载的模型和 GPU 分配
ollama ps
```

### 9.3 环境变量配置

```bash
# 设置环境变量（Linux 下编辑 systemd 服务或 ~/.bashrc）

# 指定监听地址（允许外部访问）
export OLLAMA_HOST=0.0.0.0:11434

# 模型存储路径
export OLLAMA_MODELS=/data/ollama/models

# 指定使用的 GPU（多卡环境）
export CUDA_VISIBLE_DEVICES=0,1

# 设置最大加载模型数
export OLLAMA_MAX_LOADED_MODELS=2

# 模型在内存中的保持时间（默认 5m）
export OLLAMA_KEEP_ALIVE=30m

# 最大并行请求数
export OLLAMA_NUM_PARALLEL=4

# 禁用 GPU（强制 CPU 推理）
export OLLAMA_NO_GPU=1
```

**Linux systemd 配置：**

```bash
# 编辑服务文件
sudo systemctl edit ollama

# 添加环境变量
[Service]
Environment="OLLAMA_HOST=0.0.0.0:11434"
Environment="OLLAMA_MODELS=/data/ollama/models"

# 重载并重启
sudo systemctl daemon-reload
sudo systemctl restart ollama
```

**Windows 配置：**
通过「系统属性 → 高级 → 环境变量」添加以上变量，或在 PowerShell 中：

```powershell
$env:OLLAMA_HOST = "0.0.0.0:11434"
# 重启 Ollama 服务生效
```

### 9.4 性能优化建议

| 场景       | 建议                                         |
| ---------- | -------------------------------------------- |
| 显存不足   | 使用更小的模型或更低的量化版本（如 q4_0）    |
| 推理速度慢 | 减小 `num_ctx`（上下文窗口），使用量化模型 |
| 多用户并发 | 设置 `OLLAMA_NUM_PARALLEL`，使用更小模型   |
| 长文本处理 | 增大 `num_ctx`（需要更多显存）             |
| 生成质量差 | 使用更大模型、提高量化精度、调整 temperature |

### 9.5 模型选择之显存对照

```
模型大小      最低显存      推荐显存
───────────────────────────────────
  1B-3B        2 GB         4 GB
  7B (q4)      4 GB         8 GB
  7B (q8)      8 GB        10 GB
 13B (q4)      8 GB        12 GB
 14B (q4)     10 GB        16 GB
 32B (q4)     20 GB        24 GB
 70B (q4)     40 GB        48 GB
```

---

## 10. Docker 部署

### 10.1 CPU 模式

```bash
docker run -d \
  --name ollama \
  -p 11434:11434 \
  -v ollama_data:/root/.ollama \
  ollama/ollama
```

### 10.2 GPU 模式（NVIDIA）

```bash
# 先安装 nvidia-container-toolkit
# https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html

docker run -d \
  --name ollama \
  --gpus all \
  -p 11434:11434 \
  -v ollama_data:/root/.ollama \
  ollama/ollama
```

### 10.3 Docker Compose

```yaml
# docker-compose.yml
version: '3.8'

services:
  ollama:
    image: ollama/ollama
    container_name: ollama
    ports:
      - "11434:11434"
    volumes:
      - ollama_data:/root/.ollama
    deploy:
      resources:
        reservations:
          devices:
            - driver: nvidia
              count: all
              capabilities: [gpu]
    restart: unless-stopped

volumes:
  ollama_data:
```

```bash
docker compose up -d

# 在容器中拉取模型
docker exec -it ollama ollama pull qwen2.5:7b

# 在容器中运行模型
docker exec -it ollama ollama run qwen2.5:7b
```

---

## 11. 搭配 Open WebUI 图形界面

Open WebUI 是最流行的 Ollama 前端界面，提供类似 ChatGPT 的交互体验。

### 11.1 Docker 一键部署

```bash
docker run -d \
  --name open-webui \
  -p 3000:8080 \
  --add-host=host.docker.internal:host-gateway \
  -v open-webui:/app/backend/data \
  -e OLLAMA_BASE_URL=http://host.docker.internal:11434 \
  --restart always \
  ghcr.io/open-webui/open-webui:main
```

### 11.2 Docker Compose（Ollama + Open WebUI 一体化）

```yaml
# docker-compose.yml
version: '3.8'

services:
  ollama:
    image: ollama/ollama
    container_name: ollama
    ports:
      - "11434:11434"
    volumes:
      - ollama_data:/root/.ollama
    deploy:
      resources:
        reservations:
          devices:
            - driver: nvidia
              count: all
              capabilities: [gpu]
    restart: unless-stopped

  open-webui:
    image: ghcr.io/open-webui/open-webui:main
    container_name: open-webui
    ports:
      - "3000:8080"
    volumes:
      - open-webui_data:/app/backend/data
    environment:
      - OLLAMA_BASE_URL=http://ollama:11434
    depends_on:
      - ollama
    restart: unless-stopped

volumes:
  ollama_data:
  open-webui_data:
```

```bash
docker compose up -d
# 访问 http://localhost:3000 即可使用
```

首次访问需要注册管理员账号，之后即可在浏览器中选择模型进行对话。

---

## 12. 常见问题与排错

### Q1: 下载模型速度慢？

```bash
# 方法一：使用代理
export http_proxy=http://127.0.0.1:7890
export https_proxy=http://127.0.0.1:7890
ollama pull qwen2.5:7b

# 方法二：手动下载 GGUF 文件后导入
# 从 HuggingFace 镜像站下载模型，然后用 Modelfile 导入
```

### Q2: 端口被占用？

```bash
# 查看端口占用
lsof -i :11434          # Linux/macOS
netstat -ano | findstr 11434  # Windows

# 更换端口
export OLLAMA_HOST=0.0.0.0:11435
ollama serve
```

### Q3: 显存不足 (OOM)？

```bash
# 使用更小的量化版本
ollama run qwen2.5:7b-q4_0

# 减少上下文窗口
curl http://localhost:11434/api/generate -d '{
  "model": "qwen2.5:7b",
  "prompt": "hello",
  "options": {"num_ctx": 2048}
}'

# 卸载不需要的模型
ollama stop llama3.1
```

### Q4: 如何让局域网其他机器访问？

```bash
# 设置监听所有网络接口
export OLLAMA_HOST=0.0.0.0:11434

# 重启服务
sudo systemctl restart ollama

# 其他机器访问：http://<服务器IP>:11434
```

### Q5: 模型回答质量不好？

1. 尝试更大参数量的模型
2. 调整 `temperature`（创造性任务调高，精确任务调低）
3. 使用更详细的系统提示词（System Prompt）
4. 使用更高精度的量化版本（q8_0 或 fp16）
5. 增加上下文窗口大小（`num_ctx`）

### Q6: 如何查看模型存储位置？

```bash
# 默认路径
# Linux:   ~/.ollama/models
# macOS:   ~/.ollama/models
# Windows: C:\Users\<用户名>\.ollama\models

# 自定义路径
export OLLAMA_MODELS=/your/custom/path
```

---

## 13. 实战案例

### 案例一：搭建本地知识问答系统（RAG）

```python
"""
基于 Ollama + LangChain 的简单 RAG 系统
pip install ollama langchain langchain-ollama langchain-community chromadb
"""
from langchain_ollama import ChatOllama, OllamaEmbeddings
from langchain_community.vectorstores import Chroma
from langchain_core.prompts import ChatPromptTemplate
from langchain.text_splitter import RecursiveCharacterTextSplitter
from langchain_core.output_parsers import StrOutputParser
from langchain_core.runnables import RunnablePassthrough

# 1. 准备文档
documents = [
    "Ollama 是一个开源的本地大模型运行框架，支持在个人电脑上运行各种大语言模型。",
    "Ollama 支持的模型包括 Llama、Qwen、Mistral、Gemma 等主流开源模型。",
    "Ollama 提供 REST API，兼容 OpenAI 接口格式，方便集成到现有应用中。",
    "通过 Modelfile，用户可以自定义模型的系统提示词、参数和模板。",
]

# 2. 文本分块 & 向量化
text_splitter = RecursiveCharacterTextSplitter(chunk_size=200, chunk_overlap=20)
splits = text_splitter.create_documents(documents)

embeddings = OllamaEmbeddings(model="nomic-embed-text")
vectorstore = Chroma.from_documents(splits, embeddings)

# 3. 构建 RAG 链
retriever = vectorstore.as_retriever(search_kwargs={"k": 2})
llm = ChatOllama(model="qwen2.5:7b", temperature=0.3)

prompt = ChatPromptTemplate.from_template("""
根据以下上下文回答问题。如果上下文中没有相关信息，请说"我不确定"。

上下文：{context}

问题：{question}
""")

chain = (
    {"context": retriever, "question": RunnablePassthrough()}
    | prompt
    | llm
    | StrOutputParser()
)

# 4. 提问
answer = chain.invoke("Ollama 支持哪些模型？")
print(answer)
```

### 案例二：批量文档摘要工具

```python
"""
批量文档摘要工具
pip install ollama
"""
import ollama
import os

def summarize_file(filepath: str, model: str = "qwen2.5:7b") -> str:
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
  
    response = ollama.chat(
        model=model,
        messages=[
            {'role': 'system', 'content': '你是一个文档摘要专家，请用3-5个要点总结文档内容'},
            {'role': 'user', 'content': f'请总结以下文档：\n\n{content[:4000]}'}
        ]
    )
    return response['message']['content']

def batch_summarize(directory: str):
    for filename in os.listdir(directory):
        if filename.endswith(('.md', '.txt')):
            filepath = os.path.join(directory, filename)
            print(f"\n{'='*50}")
            print(f"文件: {filename}")
            print(f"{'='*50}")
            summary = summarize_file(filepath)
            print(summary)

# 使用
batch_summarize('./docs')
```

### 案例三：命令行 AI 助手

```python
"""
命令行交互式 AI 助手
pip install ollama rich
"""
import ollama
from rich.console import Console
from rich.markdown import Markdown

console = Console()
messages = []

def chat(user_input: str, model: str = "qwen2.5:7b"):
    messages.append({"role": "user", "content": user_input})
  
    stream = ollama.chat(
        model=model,
        messages=messages,
        stream=True,
    )
  
    full_response = ""
    console.print("\n[bold green]助手:[/bold green]")
    for chunk in stream:
        content = chunk['message']['content']
        full_response += content
        console.print(content, end="")
  
    messages.append({"role": "assistant", "content": full_response})
    console.print()

def main():
    console.print("[bold blue]本地 AI 助手[/bold blue] (输入 quit 退出)\n")
  
    while True:
        user_input = console.input("[bold yellow]你: [/bold yellow]")
        if user_input.lower() in ['quit', 'exit', 'bye']:
            console.print("[dim]再见！[/dim]")
            break
        if user_input.strip() == "":
            continue
        chat(user_input)

if __name__ == "__main__":
    main()
```

---

## 附：快速上手流程

```
┌─────────────────────────────────────────────────────┐
│  第一步：安装 Ollama                                  │
│  curl -fsSL https://ollama.com/install.sh | sh      │
├─────────────────────────────────────────────────────┤
│  第二步：拉取模型                                     │
│  ollama pull qwen2.5:7b                             │
├─────────────────────────────────────────────────────┤
│  第三步：开始对话                                     │
│  ollama run qwen2.5:7b                              │
├─────────────────────────────────────────────────────┤
│  第四步：API 集成                                     │
│  curl http://localhost:11434/v1/chat/completions    │
├─────────────────────────────────────────────────────┤
│  第五步：自定义模型                                    │
│  ollama create my-model -f Modelfile                │
├─────────────────────────────────────────────────────┤
│  第六步：搭配 WebUI                                   │
│  docker run ... open-webui                          │
└─────────────────────────────────────────────────────┘
```

---

> **提示：** Ollama 官方文档 [https://github.com/ollama/ollama](https://github.com/ollama/ollama) | 模型库 [https://ollama.com/library](https://ollama.com/library) | Open WebUI [https://github.com/open-webui/open-webui](https://github.com/open-webui/open-webui)
