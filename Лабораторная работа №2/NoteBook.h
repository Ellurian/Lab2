#pragma once                                   // защита от повторного подключения
#include <string>                              // std::string для поиска по телефону
#include <iostream>                            // потоки для перегрузки <<
#include "Note.h"                              // нам нужен класс Note: книжка СОСТОИТ из записей (это композиция)

// Класс NoteBook - записная книжка: хранит неизвестное заранее количество записей Note.
// STL-контейнеры (vector и т.п.) использовать нельзя, поэтому массив делаем сами на указателях.
class NoteBook
{
private:
    Note** items;                              // указатель на динамический массив УКАЗАТЕЛЕЙ на записи (Note*)
    int count;                                 // сколько записей хранится сейчас
    int capacity;                              // на сколько записей выделено место в массиве

    void grow();                               // увеличить массив вдвое, когда место закончилось
    void checkIndex(int index) const;          // проверить, что запись с таким индексом существует (иначе исключение)

public:
    NoteBook();                                // конструктор без параметров
    NoteBook(int startCapacity);               // конструктор с параметром (начальная вместимость)
    NoteBook(const NoteBook& other);           // конструктор копирования
    ~NoteBook();                               // деструктор
    NoteBook& operator=(const NoteBook&) = delete; // присваивание книжек запрещаем (= delete), чтобы не писать лишний код

    int getCount() const;                      // get-функция: сколько записей
    void insertAt(int index, const Note& note);// вставить копию записи на позицию index (0..count)
    void removeAt(int index);                  // удалить запись с позиции index
    Note& operator[](int index);               // перегруженный []: book[i] даёт доступ к i-й записи (можно менять)
    void sortByBirthday();                     // упорядочить по возрастанию даты рождения
    int findByPhone(const std::string& phone, int startIndex) const; // найти индекс записи с телефоном, начиная с startIndex (или -1)

    friend std::ostream& operator<<(std::ostream& out, const NoteBook& book); // вывод всей книжки
};
