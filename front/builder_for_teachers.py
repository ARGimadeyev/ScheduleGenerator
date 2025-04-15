# builder.py
import json
import re
from collections import defaultdict

# Загрузка данных
with open('data/maps/subjects.json', 'r', encoding='utf-8') as f:
    subjects = json.load(f)

with open('data/maps/teachers.json', 'r', encoding='utf-8') as f:
    teachers = json.load(f)

# with open('data/maps/class_colors.json', 'w', encoding='utf-8') as f:
#     class_colors = {f"{7 + i//5}.{(i%5)+1}": f"hsl({(i*137)%360}, 70%, 80%)" for i in range(25)}
#     json.dump(class_colors, f)

with open('data/maps/class_colors.json', 'r') as f:
    class_colors = json.load(f)

with open('data/maps/subject_short.json', 'r', encoding='utf-8') as f:
    subject_short = json.load(f)
# Загрузка расписания
schedule_data = []
with open('data/Schedule.txt', 'r', encoding='utf-8') as f:
    for line in f:
        parts = list(map(int, line.strip().split()))
        if parts: schedule_data.append(parts)

# Параметры
DAYS_PER_WEEK = 6
LESSONS_PER_DAY = 9

# Создание структуры данных для учителей
teacher_schedules = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))

for class_idx in range(25):
    class_num = f"{class_idx // 5 + 7}.{(class_idx % 5) + 1}"
    if class_num == "9.5": continue

    for day in range(DAYS_PER_WEEK):
        for lesson in range(LESSONS_PER_DAY):
            pos = class_idx * DAYS_PER_WEEK * LESSONS_PER_DAY + day * LESSONS_PER_DAY + lesson
            if pos >= len(schedule_data): continue

            entry = schedule_data[pos]
            if entry[0] == 0: continue

            subject_id = entry[-2]
            teacher_ids = entry[1:-2]
            subject = subjects.get(str(subject_id), "Unknown")

            for t_id in teacher_ids:
                teacher = teachers.get(str(t_id), "Unknown")
                teacher_schedules[teacher][day][lesson].append({
                    'class': class_num,
                    'subject': subject,
                    'color': class_colors[class_num],
                    'subject_short': subject_short[subject]
                })

HTML_TEMPLATE = '''
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        .back-button {{
            position: fixed;
            top: 20px;
            left: 20px;
            padding: 10px 20px;
            background: #3498db;
            color: white;
            border-radius: 20px;
            text-decoration: none;
            z-index: 1000;
            box-shadow: 0 2px 8px rgba(0,0,0,0.1);
            transition: transform 0.2s;
        }}
        .back-button:hover {{
            transform: scale(1.05);
        }}
        .teacher-table {{
            margin: 20px;
            border-collapse: collapse;
            font-family: Arial, sans-serif;
            width: 90%;
        }}
        .teacher-table td, .teacher-table th {{
            border: 2px solid #ffffff;
            padding: 12px;
            text-align: center;
            position: relative;
            transition: all 0.3s;
            min-width: 100px;
            height: 60px;
            background-color: #f8f9fa; /* Одинаковый фон для всех ячеек */
        }}
        .class {{
            font-weight: 800;
            font-size: 18px;
            margin-bottom: 4px;
        }}
        .subject {{
            font-size: 12px;
            color: #444;
            letter-spacing: -0.5px;
        }}
        td[data-has-content="true"]:hover {{
            transform: scale(1.8);
            z-index: 100;
            box-shadow: 0 0 15px rgba(0,0,0,0.4);
            cursor: pointer;
        }}
        td:not([data-has-content="true"]) {{
            cursor: default !important;
        }}
        .full-info {{
            display: none;
            position: absolute;
            top: 110%;
            left: 50%;
            transform: translateX(-50%);
            background: white;
            padding: 10px;
            border: 2px solid #ddd;
            border-radius: 5px;
            z-index: 200;
            min-width: 200px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.15);
        }}
        td:hover .full-info {{
            display: block;
        }}
        .day-header {{
            background: #f8f9fa;
            font-weight: bold;
        }}
    </style>
</head>
<body>
    <a href="../../index.html" class="back-button">← Назад</a>
    {content}
</body>
</html>
'''

# Генерация общего расписания учителей
all_teachers_html = '<h1 style="margin-left:50px;">Общее расписание учителей</h1>'
for teacher, schedule in teacher_schedules.items():
    teacher_html = f'<h2 style="margin:30px 0 10px 50px;">{teacher}</h2><table class="teacher-table">'
    teacher_html += '<tr><th class="day-header">День</th>' + ''.join(
        [f'<th class="day-header">Урок {i + 1}</th>' for i in range(LESSONS_PER_DAY)]
    ) + '</tr>'

    for day in range(DAYS_PER_WEEK):
        teacher_html += f'<tr><td class="day-header">День {day + 1}</td>'
        for lesson in range(LESSONS_PER_DAY):
            lessons = schedule[day][lesson]
            if lessons:
                content = []
                content2 = []
                for l in lessons:
                    content.append(
                        f'<div class="class" style="color:{l["color"]}">{l["class"]}</div>'
                        f'<div class="subject">{l["subject_short"]}</div>'
                    )
                    content2.append(
                        f'''
                        <div style="font-size:16px;margin-bottom:6px;">{l['class']}</div>
                        <div style="font-size:14px;color:#666;">{l['subject']}</div>'''
                    )
                cell = f'''
                <td data-has-content="true">
                    {''.join(content)}
                    <div class="full-info">
                        {''.join(content2)}
                    </div>
                </td>
                '''
            else:
                cell = '<td>-</td>'
            teacher_html += cell
        teacher_html += '</tr>'
    teacher_html += '</table>'
    all_teachers_html += teacher_html

    # Сохранение индивидуального расписания
    with open(f'front/pages/teachers/{teacher}.html', 'w', encoding='utf-8') as f:
        f.write(HTML_TEMPLATE.format(content=teacher_html))

# Сохранение общего расписания
with open('front/pages/teachers/all_teachers.html', 'w', encoding='utf-8') as f:
    f.write(HTML_TEMPLATE.format(content=all_teachers_html))
