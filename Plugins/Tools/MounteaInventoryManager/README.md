[![Documentation](https://img.shields.io/badge/documentation-github?style=flat&logo=GitHub&labelColor=5a5a5a&color=98c510)](https://github.com/Mountea-Framework/InventoryManager/wiki)
[![license](https://img.shields.io/badge/license-MIT-99c711?labelColor=555555&style=flat&link=https://github.com/Mountea-Framework/InventoryManager/blob/master/LICENSE)](https://github.com/Mountea-Framework/InventoryManager/blob/master/LICENSE)
[![YouTube](https://img.shields.io/badge/YouTube-Subscribe-red?style=flat&logo=youtube)](https://www.youtube.com/@mounteaframework)
[![Discord](https://badgen.net/discord/online-members/2vXWEEN?label=&logoColor=ffffff&color=7389D8&icon=discord)](https://discord.com/invite/2vXWEEN)
[![Discord](https://badgen.net/discord/members/2vXWEEN?label=&logo=discord&logoColor=ffffff&color=7389D8&icon=discord)](https://discord.com/invite/2vXWEEN)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/1fd7e368d04e485086aceae2d2d0350d)](https://app.codacy.com/gh/Mountea-Framework/InventoryManager/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

# Mountea Inventory Template Editor

A browser-based Inventory Template Editor for Unreal Engine projects — manage item templates online from any device. Simple, fast, and accessible.

---

## Table of Contents

- [Features](#features)  
- [Demo](#demo)  
- [Installation](#installation)  
- [Usage](#usage)  
- [Project Structure](#project-structure)  
- [Contributing](#contributing)  
- [License](#license)  

---

## Features

- **Native Unreal import/Export**
  Fully supports Unreal engine using [Mountea Inventory & Equipment](https://github.com/Mountea-Framework/MounteaInventoryEquipment) system.

- **Create & Edit Templates**  
  Define item name, display name, GUID, descriptions, and more.  

- **Item Properties**  
  Configure type, rarity (with color coding), max stack size, weight, value, durability.  

- **Flags & Equipment**  
  Toggle stackable, droppable, usable, equippable, tradeable, quest item flags; specify equipment slot when equippable.  

- **Visual Assets**  
  Upload icon (JPG/PNG/BMP) and mesh (FBX/OBJ) files; preview and store directly in IndexedDB.  

- **Custom Properties**  
  Add arbitrary key-value pairs for game-specific behaviors (e.g. damage, armor, spell effects).  

- **Import / Export**  
  - **JSON**: Import/export all templates for backup or sharing.  
  - **Unreal**: Export single `.mnteaitem` or multiple `.mnteaitems` files ready for Unreal Engine.  

- **Settings Panel**  
  Customize categories, rarities (names & colors), and equipment slots.  

- **Live Preview & Validation**  
  Real-time JSON preview; form validation (GUID format, name rules, numeric limits).  

- **Keyboard Shortcuts**  
  - `Ctrl + N` — New Template  
  - `Ctrl + S` — Save Template  
  - `Ctrl + D` — Duplicate Template  
  - `Esc` — Close panels  

---


## Demo

[Interactive Project](https://mountea-framework.github.io/InventoryManager/)

<p align="center" width="100%">
    <img width="66%" src="https://github.com/user-attachments/assets/4cf23427-8a39-4fa2-9d37-d6466f3f1a5b">
</p>

<p align="center" width="100%">
    <img width="66%" src="https://github.com/user-attachments/assets/6728d2bb-9baf-4281-b818-88545192c1a9">
</p>

---

## Installation

1. **Clone the repo**

   ```bash
   git clone https://github.com/your-username/mountea-inventory-editor.git
   cd mountea-inventory-editor
   ```

2. **Serve locally**
   Because the app uses IndexedDB, run a local HTTP server:

   * **Python 3**

     ```bash
     python3 -m http.server 8080
     ```
   * **Node.js**

     ```bash
     npx http-server -p 8080
     ```

3. **Open in Browser**
   Navigate to `http://localhost:8080/` and start managing your inventory templates!

---

## Usage

1. **New Template**: Click **New Template** or press `Ctrl + N`.
2. **Fill Fields**: Complete all required fields (Item Name, Display Name, Item ID).
3. **Upload Assets**: Attach icon and mesh files; ensure they meet format and size limits.
4. **Save**: Click save or press `Ctrl + S`.
5. **Preview**: Toggle the JSON preview panel to inspect your template data.
6. **Export**:

   * **JSON** — Export all templates for backup.
   * **Unreal** — Select one or more templates and click **Export Item(s)**.

---

## Project Structure

```
InventoryManager/
├── .github/
│   └── workflows/            # GitHub Actions workflows
├── assets/                   # Static assets (images, icons, etc.)
├── components/               # HTML components
├── script/                   # App JS scripts
├── style/                    # CSS files
├── .gitignore                # Git ignore rules
├── LICENSE                   # Project license
├── README.md                 # Project readme
├── index.html                # Main HTML file
└── manifest.json             # Web app manifest
```

---

## Contributing

We ❤️ contributions! Please:

1. Fork the repo
2. Create a feature branch (`git checkout -b feat/my-feature`)
3. Commit your changes (`git commit -m "Add my feature"`)
4. Push to your branch (`git push origin feat/my-feature`)
5. Open a Pull Request

See [CONTRIBUTING.md](CONTRIBUTING.md) for more details.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
