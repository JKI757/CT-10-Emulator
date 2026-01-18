# CT-10 Emulator – CMake & Project Skeleton

## Directory Layout

```
ct10-emulator/
├─ CMakeLists.txt
├─ src/
│  ├─ core/
│  ├─ ui/
│  └─ app/
├─ assets/
└─ docs/
```

---

## Minimal CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(ct10_emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ct10
    src/app/main.cpp
)

target_include_directories(ct10 PRIVATE src)
```

---

## Build Goals

- Deterministic build
- No platform abstraction layer
- macOS-first

---

**End of CMake Skeleton**
