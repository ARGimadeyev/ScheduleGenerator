// #pragma GCC optimize("Ofast, unroll-loops")
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace __gnu_pbds;
using namespace std;

using ll = long long;
// using i128 = __int128_t;
using ld = long double;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

void solve(), solve1();

int32_t main() {
    freopen("ListLesson.txt", "r", stdin);
    // setlocale(LC_ALL, "Russian");
    solve();
}

constexpr int DAYS_PER_WEEK = 6;
constexpr int LESSONS_PER_DAY = 9;
constexpr int TOTAL_SLOTS = DAYS_PER_WEEK * LESSONS_PER_DAY; // 54
constexpr int CLASSES_COUNT = 5;
constexpr int CLASSES = 11 - 7 + 1;

constexpr double COOLING_RATE = 0.9998;
constexpr ll INF = 1e18;

struct Lesson {
    string teacher = "-1", lesson;
    // int Class;

    Lesson() {
    }

    Lesson(string &teacher, string &lesson, int Class) : teacher(teacher), lesson(lesson) {
    }

    bool operator ==(const Lesson &a) const {
        return lesson == a.lesson && teacher == a.teacher; // && Class == a.Class;
    }

    bool operator <(const Lesson &a) const {
        return lesson < a.lesson;
    }
};

// map<string, int> metod_day;

struct Schedule {
    ld cost = INF;
    Lesson matrix[CLASSES * CLASSES_COUNT][TOTAL_SLOTS];


    void swp(int Class, int i, int j) {
        swap(matrix[Class][i], matrix[Class][j]);
    }

    void recnt() {
        cost = 0;

        for (int j = 0; j < DAYS_PER_WEEK; ++j) {
            map<string, int> first, last, colT;
            for (int k = 0; k < LESSONS_PER_DAY; ++k) {
                map<string, bool> fall;
                for (int i = 0; i < CLASSES * CLASSES_COUNT; ++i) {
                    Lesson f = matrix[i][j * LESSONS_PER_DAY + k];
                    if (f.teacher == "-1") continue;
                    if (first.find(f.teacher) == first.end()) {
                        first[f.teacher] = k;
                    } else {
                        first[f.teacher] = min(first[f.teacher], k);
                    }
                    if (last.find(f.teacher) == last.end()) {
                        last[f.teacher] = k;
                    } else {
                        last[f.teacher] = max(last[f.teacher], k);
                    }
                    fall[f.teacher] = true;
                }
                for (auto [k,g]: fall) {
                    colT[k] += g;
                }
            }
            for (auto [k1,col1]: first) {
                cost += (1 - ((ld) colT[k1] / (last[k1] - col1 + 1)) > 0.33 ? 30000 : 0);
            }
        }
        //===============================================================
        for (int i = 0; i < CLASSES * CLASSES_COUNT; ++i) {
            map<Lesson, int> col;
            for (int j = 0; j < DAYS_PER_WEEK * LESSONS_PER_DAY; ++j) {
                col[matrix[i][j]]++;
            }
            for (int j = 0; j < DAYS_PER_WEEK; ++j) {
                map<Lesson, int> cl;
                for (int k = 0; k < LESSONS_PER_DAY; ++k) {
                    Lesson f = matrix[i][j * LESSONS_PER_DAY + k];
                    if (f.teacher == "-1") continue;
                    cl[f]++;
                }
                for (auto [k1,col1]: cl) {
                    if (col[k1] <= 3) {
                        cost += 20000 * (col1 > 1);
                    } else {
                        cost += (2 * col1 > col[k1] ? 40000 : -1000);
                    }
                }
            }
            //==============================================================
            for (int j = 0; j < DAYS_PER_WEEK; ++j) {
                for (int k = 0; k < LESSONS_PER_DAY; ++k) {
                    Lesson f = matrix[i][j * LESSONS_PER_DAY + k];
                    if (f.teacher == "-1") continue;
                }
            }
            //==============================================================
            for (int j = 0; j < DAYS_PER_WEEK; ++j) {
                for (int k = 0; k < LESSONS_PER_DAY; ++k) {
                    Lesson f = matrix[i][j * LESSONS_PER_DAY + k];

                    if (f.teacher == "-1" && k <= 5) cost += 40000;
                }
            }
        }

        for (int j = 0; j < TOTAL_SLOTS; ++j) {
            set<string> teachers;
            vector<bool> used(CLASSES * CLASSES_COUNT);
            for (int i = 0; i < CLASSES * CLASSES_COUNT; i++) {
                string teacher = matrix[i][j].teacher;
                if (teacher == "-1") continue;
                if (teachers.find(teacher) != teachers.end()) {
                    cost += 1'000'000'000;
                }
                teachers.insert(teacher);
            }
        }
    }

