import json

with open('data/maps/teachers.json', 'r', encoding='utf-8') as f:
    teachers = json.load(f)
with open('data/DayOff.txt', 'w', encoding='utf-8') as f:
    for i in range(1, 62):
        print(teachers[f"{i}"])
        s = input()
        ans = f"{i}"
        if s[0] == '-':
            ans += " -1"
        else:
            for _ in range(5):
                for j in range(9):
                    ans += ' ' + s[j]
                s = input()
            for j in range(9):
                ans += ' ' + s[j]
        ans += '\n'
        print(ans)
        f.write(ans)
