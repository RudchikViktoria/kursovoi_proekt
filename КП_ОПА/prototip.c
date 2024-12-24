//variant E14
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
} Camera;

typedef struct {
    int id;            /* ������������� ������ */
    float procent;     /* ������� ���������� ����������� */
    float t_temp;      /* ����������� ������ */ 
    float low_limit;   /* ������ ������ ����������� */
    float up_limit;    /* ������� ������ ����������� */
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
    printf("*    ����� ���������� � '������������'!          *\n");
    printf("*   ��� �������� �������� � ����������� ������!  *\n");
    printf("*   ������������� �����������, ��������� ������! *\n");
    printf("**************************************************\n");

    Camera cameras[MAX_CAMERAS];  /* ������ �������� ��� ������ � ������ */
    Deviation dev[MAX_CAMERAS];   /* ������ �������� ��� ���������� ���������� ����� */
    int camera_count = 0;          
    int deviation_count = 0;
    int choice;
    int a = 0;                     /* ���������� ����� ��� ��������� ������ */
    char filename[30];              /* ������ ��� �������� ����� ����� */
 
    do {
        printf("\n�������� ����� ������: \n");
        printf("1 > ��������� ������ �� �����\n");
        printf("2 > ������������� ��������� ������\n");
        printf("3 > �������� ���������� ������\n");
        printf("4 > ��������� ��������� �����������\n");
        printf("5 > �������� ������� ��������� ���������� ����������\n");
        printf("6 > ����������� ������ ����������\n"); 
        printf("7 > ��������� ��������������� ������ � ����� ����\n");
        printf("8 > ������� ��������������� ������ �� �����\n");
        printf("0 > �����\n");
        printf("��� ����� > \n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            camera_count = read_data("myfile.txt", cameras);
            if (camera_count == 0) {
                printf("��� ������\n");
            } else {
                printf("������ ������� ��������� �� �����\n");
            }
            break;
        case 2:
            printf("�������� ���������� ����� ��� ��������� ������\n");
            scanf("%d", &a);
            generate_data("generate_data.txt", a);
            break;
        case 3:
            printf("������� ��� ����� ��� ����������� (1 - myfile.txt, 2 - generate_data.txt): ");
            scanf("%s", &filename);
            check_temperature(filename);
            break;
        case 4:
            update_limit(cameras, camera_count);
            break;
        case 5:
            history("�������.txt");
            break;
        case 6: 
            sort_deviations("����������.txt", dev, &deviation_count); 
            break; 
        case 7: 
            save_sorted_deviations("���������������_����������.txt", dev, deviation_count); 
            break;
        case 8: 
            display_sorted_deviations(dev, deviation_count);
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
 * ���������� ���������� �����, ������ ������� ������� �������, ���������� 0, ���� ���� �� ������� �������
 */
int read_data(const char *filename, Camera *cameras) {

    FILE *file = fopen("myfile.txt", "r");
    if (file == NULL) {
        puts("������ �������� �����");
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
 * ���������� � ���� ��������������� ������ ��� �����
 * filename - ��������� �� ��� �����, count - ���������� ������������ �����
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ��������
 */
int generate_data(const char *filename, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("������ �������� �����\n");
        return 0;
    }

    srand(time(NULL));
    for (int i = 0; i < count; i++) {
        int id = i + 1;
        /* ����������� � ������� �� -20 �� 20 */
        float temp = (rand() % 4000 - 2000) / 100.0; 
        fprintf(file, "%d-%.2f\n", id, temp);
    }
    fclose(file);
    return 1;
}

/*
 * ���������� ID ������ � ������� ���������� � ����
 * camera_id - ������������� ������, procent - ������� ����������, t_temp - ������� ����������� ������, low_limit - ������ ������; up_limit - ������� ������
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ��������
 */
int log_deviation(int camera_id, float procent, float t_temp, float low_limit, float up_limit) {
    FILE *file = fopen("����������.txt", "a");
    if (file == NULL) {
        printf("������ �������� ����� ��� ������ ����������\n");
        return 0;
    }
    fprintf(file, "������ ID %d: ���������� �� %.0f%% (������� �����������: %.2f, ��������: %.2f - %.2f)\n", camera_id, procent, t_temp, low_limit, up_limit);
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
    } else if (temp < low_limit - otcl_10 || temp > up_limit + otcl_10) {
        procent = 10;
    } else if (temp < low_limit - otcl_3 || temp > up_limit + otcl_3) {
        procent = 3;
    }
    return procent;
}

/*
 * ������� ��������� � �������� ���������� �� �����
 * id - ������������� ������, temp - ����������� ������, low_limit - ������ ������, up_limit - ������� ������, procent - ������� ����������
 */
void put_otcl(int id, float temp, float low_limit, float up_limit, float procent) {
    if (procent > 0) {
        printf("\n�������� ���������� ����������� �� �����\n");
        printf("������ ID %d: ���������� �� %.0f%% (�������: %.2f, �������� %.2f - %.2f)\n", id, procent, temp, low_limit, up_limit);
    }
}

/*
 * ��������� ���������� ������ �� �����
 * filename - ��������� �� ��� �����
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ���������
 */
int check_temperature(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("������ �������� ����� ��� �������� �����������\n");
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
 * �������� ������� ���� � ����� � ��������� �������
 * buffer - ��������� �� ������, size - ������ ������ buffer.
 */
void data(char* buffer, size_t size) {
    time_t t = time(NULL);
    /* �������������� �������� ������� � ��������� � �������� ��� � ���������*/
    struct tm* tm_info = localtime(&t);
    strftime(buffer, size, "%d-%m-%Y %H:%M:%S", tm_info);
}

/*
 * ���������� ���������� �� ��������� ���������� ����������� ������ � ����
 * camera_id - ������������� ������, user - ��� ������������
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ������ ������� ��������
 */
int log_changes(int camera_id, const char *user) { 
    FILE *log_file = fopen("�������.txt", "a");
    if (log_file == NULL) {
        puts("������ �������� �����");
        return 0;
    }

    char datatime[50];
    data(datatime, sizeof(datatime));

    fprintf(log_file, "���� ��������� ���������: %s, ������ ID: %d, �������������: %s\n", datatime, camera_id, user);
    fclose(log_file);
    return 1;
}

/*
 * ��������� ������������ �������� ��������� ���������� ��� ��������� ������
 * cameras - ��������� �� ������ ��������, camera_count - ���������� ����� � �������
 * ���������� 0, ���� ������ �� �������; 1 - ���� ������ ������� ��������
 */
int update_limit(Camera *cameras, int camera_count) {
    int id;
    char user[50]; 
    printf("\n������� ID ������ ��� ��������� ��������� �����������: \n");
    scanf("%d", &id);

    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].id == id) { 
            printf("������� �������� ��� ������ %d: %.2f - %.2f\n", id, cameras[i].low_limit, cameras[i].up_limit);
            printf("������� ����� ������ ������: ");
            float new_low_limit;
            scanf("%f", &new_low_limit);
            printf("������� ����� ������� ������: ");
            float new_up_limit;
            scanf("%f", &new_up_limit);
            printf("������� ��� ��������������: ");
            scanf("%s", &user);

            log_changes(id, user); 

            cameras[i].low_limit = new_low_limit;
            cameras[i].up_limit = new_up_limit;
            puts("�������� ������� ��������!\n");
            return 1;
        }
    }
    printf("������ � ID %d �� �������\n", id);
    return 0;
}

/*
 * ������� �� ����� ������� ��������� ���������� ���������� �����
 * filename - ��������� �� ��� �����
 */
void history(const char *filename) { 
    FILE *log = fopen(filename, "r");
    if (log == NULL) {
        printf("������ �������� �����");
    }
    char line[100]; /* ������ �������� ��� �������� ����� � ����� */ 
    printf("������� ��������� ��������� ����������: \n");
    while (fgets(line, sizeof(line), log)) { 
        printf("%s", line);
    }
    fclose(log);
}

/*
 * ��������� ������ �� ����� �� �������� �������� ����������
 * filename - ��������� �� ��� �����, dev - ��������� �� ������ ��������, count - ��������� �� ���������� ��������� ����������
 * ���������� 0, ���� ���� �� ������� �������; 1 - ���� ���������� ��������� �������
 */
int sort_deviations(const char *filename, Deviation *dev, int *count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("������ �������� ����� ��� ����������\n");
        return 0;
    }
    *count = 0;
    while (fscanf(file, "������ ID %d: ���������� �� %f%% (������� �����������: %f, ��������: %f - %f)\n", &dev[*count].id, &dev[*count].procent, &dev[*count].t_temp, &dev[*count].low_limit, &dev[*count].up_limit) == 5) {
        (*count)++;
        if (*count >= MAX_CAMERAS) 
            break;
    }
    fclose(file);

    /* ���������� ������� ���������� �� �������� �������� */ 
    for (int i = 0; i < *count - 1; i++) {
        for (int j = 0; j < *count - i - 1; j++) {
            if (dev[j].procent < dev[j + 1].procent) {
                Deviation temp = dev[j];
                dev[j] = dev[j + 1];
                dev[j + 1] = temp;
            }
        }
    }
    printf("������ ������� ������������� �� �������� �������� ����������.\n");
    return 1;
}

