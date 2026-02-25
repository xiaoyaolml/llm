import ollama

messages = []
while True:
    user_input = input("\n你: ")
    if user_input.lower() in ["quit", "exit", "bye"]:
        break

    messages.append({"role": "user", "content": user_input})

    response = ollama.chat(model="gesp-coach:latest", messages=messages)
    assistant_msg = response["message"]["content"]

    print(f"\nGESP编程助手: {assistant_msg}")
    messages.append({"role": "assistant", "content": assistant_msg})
