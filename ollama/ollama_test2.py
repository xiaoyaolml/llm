import ollama

stream = ollama.chat(
    model="gesp-coach:latest",
    messages=[{"role": "user", "content": "什么是归并排序？"}],
    stream=True,
)

for chunk in stream:
    print(chunk["message"]["content"], end="", flush=True)
