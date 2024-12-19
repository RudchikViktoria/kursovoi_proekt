//variant E14

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CAMERAS 100 // Максимальное количество камер
#define MAX_DLINA 50 // Максимальная длина строки

// Структура для хранения данных о камере
typedef struct {
    int id; 
    float temperature;
    float low_limit; 
    float up_limit; 
} Camera;

// Структура для хранения данных о камере, для дальнейшей сортировки
typedef struct {
    int id;
    float procent;
    float current_temp;
    float low_limit;
    float up_limit;
} AnalysisData;

// Прототипы функций
int read_data(const char *filename, Camera *cameras);
void generate_data(const char *filename, int count);
void check_temperature(const char *filename);
void data(char *buffer, size_t size);
void log_changes(int camera_id, const char *user);
void update_limit(Camera *cameras, int camera_count);
void history();
void sort_analysis(const char *input_filename, const char *output_filename);

// Главная функция
int main()
{
    system("chcp 1251");

    // Приветствие
    printf("**************************************************\n");
    printf("*    Добро пожаловать в 'Фригомонитор'!          *\n");
    printf("*   Ваш надежный помощник в мониторинге холода!  *\n");
    printf("*   Контролируйте температуру, избегайте аварий! *\n");
    printf("**************************************************\n");
    getchar();

    Camera cameras[MAX_CAMERAS];   // массив структур для камер
    AnalysisData data[MAX_CAMERAS];// массив структур для камеры при анализе
    int camera_count = 0;         // счетчик холодильных камер
    int choice;              
    int choice_data; 

    // Количество холодильных камер
    camera_count = read_data("myfile.txt", cameras);
    if (camera_count == 0) {
        printf("Нет данных\n");
    }
 
    // Меню программы
    do {
        printf("\nВыберите режим работы: \n");
        printf("1 > Включить мониторинг данных\n");
        printf("2 > Настроить диапазоны температуры\n");
        printf("3 > Просмотр истории изменений диапазонов температуры\n");
        printf("4 > Анализ данных\n");
        printf("0 > Выход\n");
        printf("Ваш выбор > \n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("Выберите набор данных, для которых будет проводиться мониторинг\n");
            printf("1 > Данные, сгенерированные вручную\n");
            printf("2 > Данные, сгенерированные автоматически\n");
            printf("0 > Вернуться назад\n");
            scanf("%d", &choice_data);

            if (choice_data == 1) {
                check_temperature("myfile.txt");
                break;
            }
            else if (choice_data == 2) {
                generate_data("random_data", MAX_CAMERAS);
                check_temperature("random_data");
                break;
            }
            else {
                printf("Неверный выбор данных\n");
            }
            break;
        case 2:
            update_limit(cameras, camera_count);
            break;
        case 3:
            history();
            break;
        case 4:
            sort_analysis("анализ.txt", "отсортированный_анализ.txt");
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
 * filename - имя файла, из которого будут считываться данные; cameras - указатель на массив структур Camera, куда будут записаны данные
 * возвращает количество камер, данные которых удалось считать, возвращает 0, если файл не удалось открыть
 */
int read_data(const char *filename, Camera *cameras) {

    FILE *file = fopen("myfile.txt", "r");
    if (file == NULL) {
        puts("Ошибка открытия файла");
    }
    else {
        char line[MAX_DLINA];
        int count = 0;
        while (fgets(line, sizeof(line), file)) {
            if (sscanf(line, "%d-%f", &cameras[count].id, &cameras[count].temperature) == 2) {
                cameras[count].low_limit = -20;
                cameras[count].up_limit = 5;
                count++;
            }
        }
        fclose(file);
        return count;
    }
}

/*
 * генерирует случайные данные для холодильных камер и записывает их в файл
 * filename - имя файла, в который будут записаны данные; count - количество записываемых камер
 * не возвращает значений
 * создается или перезаписывается указанный файл; данные генерируются случайно (температура от -20.00 до 20.00)
 */
void generate_data(const char *filename, int count) {
    FILE *file = fopen("random_data", "w");
    if (file == NULL) {
        printf("Ошибка создания файла\n");
        return;
    }

    srand(time(NULL)); // Инициализация генератора случайных чисел
    for (int i = 0; i < count; i++) {
        int id = i + 1; // Уникальный ID для каждой камеры
        float temperature = (rand() % 4000 - 2000) / 100.0; // Температура от -20.00 до +20.00
        fprintf(file, "%d-%.2f\n", id, temperature);
    }
    fclose(file);
}

/*
 * проверяет отклонения температуры от допустимых диапазонов для камер
 * filename - имя файла, содержащего данные о камерах
 * не возвращает значений
 * анализ отклонений выводится на экран и записывается в файл "анализ.txt"
 */
void check_temperature(const char *filename) {
    FILE *file = fopen(filename, "r");
    FILE* file_1 = fopen("анализ.txt", "w");
    if (file == NULL || file_1 == NULL) {
        printf("Ошибка открытия файла\n");
        return;
    }

    char line[MAX_DLINA];
    int id = 0;
    float temp = 0;

    printf("\nПроверка отклонений температуры из файла\n");
    while (fgets(line, sizeof(line), file)) {

        if (sscanf(line, "%d-%f", &id, &temp) == 2) {
            float low_limit = -20.0;
            float up_limit = 5.0;

            float otcl_3 = (up_limit - low_limit) * 0.03;
            float otcl_10 = (up_limit - low_limit) * 0.1;
            float otcl_25 = (up_limit - low_limit) * 0.25;
            float procent = 0;

            if (temp < low_limit - otcl_25 || temp > up_limit + otcl_25) {
                procent = 25;
            }
            else if (temp < low_limit - otcl_10 || temp > up_limit + otcl_10) {
                procent = 10;
            }
            else if (temp < low_limit - otcl_3 || temp > up_limit + otcl_3) {
                procent = 3;
            }

            if (procent > 0) {
                printf("Камера ID %d: отклонение на %.0f%% (текущая: %.2f, диапазон %.2f - %.2f)\n", id, procent, temp, low_limit, up_limit);
                fprintf(file_1, "Камера ID %d: отклонение на %.0f%% (текущая: %.2f, диапазон %.2f - %.2f)\n", id, procent, temp, low_limit, up_limit);
            }
        }
    }
    fclose(file);
    fclose(file_1);
}

/*
 * получает текущие дату и время в строковом формате
 * buffer - указатель на строку, куда будет записана дата и время; size - размер строки buffer.
 * не возвращает значений
 * функция использует локальное время системы
 */
void data(char *buffer, size_t size) { 
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t); //преобразование текущего времени в локальное время и хранение его в структуре tm
    strftime(buffer, size, "%d-%m-%Y %H:%M:%S", tm_info); //заполнение буфера
}

/*
 * записывает информацию об изменении диапазонов температуры камеры в файл
 * camera_id - идентификатор камеры, для которой были изменены диапазоны; user - имя пользователя, который внес изменения
 * не возвращает значений
 * информация записывается в файл "история.txt".
 */
void log_changes(int camera_id, const char* user) { 
    FILE *log_file = fopen("история.txt", "a");
    if (log_file == NULL) {
        puts("Ошибка открытия файла");
    }

    char datatime[50];
    data(datatime, sizeof(datatime));

    fprintf(log_file, "Дата изменения диапазона: %s, Камера ID: %d, Ответственный: %s\n", datatime, camera_id, user);
    fclose(log_file);
}

/*
 * позволяет пользователю изменить диапазоны температур для указанной камеры
 * cameras - указатель на массив структур Camera; camera_count - количество камер в массиве
 * не возвращает значений
 * обновляются значения нижнего и верхнего пределов температуры для выбранной камеры; информация о внесенных изменениях записывается в файл "история.txt"
 */
void update_limit(Camera *cameras, int camera_count) {
    int id;
    char user[50]; //массив символов для хранения имени ответственного
    printf("\nВведите ID камеры для изменения диапазона температуры: \n");
    scanf("%d", &id);

    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].id == id) { // проверка на наличие камеры
            printf("Текущий диапазон для камеры %d: %.2f - %.2f\n", id, cameras[i].low_limit, cameras[i].up_limit);
            printf("Введите новый нижний предел: ");
            float new_low_limit;
            scanf("%f", &new_low_limit);
            printf("Введите новый верхний предел: ");
            float new_up_limit;
            scanf("%f", &new_up_limit);
            printf("Введите имя ответственного: ");
            scanf("%s", &user);

            log_changes(id, user); // запись в историю изменений ID камеры и ответственного

            // обновление нижнего и верхнего предела для камеры
            cameras[i].low_limit = new_low_limit;
            cameras[i].up_limit = new_up_limit;
            puts("Диапазон успешно обновлен!\n");
            return;
        }
    }
    printf("Камера с ID %d не найдена\n", id);
}


