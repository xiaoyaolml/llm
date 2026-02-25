from langchain_ollama import ChatOllama
from langchain_core.prompts import ChatPromptTemplate

llm = ChatOllama(
    model="qwen3:8b",
    base_url="http://localhost:11434"
)

prompt = ChatPromptTemplate.from_messages([
    ("system", "你是一个专业的AI大模型助手"),
    ("user", "{question}")
])
# 链式调用
chain = prompt | llm
response = chain.invoke({"question": "简单介绍一下LangChain"})
print(response.content)