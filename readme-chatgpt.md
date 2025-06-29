# 🧮 Quine–McCluskey Algorithm in Rust

Реализация алгоритма Куайна–МакКласски на языке Rust для минимизации булевых функций, заданных в виде списка минтермов.

---

## 📌 Описание

Алгоритм Куайна–МакКласски используется для систематического и точного упрощения логических выражений. В отличие от карт Карно, он подходит для автоматизации и работы с функциями от большого числа переменных.

Реализация поддерживает:

- ✅ Поиск **прайм-импликантов**
- ✅ Построение **таблицы покрытия**
- ✅ Выделение **существенных импликантов**
- ✅ **Жадную минимизацию** оставшихся минтермов

---

## 🚀 Быстрый старт

### 🔧 Требования

- [Rust](https://www.rust-lang.org/tools/install) (1.60+)

### ⚙️ Установка и запуск

```bash
git clone https://github.com/your-username/quine-mccluskey-rust.git
cd quine-mccluskey-rust
cargo run
````

---

## 💡 Пример

Минимизируем булеву функцию:

```text
F(A, B, C, D) = Σm(0, 1, 2, 5, 6, 7, 8, 9, 10, 14)
```

Вывод:

```text
Минимизированная функция в виде импликантов:
00-0
0-10
-10-
1-10
```

Интерпретация (в логической форме):

```
F = !A & !B & D
  ∨ !A & C & !D
  ∨ B & !C & D
  ∨ A & C & !D
```

---

## 📁 Структура проекта

```
├── src/
│   └── main.rs       # Основной код с реализацией алгоритма
├── Cargo.toml        # Конфигурация проекта
└── README.md         # Документация
```

---

## 📚 Возможности для расширения

* [ ] Поддержка don’t care-условий
* [ ] Генерация символьных логических выражений (`A & !B`)
* [ ] Табличная минимизация методом Петриче
* [ ] Веб-интерфейс или CLI с вводом минтермов

---

## 🛠 Используемые технологии

* Язык программирования: **Rust**
* Сборка: `cargo`
* Алгоритмическая логика: классический **Quine–McCluskey** + greedy cover

---

## 📄 Лицензия

Проект распространяется под лицензией [MIT](LICENSE).

---

## 👨‍💻 Автор

Я и chatgpt
GitHub: [@berezhko](https://github.com/berezhko)