    bool is_normal() const {
        return true;
        bool normal = true;
        // for (int i = 0; i < CLASSES * CLASSES_COUNT; i++) {
        //     for (int j = 0; j < TOTAL_SLOTS; j++) {
        //         int day = j % LESSONS_PER_DAY;
        //         string teacher = matrix[i][j].teacher;
        //         if (teacher == "-1") continue;
        //         if (metod_day[teacher] == day) {
        //             normal = false;
        //             break;
        //         }
        //     }
        // }
        for (int j = 0; j < TOTAL_SLOTS; ++j) {
            set<string> teachers;
            vector<bool> used(CLASSES * CLASSES_COUNT);
            for (int i = 0; i < CLASSES * CLASSES_COUNT; i++) {
                string teacher = matrix[i][j].teacher;
                if (teacher == "-1") continue;
                if (teachers.find(teacher) != teachers.end()) {
                    normal = false;
                    break;
                }
                teachers.insert(teacher);
            }
        }
        return normal;
    }
};

ld rnd() {
    return ld(rng()) / (UINT_MAX);
}

map<string, vector<vector<string> > > parseInputData() {
    map<string, vector<vector<string> > > classes;

    vector<string> input;
    string s;
    while (getline(cin, s)) {
        input.emplace_back(s);
    }

    string currentClass;
    vector<vector<string> > days;

    for (string &line: input) {
        if (line.empty()) continue;

        // Обработка названия класса
        if (line.find('.') != string::npos) {
            if (!currentClass.empty()) {
                classes[currentClass] = days;
                days.clear();
            }
            currentClass = line.substr(0, line.find(':'));
            continue;
        }

        // Обработка дня
        if (line.find("Day") != string::npos) {
            vector<string> lessons;
            stringstream ss(line.substr(line.find(':') + 2));
            string lesson;

            while (ss >> lesson) {
                lessons.push_back(lesson);
            }

            days.push_back(lessons);
        }
    }

    if (!currentClass.empty()) {
        classes[currentClass] = days;
    }

    return classes;
}

int getClassIndex(const string &className) {
    int grade = stoi(className.substr(0, className.find('.')));
    int parallel = stoi(className.substr(className.find('.') + 1));
    return (grade - 6) * 5 + (parallel - 1); // 5 параллелей на класс
}

void fillSchedule(Schedule &schedule, const map<string, vector<vector<string> > > &data) {
    for (const auto &[className, days]: data) {
        int classIdx = getClassIndex(className);

        for (int dayIdx = 0; dayIdx < days.size(); ++dayIdx) {
            for (int lessonIdx = 0; lessonIdx < days[dayIdx].size(); ++lessonIdx) {
                int slot = dayIdx * LESSONS_PER_DAY + lessonIdx;
                string lessonStr = days[dayIdx][lessonIdx];

                if (lessonStr == "-") {
                    schedule.matrix[classIdx][slot] = Lesson();
                    continue;
                }

                size_t dashPos = lessonStr.find('-');
                Lesson lesson;
                lesson.teacher = lessonStr.substr(dashPos + 1);
                lesson.lesson = lessonStr.substr(0, dashPos);
                schedule.matrix[classIdx][slot] = lesson;
            }
        }
    }
}

