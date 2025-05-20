#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <iomanip>
#include <random>
#include <cstring>

using namespace std;

typedef array<uint8_t, 16> Block;

// Параметры AES-128
const int Nb = 4; // Кол-во столбцов (32-битных слов) в состоянии State. Для AES всегда 4
const int Nk = 4; // Кол-во 32-битных слов в ключе. Для AES-128 — 4
const int Nr = 10;// Кол-во раундов для AES-128 — 10

// Таблица S-box (подстановка)
const uint8_t sbox[256] = {
    
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Таблица обратной подстановки (Inverse S-box)
const uint8_t rsbox[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

// Массив констант раунда Rcon
const uint32_t Rcon[11] = {
    0x00000000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1b000000, 0x36000000
};

// Быстрое умножение на x в поле GF(2^8)
uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x >> 7) * 0x1b);
}

// Умножение в поле GF(2^8)
uint8_t multiply(uint8_t x, uint8_t y) {
    uint8_t result = 0;
    for (int i = 0; i < 8; ++i) {
        if (y & 1) result ^= x;
        bool high_bit = (x & 0x80);
        x <<= 1;
        if (high_bit) x ^= 0x1b;
        y >>= 1;
    }
    return result;
}

// Печать состояния (State)
void print_state(const string &label, const Block &block) {
    cout << label << ": ";
    for (int i = 0; i < 16; ++i) {
        cout << hex << setw(2) << setfill('0') << (int)block[i] << " ";
    }
    cout << dec << endl;
}

// Преобразование 16 байт в матрицу 4x4
void bytes_to_matrix(const Block &in, uint8_t state[4][4]) {
    for (int i = 0; i < 16; ++i)
        state[i % 4][i / 4] = in[i];
}

// Преобразование матрицы 4x4 в 16 байт
Block matrix_to_bytes(const uint8_t state[4][4]) {
    Block out;
    for (int i = 0; i < 16; ++i)
        out[i] = state[i % 4][i / 4];
    return out;
}

// Расширение ключа (Key Expansion)
void key_expansion(const Block &key, vector<Block> &round_keys) {
    round_keys.resize(Nr + 1);
    uint32_t temp;
    uint32_t w[Nb * (Nr + 1)];

    // Копируем мастер-ключ в начало массива w
    for (int i = 0; i < Nk; ++i) {
        w[i] = (key[4 * i] << 24) | (key[4 * i + 1] << 16) | (key[4 * i + 2] << 8) | key[4 * i + 3];
    }

    // Генерируем раундовые ключи
    for (int i = Nk; i < Nb * (Nr + 1); ++i) {
        temp = w[i - 1];
        if (i % Nk == 0) {
            // Сдвиг слова (RotWord)
            temp = (temp << 8) | (temp >> 24);
            // Подстановка (SubWord)
            temp = (sbox[(temp >> 24) & 0xff] << 24) |
                   (sbox[(temp >> 16) & 0xff] << 16) |
                   (sbox[(temp >> 8) & 0xff] << 8) |
                   (sbox[(temp) & 0xff]);
            temp ^= Rcon[i / Nk];
        }
        w[i] = w[i - Nk] ^ temp;
    }

    // Преобразуем w[] в раундовые ключи
    for (int i = 0; i < Nr + 1; ++i) {
        Block round_key;
        for (int j = 0; j < 4; ++j) {
            round_key[4 * j]     = (w[i * 4 + j] >> 24) & 0xff;
            round_key[4 * j + 1] = (w[i * 4 + j] >> 16) & 0xff;
            round_key[4 * j + 2] = (w[i * 4 + j] >> 8) & 0xff;
            round_key[4 * j + 3] = w[i * 4 + j] & 0xff;
        }
        round_keys[i] = round_key;
    }
}

// Добавление раундового ключа (AddRoundKey)
void add_round_key(uint8_t state[4][4], const Block &round_key) {
    for (int i = 0; i < 16; ++i) {
        state[i % 4][i / 4] ^= round_key[i];
    }
}

// Подстановка байтов (SubBytes)
void sub_bytes(uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = sbox[state[i][j]];
}

// Обратная подстановка байтов (InvSubBytes)
void inv_sub_bytes(uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = rsbox[state[i][j]];
}

