__import__("pysqlite3")
import sys

sys.modules["sqlite3"] = sys.modules.pop("pysqlite3")

from langchain_ollama import ChatOllama, OllamaEmbeddings
from langchain_community.vectorstores import Chroma
from langchain_core.prompts import ChatPromptTemplate
from langchain.text_splitter import RecursiveCharacterTextSplitter
from langchain_core.output_parsers import StrOutputParser
from langchain_core.runnables import RunnablePassthrough

# 模拟一段私有文档的内容
documents = [
    "熊猫老师的爱好是编程和国际象棋。",
    "熊猫老师擅长使用 C++ 和 Python 进行编程开发和算法竞赛。",
    "熊猫老师擅长使用 AI 辅助教学，并把 AI 技术应用于教育领域。",
    "熊猫老师经常与大家分享一些编程知识和 AI 技术。",
    "熊猫老师在小红书上的唯一账号'绍兴编程联盟'受到很多绍兴学霸家长的喜欢。",
    "熊猫老师创建的'绍兴信息学交流群'已经有很多关注编程喜欢编程的家长和同学加入。",
]

# 1. 数据采集与切片处理
text_splitter = RecursiveCharacterTextSplitter(chunk_size=200, chunk_overlap=20)
splits = text_splitter.create_documents(documents)
# 2. 文本内容向量化数据
embeddings = OllamaEmbeddings(model="qwen3-embedding:latest")
# 3. 向量数据库存储
vectorstore = Chroma.from_documents(splits, embeddings)
# 4. 检索数据
retriever = vectorstore.as_retriever(search_kwargs={"k": 2})
# 5. 构建RAG链 生成回答
llm = ChatOllama(model="qwen3:8b", temperature=0.1)

# 提示词设计
prompt = ChatPromptTemplate.from_template(
    """
根据以下知识库内容回答问题。如果知识库内容中没有相关信息，请说"我不确定"。

知识库内容：{context}

问题：{question}
"""
)
# 链式调用
chain = (
    {"context": retriever, "question": RunnablePassthrough()}
    | prompt
    | llm
    | StrOutputParser()
)

# 简单的问答测试
question1 = "熊猫老师喜欢什么？"
answer1 = chain.invoke(question1)
print(f"问：{question1}")
print(f"答：{answer1}")

question2 = "熊猫老师最喜欢的是什么？"
answer2 = chain.invoke(question2)
print(f"问：{question2}")
print(f"答：{answer2}")
