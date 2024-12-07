Przy realizacji zadania do synchronizacji należy wykorzystać semafor
(tylko operacje p i v) – klasa Semaphore zdefiniowana w pliku monitor.h
znajdującym się na stronie przedmiotu. Zadanie należy wykonać z użyciem wątków
(korzystając ze zmiennych globalnych).

Bufor 9 elementowy FIFO. Dwóch konsumentów i dwóch producentów. Producent A
produkuje literę A Producent B produkuje literę B. Producent A może zapisywać
elementy do bufora jedynie gdy ich liczba jest mniejsza niż 5. Producent B może
zapisywać elementy do bufora jedynie gdy ich liczba jest większa  niż 3.