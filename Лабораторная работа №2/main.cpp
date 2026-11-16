#define NOMINMAX                               // запрещаем windows.h определять макросы min/max, они мешают стандартной библиотеке
#include <iostream>                            // cin / cout
#include <string>                              // std::string, std::getline, std::to_string
#include <limits>                              // numeric_limits - для очистки строки ввода
#include "Note.h"                              // класс записи
#include "NoteBook.h"                          // класс записной книжки
#include "NoteException.h"                     // наше исключение
#include "TextTask.h"                          // задание 2 (файловые и строковые потоки)
#ifdef _WIN32                                  // этот блок компилируется только в Windows
#include <windows.h>                           // нужен для переключения кодировки консоли
#endif                                         // конец блока

// Выбрасывает из потока ввода всё, что осталось до конца строки (в частности символ Enter)
void clearInputLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // пропускаем символы до перевода строки
}                                              // конец функции

// Выводит подсказку и читает целое число. Если ввели не число - бросает исключение.
int readInt(const std::string& prompt)
{
    std::cout << prompt;                       // показываем подсказку
    int value = 0;                             // сюда запишем число
    if (!(std::cin >> value))                  // если чтение не удалось (ввели буквы или ввод закончился)
    {
        if (std::cin.eof())                    // ввод закончился совсем (например, нажали Ctrl+Z в Windows)
        {
            throw NoteException("Ввод данных завершён."); // сообщаем об этом; главный цикл увидит конец ввода и выйдет
        }                                      // конец if
        std::cin.clear();                      // снимаем состояние ошибки с потока
        clearInputLine();                      // выбрасываем неверный ввод
        throw NoteException("Нужно ввести целое число."); // сообщаем об ошибке
    }                                          // конец if
    clearInputLine();                          // выбрасываем остаток строки (Enter), чтобы он не мешал следующему вводу
    return value;                              // возвращаем число
}                                              // конец функции

// Читает слово (строку без пробелов) с клавиатуры
std::string readWord(const std::string& prompt)
{
    std::cout << prompt;                       // подсказка
    std::string word;                          // сюда запишем слово
    std::cin >> word;                          // читаем до первого пробела
    clearInputLine();                          // убираем остаток строки
    return word;                               // возвращаем слово
}                                              // конец функции

// Ввод целой записи с клавиатуры через перегруженный оператор >>
void readNote(Note& note)
{
    std::cout << "Введите через пробел: Фамилия Имя Телефон День Месяц Год" << std::endl; // формат ввода
    std::cout << "Пример: Иванов Иван 89991234567 15 3 2001" << std::endl;                // пример
    std::cout << "> ";                         // приглашение к вводу
    std::cin >> note;                          // вызывается наш operator>> для Note
    clearInputLine();                          // убираем остаток строки
}                                              // конец функции

// Печать меню
void printMenu()
{
    std::cout << "\n========== ЗАПИСНАЯ КНИЖКА ==========" << std::endl; // заголовок
    std::cout << "1. Показать все записи" << std::endl;                // пункт 1
    std::cout << "2. Добавить запись в конец" << std::endl;            // пункт 2
    std::cout << "3. Вставить запись на выбранную позицию" << std::endl; // пункт 3
    std::cout << "4. Редактировать запись" << std::endl;               // пункт 4
    std::cout << "5. Удалить запись" << std::endl;                     // пункт 5
    std::cout << "6. Упорядочить по датам рождения" << std::endl;      // пункт 6
    std::cout << "7. Найти человека по номеру телефона" << std::endl;  // пункт 7
    std::cout << "8. Задание 2: предложения с тире из файла" << std::endl; // пункт 8
    std::cout << "0. Выход" << std::endl;                              // пункт 0
}                                              // конец функции

// Пункт 2: добавление в конец
void addToEnd(NoteBook& book)
{
    Note temp;                                 // временная запись (создаётся конструктором без параметров)
    readNote(temp);                            // вводим данные с клавиатуры
    book.insertAt(book.getCount(), temp);      // вставляем КОПИЮ в конец (позиция = количеству записей)
    std::cout << "Запись добавлена." << std::endl; // сообщение
}                                              // temp здесь уничтожается - сработает деструктор

// Пункт 3: вставка на выбранную позицию
void insertAtPosition(NoteBook& book)
{
    int pos = readInt("Позиция для вставки (1.." + std::to_string(book.getCount() + 1) + "): "); // пользователь считает с 1
    Note temp;                                 // временная запись
    readNote(temp);                            // ввод данных
    book.insertAt(pos - 1, temp);              // внутри книжки счёт с 0, поэтому вычитаем 1
    std::cout << "Запись вставлена." << std::endl; // сообщение
}                                              // конец функции

