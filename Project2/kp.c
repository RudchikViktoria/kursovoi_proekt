#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CAMERAS 100
#define MAX_DLINA 50    

typedef struct {
    int id;            /* идентификатор камеры */
    float temp;        /* температура камеры */
    float low_limit;   /* нижний предел температуры */
    float up_limit;    /* верхний предел температуры */
    float procent;     /* процент отклонения температуры */
} Camera;

int read_data(const char* filename, Camera* cameras);
int generate_data(const char* filename, int count);
float calculate_otcl(float temp, float low_limit, float up_limit);
int put_otcl(int id, float temp, float low_limit, float up_limit, float procent);
int log_deviation(int camera_id, float procent, float current_temp, float low_limit, float up_limit);
int check_temperature(const char* filename);
void data(char* buffer, size_t size);
int log_changes(int camera_id, const char* user);
int update_limit(Camera* cameras, int camera_count, int id, float new_low_limit, float new_up_limit, const char* user);
int history(const char* filename);
int sort_deviations(Camera* cameras, int camera_count);
int save_sorted_deviations(const char* output_file, Camera* cameras, int camera_count);
void display_sorted_deviations(Camera* cameras, int camera_count);

int main()
{
    system("chcp 1251");

    printf("**************************************************\n");
    printf("*    Добро пожаловать в 'Фригомонитор'!          *\n");
    printf("*   Ваш надежный помощник в мониторинге холода!  *\n");
    printf("*   Контролируйте температуру, избегайте аварий! *\n");
    printf("**************************************************\n");

    Camera cameras[MAX_CAMERAS];  /* массив структур для данных о камере */
    int camera_count = 0;
    int choice;
    int a = 0;                      /* количество камер для генерации данных */
    char filename[30];              /* массив для хранения имени файла */
    int id;
    float new_low_limit, new_up_limit;
    char user[50];

    do {
        printf("\nВыберите режим работы: \n");
        printf("1 > Загрузить данные из файла\n");
        printf("2 > Сгенерировать случайные данные\n");
        printf("3 > Включить мониторинг данных\n");
        printf("4 > Настроить диапазоны температуры\n");
        printf("5 > Просмотр истории изменений диапазонов температур\n");
        printf("6 > Сортировать данные отклонений\n");
        printf("7 > Сохранить отсортированные данные\n");
        printf("8 > Вывести отсортированные данные на экран\n");
        printf("0 > Выход\n");
        printf("Ваш выбор > \n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            camera_count = read_data("myfile.txt", cameras);
            if (camera_count == 0) {
                printf("Нет данных для чтения\n");
            }
            else {
                printf("Данные успешно прочитаны\n");
            }
            break;
        case 2:
            printf("Выберите количество камер для генерации данных > ");
            scanf("%d", &a);
            if (a > 0) {
                if (generate_data("generate_data.txt", a) == 0) {
                    printf("Ошибка создания файла");
                }
                else {
                    printf("Данные успешно сгенерированы\n");
                }
            }
            else {
                puts("Данные не сгенерированы\n");
            }
           
            break;
        case 3:
            printf("Введите имя файла для мониторинга (1 - myfile.txt, 2 - generate_data.txt): ");
            scanf("%s", filename);
            printf("\nПроверка отклонений температуры из файла\n");
            if (check_temperature(filename) == 0) {
                printf("Ошибка: файл не найден. Проверьте правильность имени файла\n");
            }
            else {
                printf("Мониторинг завершен успешно\n");
            }
            break;
        case 4: 
            printf("Введите ID камеры для изменения диапазона температуры: \n");
            scanf("%d", &id);
            printf("Введите новый нижний предел: ");
            scanf("%f", &new_low_limit);
            printf("Введите новый верхний предел: ");
            scanf("%f", &new_up_limit);
            printf("Введите имя ответственного: ");
            scanf("%s", user);

            if (update_limit(cameras, camera_count, id, new_low_limit, new_up_limit, user)) {
                printf("Диапазон успешно обновлен!\n");
            }
            else {
                printf("Камера с указанным ID не найдена\n");
            }
            break;
        case 5:
            if (history("история.txt") == 0) {
                printf("Ошибка открытия файла\n");
            }
            break;
        case 6:
            sort_deviations(cameras, camera_count);
            printf("Сортировка успешна");
            break;
        case 7:
            if (save_sorted_deviations("отсортированные_отклонения.txt", cameras, camera_count) == 0) {
                puts("Ошибка создания файла\n)");
            }
            else {
                puts("Данные успешно сохранены\n");
            }
            break;
        case 8:
            printf("\nОтсортированные данные отклонений:\n");
            display_sorted_deviations(cameras, camera_count);
            break;
        case 0:
            printf("Выход из программы\n");
            break;
        default:
            printf("Неверный выбор\n");
            break;
        }
    } while (choice != 0);
    return 0;
}

