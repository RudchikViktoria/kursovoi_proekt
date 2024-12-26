#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CAMERAS 100
#define MAX_DLINA 50    

typedef struct {
    int id;            /* ������������� ������ */
    float temp;        /* ����������� ������ */
    float low_limit;   /* ������ ������ ����������� */
    float up_limit;    /* ������� ������ ����������� */
    float procent;     /* ������� ���������� ����������� */
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
    printf("*    ����� ���������� � '������������'!          *\n");
    printf("*   ��� �������� �������� � ����������� ������!  *\n");
    printf("*   ������������� �����������, ��������� ������! *\n");
    printf("**************************************************\n");

    Camera cameras[MAX_CAMERAS];  /* ������ �������� ��� ������ � ������ */
    int camera_count = 0;
    int choice;
    int a = 0;                      /* ���������� ����� ��� ��������� ������ */
    char filename[30];              /* ������ ��� �������� ����� ����� */
    int id;
    float new_low_limit, new_up_limit;
    char user[50];

    do {
        printf("\n�������� ����� ������: \n");
        printf("1 > ��������� ������ �� �����\n");
        printf("2 > ������������� ��������� ������\n");
        printf("3 > �������� ���������� ������\n");
        printf("4 > ��������� ��������� �����������\n");
        printf("5 > �������� ������� ��������� ���������� ����������\n");
        printf("6 > ����������� ������ ����������\n");
        printf("7 > ��������� ��������������� ������\n");
        printf("8 > ������� ��������������� ������ �� �����\n");
        printf("0 > �����\n");
        printf("��� ����� > \n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            camera_count = read_data("myfile.txt", cameras);
            if (camera_count == 0) {
                printf("��� ������ ��� ������\n");
            }
            else {
                printf("������ ������� ���������\n");
            }
            break;
        case 2:
            printf("�������� ���������� ����� ��� ��������� ������ > ");
            scanf("%d", &a);
            if (a > 0) {
                if (generate_data("generate_data.txt", a) == 0) {
                    printf("������ �������� �����");
                }
                else {
                    printf("������ ������� �������������\n");
                }
            }
            else {
                puts("������ �� �������������\n");
            }
           
            break;
        case 3:
            printf("������� ��� ����� ��� ����������� (1 - myfile.txt, 2 - generate_data.txt): ");
            scanf("%s", filename);
            printf("\n�������� ���������� ����������� �� �����\n");
            if (check_temperature(filename) == 0) {
                printf("������: ���� �� ������. ��������� ������������ ����� �����\n");
            }
            else {
                printf("���������� �������� �������\n");
            }
            break;
        case 4: 
            printf("������� ID ������ ��� ��������� ��������� �����������: \n");
            scanf("%d", &id);
            printf("������� ����� ������ ������: ");
            scanf("%f", &new_low_limit);
            printf("������� ����� ������� ������: ");
            scanf("%f", &new_up_limit);
            printf("������� ��� ��������������: ");
            scanf("%s", user);

            if (update_limit(cameras, camera_count, id, new_low_limit, new_up_limit, user)) {
                printf("�������� ������� ��������!\n");
            }
            else {
                printf("������ � ��������� ID �� �������\n");
            }
            break;
        case 5:
            if (history("�������.txt") == 0) {
                printf("������ �������� �����\n");
            }
            break;
        case 6:
            sort_deviations(cameras, camera_count);
            printf("���������� �������");
            break;
        case 7:
            if (save_sorted_deviations("���������������_����������.txt", cameras, camera_count) == 0) {
                puts("������ �������� �����\n)");
            }
            else {
                puts("������ ������� ���������\n");
            }
            break;
        case 8:
            printf("\n��������������� ������ ����������:\n");
            display_sorted_deviations(cameras, camera_count);
            break;
        case 0:
            printf("����� �� ���������\n");
            break;
        default:
            printf("�������� �����\n");
            break;
        }
    } while (choice != 0);
    return 0;
}

/*
 * ������ ������ � ����������� ������� �� ���������� �����
 * filename - ��������� �� ��� �����, cameras - ��������� �� ������ �������� Camera
 * ���������� ���������� �����; ���������� 0, ���� ���� �� ������� �������
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
 * ���������� � ���� ��������������� ������ ��� �����
 * filename - ��������� �� ��� �����, count - ���������� �����
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ��������
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
 * ��������� ������� ����������
 * temp - ����������� ������, low_limit - ������ ������, up_limit - ������� ������
 * ���������� ������� ����������
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
 * ������� ��������� � �������� ���������� �� �����
 * id - ������������� ������, temp - ����������� ������, low_limit - ������ ������, up_limit - ������� ������, procent - ������� ����������
 * ���������� 1, ���� ������ ������� �������, � ��������� ������ 0
 */