string getDayName(int day) {
    const vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    return days[day];
}

string getLessonTime(int lesson_num) {
    map<int, string> times = {
        {1, "8:15 - 8:55"}, {2, "9:05 - 9:45"}, {3, "10:00 - 10:40"},
        {4, "10:55 - 11:35"}, {5, "11:45 - 12:25"}, {6, "12:35 - 13:45"},
        {7, "13:50 - 14:30"}, {8, "14:35 - 15:15"}, {9, "15:20 - 16:00"}
    };
    return times[lesson_num];
}

string getClassName(int class_id) {
    int grade = 7 + (class_id / CLASSES_COUNT);
    char parallel = '1' + (class_id % CLASSES_COUNT); // English letters
    return to_string(grade) + "-" + parallel;
}

string getTeacherCode(const string &teacher) {
    size_t dash = teacher.find('-');
    return (dash != string::npos) ? teacher.substr(dash + 1) : teacher;
}

map<string, string> subject_abbreviations = {
    {"ANGL", "ENG"}, {"BIO", "BIO"}, {"GEOG", "GEO"},
    {"MAT", "MATH"}, {"INF", "INF"}, {"FIZ", "PHY"},
    {"HIM", "CHEM"}, {"IST", "HIST"}, {"MUZ", "MUS"},
    {"TEC", "TECH"}, {"FIZRA", "PE"}
};

string abbreviateSubject(const string &subject) {
    auto it = subject_abbreviations.find(subject);
    return (it != end(subject_abbreviations)) ? it->second : subject;
}

