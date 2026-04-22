#pragma once
#include <string>
#include <vector>
#include <map>

struct UpgradeMaterial {
    std::string itemID;
    int         quantity = 0;
};

struct UpgradeRecipe {
    std::string                  weaponID;
    int                          fromTier    = 1;
    int                          toTier      = 2;
    int                          goldCost    = 0;
    std::vector<UpgradeMaterial> materials;
    int                          damageBonus = 0;
};

class UpgradeRegistry {
public:
    UpgradeRegistry();

    // Returns nullptr if no recipe for this weapon/tier.
    const UpgradeRecipe* getRecipe(const std::string& weaponID, int fromTier) const;
    std::vector<UpgradeRecipe> getAllRecipesForWeapon(const std::string& weaponID) const;
    std::vector<std::string> getUpgradeableWeaponIDs() const;

private:
    void load();
    std::map<std::string, std::vector<UpgradeRecipe>> recipes_;
};