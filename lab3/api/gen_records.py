import random
from datetime import datetime, timedelta

def generate_sql_inserts(num_records=50):
    """
    Функция для генерации SQL команд INSERT
    """
    users = [i + 1 for i in range(num_records // 10)]  # список пользователей
    states = ['start', 'stop']  # возможные состояния
    start_date = datetime.now()  # начальная точка времени

    sql_statements = []
    for _ in range(num_records):
        user_id = random.choice(users)
        state = random.choice(states)
        time_diff = timedelta(minutes=random.randint(5, 120))
        current_time = (start_date + time_diff).strftime('%Y-%m-%d %H:%M:%S')
        start_date += time_diff
        sql_statements.append(f"INSERT INTO user_activity (user_id, state, time) VALUES ({user_id}, '{state}', '{current_time}');\n")

    return sql_statements

def save_sql_to_file(filename, sql_statements):
    """
    Функция для сохранения SQL команд в файл
    """
    with open(filename, 'w') as f:
        for statement in sql_statements:
            f.write(statement)

# Генерация SQL команд
sql_inserts = generate_sql_inserts(1000)  # Сгенерировать 50 записей

# Сохранение SQL команд в файл
save_sql_to_file('insert_data.sql', sql_inserts)

print("SQL file created successfully.")