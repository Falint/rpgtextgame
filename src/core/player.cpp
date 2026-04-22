/**
 * @file player.cpp
 * @brief Player system implementation (C++ Version)
 */

#include "player.hpp"
#include "../utils/utils.hpp"
#include "element.hpp"
#include <iostream>
#include <variant>
#include <algorithm>

/* ============================================
 * CONSTRUCTOR
 * ============================================ */

Player::Player(const std::string& player_name) {
    name = player_name;
    max_hp = GameConfig::Player::STARTING_HP;
    current_hp = GameConfig::Player::STARTING_HP;
    base_attack = 5;
    base_defense = GameConfig::Player::STARTING_DEF;
    gold = GameConfig::Player::STARTING_GOLD;

    InventorySystem::init(&inventory);

    // Default equipment: Fists
    equipped_weapon = WeaponSystem::create("fists");
    has_weapon = true;

    buff_count = 0;
    // Inisialisasi std::array buff dengan state kosong
    for (auto& buff : active_buffs) {
        buff = Buff();
    }

    // Starter items
    Item potion = ItemSystem::create("small_potion");
    InventorySystem::add_item(&inventory, potion, 3);
}

/* ============================================
 * STAT CALCULATIONS
 * ============================================ */

int32_t Player::get_total_attack() const {
    int32_t total = base_attack;

    if (has_weapon) {
        total += WeaponSystem::get_damage(&equipped_weapon);
    }

    // Terapkan persentase buff ATK
    for (int i = 0; i < buff_count; ++i) {
        if (active_buffs[i].type == ConsumableType::BUFF_ATK) {
            total = static_cast<int32_t>(total * (1.0f + active_buffs[i].value / 100.0f));
        }
    }
    return total;
}

int32_t Player::get_total_defense() const {
    int32_t total = base_defense;

    // Terapkan persentase buff DEF
    for (int i = 0; i < buff_count; ++i) {
        if (active_buffs[i].type == ConsumableType::BUFF_DEF) {
            total = static_cast<int32_t>(total * (1.0f + active_buffs[i].value / 100.0f));
        }
    }
    return total;
}

/* ============================================
 * EQUIPMENT & ITEMS
 * ============================================ */

ResultCode Player::equip_weapon(int32_t inventory_slot) {
    InventorySlot* slot = InventorySystem::get_slot(&inventory, inventory_slot);
    
    if (!slot || slot->type != SlotType::WEAPON) {
        return ResultCode::INVALID_SLOT;
    }

    Weapon new_weapon = std::get<Weapon>(slot->data);

    // Tukar dengan senjata lama (kecuali tinju/fists tidak perlu masuk inv)
    if (has_weapon && equipped_weapon.id != "fists") {
        slot->data = equipped_weapon; // Slot lama diisi senjata yang dilepas
    } else {
        InventorySystem::remove_at(&inventory, inventory_slot);
    }

    equipped_weapon = new_weapon;
    has_weapon = true;

    std::cout << "Equipped: " << equipped_weapon.name << "!\n";
    return ResultCode::SUCCESS;
}

ResultCode Player::use_item(int32_t inventory_slot) {
    InventorySlot* slot = InventorySystem::get_slot(&inventory, inventory_slot);
    
    if (!slot || slot->type != SlotType::ITEM) {
        return ResultCode::INVALID_SLOT;
    }

    Item item = std::get<Item>(slot->data);

    if (!item.is_consumable()) {
        return ResultCode::FAIL;
    }

    switch (item.consumable_type) {
        case ConsumableType::HEAL:
            heal(item.value);
            std::cout << "Used " << item.name << ". Restored " << item.value << " HP.\n";
            break;

        case ConsumableType::BUFF_ATK:
        case ConsumableType::BUFF_DEF:
            add_buff(item.consumable_type, item.value, item.duration);
            std::cout << "Used " << item.name << ". Buff active for " << item.duration << " turns.\n";
            break;

        case ConsumableType::FULL_RESTORE:
            full_restore();
            std::cout << "Used " << item.name << ". Health fully restored!\n";
            break;

        default:
            return ResultCode::FAIL;
    }

    InventorySystem::use_item_at(&inventory, inventory_slot);
    return ResultCode::SUCCESS;
}

/* ============================================
 * COMBAT & BUFF LOGIC
 * ============================================ */

bool Player::take_damage(int32_t amount) {
    int32_t defense = get_total_defense();
    // damage = attack - (defense * factor)
    int32_t reduced = amount - static_cast<int32_t>(defense * GameConfig::Combat::DEF_REDUCTION_FACTOR);
    
    if (reduced < 1) reduced = 1;

    current_hp -= reduced;
    if (current_hp <= 0) {
        current_hp = 0;
        return true; // Player died
    }
    return false;
}

void Player::heal(int32_t amount) {
    current_hp = std::min(current_hp + amount, max_hp);
}

void Player::full_restore() {
    current_hp = max_hp;
}

void Player::add_buff(ConsumableType type, int32_t value, int32_t duration) {
    // 1. Cek jika buff yang sama sudah aktif, timpa jika efek lebih kuat/lama
    for (int i = 0; i < buff_count; ++i) {
        if (active_buffs[i].type == type) {
            active_buffs[i].value = std::max(active_buffs[i].value, value);
            active_buffs[i].turns_remaining = std::max(active_buffs[i].turns_remaining, duration);
            return;
        }
    }

    // 2. Tambah buff baru jika slot tersedia
    if (buff_count < GameConfig::Buffs::MAX_ACTIVE) {
        active_buffs[buff_count].type = type;
        active_buffs[buff_count].value = value;
        active_buffs[buff_count].turns_remaining = duration;
        buff_count++;
    }
}

void Player::tick_buffs() {
    for (int i = 0; i < buff_count; ++i) {
        active_buffs[i].turns_remaining--;

        if (active_buffs[i].turns_remaining <= 0) {
            // Hapus buff yang kadaluwarsa dengan shifting
            for (int j = i; j < buff_count - 1; ++j) {
                active_buffs[j] = active_buffs[j + 1];
            }
            buff_count--;
            i--; // Cek index ini lagi setelah pergeseran
        }
    }
}

/* ============================================
 * UI & STATUS
 * ============================================ */

void Player::print_status() const {
    std::cout << "[" << name << "] HP: " << current_hp << "/" << max_hp
              << " | ATK: " << get_total_attack() 
              << " | DEF: " << get_total_defense() 
              << " | Gold: " << gold;

    if (has_weapon && equipped_weapon.id != "fists") {
        std::cout << " | Weapon: " << equipped_weapon.name;
    }
    std::cout << "\n";
    if (buff_count > 0) print_buffs();
}

void Player::print_buffs() const {
    std::cout << "  Active Buffs: ";
    for (int i = 0; i < buff_count; ++i) {
        std::string type_str = (active_buffs[i].type == ConsumableType::BUFF_ATK) ? "ATK" : "DEF";
        std::cout << "[" << type_str << "+" << active_buffs[i].value << "% (" 
                  << active_buffs[i].turns_remaining << "t)] ";
    }
    std::cout << "\n";
}