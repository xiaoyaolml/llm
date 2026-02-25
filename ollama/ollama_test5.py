import ollama

response = ollama.embed(
    model='qwen3-embedding:latest',
    input='熊猫老师爱编程'
)
print(f"嵌入向量的长度: {len(response['embeddings'][0])}")
print(f"嵌入向量的内容: {response['embeddings'][0]}")