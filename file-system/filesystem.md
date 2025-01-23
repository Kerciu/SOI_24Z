Jasne, oto sprawozdanie sformatowane tak, aby kod C++ i pseudokod były czytelnie przedstawione:

# System Plików

## Systemy Operacyjne

### Kacper Górski

### 331379

## Opis koncepcji systemu plików:

### System plików z alokacją łańcuchową

<img title="a title" alt="Alt text" src="/images/image.png">

### Założenia Projektowe
- Alokacja łańcuchowa z wykorzystaniem tabeli FAT.
- Rozmiar 2048 bajtów.
- Optymalizacja wykorzystania pamięci
- Odporność na awarię w trakcie operacji krytycznych.
- Bez dostępu współbieżnego.
- Dopuszczalne proste czynności naprawcze (bezpieczeństwo zapewnione głównie przez buforowanie).

---

### Założenia Szczegółowe
- Struktura jednokatalogowa
- 64 bloki
- Rozmiar bloku: 32 bajty.
- Maksymalnie 16 deskryptorów plików.
- Blok kontrolny zapewniający transakcyjność operacji krytycznych.
- Nazwa pliku zawierająca maksymalnie 10 znaków.
- Po włączeniu systemu plików dane są buforowane i sprawdzane jest, czy żadna operacja nie zostałą przerwana w trakcie - w tym wypadku wywoływana jest funkcja naprawcza
---

#### Pola w pamięci karty (persistent storage):

- **FAT (File Allocation Table)**
  ```cpp
  int16_t fat[NUM_BLOCKS];
  ```
- **Tablica deskryptorów plików** – tablica struktur przedstawiających deskryptor pliku posiadająca odpowiednie pola:

  - nazwa
  - blok, z którego zaczyna się łańcuch w FAT
  - rozmiar

  ```cpp
  struct FileDescriptor {
      std::string name;
      int16_t starting_block;
      int16_t size;
  };

  FileDescriptor fd_table[NUM_FILES];
  ```

- **Logi transakcji** – struktura z polami:

  - flaga, czy wykonywanie się zakończyło
  - indeks ostatnio przetwarzanego pliku
  - nowy rozmiar pliku
  - ostatni ważny blok, na którym operowaliśmy
  - pierwszy nowy blok do operacji

  ```cpp
  struct TransactionLog {
      bool in_progress;
      int16_t file_idx;
      int16_t new_size;
      int16_t last_valid_block;
      int16_t first_new_block;
  };

  TransactionLog transaction_log;
  ```

- **Licznik plików w systemie**
  ```cpp
  int16_t file_count;
  ```

#### Pola w pamięci operacyjnej (RAM):

- Buforowana tablica FAT, tablica deskryptorów plików, licznik plików oraz logi transakcji pozwalające na szybki dostęp do ich zawartości
- Tablica zawierająca strukturę opisującą otwarty plik z polami:
  - indeks otwartego pliku
  - przesunięcie bajtów, które zostały odczytane z tego otwartego pliku
  ```cpp
  struct OpenedFile {
      int16_t idx;
      int16_t offset;
  };
  OpenedFile open_file_fd_table[NUM_OPENED_FILES];
  ```
- Licznik aktualnie otwartych plików
  ```cpp
  int16_t opened_file_count;
  ```

### Wyliczona procentowo efektywność wykorzystania pamięci:

| **Element**                                     | **Rozmiar**                                               |
| ----------------------------------------------- | --------------------------------------------------------- |
| char memory[MEMORY_SIZE]                        | MEMORY_SIZE = 2048                                        |
| int16_t fat[NUM_BLOCKS]                         | NUM_BLOCKS = 64, Rozmiar FAT = 64 \* 2 = 128 bajtów       |
| FileDescriptor fd_table[NUM_FILES]              | NUM_FILES = 16, FileDescriptor = ~16 × 16 = 256 bajtów    |
| OpenedFile open_file_fd_table[NUM_OPENED_FILES] | NUM_OPENED_FILES = 8, OpenedFile ~ 4 bajty × 8 = 32 bajty |
| TransactionLog transaction_log                  | 1 + 1 + 1 = 3                                             |

**Całkowita pamięć:**

- Rozmiar danych użytkownika (tablica memory) = 2048 bajtów
- Całkowita pamięć = rozmiar danych użytkownika + 128 + 256 + 32 + 3 = 2467 bajtów

**Wyliczona procentowo efektywność wykorzystania pamięci:**

- \( \frac{2048}{2467} \times 100\% \approx 82,68\% \)

---

### Operacje możliwe do wykonania na systemie plików (pseudokody):

#### **create**

```plaintext
if size > MEMORY_SIZE or size <= 0:
    return false (invalid size)

if file_count >= NUM_FILES:
    return false (no space for new files)

if file with name already exists in fd_table:
    return false (duplicate file)

blocks_needed = ceil(size / BLOCK_SIZE)
if count_free_blocks(fat) < blocks_needed:
    return false (not enough free space)

transaction_log = {in_progress = true, file_idx = file_count, last_block = -1}

Allocate blocks in FAT:
    start_block = first free block
    current_block = start_block
    for i from 1 to blocks_needed:
        mark current_block as used in FAT
        transaction_log.last_block = current_block
        find next free block
        link current_block to next_block in FAT

Add new file to fd_table:
    fd_table[file_count] = {name, start_block, size}
    Increment file_count

transaction_log.in_progress = false

Return true (file created successfully)
```

