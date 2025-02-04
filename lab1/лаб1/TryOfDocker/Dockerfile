FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

# Устанавливаем необходимые пакеты
RUN apt-get update && \
    apt-get install -y build-essential cmake && \
    apt-get clean

# Устанавливаем рабочий каталог
WORKDIR /usr/src/app

# Копируем содержимое вашего проекта в контейнер
COPY src/ .

# Команда для сборки проекта
CMD ["bash", "-c", "cmake . && make"] 
