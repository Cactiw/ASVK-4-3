#include <limits>

#include "../libs/Operation.h"

extern int count_load_on_move_task(const int Nt, int num_task, int new_proc, const int C[],
                                      const int C_curr[], const int L[], const int * P, const int answer[]);

extern int count_load_on_swap_task(const int Np, const int Nt, const int task1,
        const int task2, const int C[], const int C_curr[], const int L[], const int * P,
        const int answer[]);

extern int count_load_on_swap_several_tasks(const int Np, const int Nt, const int task1,
        const int * tasks, const int num_tasks, const int C[], const int C_curr[],
        const int L[], const int * P, const int answer[]);


Operation check_task(const int num_task, const int Np, const int Nt,
                     const int C[], const int C_curr[], const int L[], const int * P,
                     const int answer[]) {
    Operation best_operation = Operation(std::numeric_limits<int>::max());
    // Проверка операции A
    int proc_num = answer[num_task];
    for (int current_proc_num = 0; current_proc_num < Np; ++current_proc_num) {
        if (current_proc_num == proc_num) {
            continue;
        }
        int new_load = count_load_on_move_task(Nt, num_task, current_proc_num, C, C_curr, L, P, answer);
        if (new_load < 0) {
            // Операцию выполнить невозможно из-за ограничений по нагрузке на процессоры
            continue;
        }
        if (new_load < best_operation.get_load()) {
            best_operation = Operation(1, num_task, current_proc_num, new_load);
        }
    }
    // Проверка операций B и C
    for (int task_to_swap = 0; task_to_swap < Nt; ++task_to_swap) {
        if (task_to_swap == num_task) {
            continue;
        }
        if (answer[task_to_swap] != proc_num) {
            // Задачи на разных процессорах
            //std::cout << Np << " " << Nt << std::endl;
            int new_load = count_load_on_swap_task(Np, Nt, num_task, task_to_swap, C, C_curr, L, P, answer);
            //std::cout << "New load for " <<num_task << " and " << task_to_swap << ": " << new_load << std::endl;
            if (new_load < 0) {
                // Операцию выполнить невозможно из-за ограничений по нагрузке на процессоры
                continue;
            }
            if (new_load < best_operation.get_load()) {
                best_operation = Operation(2, num_task, task_to_swap, new_load);
            }

            // Проверка операции C
            int task_load = L[num_task], sum_load = L[task_to_swap];
            if (sum_load >= task_load) {
                // Нагрузки на задачах не совпадают
                continue;
            }
            int tasks[Nt], tasks_count = 1;
            tasks[0] = task_to_swap;
            for (int task_to_add = task_to_swap + 1; task_to_add < Nt; ++task_to_add) {
                if (answer[task_to_add] != answer[task_to_swap]) {
                    continue;
                }
                int task_to_add_load = L[task_to_add];
                sum_load += task_to_add_load;
                if (sum_load == task_load) {
                    tasks[tasks_count] = task_to_add;
                    ++tasks_count;
                    new_load = count_load_on_swap_several_tasks(Np, Nt, num_task, tasks, tasks_count, C, C_curr, L, P,
                            answer);
                    if (new_load < 0) {
                        break;
                    }
                    if (new_load < best_operation.get_load()) {
                        best_operation = Operation(num_task, tasks, tasks_count, new_load);
                    }
                }
            }
        }
    }
    return best_operation;
}