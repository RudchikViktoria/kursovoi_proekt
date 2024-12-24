//variant E14
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
} Camera;

typedef struct {
    int id;            /* идентификатор камеры */
    float procent;     /* процент отклонения температуры */
    float t_temp;      /* температура камеры */ 
    float low_limit;   /* нижний предел температуры */
    float up_limit;    /* верхний предел температуры */
} Deviation;

int read_data(const char *filename, Camera *cameras);
int generate_data(const char *filename, int count);
float calculate_otcl(float temp, float low_limit, float up_limit);
void put_otcl(int id, float temp, float low_limit, float up_limit, float procent);
int log_deviation(int camera_id, float procent, float current_temp, float low_limit, float up_limit);
int check_temperature(const char *filename);
void data(char* buffer, size_t size);
int log_changes(int camera_id, const char *user);
int update_limit(Camera *cameras, int camera_count);
void history(const char *filename);
int sort_deviations(const char *input_file, Deviation *deviations, int *count);
int save_sorted_deviations(const char *output_file, Deviation *deviations, int count);
int display_sorted_deviations(Deviation *deviations, int count);

int main()
{
    system("chcp 1251");

    printf("**************************************************\n");
    printf("*    Добро пожаловать в 'Фригомонитор'!          *\n");
    printf("*   Ваш надежный помощник в мониторинге холода!  *\n");
    printf("*   Контролируйте температуру, избегайте аварий! *\n");
    printf("**************************************************\n");

    Camera cameras[MAX_CAMERAS];  /* массив структур для данных о камере */
    Deviation dev[MAX_CAMERAS];   /* массив структур для отклонения температур камер */
    int camera_count = 0;          
    int deviation_count = 0;
    int choice;
    int a = 0;                     /* количество камер для генерации данных */
    char filename[30];              /* массив для хранения имени файла */
 
    do {
        printf("\nВыберите режим работы: \n");
        printf("1 > Загрузить данные из файла\n");
        printf("2 > Сгенерировать случайные данные\n");
        printf("3 > Включить мониторинг данных\n");
        printf("4 > Настроить диапазоны температуры\n");
        printf("5 > Просмотр истории изменений диапазонов температур\n");
        printf("6 > Сортировать данные отклонений\n"); 
        printf("7 > Сохранить отсортированные данные в новый файл\n");
        printf("8 > Вывести отсортированные данные на экран\n");
        printf("0 > Выход\n");
        printf("Ваш выбор > \n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            camera_count = read_data("myfile.txt", cameras);
            if (camera_count == 0) {
                printf("Нет данных\n");
            } else {
                printf("Данные успешно загружены из файла\n");
            }
            break;
        case 2:
            printf("Выберите количество камер для генерации данных\n");
            scanf("%d", &a);
            generate_data("generate_data.txt", a);
            break;
        case 3:
            printf("Введите имя файла для мониторинга (1 - myfile.txt, 2 - generate_data.txt): ");
            scanf("%s", &filename);
            check_temperature(filename);
            break;
        case 4:
            update_limit(cameras, camera_count);
            break;
        case 5:
            history("история.txt");
            break;
        case 6: 
            sort_deviations("отклонения.txt", dev, &deviation_count); 
            break; 
        case 7: 
            save_sorted_deviations("отсортированные_отклонения.txt", dev, deviation_count); 
            break;
        case 8: 
            display_sorted_deviations(dev, deviation_count);
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
 * возвращает количество камер, данные которых удалось считать, возвращает 0, если файл не удалось открыть
 */
int read_data(const char *filename, Camera *cameras) {

    FILE *file = fopen("myfile.txt", "r");
    if (file == NULL) {
        puts("Ошибка открытия файла");
        return 0;
    }
    int count = 0;
    while (fscanf(file, "%d-%f", &cameras[count].id, &cameras[count].temp) == 2) {
        cameras[count].low_limit = -20;
        cameras[count].up_limit = 5;
        count++;
        if (count >= MAX_CAMERAS) 
            break;
    }
    fclose(file);
    return count;
}

/*
 * записывает в файл сгенерированные данные для камер
 * filename - указывает на имя файла, count - количество записываемых камер
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно записаны
 */
int generate_data(const char *filename, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Ошибка создания файла\n");
        return 0;
    }

    srand(time(NULL));
    for (int i = 0; i < count; i++) {
        int id = i + 1;
        /* температура в камерах от -20 до 20 */
        float temp = (rand() % 4000 - 2000) / 100.0; 
        fprintf(file, "%d-%.2f\n", id, temp);
    }
    fclose(file);
    return 1;
}

/*
 * записывает ID камеры и процент отклонения в файл
 * camera_id - идентификатор камеры, procent - процент отклонения, t_temp - текущая температура камеры, low_limit - нижний предел; up_limit - верхний предел
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно записаны
 */
int log_deviation(int camera_id, float procent, float t_temp, float low_limit, float up_limit) {
    FILE *file = fopen("отклонения.txt", "a");
    if (file == NULL) {
        printf("Ошибка открытия файла для записи отклонений\n");
        return 0;
    }
    fprintf(file, "Камера ID %d: отклонение на %.0f%% (текущая температура: %.2f, диапазон: %.2f - %.2f)\n", camera_id, procent, t_temp, low_limit, up_limit);
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
    } else if (temp < low_limit - otcl_10 || temp > up_limit + otcl_10) {
        procent = 10;
    } else if (temp < low_limit - otcl_3 || temp > up_limit + otcl_3) {
        procent = 3;
    }
    return procent;
}

/*
 * выводит сообщение о проценте отклонения на экран
 * id - идентификатор камеры, temp - температура камеры, low_limit - нижний предел, up_limit - верхний предел, procent - процент отклонения
 */
void put_otcl(int id, float temp, float low_limit, float up_limit, float procent) {
    if (procent > 0) {
        printf("\nПроверка отклонений температуры из файла\n");
        printf("Камера ID %d: отклонение на %.0f%% (текущая: %.2f, диапазон %.2f - %.2f)\n", id, procent, temp, low_limit, up_limit);
    }
}

/*
 * выполняет мониторинг данных из файла
 * filename - указывает на имя файла
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно прочитаны
 */
int check_temperature(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла для проверки температуры\n");
        return 0;
    }

    float up_limit = 5;
    float low_limit = -20;
    int id = 0;
    float temp = 0;
    char line[MAX_DLINA];

    while (fgets(line, sizeof(line), file)) {
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
    /* преобразование текущего времени в локальное и хранение его в структуре*/
    struct tm* tm_info = localtime(&t);
    strftime(buffer, size, "%d-%m-%Y %H:%M:%S", tm_info);
}

/*
 * записывает информацию об изменении диапазонов температуры камеры в файл
 * camera_id - идентификатор камеры, user - имя пользователя
 * возвращает 0, если файл не удалось открыть; 1 - если данные успешно записаны
 */
int log_changes(int camera_id, const char *user) { 
    FILE *log_file = fopen("история.txt", "a");
    if (log_file == NULL) {
        puts("Ошибка открытия файла");
        return 0;
    }

    char datatime[50];
    data(datatime, sizeof(datatime));

    fprintf(log_file, "Дата изменения диапазона: %s, Камера ID: %d, Ответственный: %s\n", datatime, camera_id, user);
    fclose(log_file);
    return 1;
}

/*
 * позволяет пользователю изменить диапазоны температур для указанной камеры
 * cameras - указатель на массив структур, camera_count - количество камер в массиве
 * возвращает 0, если камера не найдена; 1 - если данные успешно изменены
 */
int update_limit(Camera *cameras, int camera_count) {
    int id;
    char user[50]; 
    printf("\nВведите ID камеры для изменения диапазона температуры: \n");
    scanf("%d", &id);

    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].id == id) { 
            printf("Текущий диапазон для камеры %d: %.2f - %.2f\n", id, cameras[i].low_limit, cameras[i].up_limit);
            printf("Введите новый нижний предел: ");
            float new_low_limit;
            scanf("%f", &new_low_limit);
            printf("Введите новый верхний предел: ");
            float new_up_limit;
            scanf("%f", &new_up_limit);
            printf("Введите имя ответственного: ");
            scanf("%s", &user);

            log_changes(id, user); 

            cameras[i].low_limit = new_low_limit;
            cameras[i].up_limit = new_up_limit;
            puts("Диапазон успешно обновлен!\n");
            return 1;
        }
    }
    printf("Камера с ID %d не найдена\n", id);
    return 0;
}

