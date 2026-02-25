import ollama

# 提供一张名为 'aigcml2026.png' 的图片
with open("aigcml2026.png", "rb") as f:
    response = ollama.chat(
        model="qwen3-vl:latest",
        messages=[
            {
                "role": "user",
                "content": "请详细描述这张图片的内容。",
                "images": [f.read()],
            }
        ],
    )
print(response["message"]["content"])
