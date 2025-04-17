# 🕹️ Terminal Tetris (Inspired by Electronika 60)

A retro-style **Tetris game for the terminal**, built in C, with colored blocks, funky borders, and a nostalgic nod to the **Electronika 60** — the machine that ran the original Tetris.

![Screenshot from 2025-04-16 17-10-14](https://github.com/user-attachments/assets/62b9837f-570c-4d58-b209-3e61d5b47641)

---

## 📦 Features

- 🎮 Classic Tetris mechanics (rotate, move, drop)
- 🎨 Individual block colors by shape
- 🖼️ Centered playfield with custom ASCII borders
- 🧠 Written in pure C, no external libraries (just termios & stdlib)
- 💾 Runs on any terminal — Linux, macOS, WSL (Windows Subsystem for Linux)

---

## 🚀 How to Run

### 1. Compile

```bash
gcc -o tetris tetris.c
```

### 2. Play

```bash
./tetris
```

---

## 🎮 Controls

| Key | Action        |
|-----|---------------|
| `8` | Rotate        |
| `4` | Move left     |
| `6` | Move right    |
| `2` | Speed up drop |
| `q` | Quit game     |

---

## 📜 Inspiration

Inspired by the **Electronika 60** — the Soviet computer that ran the first version of Tetris back in 1984. This version keeps the aesthetic raw and minimal but adds some flavor with colors and a cleaner layout.

---

## 🛠️ Todo

- [ ] Add high score tracking
- [ ] Add sound (beeps anyone?)
- [ ] Animate line clears
- [ ] Configurable key bindings

---

## 🧠 Learning Note

This project was an exercise in:
- Terminal rendering with ANSI escape codes
- Raw input handling via `termios`
- Game loop logic
- Structs, arrays, and grid-based movement in C

---

## 🤝 Contributions

PRs are welcome if you want to fix something, improve the look, or add features!

---
