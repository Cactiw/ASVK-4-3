#ifndef ASVK_4_3_OPERATION_H
#define ASVK_4_3_OPERATION_H

// Класс для хранения типа и аргументов операций A, B или C. Позволяет посмотреть нагрузку, которая будет в случае
// применения данной операции (get_load()), применить операцию (apply_operation(...)), посмотреть её тип
// (1 для A, 2 для B, 3 для C, метод get_type()). Перегружен оператор присваивания и деструктор для корректной работы
// динамической памяти.
class Operation {
public:
    Operation();
    explicit Operation(const int load);
    Operation(const int type, const int num_task, const int new_proc, const int load);
    Operation(const int task1, const int * tasks, const int num_tasks, const int load);

    int get_type() const;
    int get_load() const;
    int apply_operation(const int C[], int C_curr[], const int L[], int answer[]);

    Operation& operator = (const Operation& old);
    ~Operation();

private:
    int type;
    int load;
    int task1;
    int task2;
    int new_proc;
    int * tasks = nullptr;
    int num_tasks;
};


#endif //ASVK_4_3_OPERATION_H
