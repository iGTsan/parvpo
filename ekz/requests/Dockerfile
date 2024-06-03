FROM python:3.9-slim
RUN pip install websockets requests
COPY app.py /app/app.py
WORKDIR /app
CMD ["python", "app.py"]