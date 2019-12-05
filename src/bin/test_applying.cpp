#include <iostream>
#include <errno.h>
#include <cstring>
#include <climits>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "../json.hpp"

using json = nlohmann::json;
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "В аргументах командной строки укажите файл с тестами" << std::endl;
        //return 1;
    }
    char filename[PATH_MAX];
    int current_test = 0;
    // Открытие файла для записи результата
    int test_out = open("tests_out.txt", O_RDWR | O_TRUNC | O_CREAT, 0600);
    std::cout << test_out << std::endl;
    // Перенаправление потока вывода в этот файл
    if (dup2(test_out, 1) < 0) {
        std::cerr << "Error while duplicating file" << std::endl << std::strerror(errno) << std::endl;
        return 1;
    }
    // Компиляция проекта в отдельном процессе
    int pid = fork();
    if (!pid) {
        char cmd[PATH_MAX];
        snprintf(cmd, PATH_MAX, "make");
        std::cerr << "Компиляция: " << cmd << std::endl;
        execlp(cmd, "make", "all", "--silent", NULL);
        std::cerr << std::strerror(errno) << std::endl;
        exit(1);
    }
    // Ожидание завершения компиляции
    int status;
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status) || !(WEXITSTATUS(status) == 0)) {
        // Компиляция неуспешна
        std::cerr << "Ошибка компиляции" << std::endl;
        return 2;
    }
    std::cerr << "Компиляция успешна, начинаю тест " << std::endl;
    while (true) {
	// Открытие очередного теста
        snprintf(filename, PATH_MAX, "%s%d.json", argv[1], current_test);
        std::ifstream f;
        f.open(filename, std::ios::in);
        if (!f) {
	    // Открыть не удалось, значит все тесты выполнены
            std::cout << "Testing completed!" << std::endl;
            break;
        }
        std::cout << "----------------TEST " << current_test << "----------------" << std::endl << "In:" << std::endl;
        std::string json_str;
        f >> json_str;
        auto j = json::parse(json_str);
	// Создание канала для выполнения программы и перенаправление потока вывода в него
        int fd[2];
        if (pipe(fd) < 0) {
            std::cerr << "Error while opening pipe" << std::endl;
            return 1;
        }
        if (dup2(fd[0], 0) < 0) {
            std::cerr << "Error while duplicating pipe" << std::endl;
            return 1;
        }
	// Отправка тестирующих данных в канал
        FILE * out = fdopen(fd[1], "w");
        int Np = j["proc_count"];
        fprintf(out, "%d ", Np);
        printf("%d\n", Np);
        int C[Np];
        for (int i = 0; i < Np; ++i) {
            C[i] = j["max_proc_load"][i];
            fprintf(out, "%d ", C[i]);
            printf("%d ", C[i]);
        }
        printf("\n");
        int Nt = j["tasks_count"];
        int L[Nt];
        fprintf(out, "%d ", Nt);
        printf("%d\n", Nt);
        for (int i = 0; i < Nt; ++i) {
            L[i] = j["tasks_max_load"][i];
            fprintf(out, "%d ", L[i]);
            printf("%d ", L[i]);
        }
        printf("\n");
        int P[Nt][Nt];
        for (int i = 0; i < Nt; ++i) {
            for (int k = 0; k < Nt; ++k) {
                P[i][k] = j["network_load"][i][k];
                fprintf(out, "%d ", P[i][k]);
                printf("%d ", P[i][k]);
            }
            printf("\n");
        }
        printf("\n");
        fclose(out);
        close(fd[1]);
	// Начало теста
        std::cerr << "Начинаю тест " << current_test << std::endl;
        std::cout << "----------------OUT: " << std::endl;
        pid = fork();
        if (!pid) {
            char cmd[PATH_MAX];
            snprintf(cmd, PATH_MAX, "./alg_KnyazevAA_204");
            execl(cmd, "alg_KnyazevAA_204", "-t", nullptr);
            std::cerr << std::strerror(errno) << std::endl;
            exit(1);
        }
	// Ожидание окончания теста
        close(fd[0]);
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            std::cerr << "Программа не завершила выполнение" << std::endl;
            return 3;
        }
        std::cerr << "Program exited with status " << WEXITSTATUS(status) << std::endl;
        printf("\n");
        current_test++;
    }
    // Закрытие файла
    close(test_out);
}