int put_otcl(int id, float temp, float low_limit, float up_limit, float procent) {
    if (procent > 0) {
        printf("������ ID %d: ���������� �� %.0f%% (�������: %.2f, �������� %.2f - %.2f)\n", id, procent, temp, low_limit, up_limit);
        return 1;
    }
    return 0;
}

/*
 * ���������� ID ������ � ������� ���������� � ����
 * camera_id - ������������� ������, procent - ������� ����������, t_temp - ������� ����������� ������, low_limit - ������ ������, up_limit - ������� ������
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ��������
 */
int log_deviation(int camera_id, float procent, float t_temp, float low_limit, float up_limit) {
    FILE* file = fopen("����������.txt", "a");
    if (file == NULL) {
        printf("������ �������� ����� ��� ������ ����������\n");
        return 0;
    }
    if (procent > 0) {
        fprintf(file, "������ ID %d: ���������� �� %.0f%% (������� �����������: %.2f, ��������: %.2f - %.2f)\n", camera_id, procent, t_temp, low_limit, up_limit);
    }
    fclose(file);
    return 1;
}

/*
 * ��������� ���������� ������ �� �����
 * filename - ��������� �� ��� �����
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ���������
 */
int check_temperature(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("������ �������� ����� ��� �������� �����������\n");
        return 0;
    }

    float up_limit = 5;
    float low_limit = -20;
    int id = 0;
    float temp = 0;
    char line[MAX_DLINA]; /* ������ ��� ������ ����� �� ����� */

    /* ������ ������ �� ����� � ��������� �� � line */
    while (fgets(line, sizeof(line), file)) {
        /* ���� ������� �������� 2 ��������, �� ������� ����� */
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
 * �������� ������� ���� � ����� � ��������� �������
 * buffer - ��������� �� ������, size - ������ ������ buffer.
 */
void data(char* buffer, size_t size) {
    time_t t = time(NULL);
    /* �������������� �������� ������� � ��������� � �������� ��� � ��������� */
    struct tm* tm_info = localtime(&t);
    strftime(buffer, size, "%d-%m-%Y %H:%M:%S", tm_info);
}

/*
 * ���������� ���������� �� ��������� ���������� ����������� ������ � ����
 * camera_id - ������������� ������, user - ��������� �� ��� ������������
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ��������
 */
int log_changes(int camera_id, const char* user) {
    FILE* log_file = fopen("�������.txt", "a");
    if (log_file == NULL) {
        puts("������ �������� �����");
        return 0;
    }

    char datatime[50]; /* ������ ��� �������� ���� � ������� */
    data(datatime, sizeof(datatime));

    fprintf(log_file, "���� ��������� ���������: %s, ������ ID: %d, �������������: %s\n", datatime, camera_id, user);
    fclose(log_file);
    return 1;
}


/*
 * ��������� ������������ �������� ��������� ���������� ��� ��������� ������
 * cameras - ��������� �� ������ ��������, user - ��������� �� ��� ������������, ������ � ������
 * ���������� 0, ���� ������ �� �������; 1 - ���� ������ ������� ��������
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
 * ������� �� ����� ������� ��������� ���������� ���������� �����
 * filename - ��������� �� ��� �����
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
 * ��������� ������ �� �������� �������� ����������
 * cameras - ��������� �� ������ ��������, camera_count - ���������� �����
 * ���������� 0, ���� ���������� �� ���������; 1 - ���� ���������� ��������� �������
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
 * ���������� ��������������� ������ � ����
 * filename - ��������� �� ��� �����, cameras - ��������� �� ������ ��������, camera_count - ���������� �����
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ��������
 */
int save_sorted_deviations(const char* filename, Camera* cameras, int camera_count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].procent > 0) {
            fprintf(file, "������ ID %d: %.2f%% ���������� (����������� %.2f)\n", cameras[i].id, cameras[i].procent, cameras[i].temp);
        }
    }
    fclose(file);
    return 1;
}

/*
 * ������� ��������������� ������ �� �����
 * cameras - ��������� �� ������ ��������, camera_count - ���������� �����
 */
void display_sorted_deviations(Camera* cameras, int camera_count) {
    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].procent > 0) {
            printf("������ ID %d: %.2f%% ���������� (����������� %.2f)\n", cameras[i].id, cameras[i].procent, cameras[i].temp);
        }
    }
}









