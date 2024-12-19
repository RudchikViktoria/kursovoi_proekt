//variant E14

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CAMERAS 100 // ������������ ���������� �����
#define MAX_DLINA 50 // ������������ ����� ������

// ��������� ��� �������� ������ � ������
typedef struct {
    int id; 
    float temperature;
    float low_limit; 
    float up_limit; 
} Camera;

// ��������� ��� �������� ������ � ������, ��� ���������� ����������
typedef struct {
    int id;
    float procent;
    float current_temp;
    float low_limit;
    float up_limit;
} AnalysisData;

// ��������� �������
int read_data(const char *filename, Camera *cameras);
void generate_data(const char *filename, int count);
void check_temperature(const char *filename);
void data(char *buffer, size_t size);
void log_changes(int camera_id, const char *user);
void update_limit(Camera *cameras, int camera_count);
void history();
void sort_analysis(const char *input_filename, const char *output_filename);

// ������� �������
int main()
{
    system("chcp 1251");

    // �����������
    printf("**************************************************\n");
    printf("*    ����� ���������� � '������������'!          *\n");
    printf("*   ��� �������� �������� � ����������� ������!  *\n");
    printf("*   ������������� �����������, ��������� ������! *\n");
    printf("**************************************************\n");
    getchar();

    Camera cameras[MAX_CAMERAS];   // ������ �������� ��� �����
    AnalysisData data[MAX_CAMERAS];// ������ �������� ��� ������ ��� �������
    int camera_count = 0;         // ������� ����������� �����
    int choice;              
    int choice_data; 

    // ���������� ����������� �����
    camera_count = read_data("myfile.txt", cameras);
    if (camera_count == 0) {
        printf("��� ������\n");
    }
 
    // ���� ���������
    do {
        printf("\n�������� ����� ������: \n");
        printf("1 > �������� ���������� ������\n");
        printf("2 > ��������� ��������� �����������\n");
        printf("3 > �������� ������� ��������� ���������� �����������\n");
        printf("4 > ������ ������\n");
        printf("0 > �����\n");
        printf("��� ����� > \n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("�������� ����� ������, ��� ������� ����� ����������� ����������\n");
            printf("1 > ������, ��������������� �������\n");
            printf("2 > ������, ��������������� �������������\n");
            printf("0 > ��������� �����\n");
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
                printf("�������� ����� ������\n");
            }
            break;
        case 2:
            update_limit(cameras, camera_count);
            break;
        case 3:
            history();
            break;
        case 4:
            sort_analysis("������.txt", "���������������_������.txt");
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
 * filename - ��� �����, �� �������� ����� ����������� ������; cameras - ��������� �� ������ �������� Camera, ���� ����� �������� ������
 * ���������� ���������� �����, ������ ������� ������� �������, ���������� 0, ���� ���� �� ������� �������
 */
int read_data(const char *filename, Camera *cameras) {

    FILE *file = fopen("myfile.txt", "r");
    if (file == NULL) {
        puts("������ �������� �����");
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
 * ���������� ��������� ������ ��� ����������� ����� � ���������� �� � ����
 * filename - ��� �����, � ������� ����� �������� ������; count - ���������� ������������ �����
 * �� ���������� ��������
 * ��������� ��� ���������������� ��������� ����; ������ ������������ �������� (����������� �� -20.00 �� 20.00)
 */
void generate_data(const char *filename, int count) {
    FILE *file = fopen("random_data", "w");
    if (file == NULL) {
        printf("������ �������� �����\n");
        return;
    }

    srand(time(NULL)); // ������������� ���������� ��������� �����
    for (int i = 0; i < count; i++) {
        int id = i + 1; // ���������� ID ��� ������ ������
        float temperature = (rand() % 4000 - 2000) / 100.0; // ����������� �� -20.00 �� +20.00
        fprintf(file, "%d-%.2f\n", id, temperature);
    }
    fclose(file);
}

/*
 * ��������� ���������� ����������� �� ���������� ���������� ��� �����
 * filename - ��� �����, ����������� ������ � �������
 * �� ���������� ��������
 * ������ ���������� ��������� �� ����� � ������������ � ���� "������.txt"
 */
void check_temperature(const char *filename) {
    FILE *file = fopen(filename, "r");
    FILE* file_1 = fopen("������.txt", "w");
    if (file == NULL || file_1 == NULL) {
        printf("������ �������� �����\n");
        return;
    }

    char line[MAX_DLINA];
    int id = 0;
    float temp = 0;

    printf("\n�������� ���������� ����������� �� �����\n");
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
                printf("������ ID %d: ���������� �� %.0f%% (�������: %.2f, �������� %.2f - %.2f)\n", id, procent, temp, low_limit, up_limit);
                fprintf(file_1, "������ ID %d: ���������� �� %.0f%% (�������: %.2f, �������� %.2f - %.2f)\n", id, procent, temp, low_limit, up_limit);
            }
        }
    }
    fclose(file);
    fclose(file_1);
}

/*
 * �������� ������� ���� � ����� � ��������� �������
 * buffer - ��������� �� ������, ���� ����� �������� ���� � �����; size - ������ ������ buffer.
 * �� ���������� ��������
 * ������� ���������� ��������� ����� �������
 */
void data(char *buffer, size_t size) { 
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t); //�������������� �������� ������� � ��������� ����� � �������� ��� � ��������� tm
    strftime(buffer, size, "%d-%m-%Y %H:%M:%S", tm_info); //���������� ������
}