/*
 * читает данные о холодильных камерах из указанного файла
 * filename - указывает на имя файла, cameras - указывает на массив структур Camera
 * возвращает количество камер; возвращает 0, если файл не удалось открыть
 */
int read_data(const char* filename, Camera* cameras) {
    FILE* file = fopen("myfile.txt", "r");
    if (file == NULL) {
        return 0;
    }
    int count = 0;
    while (fscanf(file, "%d-%f", &cameras[count].id, &cameras[count].temp) == 2) {
        cameras[count].low_limit = -20;
        cameras[count].up_limit = 5;
        cameras[count].procent = calculate_otcl(cameras[count].temp, cameras[count].low_limit, cameras[count].up_limit);
        count++;
    }
    fclose(file);
    return count;
}

/*
 * записывает в файл сгенерированные данные для камер
 * filename - указывает на имя файла, count - количество камер
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно записаны
 */
int generate_data(const char* filename, int count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }

    srand(time(NULL));
    for (int i = 0; i < count; i++) {
        int id = i + 1;
        float temp = (rand() % 4000 - 2000) / 100.0;
        fprintf(file, "%d-%.2f\n", id, temp);
    }
    fclose(file);
    return 1;
}

/*
 * вычисляет процент отклонения
 * temp - температура камеры, low_limit - нижний предел, up_limit - верхний предел
 * возвращает процент отклонения
 */
float calculate_otcl(float temp, float low_limit, float up_limit) {
    float procent = 0;
    float otcl_3 = (up_limit - low_limit) * 0.03;
    float otcl_10 = (up_limit - low_limit) * 0.1;
    float otcl_25 = (up_limit - low_limit) * 0.25;

    if (temp < low_limit - otcl_25 || temp > up_limit + otcl_25) {
        procent = 25;
    }
    else if (temp < low_limit - otcl_10 || temp > up_limit + otcl_10) {
        procent = 10;
    }
    else if (temp < low_limit - otcl_3 || temp > up_limit + otcl_3) {
        procent = 3;
    }
    return procent;
}

/*
 * выводит сообщение о проценте отклонения на экран
 * id - идентификатор камеры, temp - температура камеры, low_limit - нижний предел, up_limit - верхний предел, procent - процент отклонения
 * возвращает 1, если данные удалось вывести, в противном случае 0
 */
int put_otcl(int id, float temp, float low_limit, float up_limit, float procent) {
    if (procent > 0) {
        printf("Камера ID %d: отклонение на %.0f%% (текущая: %.2f, диапазон %.2f - %.2f)\n", id, procent, temp, low_limit, up_limit);
        return 1;
    }
    return 0;
}

/*
 * записывает ID камеры и процент отклонения в файл
 * camera_id - идентификатор камеры, procent - процент отклонения, t_temp - текущая температура камеры, low_limit - нижний предел, up_limit - верхний предел
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно записаны
 */
int log_deviation(int camera_id, float procent, float t_temp, float low_limit, float up_limit) {
    FILE* file = fopen("отклонения.txt", "a");
    if (file == NULL) {
        printf("Ошибка открытия файла для записи отклонений\n");
        return 0;
    }
    if (procent > 0) {
        fprintf(file, "Камера ID %d: отклонение на %.0f%% (текущая температура: %.2f, диапазон: %.2f - %.2f)\n", camera_id, procent, t_temp, low_limit, up_limit);
    }
    fclose(file);
    return 1;
}

