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

<a id="toc"></a>

## 📋 Table of Contents

- [Features](#features)
- [System Architecture](#system-architecture)
- [Core Components](#core-components)
    - [Item Definition System](#1-item-definition-system)
    - [Inventory Component](#2-inventory-component)
    - [Equipment Component](#3-equipment-component)
    - [Item Component](#4-item-component)
    - [Item Data Subsystem](#5-item-data-subsystem)
    - [Loot Component](#6-loot-component)
- [How It Works](#how-it-works)
- [Technical Details](#technical-details)

---

## Features

- **Fragment Composition Pattern** - Data-driven item system using reusable components
- **Equipment Management** - Equipment with stat modifiers and visual actors
- **Procedural Loot** - Probability-based drops with randomized properties
- **Async Asset Loading** - Thread-safe batch loading with progress tracking
- **Grid-Based UI** - Drag-and-drop interface with multi-slot items
- **GAS Integration** - Integration with Gameplay Ability System

[⬆ Back to Table of Contents](#toc)

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
[⬆ Back to Table of Contents](#toc)

---

## Core Components

### 1. Item Definition System

Items are defined through **fragments** - reusable components that define item behavior and properties:

<img width="800" height="926" alt="image" src="https://github.com/user-attachments/assets/d8c3d060-994e-4f6d-a246-a26d3ed593d1" />
<img width="800" height="1500" alt="image" src="https://github.com/user-attachments/assets/2fb48814-96f1-4e4c-b54f-b58aa356b5dc" />

<br>**Available Fragments:**

- `FStackableFragment` - Allows items to be stacked and gives a random amount in range

```cpp
USTRUCT(BlueprintType)
struct FStackableFragment : public FItemFragment
{
	GENERATED_BODY()

	virtual void Roll() override;
	
	FORCEINLINE int32 GetStackCount() const { return RolledStackCount; }
	FORCEINLINE void SetStackCount(int32 NewCount) { RolledStackCount = FMath::Max(NewCount, 1); }
	
private:
	UPROPERTY(EditAnywhere, meta=(ClampMin="1"))
	FInt32Interval StackRange = FInt32Interval(1, 1);
	
	int32 RolledStackCount = 0;
};
```

- `FConsumableFragment` - Applies effects when used
  
```cpp
USTRUCT(BlueprintType)
struct FConsumableFragment : public FWidgetFragment
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void AddSoftObjectPath(TArray<FSoftObjectPath>& Paths) override;

	/** Applies consumption effect to target character. */
	void Consume(IAbilitySystemInterface* Target);
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> ConsumeEffect = nullptr;
	
	UPROPERTY(EditAnywhere)
	int32 EffectMagnitude = 0;
	
	UPROPERTY(EditAnywhere)
	FText ConsumableText;
};
```

- `FGridFragment` - Sets size in invnentory grid (UI)
  
```cpp
USTRUCT(BlueprintType)
struct FGridFragment : public FItemFragment
{
	GENERATED_BODY()

	FORCEINLINE const FItemSize& GetGridSize() const { return GridSize; }
	
private:
	UPROPERTY(EditAnywhere)
	FItemSize GridSize = FItemSize(1, 1);
};
```

- `FImageFragment` - UI icon
  
```cpp
USTRUCT(BlueprintType)
struct FImageFragment : public FItemFragment
{
	GENERATED_BODY()
	
	virtual void AddSoftObjectPath(TArray<FSoftObjectPath>& Paths) override;
	
	FORCEINLINE UTexture2D* GetIcon() const
	{
		return Icon.IsValid() ? Icon.Get() : nullptr;
	}
	
private:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
};
```

- `FEquipmentFragmentt` - Equipment with stats and modifiers
  
```cpp
USTRUCT(BlueprintType)
struct FEquipmentFragment : public FWidgetFragment
{
	GENERATED_BODY()
	
	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
    virtual void Roll() override;
	virtual void AddSoftObjectPath(TArray<FSoftObjectPath>& Paths) override;
	
	FORCEINLINE int32 GetUpgradeLevel() const { return UpgradeLevel; };
    FORCEINLINE EEquipmentSocket GetEquipmentSocket() const { return SocketAttachPoint; }
	
	/** Applies all equipment modifiers effects. */
	void OnEquip(IAbilitySystemInterface* Target);
	
	/** Removes all equipment modifiers effects. */
	void OnUnequip(IAbilitySystemInterface* Target);
	
	/** Spawns visual equipment actor attached to character mesh. */
	AGSEquipItemActor* SpawnEquipmentActor(USkeletalMeshComponent* AttachMesh);
	
	/** Destroys spawned equipment actor. */
	void DestroyEquippedActor();	
	
private:
	/** Converts enum value to socket name string */
	FName GetSocketEnumShortName() const;

	/** Sets the item upgrade level based on the DropChance curve. */
	void RollUpgradeLevel();
	
	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FEquipModifier>> EquipModifiers;
		
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> UpgradeLevelDropChance;

	UPROPERTY(EditAnywhere)
	EEquipmentSocket SocketAttachPoint = EEquipmentSocket::None;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> EquipMesh = nullptr;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AGSEquipItemActor> EquipActorClass = nullptr;

	TWeakObjectPtr<AGSEquipItemActor> EquippedActor = nullptr;
	
	int32 UpgradeLevel = 0;
	bool bEquipped = false;
};
```

**Equipment Modifiers:**
- `FCombatModifier` - Adds attack ability

 ```cpp
USTRUCT(BlueprintType)
struct FCombatModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void OnEquip(IAbilitySystemInterface* Target) override;
	virtual void OnUnequip(IAbilitySystemInterface* Target);
	virtual void AddSoftObjectPath(TArray<FSoftObjectPath>& Paths) override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayAbility> AttackAbility = nullptr;
};
```
 
- `FDamageModifier` - Adds attack damage
  
```cpp
USTRUCT(BlueprintType)
struct FDamageModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* Target) override;
	virtual void AddSoftObjectPath(TArray<FSoftObjectPath>& Paths) override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> DamageModifierEffect = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveTable> DamageCurveTable = nullptr;
};
```
  
- `FDefenceModifier` - Adds defense stats
  
```cpp
USTRUCT(BlueprintType)
struct FDefenceModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* Target) override;
	virtual void AddSoftObjectPath(TArray<FSoftObjectPath>& Paths) override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> DefenceModifierEffect = nullptr;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveTable> DefenceCurveTable = nullptr;
};
```
  
- `FAttackSpeedModifier` - Adds attack speed
  
```cpp
USTRUCT(BlueprintType)
struct FAttackSpeedModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* Target) override;
	virtual void AddSoftObjectPath(TArray<FSoftObjectPath>& Paths) override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> AttackSpeedModifierEffect = nullptr;
	
	UPROPERTY(EditAnywhere)
	int32 BonusAttackSpeedPercent = 0;
};
```
  
- `FAttributeModifier` - Adds attributes (STR, DEX, etc.)
  
```cpp
USTRUCT(BlueprintType)
struct FAttributeModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* Target) override;
	virtual void AddSoftObjectPath(TArray<FSoftObjectPath>& Paths) override;
	
	/** Draws attributes and their values that will be added to the item. */
	virtual void Roll() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> AttributeModifierEffect = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<FAttributeEntry> Attributes;

	/** For memory allocation. */
	int32 AcceptedAttributesNum = 0;
};
```
  
### 2. Inventory Component

Responsible for managing all item interactions for a character. It handles item storage, stacking, equipping, discarding, and communication with UI and gameplay systems in a clean and extensible way.

The inventory stores items as item instances, allowing each item to maintain its own runtime state (stack size, equipment status, rolled attributes, etc.), even when created from the same definition. Internally, items are stored using TInstancedStruct, enabling flexible composition and safe value-type ownership.

**Key Responsibilities**
- Manage all item instances owned by the player
- Handle item pickup, stacking, equipping, unequipping, and removal
- Track nearby world items that can be picked up
- Spawn discarded items back into the world
- Expose inventory state changes via delegates for UI and gameplay systems

**Item Management**
- Items are added either as new instances or merged into existing stacks
- Stack size is validated against a configurable maximum
- Items can be queried by ID, gameplay tag, or custom predicate
- Each item instance has a unique identifier, enabling safe runtime references

**Grid-Based Inventory**
- Uses grid positioning (FGridPosition, FGridInfo) to support inventory layouts
- Allows UI systems to determine placement and available space
- Keeps inventory logic independent from visual representation

**World Interaction**
- Tracks nearby world items using weak references
- Supports picking up and discarding items
- Discarded items are spawned back into the world using item definitions

### 3. Equipment Component

Responsible for managing all equipped items on a character. It acts as the bridge between the inventory system and in-game representation, handling equipment validation, slot management, stat application, and spawning of visual equipment actors.

The component is designed to be inventory-driven and slot-based, allowing items to be equipped and unequipped dynamically while keeping combat, animation, and UI systems loosely coupled.

![eq](https://github.com/user-attachments/assets/ab6cd094-72ef-410c-a44b-9acf92ace807)

**Key Responsibilities**
- Manage all currently equipped items and their slots
- Validate equipment rules (slot availability, requirements)
- Spawn and destroy visual equipment actors
- Track weapon equip state for gameplay systems
- Notify other systems when equipment changes

**Slot-Based Equipment System**
- Equipment is organized by gameplay tags, representing equipment slots (e.g. weapon, armor, accessory)
- Each slot can hold at most one equipped item
- Equipped items are stored in a tag-to-actor map for fast access and queries

**Inventory Integration**
- The component listens to inventory events and reacts to equip / unequip requests
- Equipment logic is kept separate from inventory component, ensuring clear responsibility boundaries
- Items are validated before equipping (e.g. slot availability, item requirements)

**Visual Representation**
- Spawns dedicated equipment actors attached to the character’s skeletal mesh
- Visual actors are fully decoupled from item data and can be replaced or extended
- Unequipping safely removes stats and destroys the associated visual actor

### 4. Item Component

Represents a physical item in the world that the player can interact with and pick up. It acts as the link between world actors and the data-driven item system, handling item identification, definition loading, procedural variation, and pickup detection.

The component is intentionally lightweight and reusable, allowing any actor to become a world item by attaching this component and assigning an item tag.

<img width="800" height="958" alt="image" src="https://github.com/user-attachments/assets/a7778df9-77e6-4c1f-9419-972f5f5cee49" />

**Key Responsibilities**
- Represent an item instance placed in the game world
- Load item data from a centralized data subsystem
- Detect player proximity for pickup interactions
- Apply procedural item variation when enabled
- Notify other systems when item data becomes available

**Data-Driven Item Identification**
- Each world item is identified by a gameplay tag
- The tag is resolved into a full FItemDefinition via the item data subsystem
- Item definitions are fully decoupled from world actors, enabling reuse and easy content iteration
- Item data can also be moved directly into the component when spawning items dynamically

**Procedural Item Variation**
- Supports optional randomization of item fragments
- When enabled, item attributes such as stats, upgrades, or modifiers are rolled on pickup or spawn
- Randomization logic is contained within item fragments, keeping the component generic

**Pickup Zone Handling**
- Inherits from USphereComponent and uses overlap events to detect nearby actors
- Automatically tracks when an actor enters or leaves the pickup zone
- Integrates cleanly with inventory systems without direct dependencies

**Event-Based Initialization**
- Broadcasts a delegate when the item definition is set or loaded
- Allows UI, inventory, or gameplay systems to react once item data is ready

### 5. Item Data Subsystem

Centralized, game-instance–level system responsible for loading, caching, and providing access to all item definitions used by the game. It serves as the singletone for item data and decouples gameplay systems from direct asset references.

The subsystem loads all item assets asynchronously at startup and exposes fast, tag-based lookup.

```cpp
/** 
	 * Finds item definition by name tag.
	 * @return	pointer to item definition, or nullptr if not found
	 */
	const FItemDefinition* FindItemDefinition(const FGameplayTag& ItemName) const;

	/** 
	 * Checks if item data has finished loading.
	 * @return	true if all assets are loaded and ready for use
	 */
	FORCEINLINE bool IsDataLoaded() const { return bDataLoaded; }

	/** Gets all loaded item definitions. */
	FORCEINLINE const TMap<FGameplayTag, FItemDefinition>& GetAllItemDefinitions() const { return ItemDefinitions; }

	/** Gets number of loaded items. */
	FORCEINLINE int32 GetItemCount() const { return ItemDefinitions.Num(); }
	
	FOnItemDataLoadedSignature OnItemDataLoaded;
```

**Key Responsibilities**
- Load all item definition assets asynchronously
- Cache item data for fast runtime access
- Provide tag-based item definition lookup
- Notify dependent systems when item data is ready

**Asynchronous Asset Loading**
- Uses Unreal’s asset management and streaming system to load item data in bulk
- Ensures non-blocking startup and smooth runtime performance
- Tracks loading progress and finalization state internally
- Broadcasts a completion event once all assets are available

**Centralized Item Definition Storage**
- Stores all loaded FItemDefinition instances in a map keyed by gameplay tags
- Prevents duplicated loads and redundant asset references
- Supports efficient queries by item name or category tag

**Runtime Access API**
- Provides safe lookup functions that return null if data is unavailable
- Exposes read-only access to all loaded item definitions
- Includes simple readiness checks for defensive programming

### 6. Loot Component

Data-driven loot drop system responsible for spawning item rewards for world entities such as enemies, chests, or interactive objects. It evaluates configurable loot tables at runtime and spawns physical item actors in the world based on drop chance and quantity rules.

The component is fully decoupled from item data and inventory logic, relying on gameplay tags and the item system to resolve definitions and initialize dropped items.

<img width="800" height="625" alt="image" src="https://github.com/user-attachments/assets/9d627f0e-65cb-45bd-9e2d-cac680914e1c" />

![loot](https://github.com/user-attachments/assets/7ad4b5ed-2429-4aba-8085-42a6f3b0f3aa)

**Key Responsibilities**
- Define loot tables with drop chance and quantity ranges
- Roll loot outcomes at runtime
- Spawn world item actors at randomized positions
- Support reusable loot behavior across multiple actor types

**Data-Driven Loot Tables**
- Each loot entry defines:
    - Item identifier via gameplay tag
    - Percentage-based drop chance
    - Quantity range for successful drops
- Multiple loot entries can succeed in a single roll
- Validation logic ensures only properly configured loot is processed

**Runtime Loot Rolling**
- Evaluates each loot entry independently
- Uses random rolls to determine which items drop
- Supports variable stack sizes and multiple item spawns per entry
- Enables flexible reward design without code changes

**World Item Spawning**
- Spawns item actors using a configurable world item class
- Applies vertical and horizontal offsets to avoid overlap
- Supports spawning at owner location or at an explicit world position
- Keeps spawn logic isolated from inventory and pickup systems

**Reusability and Integration**
- Designed to be attached to any actor that can drop loot
- Allows designers to configure loot entirely in the editor


[⬆ Back to Table of Contents](#toc)

---
