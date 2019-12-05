#include <algorithm>

extern int count_network_load(const int &Nt, const int * P, const int answer[]);


// Функция, которая возвращает новое значение нагрузки на сеть, если применить операцию A (переместить задачу
// с одного процессора на другой), при этом сама операция НЕ применяется. При невозможности выполнения из-за
// ограничений нагрузки на процессоры, возвращает -1
int count_load_on_move_task(const int Nt, int num_task, int new_proc, const int C[],
        const int C_curr[], const int L[], const int * P, const int answer[]) {
    // Проверка ограничений по нагрузке на процессоры
    if (C_curr[new_proc] + L[num_task] > C[new_proc]) {
        return -1;
    }
    int answer_copy[Nt];
    std::copy(answer, answer + Nt, answer_copy); // Создание копии массива для сохранения неизменности answer
    answer_copy[num_task] = new_proc;
    return count_network_load(Nt, (int *)P, answer_copy);
}

// Функция, которая применяет операцию A. При невозможности выполнения из-за ограничений нагрузки на процессоры,
// возвращает -1 (если флаг ingore_limits установлен, то задача передвигается принудительно, игнорируя
// ограничения по нагрузке на процессоры). При успешном выполнение возвращает 0
int move_task(const int num_task, const int new_proc, const int C[], int C_curr[], const int L[],
        int answer[], bool ignore_limits = false) {
    int current_L = L[num_task];
    // Проверка ограничений по нагрузке на процессоры
    if (!ignore_limits && C_curr[new_proc] + current_L > C[new_proc]) {
        return -1;
    }
    // Ограничения выполнены, передвигаем задачи
    C_curr[answer[num_task]] -= current_L;
    C_curr[new_proc] += current_L;
    answer[num_task] = new_proc;
    return 0;
}

// Функция, которая применяет операцию B ("Поменять местами две задачи на разных процессорах.
// При невозможности выполнения из-за ограничений нагрузки на процессоры, возвращает -1.
// При успешном выполнении возвращает 0. (если флаг ingore_limits установлен, то задача передвигается принудительно,
// игнорируя ограничения по нагрузке на процессоры)
int swap_tasks(const int task1, const int task2, const int C[], int C_curr[], const int L[],
        int answer[], bool ignore_limits = false) {
    int l1 = L[task1], l2 = L[task2];
    int proc1 = answer[task1], proc2 = answer[task2];
    // Проверка ограничений по нагрузке на процессоры
    if (!ignore_limits && (C_curr[proc1] - l1 + l2 > C[proc1] || C_curr[proc2] - l2 + l1 > C[proc2])) {
        return -1;
    }
    // Ограничения выполнены, меняем задачи местами
    move_task(task1, proc2, C, C_curr, L, answer, true);
    move_task(task2, proc1, C, C_curr, L, answer, true);
    return 0;
}

// Функция, которая возвращает новое значение нагрузки на сеть, если применить операцию B, при этом
// сама операция НЕ применяется. При невозможности выполнения из-за ограничений по нагрузке на процессоры, возвращает -1
int count_load_on_swap_task(const int Np, const int Nt, const int task1, const int task2,
        const int C[], const int C_curr[], const int L[], const int * P, const int answer[]) {
    int l1 = L[task1], l2 = L[task2];
    int proc1 = answer[task1], proc2 = answer[task2];
    // Проверка ограничений по нагрузке на процессоры
    if (C_curr[proc1] - l1 + l2 > C[proc1] || C_curr[proc2] - l2 + l1 > C[proc2]) {
        return -1;
    }
    // Создание копий массивов, которые нужно изменить для эмуляции применения операции, при этом исходные массивы не
    // изменяются - таким образом, операция не применяется, но получается новая нагрузка в случае, если бы операция
    // применилась
    int answer_copy[Nt];
    std::copy(answer, answer + Nt, answer_copy);
    int C_curr_copy[Np];
    std::copy(C_curr, C_curr + Np, C_curr_copy);
    // Применение операции на новых массивах
    swap_tasks(task1, task2, C, C_curr_copy, L, answer_copy); // Бросит -1, если нельзя поменять из-за ограничений
    return count_network_load(Nt, P, answer_copy);
}

// Функция, которая применяет операцию C ("Поменять местами задачу на одноьм процессоре с группой задач на другом
// процессоре при условии равенствва суммарной нагрузки на процессоры. При невозможности выполнения
// из-за ограничений нагрузки на процессоры, возвращает -1. При успешном выполнении возвращает 0
int swap_several_tasks(const int task1, const int * tasks, const int num_tasks,
        const int C[], int C_curr[], const int L[], int answer[], bool ignore_limits = false) {
    // Проверка на выполнение ограничений при применении операции C
    int proc1 = answer[task1], proc2 = answer[tasks[0]];
    if (!ignore_limits) {
        int load1 = C_curr[proc1] - L[task1], load2 = C_curr[proc2] + L[task1];
        for (int i = 0; i < num_tasks; ++i) {
            int current_task_load = L[tasks[i]];
            load1 += current_task_load;
            load2 -= current_task_load;
        }
        if (load1 > C[proc1] || load2 > C[proc2]) {
            return -1;
        }
    }
    // Ограничения выполнены, меняем задачи местами
    move_task(task1, proc2, C, C_curr, L, answer, true);
    for (int i = 0; i < num_tasks; ++i) {
        move_task(tasks[i], proc1, C, C_curr, L, answer, true);
    }
    return 0;
}

// Функция, которая возвращает новое значение нагрузки на сеть, если применить операцию C, при этом
// сама операция НЕ применяется. При невозможности выполнения из-за ограничений по нагрузке на процессоры, возвращает -1
int count_load_on_swap_several_tasks(const int Np, const int Nt, const int task1,
        const int * tasks, const int num_tasks, const int C[], const int C_curr[],
        const int L[], const int * P, const int answer[]) {
    // Проверка на выполнение ограничений при применении операции C во избежание лишнего копированя массивов
    int proc1 = answer[task1], proc2 = answer[tasks[0]];
    int load1 = C_curr[proc1] - L[task1], load2 = C_curr[proc2] + L[task1];
    for (int i = 0; i < num_tasks; ++i) {
        int current_task_load = L[tasks[i]];
        load1 += current_task_load;
        load2 -= current_task_load;
    }
    if (load1 > C[proc1] || load2 > C[proc2]) {
        return -1;
    }
    // Создание копии массивов
    int answer_copy[Nt];
    std::copy(answer, answer + Nt, answer_copy);
    int C_curr_copy[Np];
    std::copy(C_curr, C_curr + Np, C_curr_copy);
    // Применение операции на новых массивах
    // Флаг ignore_limits установлен, так как проверка на корректность операции уже выполнена
    swap_several_tasks(task1, tasks, num_tasks, C, C_curr_copy, L, answer_copy, true);
    return count_network_load(Nt, P, answer_copy);
}