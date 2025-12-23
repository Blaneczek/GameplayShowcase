# Getting Started
<sub>Unreal Engine version 5.7</sub>

Clone the repository or download the ZIP, generate Visual Studio files, and compile using the IDE.

# Gameplay Showcase
The project was created to showcase various systems and gameplay mechanics developed in C++ and Blueprints.

---

# Key elements

|                                                                               | Description                                                     |
|-------------------------------------------------------------------------------|-----------------------------------------------------------------|
| [Data Management System (Inventory)](#data-management-system-inventory-code)  | Modular inventory system using fragment composition pattern.    |
| [Ability System (GAS)](#ability-system-GAS-code)                              | WIP                                                             |
| [Combat System](#combat-system-code)                                          | WIP                                                             |
| [Leveling System](#leveling-system-code)                                      | WIP                                                             |

---

# Data Management System (Inventory) ([code](Source/GameplayShowcase/Public/Systems/Inventory/))
<details>
<summary>More</summary>
<br>
A modular inventory system featuring fragment composition, equipment management, procedural loot generation, grid-based UI, and thread-safe async asset loading.
  
---

## 📋 Table of Contents

- [Features](#features)
- [System Architecture](#system-architecture)
- [Core Components](#core-components)
- [How It Works](#how-it-works)
- [Technical Details](#technical-details)

---

## Features

- **Fragment Composition Pattern** - Data-driven item system using reusable components
- **Equipment Management** - Level-gated equipment with stat modifiers and visual actors
- **Procedural Loot** - Probability-based drops with randomized properties
- **Async Asset Loading** - Thread-safe batch loading with progress tracking
- **Grid-Based UI** - Drag-and-drop interface with multi-slot items
- **GAS Integration** - Integration with Gameplay Ability System

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Item Data Subsystem                     │
│  (Loads and manages all item definitions from Data Assets)  │
└─────────────────────────────┬───────────────────────────────┘
                              │
                              ↓
                    ┌──────────────────┐      
                    │   Loot           │
                    │   Component      │
                    │   (Spawns        │
                    │   world items)   │
                    └─────────┬────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐          ┌──────────────────┐    
│                    Inventory Component                      │          │   Equipment      │
│    (Manages item instances, stacking, equipping, etc.)      │ ──────>  │   Component      │ 
└─────────────────────────────┬───────────────────────────────┘          │   (Handles       │ 
                              │                                          │   equipping)     │ 
                              ↓                                          └──────────────────┘ 
┌─────────────────────────────────────────────────────────────┐
│              Widget Controller (MVC Pattern)                │
│         (Bridges UI and game logic via delegates)           │
└─────────────────────────────┬───────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                      UI Layer (UMG)                         │
│      (Grid widgets, item slots, drag-and-drop proxies)      │
└─────────────────────────────────────────────────────────────┘
```
