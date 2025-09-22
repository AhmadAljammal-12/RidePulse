# RidePulse – Part 2 (Interactive Program, C++)

This is a simple C++ console app that simulates **RidePulse** fare calculation and ride selection.

## Features (matches rubric)
- Clear **inputs/outputs** with friendly prompts.
- **Validation** for numbers and choices (no crashes on bad input).
- **Logic** via `if/else` and functions: ride type, min fare, surge, service fee, promo.
- **Loop** to book multiple rides.
- Ready for **VS Code + g++**.

## How to Run in VS Code (Windows)
1. Install **VS Code** and **MinGW-w64 (g++)**. Add `...\mingw64\bin` to PATH.
2. Open folder in VS Code → create file `ridepulse.cpp` (or use the one provided).
3. Terminal →
   ```bash
   g++ ridepulse.cpp -o ridepulse
   ./ridepulse
   ```
4. Example inputs:
   - Distance: `12.5`
   - Ride type: `1` (Economy)
   - Peak: `y`
   - Promo: `PULSE10`

## Git Log (what to show in report)
```bash
git init
git add ridepulse.cpp
git commit -m "feat: add basic program structure"
git commit -am "feat: implement user input & validation"
git commit -am "feat: add fare calculation + min fare"
git commit -am "feat: add surge, booking & service fees"
git commit -am "feat: add promo code and summary output"
git commit -am "chore: improve prompts & formatting"
git log --oneline --graph
```

Take **screenshots** of:
- Running the program in VS Code terminal.
- `git log --oneline --graph`.

## Report Checklist (put in PDF)
- ✅ Code screenshot(s)
- ✅ Git log screenshot
- ✅ Short paragraph: how this program connects to **RidePulse** disruption (Part 1)
- ✅ Link to video demo (≤ 15 min)

## Notes
- Edit rates/min fares in the constants if your tutor specifies different values.
- All currency shown as **RM**.