/*
 * ���������� ���������� �� ��������� ���������� ����������� ������ � ����
 * camera_id - ������������� ������, ��� ������� ���� �������� ���������; user - ��� ������������, ������� ���� ���������
 * �� ���������� ��������
 * ���������� ������������ � ���� "�������.txt".
 */
void log_changes(int camera_id, const char* user) { 
    FILE *log_file = fopen("�������.txt", "a");
    if (log_file == NULL) {
        puts("������ �������� �����");
    }

    char datatime[50];
    data(datatime, sizeof(datatime));

    fprintf(log_file, "���� ��������� ���������: %s, ������ ID: %d, �������������: %s\n", datatime, camera_id, user);
    fclose(log_file);
}

/*
 * ��������� ������������ �������� ��������� ���������� ��� ��������� ������
 * cameras - ��������� �� ������ �������� Camera; camera_count - ���������� ����� � �������
 * �� ���������� ��������
 * ����������� �������� ������� � �������� �������� ����������� ��� ��������� ������; ���������� � ��������� ���������� ������������ � ���� "�������.txt"
 */
void update_limit(Camera *cameras, int camera_count) {
    int id;
    char user[50]; //������ �������� ��� �������� ����� ��������������
    printf("\n������� ID ������ ��� ��������� ��������� �����������: \n");
    scanf("%d", &id);

    for (int i = 0; i < camera_count; i++) {
        if (cameras[i].id == id) { // �������� �� ������� ������
            printf("������� �������� ��� ������ %d: %.2f - %.2f\n", id, cameras[i].low_limit, cameras[i].up_limit);
            printf("������� ����� ������ ������: ");
            float new_low_limit;
            scanf("%f", &new_low_limit);
            printf("������� ����� ������� ������: ");
            float new_up_limit;
            scanf("%f", &new_up_limit);
            printf("������� ��� ��������������: ");
            scanf("%s", &user);

            log_changes(id, user); // ������ � ������� ��������� ID ������ � ��������������

            // ���������� ������� � �������� ������� ��� ������
            cameras[i].low_limit = new_low_limit;
            cameras[i].up_limit = new_up_limit;
            puts("�������� ������� ��������!\n");
            return;
        }
    }
    printf("������ � ID %d �� �������\n", id);
}


/*
 * ������� �� ����� ������� ��������� ���������� ���������� �����
 * �� ��������� ����������
 * �� ���������� ��������
 * ��������� ������ �� ����� "�������.txt"
 */
void history() { 
    FILE *log = fopen("�������.txt", "r");
    if (log == NULL) {
        printf("������ �������� �����");
    }
    char line[100]; // ������ �������� ��� �������� ����� � �����
    printf("������� ��������� ��������� ����������: \n");
    while (fgets(line, sizeof(line), log)) { // ��������� ������ � �������� �� � ������
        printf("%s", line);
    }
    fclose(log);
}

/*
 * ��������� ������ �� ����� "������.txt" �� �������� ���������� � ���������� ��������� � ����� ����
 * input_filename - ��� ����� � ������� �������; output_filename - ��� �����, ���� ����� �������� ��������������� ������
 * �� ���������� ��������
 * ������ ������ �� ����� input_filename; ��������� ������ ������� ��������;���������� ��������������� ������ � ���� output_filename
 */
void sort_analysis(const char *input_filename, const char *output_filename) {


    AnalysisData data[MAX_CAMERAS];
    int count = 0; // ������� ���������� �����

    // �������� ����� ��� ������
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        printf("������ �������� �����\n");
        return;
    }

    // ������ ������ �� ����� � ������
    while (count < MAX_CAMERAS) {
        if (fscanf(input_file, "������ ID %d: ���������� �� %f%% (�������: %f, �������� %f - %f)\n",
            &data[count].id,
            &data[count].procent,
            &data[count].current_temp,
            &data[count].low_limit,
            &data[count].up_limit) == 5) {
            count++;
        }
        else {
            break; // ��������� ����, ���� ������ �� ������������� �������
        }
    }

    // ���������� ������� ��������
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (data[j].procent < data[j + 1].procent) {
                AnalysisData temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }

    // ����� ��������������� ������ �� �����
    printf("\n��������������� ������ �� �������� ����������:\n");
    for (int i = 0; i < count; i++) {
        printf("������ ID %d: ���������� �� %.0f%% (�������: %.2f, �������� %.2f - %.2f)\n", data[i].id, data[i].procent, data[i].current_temp, data[i].low_limit, data[i].up_limit);
    }

    // ������ ��������������� ������ � ����� ����
    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        printf("������ �������� �����\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(output_file, "������ ID %d: ���������� �� %.0f%% (�������: %.2f, �������� %.2f - %.2f)\n", data[i].id, data[i].procent, data[i].current_temp, data[i].low_limit, data[i].up_limit);
    }
    fclose(output_file);
    printf("\n������ ������� �������� � ����\n");
}