/*
 * выводит на экран историю изменений диапазонов температур камер
 * filename - указывает на имя файла
 */
void history(const char *filename) { 
    FILE *log = fopen(filename, "r");
    if (log == NULL) {
        printf("Ошибка открытия файла");
    }
    char line[100]; /* массив символов для хранения строк с файла */ 
    printf("История изменений диапазона температур: \n");
    while (fgets(line, sizeof(line), log)) { 
        printf("%s", line);
    }
    fclose(log);
}

/*
 * сортирует данные из файла по убыванию процента отклонения
 * filename - указывает на имя файла, dev - указывает на массив структур, count - указывает на количество считанных отклонений
 * возвращает 0, если файл не удалось открыть; 1 - если сортировка выполнена успешна
 */
int sort_deviations(const char *filename, Deviation *dev, int *count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла для сортировки\n");
        return 0;
    }
    *count = 0;
    while (fscanf(file, "Камера ID %d: отклонение на %f%% (текущая температура: %f, диапазон: %f - %f)\n", &dev[*count].id, &dev[*count].procent, &dev[*count].t_temp, &dev[*count].low_limit, &dev[*count].up_limit) == 5) {
        (*count)++;
        if (*count >= MAX_CAMERAS) 
            break;
    }
    fclose(file);

    /* сортировка массива отклонений по убыванию процента */ 
    for (int i = 0; i < *count - 1; i++) {
        for (int j = 0; j < *count - i - 1; j++) {
            if (dev[j].procent < dev[j + 1].procent) {
                Deviation temp = dev[j];
                dev[j] = dev[j + 1];
                dev[j + 1] = temp;
            }
        }
    }
    printf("Данные успешно отсортированы по убыванию процента отклонения.\n");
    return 1;
}

