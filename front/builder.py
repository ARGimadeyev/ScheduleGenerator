import json
from collections import defaultdict

with open('../data/subjects.json', 'r', encoding='utf-8') as f:
    subjects = json.load(f)

with open('../data/teachers.json', 'r', encoding='utf-8') as f:
    teachers = json.load(f)

schedule_data = []
with open('../data/Schedule.txt', 'r', encoding='utf-8') as f:
    for line in f:
        parts = list(map(int, line.strip().split()))
        if not parts:
            continue
        schedule_data.append(parts)

# Параметры
DAYS_PER_WEEK = 6
LESSONS_PER_DAY = 9
TOTAL_CLASSES = 25  # Примерное количество классов (11-7+1)*5=25, но зависит от данных

# Словарь для хранения расписания: class_idx -> day -> lesson -> data
class_schedules = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))

# Обработка данных
for class_idx in range(TOTAL_CLASSES):
    for day in range(DAYS_PER_WEEK):
        for lesson in range(LESSONS_PER_DAY):
            pos = class_idx * DAYS_PER_WEEK * LESSONS_PER_DAY + day * LESSONS_PER_DAY + lesson
            if pos >= len(schedule_data):
                exit(-2)
            entry = schedule_data[pos]
            if entry[0] == 0 and entry[1] == -1:  # Нет урока
                continue

            subject_id = entry[-2]
            teacher_ids = entry[1:-2]

            # Получение названия предмета
            subject = subjects.get(str(subject_id), "Unknown Subject")
            subj_short = subject

            # Получение ФИО учителей
            teachers_short = []
            for t_id in teacher_ids:
                teacher = teachers.get(str(t_id), "Unknown Teacher")
                parts = teacher.split()
                initials = ''.join([p[0] + '.' for p in parts[:-1]])
                teachers_short.append(f"{initials}{parts[-1]}")

            class_schedules[class_idx][day][lesson] = {
                'subject': subj_short,
                'teachers': ', '.join(teachers_short)
            }

# Генерация HTML
html = '''
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        .class-table { 
            margin: 20px; 
            border-collapse: collapse; 
        }
        .class-table td, .class-table th {
            border: 1px solid #ddd;
            padding: 8px;
            text-align: center;
        }
        .subject { font-weight: bold; font-size: 14px; }
        .teachers { font-size: 12px; color: #666; }
    </style>
</head>
<body>
'''

for class_idx in range(TOTAL_CLASSES):
    html += f'<h2>Класс {class_idx//5 + 7}.{(class_idx % 5) + 1}</h2>'
    html += '<table class="class-table">'
    html += '<tr><th>День</th>' + ''.join([f'<th>Урок {i + 1}</th>' for i in range(LESSONS_PER_DAY)]) + '</tr>'

    for day in range(DAYS_PER_WEEK):
        html += f'<tr><td>День {day + 1}</td>'
        for lesson in range(LESSONS_PER_DAY):
            data = class_schedules[class_idx][day][lesson]
            if data:
                cell = f'<td><div class="subject">{data["subject"]}</div><div class="teachers">{data["teachers"]}</div></td>'
            else:
                cell = '<td>-</td>'
            html += cell
        html += '</tr>'
    html += '</table>'

html += '</body></html>'

with open('../schedule.html', 'w', encoding='utf-8') as f:
    f.write(html)