// Сдвиг строк (ShiftRows)
void shift_rows(uint8_t state[4][4]) {
    uint8_t tmp[4];
    // 1-я строка сдвигается влево на 1
    tmp[0] = state[1][0];
    tmp[1] = state[1][1];
    tmp[2] = state[1][2];
    tmp[3] = state[1][3];
    state[1][0] = tmp[1];
    state[1][1] = tmp[2];
    state[1][2] = tmp[3];
    state[1][3] = tmp[0];
    // 2-я строка сдвигается влево на 2
    tmp[0] = state[2][0];
    tmp[1] = state[2][1];
    tmp[2] = state[2][2];
    tmp[3] = state[2][3];
    state[2][0] = tmp[2];
    state[2][1] = tmp[3];
    state[2][2] = tmp[0];
    state[2][3] = tmp[1];
    // 3-я строка сдвигается влево на 3
    tmp[0] = state[3][0];
    tmp[1] = state[3][1];
    tmp[2] = state[3][2];
    tmp[3] = state[3][3];
    state[3][0] = tmp[3];
    state[3][1] = tmp[0];
    state[3][2] = tmp[1];
    state[3][3] = tmp[2];
}

// Обратный сдвиг строк (InvShiftRows)
void inv_shift_rows(uint8_t state[4][4]) {
    uint8_t tmp[4];
    // 1-я строка сдвигается вправо на 1
    tmp[0] = state[1][0];
    tmp[1] = state[1][1];
    tmp[2] = state[1][2];
    tmp[3] = state[1][3];
    state[1][0] = tmp[3];
    state[1][1] = tmp[0];
    state[1][2] = tmp[1];
    state[1][3] = tmp[2];
    // 2-я строка сдвигается вправо на 2
    tmp[0] = state[2][0];
    tmp[1] = state[2][1];
    tmp[2] = state[2][2];
    tmp[3] = state[2][3];
    state[2][0] = tmp[2];
    state[2][1] = tmp[3];
    state[2][2] = tmp[0];
    state[2][3] = tmp[1];
    // 3-я строка сдвигается вправо на 3
    tmp[0] = state[3][0];
    tmp[1] = state[3][1];
    tmp[2] = state[3][2];
    tmp[3] = state[3][3];
    state[3][0] = tmp[1];
    state[3][1] = tmp[2];
    state[3][2] = tmp[3];
    state[3][3] = tmp[0];
}

// Перемешивание столбцов (MixColumns)
void mix_columns(uint8_t state[4][4]) {
    uint8_t tmp[4];
    for (int j = 0; j < 4; ++j) {
        tmp[0] = multiply(0x02, state[0][j]) ^ multiply(0x03, state[1][j]) ^ state[2][j] ^ state[3][j];
        tmp[1] = state[0][j] ^ multiply(0x02, state[1][j]) ^ multiply(0x03, state[2][j]) ^ state[3][j];
        tmp[2] = state[0][j] ^ state[1][j] ^ multiply(0x02, state[2][j]) ^ multiply(0x03, state[3][j]);
        tmp[3] = multiply(0x03, state[0][j]) ^ state[1][j] ^ state[2][j] ^ multiply(0x02, state[3][j]);
        for (int i = 0; i < 4; ++i)
            state[i][j] = tmp[i];
    }
}

// Обратное перемешивание столбцов (InvMixColumns)
void inv_mix_columns(uint8_t state[4][4]) {
    uint8_t tmp[4];
    for (int j = 0; j < 4; ++j) {
        tmp[0] = multiply(0x0e, state[0][j]) ^ multiply(0x0b, state[1][j]) ^ multiply(0x0d, state[2][j]) ^ multiply(0x09, state[3][j]);
        tmp[1] = multiply(0x09, state[0][j]) ^ multiply(0x0e, state[1][j]) ^ multiply(0x0b, state[2][j]) ^ multiply(0x0d, state[3][j]);
        tmp[2] = multiply(0x0d, state[0][j]) ^ multiply(0x09, state[1][j]) ^ multiply(0x0e, state[2][j]) ^ multiply(0x0b, state[3][j]);
        tmp[3] = multiply(0x0b, state[0][j]) ^ multiply(0x0d, state[1][j]) ^ multiply(0x09, state[2][j]) ^ multiply(0x0e, state[3][j]);
        for (int i = 0; i < 4; ++i)
            state[i][j] = tmp[i];
    }
}