/*
 * ���������� ��������������� ������ � ����� ����
 * filename - ��������� �� ��� �����, dev - ��������� �� ������ ��������, count - ���������� ���������� ����������
 * ���������� 0, ���� ���� �� ������� �������, 1 - ���� ������ �������
 */
int save_sorted_deviations(const char *filename, Deviation *dev, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("������ �������� ����� ��� ������ ��������������� ������\n");
        return 0;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "������ ID %d: ���������� �� %.0f%% (������� �����������: %.2f, ��������: %.2f - %.2f)\n", dev[i].id, dev[i].procent, dev[i].t_temp, dev[i].low_limit, dev[i].up_limit);
    }
    fclose(file);
    printf("��������������� ������ ������� ���������\n");
    return 1;
}

/*
 * ������� ��������������� ������ ���������� �� �����
 * dev - ��������� �� ������ ��������, count - ���������� ���������� ����������
 * ���������� 0, ���� ���� �� ������� �������, 1 - ���� ������ �������� �������
 */
int display_sorted_deviations(Deviation *dev, int count) {
    if (count == 0) {
        printf("������� ��������� ����������\n");
        return 0;
    }

    printf("\n��������������� ������ ����������:\n");
    for (int i = 0; i < count; i++) {
        printf("������ ID %d: ���������� �� %.0f%% (������� �����������: %.2f, ��������: %.2f - %.2f)\n", dev[i].id, dev[i].procent, dev[i].t_temp, dev[i].low_limit, dev[i].up_limit);
    }
    return 1;
}