// Пункт 4: редактирование любой записи
void editNote(NoteBook& book)
{
    if (book.getCount() == 0)                  // если записей нет
    {
        throw NoteException("Записная книжка пуста, редактировать нечего."); // ошибка
    }                                          // конец if
    int pos = readInt("Номер записи для редактирования (1.." + std::to_string(book.getCount()) + "): "); // выбор записи
    Note& target = book[pos - 1];              // ссылка на выбранную запись (оператор [] сам проверит номер)
    std::cout << "Текущая запись: " << target << std::endl; // показываем текущие данные
    std::cout << "Что изменить? 1-Фамилия 2-Имя 3-Телефон 4-Дата рождения 5-Вся запись" << std::endl; // варианты
    int field = readInt("Ваш выбор: ");        // выбор поля

    switch (field)                             // выбираем действие по номеру
    {
    case 1:                                    // изменить фамилию
        target.setSurname(readWord("Новая фамилия: ")); // set-функция проверит значение
        break;                                 // выход из switch
    case 2:                                    // изменить имя
        target.setName(readWord("Новое имя: "));       // set-функция
        break;                                 // выход из switch
    case 3:                                    // изменить телефон
        target.setPhone(readWord("Новый телефон: "));  // set-функция
        break;                                 // выход из switch
    case 4:                                    // изменить дату рождения
    {
        int d = readInt("День: ");             // читаем день
        int m = readInt("Месяц: ");            // читаем месяц
        int y = readInt("Год: ");              // читаем год
        target.setBirthday(d, m, y);           // set-функция проверит дату
        break;                                 // выход из switch
    }
    case 5:                                    // изменить всё сразу
    {
        Note temp;                             // временная запись
        readNote(temp);                        // ввод новых данных
        target = temp;                         // оператор присваивания копирует данные в редактируемую запись
        break;                                 // выход из switch
    }
    default:                                   // номер не из списка
        throw NoteException("Такого пункта нет.");    // ошибка
    }                                          // конец switch
    std::cout << "Запись изменена: " << target << std::endl; // показываем результат
}                                              // конец функции

// Пункт 5: удаление записи
void removeNote(NoteBook& book)
{
    if (book.getCount() == 0)                  // если нечего удалять
    {
        throw NoteException("Записная книжка пуста, удалять нечего."); // ошибка
    }                                          // конец if
    int pos = readInt("Номер записи для удаления (1.." + std::to_string(book.getCount()) + "): "); // выбор записи
    book.removeAt(pos - 1);                    // удаляем (проверка номера внутри)
    std::cout << "Запись удалена." << std::endl; // сообщение
}                                              // конец функции

// Пункт 7: поиск по телефону
void searchByPhone(NoteBook& book)
{
    std::string phone = readWord("Введите номер телефона: "); // читаем искомый номер
    bool found = false;                        // нашли ли хоть одного
    int index = book.findByPhone(phone, 0);    // ищем первое совпадение с начала
    while (index != -1)                        // пока совпадения находятся
    {
        Note& person = book[index];            // получаем запись по индексу (перегруженный оператор [])
        std::cout << "Найдено: " << person << std::endl;   // выводим информацию о человеке
        found = true;                          // отмечаем, что нашли
        index = book.findByPhone(phone, index + 1); // ищем следующее совпадение дальше по списку
    }                                          // конец while
    if (!found)                                // если не нашли никого
    {
        std::cout << "Человека с таким номером телефона нет." << std::endl; // сообщение по условию задания
    }                                          // конец if
}                                              // конец функции

// Пункт 8: задание 2
void runTextTask()
{
    std::cout << "Введите путь к текстовому файлу (например, text.txt): "; // подсказка
    std::string fileName;                      // имя файла
    std::getline(std::cin, fileName);          // читаем всю строку (в пути могут быть пробелы)
    printDashSentences(fileName);              // запускаем обработку файла
}                                              // конец функции

int main()
{
#ifdef _WIN32                                  // только для Windows
    SetConsoleOutputCP(CP_UTF8);               // консоль будет выводить текст в UTF-8
    SetConsoleCP(CP_UTF8);                     // и принимать ввод в UTF-8 (русские буквы)
#endif

    NoteBook book;                             // создаём записную книжку (сработает конструктор без параметров)
    bool running = true;                       // флаг: продолжать ли работу программы

    while (running)                            // главный цикл меню
    {
        try                                    // в блоке try выполняем код, который может бросить исключение
        {
            printMenu();                       // показываем меню
            int choice = readInt("Ваш выбор: "); // читаем пункт меню
            switch (choice)                    // выполняем выбранное действие
            {
            case 1: std::cout << book; break;  // показать все записи (оператор <<)
            case 2: addToEnd(book); break;     // добавить в конец
            case 3: insertAtPosition(book); break; // вставить на позицию
            case 4: editNote(book); break;     // редактировать
            case 5: removeNote(book); break;   // удалить
            case 6:                            // сортировка
                book.sortByBirthday();         // упорядочиваем по датам рождения
                std::cout << "Записи упорядочены по датам рождения." << std::endl; // сообщение
                break;                         // выход из switch
            case 7: searchByPhone(book); break; // поиск по телефону
            case 8: runTextTask(); break;      // задание 2
            case 0: running = false; break;    // выход: цикл while закончится
            default:                           // неизвестный пункт меню
                std::cout << "Такого пункта меню нет." << std::endl; // сообщение
            }                                  // конец switch
        }                                      // конец try
        catch (const NoteException& e)         // ловим наши исключения (по ссылке, чтобы не копировать)
        {
            std::cout << "ОШИБКА: " << e.what() << std::endl; // выводим пояснение
            if (std::cin.eof())                // если ввод закончился совсем
            {
                running = false;               // продолжать бессмысленно - выходим из главного цикла
            }                                  // конец if
            else if (std::cin.fail())          // иначе, если поток ввода остался в состоянии ошибки
            {
                std::cin.clear();              // снимаем ошибку
                clearInputLine();              // выбрасываем неверный ввод
            }                                  // конец if
        }                                      // конец catch
        catch (const std::exception& e)        // ловим любые другие стандартные исключения
        {
            std::cout << "Непредвиденная ошибка: " << e.what() << std::endl; // выводим текст
        }                                      // конец catch
    }                                          // конец while

    std::cout << "Завершение работы..." << std::endl; // сообщение перед выходом
    return 0;                                  // книжка уничтожится здесь: сработают деструкторы книжки и всех записей
}                                              // конец main