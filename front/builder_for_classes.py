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
                initials = ''.join([p[0] for p in parts if p][:3])  # Берем первые 3 буквы
                teachers_list.append(initials)

            class_schedules[class_idx][day][lesson] = {
                'short': short_subject,
                'full_subj': full_subject,
                'teachers': ' '.join(teachers_list),
                'full_teachers': ', '.join(teachers_full_list),
                'color': bg_color
            }

HTML = '''
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        .class-table {
            margin: 20px;
            border-collapse: collapse;
            font-family: Arial, sans-serif;
            width: 90%;
        }
        .class-table td, .class-table th {
            border: 2px solid #ffffff;
            padding: 12px;
            text-align: center;
            position: relative;
            transition: all 0.3s;
            min-width: 100px;
            height: 60px;
            cursor: pointer;
        }
        .subject {
            font-weight: 800;
            font-size: 18px;
            pointer-events: none;
            margin-bottom: 4px;
        }
        .teachers {
            font-size: 12px;
            color: #444;
            pointer-events: none;
            letter-spacing: -0.5px;
        }
        td:hover {
            transform: scale(1.8);
            z-index: 100;
            box-shadow: 0 0 15px rgba(0,0,0,0.4);
            font-weight: bold;
        }
        .full-info {
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
        }
        td:hover .full-info {
            display: block;
        }
        .day-header {
            background: #f8f9fa;
            font-weight: bold;
        }
    </style>
</head>
<body>
'''
html = HTML
for class_idx in range(TOTAL_CLASSES):
    class_num = f"{class_idx // 5 + 7}.{(class_idx % 5) + 1}"
    if class_num == "9.5": continue
    classHtml = HTML

    classHtml += f'<h2 style="margin:30px 0 10px 50px;">Класс {class_num}</h2>'
    html += f'<h2 style="margin:30px 0 10px 50px;">Класс {class_num}</h2>'

    html += '<table class="class-table">'
    classHtml += '<table class="class-table">'

    html += '<tr><th class="day-header">День</th>' + ''.join(
        [f'<th class="day-header">Урок {i + 1}</th>' for i in range(LESSONS_PER_DAY)]
    ) + '</tr>'
    classHtml += '<tr><th class="day-header">День</th>' + ''.join(
        [f'<th class="day-header">Урок {i + 1}</th>' for i in range(LESSONS_PER_DAY)]
    ) + '</tr>'
    for day in range(DAYS_PER_WEEK):
        html += f'<tr><td class="day-header">День {day + 1}</td>'
        classHtml += f'<tr><td class="day-header">День {day + 1}</td>'
        for lesson in range(LESSONS_PER_DAY):
            data = class_schedules[class_idx][day][lesson]
            if data:
                cell = f'''
                <td style="background-color:{data['color']}">
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

            html += cell
            classHtml += cell

        html += '</tr>'
        classHtml += '</tr>'

    html += '</table>'
    classHtml += '</table>'

    classHtml += '</body></html>'
    with open(f'front/pages/{class_num}.html', 'w', encoding='utf-8') as f:
        f.write(classHtml)

html += '</body></html>'

with open('front/pages/schedule.html', 'w', encoding='utf-8') as f:
    f.write(html)
