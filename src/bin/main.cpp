#include <iostream>
#include <random>
#include <chrono>
#include <cstring>

#include "../libs/Operation.h"

extern Operation check_task(const int num_task, const int Np, const int Nt,
        const int C[], const int C_curr[], const int L[], const int * P, const int answer[]);

enum { MAX_RETRIES = 10000 };

// Функция для подсчёта нагрузки на сеть
int count_network_load(const int &Nt, const int * P, const int * answer) {
    int load = 0;
    for (int task_num = 0; task_num < Nt; ++task_num) {
        for (int count_task_num = task_num + 1; count_task_num < Nt; ++count_task_num) {
            if (count_task_num != task_num && answer[task_num] != answer[count_task_num]) {
                load += P[task_num * Nt + count_task_num];
            }
        }
    }
    return load;
}


int main(int argc, char ** argv) {
    bool interactive = false; // Флаг для работы в интерактивном режиме (подсказки о прогрессе работы алгоритма)
    bool count_time = false; // Требуется ли печать времени
    bool print_load = false; // Требуется ли печать нагрузки
    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "-i") == 0) {
                interactive = true;
                continue;
            }
            if (strcmp(argv[i], "-t") == 0) {
                // Проводится автоматическое тестирование
                count_time = true;
                print_load = true;
                continue;
            }
        }
    }
    // Считывание вводных данных
    auto starting_time = std::chrono::high_resolution_clock::now();
    int Np;
    std::cin >> Np;
    int C[Np], C_curr[Np];
    for (int i = 0; i < Np; ++i) {
        std::cin >> C[i];
        C_curr[i] = 0;
    }
    if (interactive) {
        std::cout << "Max capacity accepted\n";
    }
    int Nt;
    std::cin >> Nt;
    int L[Nt];
    int answer[Nt];
    for (int i = 0; i < Nt; ++i) {
        std::cin >> L[i];
        answer[i] = -1;
    }
    if (interactive) {
        std::cout << "Tasks accepted\n";
    }
    int P[Nt][Nt];
    for (int i = 0; i < Nt; ++i) {
        for (int j = 0; j < Nt; ++j) {
            std::cin >> P[i][j];
        }
    }
    if (interactive) {
        std::cout << "Network load accepted\n";
    }
    // Инициирование генератора случайных чисел
    srand(time(nullptr));
    bool random_solution_generation_success = false; // Флаг для отслеживания успешности генерации решения
    // Генерация случайного решения
    for (int first_try_num = 0; first_try_num < MAX_RETRIES; ++first_try_num) {
        try {
            for (int task_num = 0; task_num < Nt; ++task_num) {
                bool second_try_success = false; // Флаг для отслеживания успешности случайных попыток
                for (int second_try_num = 0; second_try_num < Np; ++second_try_num) {
                    int proc_num = rand() % Np;   // Генерирует число от 0 до Np - 1
                    if (C_curr[proc_num] + L[task_num] <= C[proc_num]) {
                        // Можно поставить задачу на этот процессор
                        answer[task_num] = proc_num;
                        C_curr[proc_num] += L[task_num];
                        second_try_success = true;
                        break;
                    }
                }
                if (not second_try_success) {
                    throw (1);
                }
            }
        } catch (int &a) {
            if (a == 1) {
                // Не удалось подобрать решение в этой попытке
                for(int i = 0; i < Np; ++i) {
                    C_curr[i] = 0;
                }
                continue;
            }
        }
        // Решение подобрано
        random_solution_generation_success = true;
        break;
    }
    if (!random_solution_generation_success) {
        std::cout << "Невозможно сгенерировать случайное решение. Оно существует?" << std::endl;
        return 1;
    }
    if (interactive) {
        std::cout << "Случайное решение:\n";
        for (long i = 0; i < Nt; ++i) {
            std::cout << answer[i] + 1 << " ";
        }
    }
    int current_network_load = count_network_load(Nt, (int *)P, answer);
    if (interactive) {
        std::cout << std::endl << "Сетевая нагрузка: " << current_network_load << std::endl;
    }
    // Рассматриваем все возможные варианты применения операций A, B, C
    auto best = Operation();
    while (true) {
        best = Operation(current_network_load);
        for (int task = 0; task < Nt; ++task) {
            auto current = check_task(task, Np, Nt, C, C_curr, L, (int *) P, answer);
            if (current.get_load() < best.get_load()) {
                best = current;
            }
        }
        if (best.get_type() == 0) {
            break;
        }
        best.apply_operation(C, C_curr, L, answer);
        current_network_load = best.get_load();
        // Если не удалось найти решение с меньшей нагрузкой,
        // то тип best будет равен 0, и будет произведён выход из цикла
    }
    if (interactive) {
        std::cout << "Лучшее решение:\n";
    }
    for (long i = 0; i < Nt; ++i) {
        std::cout << answer[i] + 1 << " ";
    }
    current_network_load = count_network_load(Nt, (int *)P, answer);
    if (interactive || print_load) {
        std::cout << std::endl << "Сетевая нагрузка: " << current_network_load << std::endl;
        // Подсчёт максимально возможной нагрузки
        int total_load = 0;
        const int * P_temp = (int *) P;
        for (int task_num = 0; task_num < Nt; ++task_num) {
            for (int count_task_num = task_num + 1; count_task_num < Nt; ++count_task_num) {
                total_load += P_temp[task_num * Nt + count_task_num];
            }
        }
        std::cout << "Максимально возможная нагрузка: " << total_load << std::endl << "Качество решения:" <<
        double(current_network_load) / total_load << std::endl;
    }
    if (count_time || interactive) {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::cout << "Время работы: " << (double)(std::chrono::duration_cast<std::chrono::nanoseconds>
                (end_time - starting_time).count()) / 1000000000 << "s" << std::endl;
    }
    return 0;
}