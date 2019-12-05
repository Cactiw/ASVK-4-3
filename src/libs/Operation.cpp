#include <iostream>

#include "Operation.h"

extern int move_task(const int num_task, const int new_proc, const int C[], int C_curr[],
        const int L[], int answer[], bool ignore_limits = false);

extern int swap_tasks(const int task1, const int task2, const int C[], int C_curr[], const int L[],
                      int answer[], bool ignore_limits = false);

extern int swap_several_tasks(const int task1, const int * tasks, const int num_tasks,
        const int C[], int C_curr[], const int L[], int answer[], bool ignore_limits = false);

Operation::Operation() {
    // Конструктор для пустого объявления операции
    this->type = 4;
}

Operation::Operation(const int load) {
    // Конструктор для пустого объявления операции и хранения нагрузки
    this->type = 0;
    this->load = load;
}

// Конструктор для операций A, B
Operation::Operation(const int type, const int num_task, const int new_proc, const int load) {
    this->type = type;
    this->load = load;
    this->task1 = num_task;
    if (type == 1) {
        this->new_proc = new_proc;
    } else {
        this->task2 = new_proc;
    }
}

// Конструктор для операции C
Operation::Operation(const int task1, const int * tasks, const int num_tasks, const int load) {
    this->type = 3;
    this->load = load;
    this->task1 = task1;
    this->tasks = new int[num_tasks];
    std::copy(tasks, tasks + num_tasks, this->tasks);
    this->num_tasks = num_tasks;
}

int Operation::get_type() const {
    return this->type;
}

int Operation::get_load() const {
    return this->load;
}

// Применение операции A, B или C
int Operation::apply_operation(const int *C, int *C_curr, const int *L, int *answer) {
    if (this->type == 1) {
        // Применение операции A. Флаг установлен, так как проверка коректности операции была осуществлена ранее
        return move_task(this->task1, this->new_proc, C, C_curr, L, answer, true);
    }
    if (this->type == 2) {
        // Применение операции B. Флаг установлен, так как проверка коректности операции была осуществлена ранее
        return swap_tasks(this->task1, this->task2, C, C_curr, L, answer, true);
    }
    // Применение операции C. Флаг установлен, так как проверка коректности операции была осуществлена ранее
    return swap_several_tasks(this->task1, this->tasks, this->num_tasks, C, C_curr, L, answer, true);
}

// Конструктор оператора присваивания, нужен для корректного копирования массива tasks для операции C
Operation& Operation::operator= (const Operation &old) {
    if (&old == this) {
        return *this;
    }
    this->type = old.type;
    this->load = old.load;
    this->task1 = old.task1;
    this->task2 = old.task2;
    this->new_proc = old.new_proc;
    if (old.tasks != nullptr) {
        if (this->tasks == nullptr) {
            this->tasks = new int[old.num_tasks];
        }
        std::copy(old.tasks, old.tasks + old.num_tasks, this->tasks);
    } else {
        this->tasks = nullptr;
    }
    this->num_tasks = old.num_tasks;
    return *this;
}

// Деструктор для предотвращения утечки памяти
Operation::~Operation() {
    if (this->tasks != nullptr) {
        delete[] tasks;
        this->tasks = nullptr;
    }
}