// Шифрование одного блока AES
Block aes_encrypt_block(const Block &input, const vector<Block> &round_keys, bool verbose = true) {
    uint8_t state[4][4];
    bytes_to_matrix(input, state);

    if (verbose) print_state("Начальное состояние", input);

    // Добавление раундового ключа
    add_round_key(state, round_keys[0]);
    if (verbose) print_state("После AddRoundKey(0)", matrix_to_bytes(state));

    for (int round = 1; round < Nr; ++round) {
        sub_bytes(state);
        if (verbose) print_state("После SubBytes("+to_string(round)+")", matrix_to_bytes(state));
        shift_rows(state);
        if (verbose) print_state("После ShiftRows("+to_string(round)+")", matrix_to_bytes(state));
        mix_columns(state);
        if (verbose) print_state("После MixColumns("+to_string(round)+")", matrix_to_bytes(state));
        add_round_key(state, round_keys[round]);
        if (verbose) print_state("После AddRoundKey("+to_string(round)+")", matrix_to_bytes(state));
    }

    // Финальный раунд (без MixColumns)
    sub_bytes(state);
    if (verbose) print_state("После SubBytes(Финал)", matrix_to_bytes(state));
    shift_rows(state);
    if (verbose) print_state("После ShiftRows(Финал)", matrix_to_bytes(state));
    add_round_key(state, round_keys[Nr]);
    if (verbose) print_state("После AddRoundKey(Финал)", matrix_to_bytes(state));

    return matrix_to_bytes(state);
}

// Расшифрование одного блока AES
Block aes_decrypt_block(const Block &input, const vector<Block> &round_keys, bool verbose = true) {
    uint8_t state[4][4];
    bytes_to_matrix(input, state);

    if (verbose) print_state("Начальное состояние (дешифр.)", input);

    add_round_key(state, round_keys[Nr]);
    if (verbose) print_state("После AddRoundKey(Финал)", matrix_to_bytes(state));
    for (int round = Nr - 1; round >= 1; --round) {
        inv_shift_rows(state);
        if (verbose) print_state("После InvShiftRows("+to_string(round)+")", matrix_to_bytes(state));
        inv_sub_bytes(state);
        if (verbose) print_state("После InvSubBytes("+to_string(round)+")", matrix_to_bytes(state));
        add_round_key(state, round_keys[round]);
        if (verbose) print_state("После AddRoundKey("+to_string(round)+")", matrix_to_bytes(state));
        inv_mix_columns(state);
        if (verbose) print_state("После InvMixColumns("+to_string(round)+")", matrix_to_bytes(state));
    }
    inv_shift_rows(state);
    if (verbose) print_state("После InvShiftRows(0)", matrix_to_bytes(state));
    inv_sub_bytes(state);
    if (verbose) print_state("После InvSubBytes(0)", matrix_to_bytes(state));
    add_round_key(state, round_keys[0]);
    if (verbose) print_state("После AddRoundKey(0)", matrix_to_bytes(state));

    return matrix_to_bytes(state);
}

// PKCS#7 дополнение (padding)
void pkcs7_pad(vector<uint8_t> &data) {
    size_t pad_len = 16 - (data.size() % 16);
    for (size_t i = 0; i < pad_len; ++i)
        data.push_back(static_cast<uint8_t>(pad_len));
}

// Удаление PKCS#7 дополнения (padding)
void pkcs7_unpad(vector<uint8_t> &data) {
    if (data.empty()) return;
    uint8_t pad_len = data.back();
    if (pad_len > 16) return;
    data.resize(data.size() - pad_len);
}

// Шифрование в режиме CBC
vector<uint8_t> aes_cbc_encrypt(const vector<uint8_t> &plaintext, const Block &key, const Block &iv, vector<Block> &round_keys, bool verbose = true) {
    vector<uint8_t> data = plaintext;
    pkcs7_pad(data);
    size_t blocks = data.size() / 16;

    vector<uint8_t> ciphertext(data.size());
    Block prev = iv;

    for (size_t i = 0; i < blocks; ++i) {
        Block block;
        for (int j = 0; j < 16; ++j)
            block[j] = data[i * 16 + j] ^ prev[j];

        Block enc = aes_encrypt_block(block, round_keys, verbose);

        for (int j = 0; j < 16; ++j) {
            ciphertext[i * 16 + j] = enc[j];
            prev[j] = enc[j];
        }
    }
    return ciphertext;
}

