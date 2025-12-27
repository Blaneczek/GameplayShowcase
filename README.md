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

## Core Components

### 1. Item Definition System

Items are defined through **fragments** - reusable components that define item behavior:

```cpp
// Example: Healing Potion
FItemDefinition HealthPotion
{
    Name: "Item.Consumable.HealthPotion"
    Type: "Item.Consumable"
    Level: 1
    
    Fragments:
    [
        FConsumableFragment    // Restores 50 HP when used
        FStackableFragment     // Can stack up to 100
        FImageFragment         // Icon for UI
        FGridFragment          // 1x1 size in inventory
    ]
}

// Example: Legendary Sword
FItemDefinition LegendarySword
{
    Name: "Item.Equipment.Sword.Legendary"
    Type: "Item.Equipment.Weapon"
    Level: 10
    
    Fragments:
    [
        FEquipmentFragment     // Stats, socket, visual mesh
        {
            UpgradeLevel: 5
            Socket: Weapon_1H
            Modifiers:
            [
                FDamageModifier      // +150-200 Attack
                FAttackSpeedModifier // +15% Speed
                FAttributeModifier   // +50 STR, +30 DEX
            ]
        }
        FImageFragment
        FGridFragment          // 2x3 size in inventory
    ]
}
```

**Available Fragments:**
- `FStackableFragment` - For consumables and materials
- `FConsumableFragment` - Apply effects when used
- `FEquipmentFragment` - Equipment with stats and modifiers
- `FImageFragment` - UI icon
- `FGridFragment` - Inventory size

**Equipment Modifiers:**
- `FDamageModifier` - Attack damage (min/max, physical/magic)
- `FDefenceModifier` - Defense stats
- `FAttackSpeedModifier` - Attack speed percentage
- `FAttributeModifier` - Generic attributes (STR, DEX, etc.)

### 2. Inventory Component

Manages the player's inventory:

```cpp
UCLASS()
class UGSInventoryComponent : public UActorComponent
{
    // Add item to inventory
    bool TryAddNewItem(FItemDefinition& Def);
    
    // Find item by ID or name
    FItemInstance* FindItemInstanceByID(const FGuid& ItemID);
    FItemInstance* FindItemInstanceByNameTag(const FGameplayTag& ItemName);
    
    // Stack management
    void TryAddToItemStack(const FGuid& ItemIDTo, const FGuid& ItemIDFrom);
    bool CheckIfCanAddToStack(const FGuid& ItemIDTo, const FGuid& ItemIDFrom);
    
    // Item actions
    bool TryActivateItemAction(const FGuid& ItemID);  // Equip or consume
    void DiscardItemInstance(const FGuid& ItemID);
    
    // Equipment
    bool TryEquipItem(const FGuid& ItemID);
    void UnequipItem(const FGuid& ItemID);
};
```

### 3. Equipment Component

Handles equipment slots and stat modifications:

```cpp
UCLASS()
class UGSEquipmentComponent : public UActorComponent
{
    // Validates level requirements
    // Applies stat modifiers via GAS
    // Spawns visual equipment actors
    // Manages equipment slots
    
private:
    // Maps equipment type to equipped actor
    TMap<FGameplayTag, AGSEquipItemActor*> EquippedActors;
};
```

### 4. Loot Component

Generates procedural loot drops:

```cpp
UCLASS()
class UGSLootComponent : public UActorComponent
{
    UPROPERTY(EditAnywhere)
    TArray<FLootInfo> LootTable;  // Configure in editor
    
    UFUNCTION(BlueprintCallable)
    void SpawnLoot();  // Call on death/chest open
};

USTRUCT()
struct FLootInfo
{
    UPROPERTY(EditAnywhere)
    FGameplayTag Tag;  // Item to drop
    
    UPROPERTY(EditAnywhere)
    float DropChance = 100.0f;  // 0-100%
    
    UPROPERTY(EditAnywhere)
    FInt32Interval QuantityRange = {1, 1};  // How many
};
```

### 5. Item Data Subsystem

Centralized item database with async loading:

```cpp
UCLASS()
class UGSItemDataSubsystem : public UGameInstanceSubsystem
{
    // Auto-loads on game start
    void LoadItemsData();
    
    // Check loading status
    bool IsDataLoaded() const;
    float GetLoadProgress() const;
    
    // Query items
    const FItemDefinition* FindItemDefinition(const FGameplayTag& ItemName) const;
    const TMap<FGameplayTag, FItemDefinition>& GetAllItemDefinitions() const;
    
    // Event when loading complete
    FOnItemDataLoaded OnItemDataLoaded;
};
```

---
