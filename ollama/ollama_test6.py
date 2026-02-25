import asyncio
import ollama


async def main():
    client = ollama.AsyncClient()
    response = await client.chat(
        model="gesp-coach:latest", messages=[{"role": "user", "content": "你是谁"}]
    )
    print(response["message"]["content"])


asyncio.run(main())
