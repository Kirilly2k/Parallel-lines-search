#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>

// структура координат точки (x, y)
struct Coordinate {
    double x;
    double y;
};

// структура коэффициентов прямой Ax + By + C = 0
struct Line {
    double A;
    double B;
    double C;

};

// функция считывания точек
Coordinate* getCoordinates(int& coordinatesSize, std::ofstream& protocol) {
    std::ifstream inputFile("input.txt"); // файл на ввод
    if (!inputFile.is_open()) { // проверка файла на корректность
        protocol << "Ошибка: файл не может быть открыт\n";
        return nullptr;
    }
    coordinatesSize = 10; // размер выделяемой памяти
    Coordinate* coordinates = new Coordinate[coordinatesSize]; // выделение памяти под массив точек
    int iter = 0; // счётчик итераций
    char line[128]; // буффер для считывания
    while (inputFile.getline(line, 128)) {
        if (iter == coordinatesSize) { // условие, когда место в массиве закончилось
            Coordinate* temp = coordinates;
            int newSize = coordinatesSize + 10;
            coordinates = new Coordinate[newSize];
            for (int i = 0; i < coordinatesSize; i++)
                coordinates[i] = temp[i];
            delete[] temp;
            coordinatesSize = newSize;
        }
        double x, y;
        char* end;
        int size = 10; // размер выделяемой памяти
        double* nums = new double[10]; // выделение памяти под массив чисел
        int i = 0;
        for (double f = std::strtod(line, &end); line != end; f = std::strtod(line, &end)) { // преобразование char* к int
            strcpy_s(line, end);
            if (errno == ERANGE) {
                protocol << "Ошибка: некорректная строка\n";
                return nullptr;
            }
            if (i == size) {
                double* temp = nums;
                int newSize = size + 10;
                nums = new double[newSize];
                for (int i = 0; i < size; i++)
                    nums[i] = temp[i];
                delete[] temp;
                size = newSize;
            }
            nums[i] = f;
            i++;
        }
        if (i < 2) { // условие, когда чисел в строке не 2
            protocol << "Ошибка: некорректная строка\n";
        }
        else if (i > 2) {
            protocol << "Новая точка: (" << nums[0] << "; " << nums[1] << "), но слишком много аргументов\n";
            coordinates[iter].x = nums[0];
            coordinates[iter].y = nums[1];
            iter++;
        }
        else {
            protocol << "Новая точка: (" << nums[0] << "; " << nums[1] << ")\n";
            coordinates[iter].x = nums[0];
            coordinates[iter].y = nums[1];
            iter++;
        }
        delete[] nums;
    }
    coordinatesSize = iter; // итоговый размер массива
    inputFile.close(); // закрытие файла
    return coordinates;
}

// функция создания массива линий
Line* getLines(Coordinate* coordinates, int coordinatesSize, int& linesSize, std::ofstream& protocol) {
    linesSize = coordinatesSize * (coordinatesSize - 1) / 2; // количество линий
    Line* lines = new Line[linesSize]; // выделение памяти под массив линий
    int iter = 0;
    for (int i = 0; i < coordinatesSize - 1; i++) {
        for (int j = i + 1; j < coordinatesSize; j++) {
            double a = coordinates[i].y - coordinates[j].y;
            double b = coordinates[j].x - coordinates[i].x;
            double c = coordinates[i].x * coordinates[j].y - coordinates[j].x * coordinates[i].y;
            if (a < 0 || (std::fabs(a) < 0.001 && b < 0)) {
                a = -a;
                b = -b;
                c = -c;
            }
            if (std::fabs(a) > 0.001) {
                c /= a;
                b /= a;
                a /= a;
            }
            else if (std::fabs(b) > 0.001) {
                c /= b;
                a /= b;
                b /= b;
            }
            bool flag = true;
            for (int k = 0; k < iter; k++)
                if (std::fabs(lines[k].A - a) < 0.001 && std::fabs(lines[k].B - b) < 0.001 && std::fabs(lines[k].C - c) < 0.001)
                    flag = false;
            if ((std::fabs(a) > 0.001 || std::fabs(b) > 0.001) && flag) { // условие, когда A и B не равны 0 одновременно
                lines[iter].A = a;
                lines[iter].B = b;
                lines[iter].C = c;
                protocol << "Новая линия: (" << lines[iter].A << ")x + (" << lines[iter].B << ")y + (" << lines[iter].C << ") = 0\n";
                iter++;
            }
        }
    }
    linesSize = iter; // итоговый размер массива
    return lines;
}

// решение задания
void task(Line* lines, int linesSize, int a, int b, std::ofstream& protocol) {
    std::ofstream outputFile("output.txt"); // файл на запись
    if (!outputFile.is_open()) { // проверка файла на корректность
        protocol << "Ошибка: файл не может быть открыт\n";
        return;
    }
    for (int i = 0; i < linesSize - 1; i++) {
        for (int j = i + 1; j < linesSize; j++) {
            if (std::fabs(lines[i].A - lines[j].A) < 0.001 &&
                std::fabs(lines[i].B - lines[j].B) < 0.001) { // условие параллельности
                double h = std::fabs(lines[i].C - lines[j].C) / std::sqrt(lines[i].A * lines[i].A + lines[i].B * lines[i].B);
                protocol << "Линии a: (" << lines[i].A << ")x + (" << lines[i].B << ")y + (" << lines[i].C << ") = 0 и b: ("
                    << lines[j].A << ")x + (" << lines[j].B << ")y + (" << lines[j].C << ") = 0 параллельные и r(a, b) = " << h << '\n';
                if (a - 0.001 < h && h < b + 0.001 && std::fabs(h) > 0.001) // условие, когда расстояние меньше d
                    outputFile << "Линии a: (" << lines[i].A << ")x + (" << lines[i].B << ")y + (" << lines[i].C << ") = 0 и b: ("
                    << lines[j].A << ")x + (" << lines[j].B << ")y + (" << lines[j].C << ") = 0 параллельные и r(a, b) = " << h << '\n';
            }
            else {
                protocol << "Линии a: (" << lines[i].A << ")x + (" << lines[i].B << ")y + (" << lines[i].C << ") = 0 и b: ("
                    << lines[j].A << ")x + (" << lines[j].B << ")y + (" << lines[j].C << ") = 0 не параллельные\n";
            }
        }
    }
    std::cout << "Файл вывода сформирован\n";
    outputFile.close(); // закрытие файла
}

// функция main()
int main() {
    setlocale(LC_ALL, "Russian");
    std::ofstream protocol("protocol.txt"); // файл протокола
    if (!protocol.is_open()) { // проверка файла на корректность
        std::cerr << "Ошибка: файл не может быть открыт\n";
        return 0;
    }
    int coordinatesSize = 0;
    Coordinate* coordinates = getCoordinates(coordinatesSize, protocol); // получение массива точек
    protocol << '\n';
    if (!coordinates)
        return 0;
    int linesSize = 0;
    Line* lines = getLines(coordinates, coordinatesSize, linesSize, protocol); // получение массива линий
    protocol << '\n';

    int a, b;
    std::cout << "Введите диапазон [a, b] по оси X: \na: ";
    std::cin >> a;
    std::cout << "b: ";
    std::cin >> b;
    task(lines, linesSize, a, b, protocol); // вызов функции task()

    delete[] lines;
    delete[] coordinates;
    protocol.close(); // закрытие файла
    return 0;
}