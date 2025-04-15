#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <map>
#include <set>
#include <string>

using namespace std;


constexpr int DAYS_PER_WEEK = 6;
constexpr int LESSONS_PER_DAY = 9;
constexpr int TOTAL_LESSONS = DAYS_PER_WEEK * LESSONS_PER_DAY;

constexpr int CLASSES_COUNT = 5;
constexpr int CLASSES = (11 - 7) + 1;
constexpr int TOTAL_CLASSES = CLASSES * CLASSES_COUNT;

const string SCHEDULE_NAME = "Schedule";
const string TEACHERS_HOURS_NAME = "ListLesson";
constexpr long long INF = 1e18;


constexpr long long INTERSECT = 1e9;
constexpr long long SKIP = 4e6;

const map<int, vector<set<int> > > lessonToGroups = {
    {1, {{21, 22}}},
    {2, {{15, 16}, {18, 19}, {0, 3, 4}, {1, 2}, {5, 6, 7}, {8, 9}, {10, 11}, {12, 13}}},
    {3, {{15, 16}, {18, 19}, {0, 3, 4}, {1, 2}, {5, 6, 7}, {8, 9}, {10, 11}, {12, 13}}},
    {4, {{15, 16, 17}, {18, 19}, {0, 1}, {2, 3, 4}, {5, 6}, {7, 8, 9}, {10, 11}, {12, 13}}},
    {5, {{22, 23}, {5, 6}}},
    {6, {{20, 22, 23, 24}, {5, 6}, {12, 13}}},
    {9, {{21, 23}}},
    {10, {{21, 23}}},
    {12, {{20, 22, 23, 24}, {8, 9}, {12, 13}}},
    {16, {{8, 9}}},
    {17, {{0, 1}, {10, 11}, {12, 13}}},
    {20, {{15, 17}, {16, 18}, {20, 21, 22, 23, 24}, {0, 1}, {2, 3}, {5, 7}, {8, 9}, {10, 11}}}
};

class Lesson {
public:
    vector<int> teachers;
    int lesson = -1, Class = -1;
    bool dop = false;

    Lesson() {
    }

    Lesson(const vector<int> &teachers, int lesson, int Class, bool dop = false): teachers(teachers), lesson(lesson),
        Class(Class),
        dop(dop) {
    }

    bool operator ==(const Lesson &a) const {
        if (lesson != a.lesson) return false;
        if (lesson == -1) return true;

        bool T = false;
        auto it = lessonToGroups.find(lesson);
        if (it == lessonToGroups.end()) return false;
        for (const auto &set: it->second) {
            if (set.find(Class) == set.end()) continue;
            T = set.find(Class) != set.end();
            break;
        }
        return T;
    }

    bool operator <(const Lesson &a) const {
        return lesson < a.lesson;
    }
};

class Schedule {
public:
    long long cost = INF;
    Lesson matrix[TOTAL_CLASSES][TOTAL_LESSONS];

    void swp(int Class, int i, int j) {
        vector<int> Classes = {Class};
        vector<bool> used(TOTAL_CLASSES);
        used[Class] = true;
        deque<int> q = {Class};
        while (!q.empty()) {
            int v = q.front();
            q.pop_front();
            auto it = lessonToGroups.find(matrix[v][i].lesson);
            if (it != lessonToGroups.end()) {
                for (auto &set: it->second) {
                    if (set.find(v) != set.end()) {
                        for (const int u: set) {
                            if (!used[u]) {
                                used[u] = true;
                                q.push_back(u);
                            }
                        }
                        break;
                    }
                }
            }
            it = lessonToGroups.find(matrix[v][j].lesson);
            if (it != lessonToGroups.end()) {
                for (auto &set: it->second) {
                    if (set.find(v) != set.end()) {
                        for (const int u: set) {
                            if (!used[u]) {
                                used[u] = true;
                                q.push_back(u);
                            }
                        }
                        break;
                    }
                }
            }
        }
        for (int kl = 0; kl < TOTAL_CLASSES; ++kl) {
            if (!used[kl]) continue;
            swap(matrix[kl][i], matrix[kl][j]);
        }
    }