#### **delete**

```plaintext
Find file in fd_table by name:
    if not found:
        return false (file does not exist)

transaction_log = {
    in_progress = true, file_idx = this_file_idx,
    last_block = fd_table[file_idx].starting_block
}

Free blocks in FAT:
    start_block = file.starting_block
    while start_block != END_OF_CHAIN:
        next_block = get_next_block_from_FAT(start_block)
        mark start_block as FREE_BLOCK
        start_block = next_block

Remove file from fd_table:
    shift remaining files in fd_table to fill gap
    Decrement file_count

Close all opened instances of file:
    for each entry in open_file_fd_table:
        if entry.idx == index_in_fd_table:
            entry = {NO_OPENED_FILE, 0}
            Decrement opened_file_count

transaction_log.in_progress = false

Return true (file deleted successfully)
```

#### **repair**

```plaintext
if !transaction_log.in_progress
    return (nothing to be done)

restore original file size
clear new allocated blocks

if transaction_log.last_valid_block != -1
    restore end of chain for the last valid block

clear transaction log
```

- **open**

```plaintext
if opened_file_count >= NUM_FILES:
    return false (too many files opened)

Find file in fd_table by name:
    if not found:
        return false (file does not exist)

Find free slot in open_file_fd_table:
    if none:
        return false (no space for opened files)

transaction_log = {in_progress = true, file_idx = file_idx, last_block = -1}

Add file to open_file_fd_table:
    open_file_fd_table[free_slot] = {index_in_fd_table, offset=0}
    Increment opened_file_count

transaction_log.in_progress = false;

Return true (file opened successfully)

```

- **close**

```plaintext
if file_index < 0 or file_index >= NUM_FILES:
    return false (invalid index)

if open_file_fd_table[file_index].idx == NO_OPENED_FILE:
    return false (file not open)

transaction_log = {
    in_progress = true, file_idx = file_index,
    last_block = open_file_fd_table[file_index].offset
}

Close file:
    open_file_fd_table[file_index] = {NO_OPENED_FILE, 0}
    Decrement opened_file_count

transaction_log.in_progress = false;

Return true (file closed successfully)

```

- **read**

```plaintext
if file_index < 0 or file_index >= NUM_FILES:
    return false (invalid index)

if open_file_fd_table[file_index].idx == NO_OPENED_FILE:
    return false (file not open)

Find file in fd_table using open_file_fd_table[file_index].idx:
    if offset + size > file.size:
        size = file.size - offset (adjust size to avoid overflow)

Read data:
    start_block = file.starting_block
    current_offset = offset
    bytes_to_read = size
    while bytes_to_read > 0:
        calculate block and offset within block
        copy data from memory to buffer
        update bytes_to_read and current_offset

Update offset in open_file_fd_table:
    open_file_fd_table[file_index].offset += size

Return true (data read successfully)

```

- **write**

```plaintext
if file_index < 0 or file_index >= NUM_FILES:
    return false (invalid index)

if open_file_fd_table[file_index].idx == NO_OPENED_FILE:
    return false (file not open)

Find file in fd_table using open_file_fd_table[file_index].idx:
    if offset + size > file.size:
        new_size = offset + size
        extendChain
        adjust new file size

Write data:
    start_block = file.starting_block
    current_offset = offset
    bytes_to_write = size
    while bytes_to_write > 0:
        calculate block and offset within block
        copy data from buffer to memory
        update bytes_to_write and current_offset

Update offset in open_file_fd_table:
    open_file_fd_table[file_index].offset += size

Return true (data written successfully)

```

- **delete**

```plaintext
Find file in fd_table by name:
    if not found:
        return false (file does not exist)

transaction_log = {
    in_progress = true, file_idx = this_file_idx,
    last_block = fd_table[file_idx].starting_block
}

Free blocks in FAT:
    start_block = file.starting_block
    while start_block != END_OF_CHAIN:
        next_block = get_next_block_from_FAT(start_block)
        mark start_block as FREE_BLOCK
        start_block = next_block

Remove file from fd_table:
    shift remaining files in fd_table to fill gap
    Decrement file_count

Close all opened instances of file:
    for each entry in open_file_fd_table:
        if entry.idx == index_in_fd_table:
            entry = {NO_OPENED_FILE, 0}
            Decrement opened_file_count

transaction_log.in_progress = false;

Return true (file deleted successfully)

```

- **repair** - podstawowa funkcja naprawcza

```plaintext
if !transaction_log.in_progress
    return (nothing to be done)

restore original file size

clear new allocated blocks

transaction_log.last_valid_block != -1
    restore end of chain for the last valid block

clear transaction log;

```
