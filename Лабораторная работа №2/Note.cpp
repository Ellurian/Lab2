#include "Note.h"                              // описание класса, который реализуем
#include "NoteException.h"                     // наше исключение, чтобы сообщать об ошибках
#include <iostream>                            // cout / endl
#include <iomanip>                             // setw и setfill для вывода даты вида 05.03.2001

int Note::objectCounter = 0;                   // статическое поле нужно "создать" один раз вне класса; пока объектов нет - 0

// ====================== ПРОВЕРКИ ЗНАЧЕНИЙ ======================

// Проверка текстового поля (фамилия, имя)
void Note::validateText(const std::string& value, const std::string& fieldName)
{
    if (value.empty())                         // если строка пустая
    {
        throw NoteException("Поле \"" + fieldName + "\" не может быть пустым."); // "бросаем" исключение с пояснением
    }                                          // конец if
}                                              // конец функции

// Проверка номера телефона
void Note::validatePhone(const std::string& value)
{
    int digits = 0;                            // счётчик цифр в номере
    for (size_t i = 0; i < value.size(); ++i)  // перебираем все символы строки по очереди
    {
        char c = value[i];                     // берём i-й символ
        bool isDigit = (c >= '0' && c <= '9'); // это цифра?
        bool isLeadingPlus = (c == '+' && i == 0); // это плюс в самом начале (например +7...)?
        if (!isDigit && !isLeadingPlus)        // если символ - не цифра и не начальный плюс
        {
            throw NoteException("Телефон может содержать только цифры (и + в начале)."); // ошибка
        }                                      // конец if
        if (isDigit)                           // если это цифра
        {
            ++digits;                          // увеличиваем счётчик цифр
        }                                      // конец if
    }                                          // конец for
    if (digits < 5 || digits > 15)             // в номере должно быть от 5 до 15 цифр
    {
        throw NoteException("В номере телефона должно быть от 5 до 15 цифр."); // ошибка
    }                                          // конец if
}                                              // конец функции

// Високосный год: делится на 4, но не на 100, либо делится на 400
bool Note::isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); // логическое выражение, возвращаем true/false
}                                              // конец функции

// Количество дней в месяце
int Note::daysInMonth(int month, int year)
{
    static const int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // дни по месяцам (для обычного года)
    if (month == 2 && isLeapYear(year))        // если февраль високосного года
    {
        return 29;                             // в нём 29 дней
    }                                          // конец if
    return days[month - 1];                    // иначе берём из таблицы (месяц 1 -> индекс 0)
}                                              // конец функции

// Проверка даты рождения
void Note::validateDate(int day, int month, int year)
{
    if (year < 1900 || year > 2100)            // разумные границы года
    {
        throw NoteException("Год должен быть в диапазоне 1900..2100."); // ошибка
    }                                          // конец if
    if (month < 1 || month > 12)               // месяц должен быть от 1 до 12
    {
        throw NoteException("Месяц должен быть в диапазоне 1..12."); // ошибка
    }                                          // конец if
    if (day < 1 || day > daysInMonth(month, year)) // день должен существовать в этом месяце
    {
        throw NoteException("Такого дня нет в указанном месяце."); // ошибка
    }                                          // конец if
}                                              // конец функции

// ====================== КОНСТРУКТОРЫ И ДЕСТРУКТОР ======================

// Конструктор без параметров: создаёт запись со значениями по умолчанию
Note::Note() : id(++objectCounter), surname("Не указано"), name("Не указано"), phone("00000"), birthday(new int[3])
{
    // id получает номер: сначала счётчик увеличивается на 1, потом значение копируется в id
    // new int[3] - выделяем в динамической памяти массив из трёх чисел
    birthday[0] = 1;                           // день по умолчанию
    birthday[1] = 1;                           // месяц по умолчанию
    birthday[2] = 2000;                        // год по умолчанию
    std::cout << "  [NOTE #" << id << "] Конструктор без параметров" << std::endl; // сообщение о вызове
}                                              // конец конструктора

// Конструктор с параметрами
Note::Note(const std::string& newSurname, const std::string& newName, const std::string& newPhone,
    int day, int month, int year)
    : id(++objectCounter), surname(newSurname), name(newName), phone(newPhone), birthday(nullptr)
{
    // Сначала проверяем данные. Память под birthday ещё НЕ выделена (nullptr),
    // поэтому если проверка бросит исключение, утечки памяти не будет.
    validateText(newSurname, "Фамилия");       // проверяем фамилию
    validateText(newName, "Имя");              // проверяем имя
    validatePhone(newPhone);                   // проверяем телефон
    validateDate(day, month, year);            // проверяем дату
    birthday = new int[3];                     // все данные корректны - выделяем динамический массив
    birthday[0] = day;                         // записываем день
    birthday[1] = month;                       // записываем месяц
    birthday[2] = year;                        // записываем год
    std::cout << "  [NOTE #" << id << "] Конструктор с параметрами (" << surname << " " << name << ")" << std::endl; // сообщение
}                                              // конец конструктора

// Конструктор копирования: создаёт НОВЫЙ объект - точную копию other
Note::Note(const Note& other)
    : id(++objectCounter), surname(other.surname), name(other.name), phone(other.phone), birthday(new int[3])
{
    // ВАЖНО: нельзя просто скопировать указатель (birthday = other.birthday), иначе два объекта
    // будут указывать на один массив и при удалении одного второй "сломается".
    // Поэтому выделяем СВОЙ массив и копируем в него значения ("глубокое копирование").
    for (int i = 0; i < 3; ++i)                // перебираем три элемента
    {
        birthday[i] = other.birthday[i];       // копируем значение
    }                                          // конец for
    std::cout << "  [NOTE #" << id << "] Конструктор копирования (копия объекта #" << other.id << ")" << std::endl; // сообщение
}                                              // конец конструктора