/*
 * выводит на экран историю изменений диапазонов температур камер
 * не принимает параметров
 * не возвращает значений
 * считывает данные из файла "история.txt"
 */
void history() { 
    FILE *log = fopen("история.txt", "r");
    if (log == NULL) {
        printf("Ошибка открытия файла");
    }
    char line[100]; // массив символов для хранения строк с файла
    printf("История изменений диапазона температур: \n");
    while (fgets(line, sizeof(line), log)) { // считывает строки и помещает их в массив
        printf("%s", line);
    }
    fclose(log);
}

/*
 * сортирует данные из файла "анализ.txt" по проценту отклонения и записывает результат в новый файл
 * input_filename - имя файла с данными анализа; output_filename - имя файла, куда будут записаны отсортированные данные
 * не возвращает значений
 * читает данные из файла input_filename; сортирует данные методом пузырька;записывает отсортированные данные в файл output_filename
 */
void sort_analysis(const char *input_filename, const char *output_filename) {


    AnalysisData data[MAX_CAMERAS];
    int count = 0; // счётчик записанных камер

    // открытие файла для чтения
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        printf("Ошибка открытия файла\n");
        return;
    }

    // чтение данных из файла в массив
    while (count < MAX_CAMERAS) {
        if (fscanf(input_file, "Камера ID %d: отклонение на %f%% (текущая: %f, диапазон %f - %f)\n",
            &data[count].id,
            &data[count].procent,
            &data[count].current_temp,
            &data[count].low_limit,
            &data[count].up_limit) == 5) {
            count++;
        }
        else {
            break; // прерываем цикл, если строка не соответствует формату
        }
    }

    // сортировка методом пузырька
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (data[j].procent < data[j + 1].procent) {
                AnalysisData temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }

    // вывод отсортированных данных на экран
    printf("\nОтсортированные данные по проценту отклонения:\n");
    for (int i = 0; i < count; i++) {
        printf("Камера ID %d: отклонение на %.0f%% (текущая: %.2f, диапазон %.2f - %.2f)\n", data[i].id, data[i].procent, data[i].current_temp, data[i].low_limit, data[i].up_limit);
    }

    // запись отсортированных данных в новый файл
    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        printf("Ошибка создания файла\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(output_file, "Камера ID %d: отклонение на %.0f%% (текущая: %.2f, диапазон %.2f - %.2f)\n", data[i].id, data[i].procent, data[i].current_temp, data[i].low_limit, data[i].up_limit);
    }
    fclose(output_file);
    printf("\nДанные успешно записаны в файл\n");
}