    void recnt() {
        cost = 0;
        for (int j = 0; j < TOTAL_LESSONS; ++j) {
            map<int, vector<set<int> > > teacherToGroups;
            for (const auto &[key,l]: lessonToGroups) {
                teacherToGroups[key].resize(l.size());
            }

            set<int> st;
            for (int i = 0; i < TOTAL_CLASSES; ++i) {
                for (int u: matrix[i][j].teachers) {
                    auto it = lessonToGroups.find(matrix[i][j].lesson);
                    if (it == lessonToGroups.end()) {
                        if (st.find(u) != st.end()) {
                            cost += INTERSECT;
                        }
                        st.insert(u);
                        continue;
                    }
                    int indx = 0;
                    bool b = true;
                    for (const auto &set: it->second) {
                        if (set.find(i) != set.end()) {
                            teacherToGroups[matrix[i][j].lesson][indx].insert(u);
                            b = false;
                            break;
                        }
                        indx++;
                    }
                    if (b) {
                        if (st.find(u) != st.end()) {
                            cost += INTERSECT;
                        }
                        st.insert(u);
                    }
                }
            }
            for (const auto &[k,sets]: teacherToGroups) {
                for (const auto &set: sets) {
                    for (int u: set) {
                        if (st.find(u) != st.end()) {
                            cost += INTERSECT;
                        }
                        st.insert(u);
                    }
                }
            }
        }
        for (int i = 0; i < TOTAL_CLASSES; ++i) {
            map<Lesson, int> col;
            for (int j = 0; j < TOTAL_LESSONS; ++j) {
                col[matrix[i][j]]++;
            }
            for (int j = 0; j < DAYS_PER_WEEK; ++j) {
                map<Lesson, int> cl;
                for (int k = 0; k < LESSONS_PER_DAY; ++k) {
                    Lesson f = matrix[i][j * LESSONS_PER_DAY + k];
                    if (f.teachers.empty()) continue;
                    cl[f]++;
                }
                for (const auto [k1,col1]: cl) {
                    if (col[k1] <= 3) {
                        cost += 400000 * (col1 > 1);
                    } else {
                        cost += (2 * col1 > col[k1] ? 500000 : -10);
                    }
                }
            }
            //==============================================================
            for (int j = 0; j < DAYS_PER_WEEK; ++j) {
                map<int, int> cl;
                for (int k = 0; k < LESSONS_PER_DAY; ++k) {
                    Lesson f = matrix[i][j * LESSONS_PER_DAY + k];
                    if (f.teachers.empty()) continue;
                    for (int u: f.teachers) {
                        cl[u]++;
                    }
                }
                for (auto [k1,col1]: cl) {
                    cost += 80000 * (col1 < 3);
                }
            }
            //==============================================================
            for (int j = 0; j < DAYS_PER_WEEK; ++j) {
                for (int k = 0; k < LESSONS_PER_DAY; ++k) {
                    Lesson f = matrix[i][j * LESSONS_PER_DAY + k];
                    if (j != 5 && f.teachers.empty() && k <= 5) {
                        cost += SKIP;
                    }
                    if (j == 5) {
                        if (f.teachers.empty() && k < 4) {
                            cost += 40000;
                        }
                        if (!f.teachers.empty() && k >= 6) {
                            cost += INTERSECT;
                        }
                    }
                }
            }
            //==============================================================
            for (int j = 0; j < DAYS_PER_WEEK; ++j) {
                for (int k = 0; k < LESSONS_PER_DAY; ++k) {
                    Lesson f = matrix[i][j * LESSONS_PER_DAY + k];
                    if (f.teachers.empty()) continue;
                    bool b = false;
                    if (k) {
                        b |= (matrix[i][j * LESSONS_PER_DAY + k].lesson == matrix[i][j * LESSONS_PER_DAY + k - 1].
                              lesson);
                    }
                    if (k + 1 < LESSONS_PER_DAY) {
                        b |= (matrix[i][j * LESSONS_PER_DAY + k].lesson == matrix[i][j * LESSONS_PER_DAY + k + 1].
                              lesson);
                    }
                    if (f.lesson == 6) {
                        cost += (1 - b) * 1e6;
                    }
                    if (f.lesson == 5) {
                        cost += (1 - b) * 2e5;
                    }
                    if (f.lesson == 1 || f.lesson == 20) {
                        cost += (1 - b) * 5000;
                    }
                }
            }
        }
    }

    void print() const {
        ofstream fout("../data/" + SCHEDULE_NAME + ".txt");
        for (int i = 0; i < TOTAL_CLASSES; ++i) {
            for (int j = 0; j < TOTAL_LESSONS; ++j) {
                fout << matrix[i][j].teachers.size() << ' ';
                for (int u: matrix[i][j].teachers) {
                    fout << u << ' ';
                }
                fout << matrix[i][j].lesson << ' ' << matrix[i][j].dop << endl;
            }
        }
        fout.close();
    }

