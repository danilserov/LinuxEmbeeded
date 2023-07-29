# LinuxEmbeeded
COM-UDP proxy

1) Создать репозиторий на GitHub или аналоге
2) Добавить в репозиторий этот файл и запушить его
3) Разработать программу:
Консольное приложение, которое открывает COM-порт(его имя передаётся первым параметром)
и UDP порт(его имя передаётся вторым параметром).

Разработать класс Message, который в конструкторе принимает строку и имеет метод encrypt.
Метод encrypt возвращает массив байт, который содержит заголовок “HELLO” и зашифрованную строку:
Код символа каждой буквы или цифры исходной строки увеличен на 1.

Из COM-порта в цикле необходимо считать 10 строк.
Окончанием строки считается приём символа перевода строки или если символ не был введен в течение 5 секунд.
Пустые строки не обрабатываются.

После считывания строки, формируется шифрованное сообщение и передаётся в UDP порт броадкастом, затем читается следующая строка.

4) Разработку вести на языке С++, стандарт не ниже С++14
5) Для сборки проекта использовать CMake
6) Для работы с сетью и СОM-портом использовать кроссплатформенные библиотеки(если есть опыт)
7) По ходу работы делать коммиты с осознанными комментариями.
8) Запушить разработанную программу в репозиторий
9) Прислать нам ссылку на этот репозиторий, он должен быть открыт для скачивания без авторизации

