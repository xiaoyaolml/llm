import ollama

# 单轮对话
response = ollama.chat(
    model="gesp-coach:latest",
    messages=[{"role": "user", "content": "什么是快速排序？"}],
)
print(response["message"]["content"])
