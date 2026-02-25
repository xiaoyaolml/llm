from openai import OpenAI
# 初始化客户端，指向本地 Ollama 服务
client = OpenAI(
    base_url='http://localhost:11434/v1',
    api_key='ollama'  # 任意值即可
)
response = client.chat.completions.create(
    model='qwen3:8b',
    messages=[
        {'role': 'system', 'content': '你是一个专业的AI大模型助手'},
        {'role': 'user', 'content': '解释一下什么是 RAG'}
    ]
)
print(response.choices[0].message.content)
