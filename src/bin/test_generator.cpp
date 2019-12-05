#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <climits>

#include "../json.hpp"

using json = nlohmann::json;

int choose_random(int n, const int *chances)
{
    int sum = 0;
    int random = rand() % 101;
    for (int i = 0; i < n; ++i) {
        sum += chances[i];
        if (sum >= random) {
            return i;
        }
    }
    return 0;
}

int main(int argc, char ** argv)
{
    if (argc <= 2) {
        std::cout << "В аргументах командной строки укажите количество тестов и название файлов, в которые тесты "
                     "будут записаны" << std::endl;
        return 1;
    }
    srand(time(nullptr));
    std::fstream fin("data/generator_input.txt", std::ios::in);
    char filename_template[PATH_MAX];
    snprintf(filename_template, PATH_MAX, "%s", argv[2]);
    int test_begin = 0;
    while (true) {
        // Получение номера первого теста, который нужно сгенерировать
        char filename[PATH_MAX + 20];
        snprintf(filename, PATH_MAX + 20, "%s%d%.5s", filename_template, test_begin, ".json");
        std::ifstream f;
        f.open(filename, std::ios::in);
        if (!f) {
            break;
        }
        f.close();
        ++test_begin;
    }
    int test_count = strtol(argv[1], nullptr, 10);
    int test_end = test_begin + test_count;
    // Считывание вводных данных
    int proc_num = 0;
    fin >> proc_num;
    int tasks_load_probability[3], chance_sum = 0;
    for (int i = 0; i < 3; ++i) {
        fin >> tasks_load_probability[i];
        chance_sum += tasks_load_probability[i];
    }

    int max_load_probability[4];
    chance_sum = 0;
    for (int i = 0; i < 4; ++i) {
        fin >> max_load_probability[i];
        chance_sum += max_load_probability[i];
    }

    int reserve = 0;
    fin >> reserve;

    int intense_probability[4];
    chance_sum = 0;
    for (int i = 0; i < 4; ++i) {
        fin >> intense_probability[i];
        chance_sum += intense_probability[i];
    }
    fin.close();

    // Генерация тестов для поданных данных
    for (int test_num = test_begin; test_num < test_end; ++test_num) {

        // Назначение процессорам лимита нагрузки
        int proc_max_load[4] {40, 60, 80, 100};
        int max_proc_load[proc_num];
        int sum_loads = 0;
        for (int i = 0; i < proc_num; ++i) {
            sum_loads += max_proc_load[i] = proc_max_load[choose_random(4, max_load_probability)];
        }

        // Рассчет запаса производительности
        int capacity_reserve = int(sum_loads * reserve / 100);

        // Добавление задач
        int tasks_load_const[3] {5, 10, 20};
        int tasks_load_sum = 0;
        // Создание вектора с задачами, так как их количество изначально неизвестно
        std::vector<int> tasks_load;
        while (true) {
            int i = choose_random(3, tasks_load_probability);
            // Проверка ограничений на нагрузку
            if (tasks_load_sum + tasks_load_const[i] > capacity_reserve) {
                tasks_load_const[i] = -1;
            } else {
                // Проверка успешна, добавление задачи
                if (tasks_load_const[i] != -1) {
                    tasks_load.push_back(tasks_load_const[i]);
                    tasks_load_sum += tasks_load_const[i];
                }
            }
            // Проверка на возможность добавления хоть одной задачи
            bool empty = true;
            for (auto x : tasks_load_const) {
                if (x != -1) {
                    empty = false;
                    break;
                }
            }
            if (empty) {
                break;
            }
        }
        int tasks_count = int(tasks_load.size());

        //Генерирование интенсивности обмена данными между задачами
        int network_load_consts[4] {0, 10, 50, 100};
        int network_load[tasks_count][tasks_count];
        for (int i = 0; i < tasks_count; ++i) {
            for (int j = 0; j <= i; ++j) {
                if (j == i) {
                    network_load[i][j] = 0;
                    continue;
                }
                int k = choose_random(4, intense_probability);
                network_load[i][j] = network_load_consts[k];
                network_load[j][i] = network_load_consts[k];
            }
        }

        // Переношу результаты в json
        json j;
        j["proc_count"] = proc_num;
        std::vector<int> v{};
        for (int i = 0; i < proc_num; ++i) {
            v.push_back(max_proc_load[i]);
        }
        j["max_proc_load"] = v;
        j["tasks_count"] = tasks_count;
        std::vector<int> v1{};
        for (int i = 0; i < tasks_count; ++i) {
            v1.push_back(tasks_load[i]);
        }
        j["tasks_max_load"] = v1;
        std::vector < std::vector < int >> v3{};
        for (int i = 0; i < tasks_count; ++i) {
            std::vector <int> v4{};
            for (int k = 0; k < tasks_count; ++k) {
                int tmp = network_load[i][k];
                v4.push_back(tmp);
            }
            v3.push_back(v4);
        }
        j["network_load"] = v3;
        // Дамп json в файл
        char filename[PATH_MAX + 20];
        snprintf(filename, PATH_MAX + 20, "%s%d%.5s", filename_template, test_num, ".json");
        std::fstream fout(filename, std::ios::out | std::ios::trunc);
        std::string s = j.dump();
        fout << s <<std::endl;
        fout.close();
    }
    return 0;
}