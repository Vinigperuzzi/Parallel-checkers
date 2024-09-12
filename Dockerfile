FROM ubuntu:20.04 as builder
FROM python:3.11.0

WORKDIR /app

COPY requirements.txt ./

RUN pip install -r requirements.txt

COPY . .

CMD ["python", "checkers.py"]