// Деструктор: освобождает динамическую память, которую занимал объект
Note::~Note()
{
    std::cout << "  [NOTE #" << id << "] Деструктор (" << surname << " " << name << ")" << std::endl; // сообщение
    delete[] birthday;                         // освобождаем массив, выделенный через new[] (для массивов - delete[])
}                                              // конец деструктора

// ====================== ОПЕРАТОРЫ ======================

// Оператор присваивания: a = b копирует содержимое b в уже существующий a
Note& Note::operator=(const Note& other)
{
    if (this != &other)                        // защита от присваивания самому себе (a = a)
    {
        surname = other.surname;               // копируем фамилию
        name = other.name;                     // копируем имя
        phone = other.phone;                   // копируем телефон
        for (int i = 0; i < 3; ++i)            // копируем дату
        {
            birthday[i] = other.birthday[i];   // свой массив уже есть, просто переписываем значения
        }                                      // конец for
    }                                          // конец if
    return *this;                              // возвращаем сам объект (чтобы работало a = b = c)
}                                              // конец оператора

// Оператор "меньше": запись A меньше B, если A родился раньше
bool Note::operator<(const Note& other) const
{
    if (birthday[2] != other.birthday[2])      // если годы разные
    {
        return birthday[2] < other.birthday[2]; // решает год
    }                                          // конец if
    if (birthday[1] != other.birthday[1])      // годы равны, но месяцы разные
    {
        return birthday[1] < other.birthday[1]; // решает месяц
    }                                          // конец if
    return birthday[0] < other.birthday[0];    // год и месяц равны - решает день
}                                              // конец оператора

// ====================== GET-ФУНКЦИИ ======================

std::string Note::getSurname() const { return surname; }   // вернуть фамилию
std::string Note::getName() const { return name; }         // вернуть имя
std::string Note::getPhone() const { return phone; }       // вернуть телефон
int Note::getDay() const { return birthday[0]; }           // вернуть день
int Note::getMonth() const { return birthday[1]; }         // вернуть месяц
int Note::getYear() const { return birthday[2]; }          // вернуть год

// ====================== SET-ФУНКЦИИ ======================

// Установить фамилию (с проверкой)
void Note::setSurname(const std::string& value)
{
    validateText(value, "Фамилия");            // если значение плохое - здесь вылетит исключение, и ниже мы не пойдём
    surname = value;                           // значение хорошее - сохраняем
}                                              // конец функции

// Установить имя (с проверкой)
void Note::setName(const std::string& value)
{
    validateText(value, "Имя");                // проверка
    name = value;                              // сохранение
}                                              // конец функции

// Установить телефон (с проверкой)
void Note::setPhone(const std::string& value)
{
    validatePhone(value);                      // проверка
    phone = value;                             // сохранение
}                                              // конец функции

// Установить дату рождения (с проверкой)
void Note::setBirthday(int day, int month, int year)
{
    validateDate(day, month, year);            // проверка
    birthday[0] = day;                         // сохраняем день
    birthday[1] = month;                       // сохраняем месяц
    birthday[2] = year;                        // сохраняем год
}                                              // конец функции

// ====================== ПОТОКОВЫЕ ОПЕРАТОРЫ ======================

// Вставка в поток (вывод): std::cout << note;
std::ostream& operator<<(std::ostream& out, const Note& note)
{
    out << note.surname << " " << note.name    // выводим фамилию и имя
        << " | тел.: " << note.phone           // выводим телефон
        << " | дата рождения: "                // подпись
        << std::setfill('0') << std::setw(2) << note.birthday[0] << "." // день в 2 символа, пустое место заполняем нулём
        << std::setw(2) << note.birthday[1] << "."                      // месяц в 2 символа
        << note.birthday[2]                    // год
        << std::setfill(' ');                  // возвращаем заполнитель по умолчанию (пробел)
    return out;                                // возвращаем поток, чтобы можно было писать цепочкой: cout << a << b
}                                              // конец оператора

// Извлечение из потока (ввод): std::cin >> note;
// Формат ввода в одну строку через пробел: Фамилия Имя Телефон День Месяц Год
std::istream& operator>>(std::istream& in, Note& note)
{
    std::string newSurname, newName, newPhone; // временные переменные для текста
    int day = 0, month = 0, year = 0;          // временные переменные для даты
    in >> newSurname >> newName >> newPhone >> day >> month >> year; // читаем все 6 значений из потока по порядку
    if (in.fail())                             // если поток "сломался" (например, вместо числа ввели буквы)
    {
        throw NoteException("Ошибка ввода: ожидалось Фамилия Имя Телефон День Месяц Год (день, месяц и год - числа).");
    }                                          // конец if
    Note::validateText(newSurname, "Фамилия"); // проверяем фамилию (friend-функции разрешён доступ к закрытым static-методам)
    Note::validateText(newName, "Имя");        // проверяем имя
    Note::validatePhone(newPhone);             // проверяем телефон
    Note::validateDate(day, month, year);      // проверяем дату
    note.surname = newSurname;                 // всё хорошо - записываем фамилию в объект
    note.name = newName;                       // имя
    note.phone = newPhone;                     // телефон
    note.birthday[0] = day;                    // день
    note.birthday[1] = month;                  // месяц
    note.birthday[2] = year;                   // год
    return in;                                 // возвращаем поток
}                                              // конец оператора