// Дешифрование в режиме CBC
vector<uint8_t> aes_cbc_decrypt(const vector<uint8_t> &ciphertext, const Block &key, const Block &iv, vector<Block> &round_keys, bool verbose = true) {
    size_t blocks = ciphertext.size() / 16;
    vector<uint8_t> plaintext(ciphertext.size());
    Block prev = iv;

    for (size_t i = 0; i < blocks; ++i) {
        Block block;
        for (int j = 0; j < 16; ++j)
            block[j] = ciphertext[i * 16 + j];

        Block dec = aes_decrypt_block(block, round_keys, verbose);

        for (int j = 0; j < 16; ++j) {
            plaintext[i * 16 + j] = dec[j] ^ prev[j];
            prev[j] = block[j];
        }
    }
    pkcs7_unpad(plaintext);
    return plaintext;
}

// Генерация случайного 16-байтового блока (ключ/IV)
Block random_block() {
    Block blk;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < 16; ++i)
        blk[i] = dis(gen);
    return blk;
}

// Запись ключа в файл
void save_key_to_file(const Block &key, const string &filename) {
    ofstream ofs(filename);
    for (int i = 0; i < 16; ++i) {
        ofs << hex << setw(2) << setfill('0') << (int)key[i];
        if (i != 15) ofs << " "; // Добавляем пробел между байтами
    }
    ofs << endl;
    ofs.close();
}

// Печать блока (ключ, IV, раундовые ключи)
void print_block(const string &label, const Block &block) {
    cout << label << ": ";
    for (int i = 0; i < 16; ++i)
        cout << hex << setw(2) << setfill('0') << (int)block[i] << " ";
    cout << dec << endl;
}

int main() {
    setlocale(LC_ALL, "");

    // Многострочный ввод текста с консоли
    cout << "Введите текст для шифрования (русский или английский, завершите ввод пустой строкой или Ctrl+D):\n";
    string line, input_text;
    while (getline(cin, line)) {
        if (line.empty()) break; // Пустая строка завершает ввод
        input_text += line + '\n'; // Сохраняем все строки с переносами
    }
    if (!input_text.empty() && input_text.back() == '\n')
        input_text.pop_back(); // Удаляем последний перевод строки, если нужно

    // Переводим текст в байты (UTF-8)
    vector<uint8_t> plaintext(input_text.begin(), input_text.end());
    cout << "Длина исходного текста в байтах: " << plaintext.size() << endl;

    // Генерируем случайный ключ и IV
    Block key = random_block();
    Block iv = random_block();

    // Печать ключа и IV
    print_block("Ключ", key);
    print_block("Вектор инициализации (IV)", iv);

    // Запись ключа в файл
    save_key_to_file(key, "/home/suz/lab7/aes_key.txt");
    cout << "Ключ сохранён в файл aes_key.bin" << endl;

    // Расширение ключа (выводим все раундовые ключи)
    vector<Block> round_keys;
    key_expansion(key, round_keys);
    for (int i = 0; i < round_keys.size(); ++i) {
        print_block("Раундовый ключ " + to_string(i), round_keys[i]);
    }

    // Шифрование текста
    cout << "\n--- Шифрование (AES128-CBC) ---\n";
    vector<uint8_t> ciphertext = aes_cbc_encrypt(plaintext, key, iv, round_keys, true);

    cout << "Зашифрованный текст (hex): ";
    for (uint8_t c : ciphertext)
        cout << hex << setw(2) << setfill('0') << (int)c << " ";
    cout << dec << endl;

    // Дешифрование текста
    cout << "\n--- Дешифрование (AES128-CBC) ---\n";
    vector<uint8_t> decrypted = aes_cbc_decrypt(ciphertext, key, iv, round_keys, true);

    // Вывод расшифрованного текста
    string decrypted_text(decrypted.begin(), decrypted.end());
    cout << "Расшифрованный текст: " << endl << decrypted_text << endl;

    return 0;
}
