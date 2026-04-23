#pragma once
#include <string>

// ResultCode — typed outcome enum for all domain operations.
enum class ResultCode {
    Success = 0,
    Fail,
    InventoryFull,
    NotEnoughGold,
    NotEnoughMaterials,
    ItemNotFound,
    InvalidSlot,
    InvalidTarget,
    MaxTierReached,
    CannotUpgrade,
};

inline const char* resultCodeStr(ResultCode r) {
    switch (r) {
    case ResultCode::Success:           return "Success";
    case ResultCode::Fail:              return "Operation failed";
    case ResultCode::InventoryFull:     return "Inventory is full";
    case ResultCode::NotEnoughGold:     return "Not enough gold";
    case ResultCode::NotEnoughMaterials:return "Not enough materials";
    case ResultCode::ItemNotFound:      return "Item not found";
    case ResultCode::InvalidSlot:       return "Invalid inventory slot";
    case ResultCode::InvalidTarget:     return "Invalid target";
    case ResultCode::MaxTierReached:    return "Already at maximum tier";
    case ResultCode::CannotUpgrade:     return "Cannot upgrade this item";
    default:                            return "Unknown error";
    }
}
