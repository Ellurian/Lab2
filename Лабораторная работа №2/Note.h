#pragma once                                   // защита от повторного подключения файла
#include <string>                              // класс std::string для текстовых полей
#include <iostream>                            // типы std::istream и std::ostream для перегрузки >> и <<

// Класс NOTE - одна запись записной книжки: фамилия, имя, телефон и день рождения.
class Note                                     // начало описания класса
{
private:                                       // закрытые данные: напрямую снаружи к ним не обратиться
    static int objectCounter;                  // общий для всех объектов счётчик: сколько объектов создано (нужен для номеров в сообщениях)
    int id;                                    // порядковый номер этого объекта (чтобы в консоли было видно, КАКОЙ объект создан/удалён)
    std::string surname;                       // фамилия
    std::string name;                          // имя
    std::string phone;                         // номер телефона (хранится строкой, т.к. может начинаться с + или с нуля)
    int* birthday;                             // указатель на ДИНАМИЧЕСКИЙ массив из 3 чисел: [0] - день, [1] - месяц, [2] - год

    // Проверки значений. Если значение плохое - бросают исключение NoteException с пояснением.
    static void validateText(const std::string& value, const std::string& fieldName); // фамилия/имя не должны быть пустыми
    static void validatePhone(const std::string& value);                                // телефон: только цифры (и + в начале), 5-15 цифр
    static void validateDate(int day, int month, int year);                             // дата должна существовать в календаре
    static bool isLeapYear(int year);                                                   // true, если год високосный
    static int daysInMonth(int month, int year);                                        // сколько дней в месяце

public:                                        // открытая часть: интерфейс класса
    Note();                                    // конструктор без параметров
    Note(const std::string& newSurname, const std::string& newName, const std::string& newPhone,
        int day, int month, int year);        // конструктор с параметрами
    Note(const Note& other);                   // конструктор копирования
    ~Note();                                   // деструктор

    Note& operator=(const Note& other);        // перегруженный оператор присваивания (a = b)
    bool operator<(const Note& other) const;   // перегруженный оператор "меньше": сравнивает записи по дате рождения

    // get-функции (получить значение поля)
    std::string getSurname() const;            // вернуть фамилию
    std::string getName() const;               // вернуть имя
    std::string getPhone() const;              // вернуть телефон
    int getDay() const;                        // вернуть день рождения (число)
    int getMonth() const;                      // вернуть месяц рождения
    int getYear() const;                       // вернуть год рождения

    // set-функции (установить значение поля, с проверкой)
    void setSurname(const std::string& value); // установить фамилию
    void setName(const std::string& value);    // установить имя
    void setPhone(const std::string& value);   // установить телефон
    void setBirthday(int day, int month, int year); // установить дату рождения целиком

    // Перегрузка вставки (<<, вывод) и извлечения (>>, ввод).
    // friend означает: эти функции НЕ методы класса, но им разрешён доступ к закрытым полям.
    friend std::ostream& operator<<(std::ostream& out, const Note& note); // вывод записи в поток
    friend std::istream& operator>>(std::istream& in, Note& note);        // ввод записи из потока
};                                             // конец описания класса
