import json
import re
from collections import defaultdict

with open('data/maps/subjects.json', 'r', encoding='utf-8') as f:
    subjects = json.load(f)

with open('data/maps/teachers.json', 'r', encoding='utf-8') as f:
    teachers = json.load(f)

with open('data/maps/subject_colors.json', 'r', encoding='utf-8') as f:
    subject_colors = json.load(f)

with open('data/maps/subject_short.json', 'r', encoding='utf-8') as f:
    subject_short = json.load(f)


def shorten_subject(name):
    return subject_short.get(name)


schedule_data = []
with open('data/Schedule.txt', 'r', encoding='utf-8') as f:
    for line in f:
        parts = list(map(int, line.strip().split()))
        if parts: schedule_data.append(parts)

DAYS_PER_WEEK = 6
LESSONS_PER_DAY = 9
TOTAL_CLASSES = 25

class_schedules = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))

for class_idx in range(TOTAL_CLASSES):
    for day in range(DAYS_PER_WEEK):
        for lesson in range(LESSONS_PER_DAY):
            pos = class_idx * DAYS_PER_WEEK * LESSONS_PER_DAY + day * LESSONS_PER_DAY + lesson
            if pos >= len(schedule_data): continue

            entry = schedule_data[pos]
            if entry[0] == 0 and entry[1] == -1: continue

            subject_id = entry[-2]
            teacher_ids = entry[1:-2]

            full_subject = subjects.get(str(subject_id), "Unknown")
            short_subject = shorten_subject(full_subject)
            bg_color = subject_colors.get(full_subject, "#FFFFFF")

            teachers_list = []
            teachers_full_list = []
            for t_id in teacher_ids:
                teacher = teachers.get(str(t_id), "")
                teachers_full_list.append(teacher)
                parts = re.split(r'[,.?\n ]+', teacher)
                initials = ''.join([p[0] for p in parts if p][:3])
                teachers_list.append(initials)

            class_schedules[class_idx][day][lesson] = {
                'short': short_subject,
                'full_subj': full_subject,
                'teachers': ' '.join(teachers_list),
                'full_teachers': ', '.join(teachers_full_list),
                'color': bg_color
            }

HTML_BASE = '''<!DOCTYPE html>
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
        .class-table {{
            margin: 20px auto;
            border-collapse: collapse;
            font-family: Arial, sans-serif;
            width: 90%;
        }}
        .class-table td, .class-table th {{
            border: 2px solid #ffffff;
            padding: 12px;
            text-align: center;
            position: relative;
            transition: all 0.3s;
            min-width: 100px;
            height: 60px;
        }}
        .subject {{
            font-weight: 800;
            font-size: 18px;
            margin-bottom: 4px;
        }}
        .teachers {{
            font-size: 12px;
            color: #444;
            letter-spacing: -0.5px;
        }}
        td[data-has-content="true"]:hover {{
    transform: scale(1.6); /* Уменьшили масштаб */
    z-index: 100;
    box-shadow: 0 0 15px rgba(0,0,0,0.4);
    cursor: pointer;
    transition: all 0.4s cubic-bezier(0.25, 0.46, 0.45, 0.94); /* Увеличили время анимации */
}}
        td:not([data-has-content="true"]) {{
            cursor: default;
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
    <a href="../index.html" class="back-button">← Назад</a>
    {content}
</body>
</html>'''


def generate_class_html(class_idx):
    class_num = f"{class_idx // 5 + 7}.{(class_idx % 5) + 1}"
    if class_num == "9.5":
        return ""

    content = []
    content.append(f'<h2 style="margin: 30px auto; text-align: center;">Класс {class_num}</h2>')
    content.append('<table class="class-table">')
    content.append('<tr><th class="day-header">День</th>' +
                   ''.join(f'<th class="day-header">Урок {i + 1}</th>' for i in range(LESSONS_PER_DAY)) +
                   '</tr>')

    for day in range(DAYS_PER_WEEK):
        row = [f'<tr><td class="day-header">День {day + 1}</td>']
        for lesson in range(LESSONS_PER_DAY):
            data = class_schedules[class_idx][day][lesson]
            if data:
                cell = f'''
                <td style="background-color:{data['color']}" data-has-content="true">
                    <div class="subject">{data['short']}</div>
                    <div class="teachers">{data['teachers']}</div>
                    <div class="full-info">
                        <div style="font-size:16px;margin-bottom:6px;">{data['full_subj']}</div>
                        <div style="font-size:14px;color:#666;">{data['full_teachers']}</div>
                    </div>
                </td>
                '''
            else:
                cell = '<td style="background-color:#f8f9fa">-</td>'
            row.append(cell)
        content.append(''.join(row) + '</tr>')

    content.append('</table>')
    return '\n'.join(content)


# Генерация отдельных файлов классов
for class_idx in range(TOTAL_CLASSES):
    class_html = generate_class_html(class_idx)
    if not class_html:
        continue

    # Сохраняем отдельный файл
    class_num = f"{class_idx // 5 + 7}.{(class_idx % 5) + 1}"
    with open(f'front/pages/{class_num}.html', 'w', encoding='utf-8') as f:
        f.write(HTML_BASE.format(content=class_html))

# Генерация общего расписания
all_content = []
for class_idx in range(TOTAL_CLASSES):
    class_content = generate_class_html(class_idx)
    if class_content:
        all_content.append(class_content)

with open('front/pages/schedule.html', 'w', encoding='utf-8') as f:
    f.write(HTML_BASE.format(content='\n'.join(all_content)))