void printSchedule(const Schedule &schedule) {
    // Generate colors for subjects
    map<string, string> subject_colors;
    vector<string> colors = {
        "#FFB3BA", "#FFDFBA", "#FFFFBA",
        "#BAFFC9", "#BAE1FF", "#D0BAFF",
        "#FFBAF0", "#87CEEB", "#98FB98",
        "#D2B48C", "#FFA07A", "#20B2AA"
    };

    // Collect all subjects and assign colors
    set<string> all_subjects;
    for (int c = 0; c < CLASSES * CLASSES_COUNT; ++c) {
        for (int s = 0; s < TOTAL_SLOTS; ++s) {
            const Lesson &lesson = schedule.matrix[c][s];
            if (lesson.teacher != "-1") {
                all_subjects.insert(lesson.lesson);
            }
        }
    }

    int color_idx = 0;
    for (const auto &subj: all_subjects) {
        subject_colors[subj] = colors[color_idx % colors.size()];
        color_idx++;
    }

    // Generate Class Schedules
    ofstream class_out("schedule_classes.html");
    class_out << "<html><head><style>"
            << "table { border-collapse: collapse; margin: 20px; }"
            << "th, td { border: 1px solid #ddd; padding: 8px; text-align: center; }"
            << ".class-schedule { margin-bottom: 40px; }"
            << "h2 { color: #2c3e50; }"
            << "small { font-size: 0.8em; opacity: 0.7; }"
            << "</style></head><body>";

    for (int c = 0; c < CLASSES * CLASSES_COUNT; ++c) {
        class_out << "<div class='class-schedule'><h2>Class " << getClassName(c) << "</h2><table>";
        class_out << "<tr><th>Day</th>";
        for (int l = 1; l <= 9; ++l)
            class_out << "<th>" << l << "<br>" << getLessonTime(l) << "</th>";
        class_out << "</tr>";

        for (int d = 0; d < 6; ++d) {
            class_out << "<tr><td>" << getDayName(d) << "</td>";
            for (int l = 0; l < 9; ++l) {
                int slot = d * 9 + l;
                const Lesson &lesson = schedule.matrix[c][slot];
                string style, cell;

                if (lesson.teacher != "-1") {
                    string bg_color = subject_colors[lesson.lesson];
                    style = "style='background-color:" + bg_color + "'";
                    cell = abbreviateSubject(lesson.lesson)
                           + "<br><small>" + getTeacherCode(lesson.teacher) + "</small>";
                }

                class_out << "<td " << style << ">" << cell << "</td>";
            }
            class_out << "</tr>";
        }
        class_out << "</table></div>";
    }
    class_out << "</body></html>";

    // Generate Teacher Schedules
    ofstream teacher_out("schedule_teachers.html");
    teacher_out << "<html><head><style>"
            << "table { border-collapse: collapse; margin: 20px; }"
            << "th, td { border: 1px solid #ddd; padding: 8px; text-align: center; }"
            << ".teacher-schedule { margin-bottom: 40px; }"
            << "h2 { color: #2c3e50; }"
            << "td { max-width: 120px; }"
            << "</style></head><body>";

    set<string> teachers;
    for (int c = 0; c < CLASSES * CLASSES_COUNT; ++c)
        for (int s = 0; s < TOTAL_SLOTS; ++s)
            if (schedule.matrix[c][s].teacher != "-1")
                teachers.insert(schedule.matrix[c][s].teacher);

    // В функции printSchedule(), часть отвечающая за расписание учителей
    for (const string &t: teachers) {
        teacher_out << "<div class='teacher-schedule'><h2>" << t << "</h2><table>";
        teacher_out << "<tr><th>Day</th>";
        for (int l = 1; l <= 9; ++l)
            teacher_out << "<th>" << l << "<br>" << getLessonTime(l) << "</th>";
        teacher_out << "</tr>";

        for (int d = 0; d < 6; ++d) {
            teacher_out << "<tr><td>" << getDayName(d) << "</td>";
            for (int l = 0; l < 9; ++l) {
                int slot = d * 9 + l;
                vector<string> classes;
                string style, subject;

                // Собираем все классы для этого слота
                for (int c = 0; c < CLASSES * CLASSES_COUNT; ++c) {
                    const Lesson &lesson = schedule.matrix[c][slot];
                    if (lesson.teacher == t) {
                        subject = lesson.lesson;
                        string class_info = getClassName(c);
                        classes.push_back(class_info);
                    }
                }

                // Формируем ячейку
                string cell;
                if (!classes.empty()) {
                    string bg_color = subject_colors[subject];
                    style = "style='background-color:" + bg_color + "'";

                    // Объединяем информацию о классах
                    for (const auto &cls: classes) {
                        cell += cls + "<br>";
                    }
                    cell = cell.substr(0, cell.length() - 4); // Удаляем последний <br>
                }

                teacher_out << "<td " << style << ">" << cell << "</td>";
            }
            teacher_out << "</tr>";
        }
        teacher_out << "</table></div>";
    }
}

void solve() {
    Schedule now;
    auto data = parseInputData();
    fillSchedule(now, data);
    now.recnt();
    Schedule best = now;
    best.recnt();
    ld T = 100'000;
    uniform_int_distribution<> random_class(0, CLASSES * CLASSES_COUNT - 1);
    uniform_int_distribution<> random_lesson(0, TOTAL_SLOTS - 1);
    for (; T >= 1e-1; T *= COOLING_RATE) {
        Schedule nw = now;
        do {
            int Class = random_class(rng);
            nw.swp(Class, random_lesson(rng), random_lesson(rng));
        } while (!nw.is_normal());
        nw.recnt();
        ll delta = nw.cost - now.cost;
        if (delta < 0 || rnd() < exp(-delta / T)) {
            now = nw;
            if (now.cost < best.cost) {
                best = now;
            }
        }
        cout << T << ' ' << best.cost << ' ' << now.cost << '\n';
    }
    cout << best.cost << endl;
    printSchedule(best);
}