/*
 * записывает отсортированные данные в новый файл
 * filename - указывает на имя файла, dev - указывает на массив структур, count - количество записанных отклонений
 * возвращает 0, если файл не удалось открыть, 1 - если запись успешна
 */
int save_sorted_deviations(const char *filename, Deviation *dev, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Ошибка создания файла для записи отсортированных данных\n");
        return 0;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "Камера ID %d: отклонение на %.0f%% (текущая температура: %.2f, диапазон: %.2f - %.2f)\n", dev[i].id, dev[i].procent, dev[i].t_temp, dev[i].low_limit, dev[i].up_limit);
    }
    fclose(file);
    printf("Отсортированные данные успешно сохранены\n");
    return 1;
}

/*
 * выводит отсортированные данные отклонений на экран
 * dev - указатель на массив структур, count - количество записанных отклонений
 * возвращает 0, если файл не удалось открыть, 1 - если данные вывелись успешно
 */
int display_sorted_deviations(Deviation *dev, int count) {
    if (count == 0) {
        printf("Сначала выполните сортировку\n");
        return 0;
    }

    printf("\nОтсортированные данные отклонений:\n");
    for (int i = 0; i < count; i++) {
        printf("Камера ID %d: отклонение на %.0f%% (текущая температура: %.2f, диапазон: %.2f - %.2f)\n", dev[i].id, dev[i].procent, dev[i].t_temp, dev[i].low_limit, dev[i].up_limit);
    }
    return 1;
}