/*
 * выполняет мониторинг данных из файла
 * filename - указывает на имя файла
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно прочитаны
 */
int check_temperature(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла для проверки температуры\n");
        return 0;
    }

    float up_limit = 5;
    float low_limit = -20;
    int id = 0;
    float temp = 0;
    char line[MAX_DLINA]; /* массив для чтения строк из файла */

    /* читает строку из файла и сохраняет ее в line */
    while (fgets(line, sizeof(line), file)) {
        /* если успешно прочтено 2 значения, то условие верно */
        if (sscanf(line, "%d-%f", &id, &temp) == 2) {
            float procent = calculate_otcl(temp, low_limit, up_limit);
            put_otcl(id, temp, low_limit, up_limit, procent);
            log_deviation(id, procent, temp, low_limit, up_limit);
        }
    }
    fclose(file);
    return 1;
}

/*
 * получает текущие дату и время в строковом формате
 * buffer - указывает на строку, size - размер строки buffer.
 */
void data(char* buffer, size_t size) {
    time_t t = time(NULL);
    /* преобразование текущего времени в локальное и хранение его в структуре */
    struct tm* tm_info = localtime(&t);
    strftime(buffer, size, "%d-%m-%Y %H:%M:%S", tm_info);
}

/*
 * записывает информацию об изменении диапазонов температуры камеры в файл
 * camera_id - идентификатор камеры, user - указатель на имя пользователя
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно записаны
 */
int log_changes(int camera_id, const char* user) {
    FILE* log_file = fopen("история.txt", "a");
    if (log_file == NULL) {
        puts("Ошибка открытия файла");
        return 0;
    }

    char datatime[50]; /* массив для хранения даты и времени */
    data(datatime, sizeof(datatime));

    fprintf(log_file, "Дата изменения диапазона: %s, Камера ID: %d, Ответственный: %s\n", datatime, camera_id, user);
    fclose(log_file);
    return 1;
}


/*
 * позволяет пользователю изменить диапазоны температур для указанной камеры
 * cameras - указатель на массив структур, user - указатель на имя пользователя, данные о камере
 * возвращает 0, если камера не найдена; 1 - если данные успешно изменены
 */
int update_limit(Camera* cameras, int camera_count, int id, float new_low_limit, float new_up_limit, const char* user) {
    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].id == id) {
            log_changes(id, user);
            cameras[i].low_limit = new_low_limit;
            cameras[i].up_limit = new_up_limit;
            return 1;
        }
    }
    return 0;
}

/*
 * выводит на экран историю изменений диапазонов температур камер
 * filename - указывает на имя файла
 */
int history(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }
    char line[MAX_DLINA];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
    return 1;
}

/*
 * сортирует данные по убыванию процента отклонения
 * cameras - указывает на массив структур, camera_count - количество камер
 * возвращает 0, если сортировка не выполнена; 1 - если сортировка выполнена успешна
 */
int sort_deviations(Camera* cameras, int camera_count) {
    for (int i = 0; i < camera_count - 1; i++) {
        for (int j = i + 1; j < camera_count; j++) {
            if (cameras[i].procent < cameras[j].procent) {
                Camera temp = cameras[i];
                cameras[i] = cameras[j];
                cameras[j] = temp;
                return 1;
            }
        }
    }
    return 0;
}

/*
 * записывает отсортированные данные в файл
 * filename - указывает на имя файла, cameras - указывает на массив структур, camera_count - количество камер
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно записаны
 */
int save_sorted_deviations(const char* filename, Camera* cameras, int camera_count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].procent > 0) {
            fprintf(file, "Камера ID %d: %.2f%% отклонение (температура %.2f)\n", cameras[i].id, cameras[i].procent, cameras[i].temp);
        }
    }
    fclose(file);
    return 1;
}

/*
 * выводит отсортированные данные на экран
 * cameras - указывает на массив структур, camera_count - количество камер
 */
void display_sorted_deviations(Camera* cameras, int camera_count) {
    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].procent > 0) {
            printf("Камера ID %d: %.2f%% отклонение (температура %.2f)\n", cameras[i].id, cameras[i].procent, cameras[i].temp);
        }
    }
}