    void read() {
        ifstream fin("../data/" + SCHEDULE_NAME + ".txt");
        int subject, Class, dop;
        for (int i = 0; i < TOTAL_CLASSES; ++i) {
            for (int j = 0; j < TOTAL_LESSONS; ++j) {
                int len;
                fin >> len;
                vector<int> teachers(len);
                for (int k = 0; k < len; ++k) {
                    fin >> teachers[k];
                }
                fin >> subject >> dop;
                matrix[i][j] = Lesson(teachers, subject, i, dop);
            }
        }
    }

    void readNew() {
        ifstream fin("../data/" + TEACHERS_HOURS_NAME + ".txt");
        vector<Lesson> rubish;
        map<int, vector<vector<int> > > group;
        map<int, vector<set<int> > > teachers;
        int teacher, lesson;
        string Class1;
        bool dop;
        while (fin >> teacher >> lesson >> Class1 >> dop) {
            int Class = classToIndx(Class1);
            bool found = false;
            auto it = lessonToGroups.find(lesson);
            if (teacher == 19) continue;
            if (Class >= classToIndx("11.1")) {
                if (lesson != 1 && lesson != 5 && lesson != 6 && lesson != 20)
                    continue;
            }
            if (it != lessonToGroups.end()) {
                int indx = 0;
                group[lesson].resize(it->second.size());
                teachers[lesson].resize(it->second.size());
                for (const auto &set: it->second) {
                    if (set.find(Class) != set.end()) {
                        found = true;
                        group[lesson][indx].push_back(Class);
                        teachers[lesson][indx].insert(teacher);
                        break;
                    }
                    indx++;
                }
            }
            if (!found) rubish.push_back({{teacher}, lesson, Class, dop});
        }
        for (int i = classToIndx("11.1"); i <= classToIndx("11.5"); ++i) {
            for (int j = 0; j < LESSONS_PER_DAY; ++j) {
                matrix[i][j + LESSONS_PER_DAY + 1].teachers = {19};
                matrix[i][j + LESSONS_PER_DAY + 1].lesson = 5;
                matrix[i][j + LESSONS_PER_DAY + 1].Class = i;
                matrix[i][j + LESSONS_PER_DAY + 1].dop = 0;
            }
        }
        for (int i = classToIndx("8.3"); i <= classToIndx("8.5"); ++i) {
            matrix[i][0].teachers = {49};
            matrix[i][0].lesson = 13;
            matrix[i][0].Class = i;
            matrix[i][0].dop = 1;
        }
        for (const auto &[lesson, col]: group) {
            for (int k = 0; k < col.size(); ++k) {
                auto setOfTeachers = teachers[lesson][k];
                auto classes = lessonToGroups.find(lesson)->second[k];
                int count = col[k].size();
                count /= classes.size();
                while (count--) {
                    bool b = false;
                    for (int j = 0; j < TOTAL_LESSONS && !b; ++j) {
                        bool g = true;
                        for (int i: classes) {
                            if (matrix[i][j].lesson != -1) {
                                g = false;
                                break;
                            }
                        }
                        if (g) {
                            b = true;
                            for (int i: classes) {
                                for (int t: setOfTeachers)
                                    matrix[i][j].teachers.emplace_back(t);
                                matrix[i][j].lesson = lesson;
                                matrix[i][j].Class = i;
                                matrix[i][j].dop = 0;
                            }
                        }
                    }
                    if (b == false) {
                        cout << "UNLUCK in read((((\n";
                        exit(0);
                    }
                }
            }
        }
        for (const auto &[teachers,lesson,Class,dop]: rubish) {
            bool found = false;
            for (int k = 0; k < TOTAL_LESSONS; ++k) {
                if (matrix[Class][k].lesson != -1) continue;
                matrix[Class][k] = {teachers, lesson, Class, dop};
                found = true;
                break;
            }
            if (!found) {
                found = false;
                for (int k = 0; k < TOTAL_LESSONS; ++k) {
                    if (matrix[Class][k].lesson != -1) continue;
                    matrix[Class][k] = {teachers, lesson, Class, dop};
                    found = true;
                    break;
                }
                if (!found) {
                    cerr << "UNLUCK in write((((\n";
                    exit(0);
                }
            }
        }
    }

private
:
    static int classToIndx(string s) {
        int clas = 0, num = int(s[s.size() - 1] - '0');
        if (s.size() == 4) {
            clas = (s[0] - '0') * 10 + (s[1] - '0');
        } else {
            clas = (s[0] - '0');
        }
        return (clas - 7) * CLASSES_COUNT + (num - 1);
    }

    static string indxToClass(int a) {
        return to_string(a / CLASSES_COUNT + 7) + '.' + to_string((a % CLASSES_COUNT) + 1);
    }

    // set<Lesson> inter = {{43, 12}, {21, 5}, {18, 5}, {39, 11}, {16, 5}};
};
#endif //SCHEDULER_H
