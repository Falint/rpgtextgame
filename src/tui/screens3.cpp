// screens3.cpp — InventoryScreen, EquipScreen, UpgradeScreen, StatusScreen
#include "screens.hpp"
#include <algorithm>
#include <sstream>

// ═══════════════════════════════════════════════════════════════════════════════
// InventoryScreen
// ═══════════════════════════════════════════════════════════════════════════════
InventoryScreen::InventoryScreen(Player* p, ItemService* svc): player_(p), svc_(svc){}
bool InventoryScreen::update(Key key, char ch){
    if(key==Key::Up||(key==Key::Char&&ch=='k')){if(cursor_>0)--cursor_;return true;}
    if(key==Key::Down||(key==Key::Char&&ch=='j')){
        int maxIdx = (mode_==Mode::Main) ? 4 : cursor_;
        if(cursor_<maxIdx)++cursor_;
        return true;
    }
    if(key==Key::Enter){
        if(mode_==Mode::Main){
            switch(cursor_){
            case 0: mode_=Mode::Weapons;     cursor_=0; break;
            case 1: mode_=Mode::Consumables; cursor_=0; break;
            case 2: mode_=Mode::Materials;   cursor_=0; break;
            case 3: mode_=Mode::UseItem;     cursor_=0; break;
            case 4: return false;
            }
        } else if(mode_==Mode::UseItem) handleUseItem();
        return true;
    }
    if(key==Key::Escape||key==Key::Backspace){
        if(mode_!=Mode::Main){mode_=Mode::Main;cursor_=0;msg_="";return true;}
        return false;
    }
    if(key>=Key::K1&&key<=Key::K5&&mode_==Mode::Main){
        cursor_=static_cast<int>(key)-static_cast<int>(Key::K1);
        update(Key::Enter,0);
    }
    return true;
}
void InventoryScreen::handleSelect(){
    // Note: Main-mode selection is now handled directly in update()
    if(mode_==Mode::UseItem) handleUseItem();
}
void InventoryScreen::handleUseItem(){
    auto cons=player_->inventory->getConsumables();
    if(cons.empty()){msg_="No consumables.";msgStyle_="error";return;}
    tui::clamp(cursor_,(int)cons.size());
    auto* item=cons[cursor_]->item.get();
    auto result=svc_->useConsumable(player_,item);
    if(!result.success){msg_=result.error;msgStyle_="error";return;}
    int slotIdx=findConsumableSlot(cursor_);
    if(slotIdx!=-1) player_->inventory->removeItem(slotIdx,1);
    msg_=result.message; msgStyle_="success";
}
int InventoryScreen::findConsumableSlot(int n) const {
    int count=0;
    for(int i=0;i<MAX_INVENTORY_SLOTS;++i){
        auto* s=player_->inventory->getSlot(i);
        if(s&&s->type==SlotType::ItemSlot&&s->item&&s->item->category==ItemCategory::Consumable){
            if (count == n) return i;
            ++count;
        }
    }
    return -1;
}
std::string InventoryScreen::render() const {
    std::string out;
    out+=tui::header("=== INVENTORY ===")+"\n\n";
    if(mode_==Mode::Main){
        out+="Total: "+std::to_string(player_->inventory->usedSlots())+"/"+std::to_string(MAX_INVENTORY_SLOTS)+"\n\n";
        const char* opts[]={"1. View Weapons","2. View Consumables","3. View Materials","4. Use Item","5. Back"};
        for(int i=0;i<5;++i)
            out+=(i==cursor_?Terminal::selected(opts[i]):Terminal::normal(opts[i]))+"\n";
        out+="Choice: ";
    } else if(mode_==Mode::Weapons){
        out+=tui::header("--- WEAPONS ---")+"\n";
        auto ws=player_->inventory->getWeapons();
        if(ws.empty()) out+=Terminal::muted("  No weapons\n");
        else for(int i=0;i<(int)ws.size();++i) out+=std::to_string(i+1)+". "+ws[i]->displayName()+"\n";
        out+="\n"+Terminal::muted("[Esc] Back");
    } else if(mode_==Mode::Consumables){
        out+=tui::header("--- CONSUMABLES ---")+"\n";
        auto cs=player_->inventory->getConsumables();
        if(cs.empty()) out+=Terminal::muted("  No consumables\n");
        else for(int i=0;i<(int)cs.size();++i)
            out+=std::to_string(i+1)+". "+cs[i]->item->name+" x"+std::to_string(cs[i]->quantity)+"\n";
        out+="\n"+Terminal::muted("[Esc] Back");
    } else if(mode_==Mode::Materials){
        out+=tui::header("--- MATERIALS ---")+"\n";
        auto ms=player_->inventory->getMaterials();
        if(ms.empty()) out+=Terminal::muted("  No materials\n");
        else for(int i=0;i<(int)ms.size();++i)
            out+=std::to_string(i+1)+". "+ms[i].name+" x"+std::to_string(ms[i].quantity)+"\n";
        out+="\n"+Terminal::muted("[Esc] Back");
    } else if(mode_==Mode::UseItem){
        out+=tui::header("--- USE ITEM ---")+"\n";
        auto cs=player_->inventory->getConsumables();
        if(cs.empty()){out+=Terminal::muted("  No items to use\n");}
        else{
            int cur3=tui::clamped(cursor_,(int)cs.size());
            for(int i=0;i<(int)cs.size();++i){
                std::string line=std::to_string(i+1)+". "+cs[i]->item->name+" x"+std::to_string(cs[i]->quantity);
                out+=(i==cur3?Terminal::selected(line):Terminal::normal(line))+"\n";
            }
            out+="\n"+Terminal::muted("[Enter] Use  [Esc] Back");
        }
    }
    if(!msg_.empty()) out+="\n"+(msgStyle_=="success"?Terminal::success(msg_):Terminal::error(msg_));
    return out;
}

// ═══════════════════════════════════════════════════════════════════════════════
// EquipScreen
// ═══════════════════════════════════════════════════════════════════════════════
EquipScreen::EquipScreen(Player* p):player_(p){}
std::vector<EquipScreen::Entry> EquipScreen::getEquippable() const {
    std::vector<Entry> v;
    for(int i=0;i<MAX_INVENTORY_SLOTS;++i){
        auto* s=player_->inventory->getSlot(i);
        if(s&&s->type==SlotType::WeaponSlot) v.push_back({s->weapon,i});
    }
    return v;
}
void EquipScreen::handleEquip(){
    auto ws=getEquippable();
    if(ws.empty()) return;
    tui::clamp(cursor_,(int)ws.size());
    // Put old weapon back
    if(player_->equippedWeapon) player_->inventory->addWeapon(player_->equippedWeapon);
    ResultCode rc;
    auto w=player_->inventory->removeWeapon(ws[cursor_].slotIdx,rc);
    if(rc==ResultCode::Success){player_->equippedWeapon=w;msg_="Equipped "+w->name+"!";msgStyle_="success";}
    else{msg_="Failed to equip!";msgStyle_="error";}
}
bool EquipScreen::update(Key key,char ch){
    if(key==Key::Up||(key==Key::Char&&ch=='k')){if(cursor_>0)--cursor_;return true;}
    if(key==Key::Down||(key==Key::Char&&ch=='j')){++cursor_;return true;}
    if(key==Key::Enter){handleEquip();return true;}
    if(key==Key::Escape||key==Key::Backspace) return false;
    return true;
}
std::string EquipScreen::render() const {
    std::string out;
    out+=tui::header("=== EQUIP WEAPON ===")+"\n\n";
    out+="Currently Equipped: ";
    if(player_->equippedWeapon)
        out+=std::string(CLR_TEXT)+player_->equippedWeapon->displayName()+ANSI_RESET
            +" (DMG: "+std::to_string(player_->equippedWeapon->damage())+")\n\n";
    else out+=Terminal::muted("None")+"\n\n";
    out+=tui::header("--- INVENTORY WEAPONS ---")+"\n";
    auto ws=getEquippable();
    if(ws.empty()) out+=Terminal::muted("  No weapons in inventory\n");
    else{
        int curE=tui::clamped(cursor_,(int)ws.size());
        for(int i=0;i<(int)ws.size();++i){
            std::string line=std::to_string(i+1)+". "+ws[i].weapon->displayName()
                +" (DMG: "+std::to_string(ws[i].weapon->damage())+")";
            out+=(i==curE?Terminal::selected(line):Terminal::normal(line))+"\n";
        }
    }
    if(!msg_.empty()) out+="\n"+(msgStyle_=="success"?Terminal::success(msg_):Terminal::error(msg_));
    out+="\n"+Terminal::muted("[Enter] Equip  [Esc] Back");
    return out;
}

// ═══════════════════════════════════════════════════════════════════════════════
// UpgradeScreen
// ═══════════════════════════════════════════════════════════════════════════════
UpgradeScreen::UpgradeScreen(Player* p,WeaponRegistry* w,ItemRegistry* it,UpgradeRegistry* u)
    :player_(p),weapons_(w),items_(it),upgrades_(u){}

std::vector<UpgradeScreen::Entry> UpgradeScreen::getUpgradeable() const {
    std::vector<Entry> v;
    if(player_->equippedWeapon&&player_->equippedWeapon->canUpgrade())
        v.push_back({player_->equippedWeapon.get(),-1,true});
    for(int i=0;i<MAX_INVENTORY_SLOTS;++i){
        auto* s=player_->inventory->getSlot(i);
        if(s&&s->type==SlotType::WeaponSlot&&s->weapon&&s->weapon->canUpgrade())
            v.push_back({s->weapon.get(),i,false});
    }
    return v;
}
bool UpgradeScreen::checkMaterials(const UpgradeRecipe* r) const {
    for(auto& m:r->materials) if(player_->inventory->countItem(m.itemID)<m.quantity) return false;
    return true;
}
void UpgradeScreen::consumeMaterials(const UpgradeRecipe* r){
    for(auto& m:r->materials) player_->inventory->removeItemByID(m.itemID,m.quantity);
}
void UpgradeScreen::doUpgradeEquipped(){
    if(!player_->equippedWeapon){msg_="No weapon equipped.";msgStyle_="error";return;}
    auto* w=player_->equippedWeapon.get();
    auto* r=upgrades_->getRecipe(w->id,w->tier);
    if(!r){msg_=w->tier>=w->maxTier?"Max tier reached!":"No upgrade path.";msgStyle_="error";return;}
    if(player_->gold<r->goldCost){msg_="Not enough gold! Need "+std::to_string(r->goldCost)+" G";msgStyle_="error";return;}
    if(!checkMaterials(r)){msg_="Not enough materials!";msgStyle_="error";return;}
    player_->gold-=r->goldCost; consumeMaterials(r); w->tier=r->toTier; w->baseDamage+=r->damageBonus;
    msg_="Upgraded! "+w->name+" +"+std::to_string(w->tier-1); msgStyle_="success";
}
void UpgradeScreen::doUpgradeInventory(){
    auto us=getUpgradeable();
    if(cursor_<0||cursor_>=(int)us.size()) return;
    auto* w=us[cursor_].weapon;
    auto* r=upgrades_->getRecipe(w->id,w->tier);
    if(!r){msg_="No upgrade available.";msgStyle_="error";return;}
    if(player_->gold<r->goldCost){msg_="Not enough gold!";msgStyle_="error";return;}
    if(!checkMaterials(r)){msg_="Not enough materials!";msgStyle_="error";return;}
    player_->gold-=r->goldCost; consumeMaterials(r); w->tier=r->toTier; w->baseDamage+=r->damageBonus;
    msg_="Upgraded! "+w->name+" +"+std::to_string(w->tier-1); msgStyle_="success";
}
bool UpgradeScreen::update(Key key,char ch){
    if(key==Key::Up||(key==Key::Char&&ch=='k')){if(cursor_>0)--cursor_;return true;}
    if(key==Key::Down||(key==Key::Char&&ch=='j')){++cursor_;return true;}
    if(key==Key::Enter){
        if(mode_==Mode::Main){
            switch(cursor_){
            case 0:doUpgradeEquipped();break;
            case 1:mode_=Mode::InventorySelect;cursor_=0;msg_="";break;
            case 2:mode_=Mode::Details;cursor_=0;break;
            case 3:return false;
            }
        } else if(mode_==Mode::InventorySelect) doUpgradeInventory();
        return true;
    }
    if(key==Key::Escape||key==Key::Backspace){
        if(mode_!=Mode::Main){mode_=Mode::Main;cursor_=0;msg_="";return true;}
        return false;
    }
    if(key>=Key::K1&&key<=Key::K4&&mode_==Mode::Main){
        cursor_=static_cast<int>(key)-static_cast<int>(Key::K1); update(Key::Enter,0);
    }
    return true;
}
std::string UpgradeScreen::render() const {
    std::string out;
    out+=tui::header("=== WEAPON UPGRADE ===")+"\n\n";
    out+="Your Gold: "+Terminal::gold(std::to_string(player_->gold)+" G")+"\n\n";
    if(mode_==Mode::Main){
        auto us=getUpgradeable();
        out+=tui::header("=== Upgradeable ===")+"\n";
        if(us.empty()) out+=Terminal::muted("  No upgradeable weapons.\n");
        else for(auto& e:us){
            std::string tag=e.equipped?"[E] ":std::string();
            auto* r=upgrades_->getRecipe(e.weapon->id,e.weapon->tier);
            std::string status=r?(checkMaterials(r)&&player_->gold>=r->goldCost?"READY":"missing mats"):"max";
            out+="  "+tag+e.weapon->displayName()+(e.equipped?" (EQUIPPED)":"")+
                 (r?" -> Tier "+std::to_string(r->toTier)+" ("+status+")\n":"\n");
        }
        out+="\n"+tui::header("--- Upgrade Menu ---")+"\n";
        const char* opts[]={"1. Upgrade Equipped","2. Upgrade Inventory","3. View Details","4. Back"};
        for(int i=0;i<4;++i)
            out+=(i==cursor_?Terminal::selected(opts[i]):Terminal::normal(opts[i]))+"\n";
        out+="Choice: ";
    } else if(mode_==Mode::InventorySelect){
        out+=tui::header("=== Select Weapon ===")+"\n\n";
        auto us=getUpgradeable();
        int curU=tui::clamped(cursor_,(int)us.size());
        for(int i=0;i<(int)us.size();++i){
            auto* r=upgrades_->getRecipe(us[i].weapon->id,us[i].weapon->tier);
            std::string line=std::to_string(i+1)+". "+us[i].weapon->displayName()
                +(us[i].equipped?" (EQUIPPED)":"")+(r?" ["+std::to_string(r->goldCost)+" G]":"");
            out+=(i==curU?Terminal::selected(line):line)+"\n";
            if(i==curU&&r){
                out+="  Requirements:\n";
                out+="    Gold: "+std::to_string(r->goldCost)+(player_->gold>=r->goldCost?" [OK]":" [NEED MORE]")+"\n";
                for(auto& m:r->materials){
                    int have=player_->inventory->countItem(m.itemID);
                    std::string name=m.itemID;
                    if(auto* it=items_->getByID(m.itemID)) name=it->name;
                    out+="    "+name+": "+std::to_string(have)+"/"+std::to_string(m.quantity)
                        +(have>=m.quantity?" [OK]":" [NEED MORE]")+"\n";
                }
            }
        }
        out+="\n"+Terminal::muted("[Enter] Upgrade  [Esc] Back");
    } else if(mode_==Mode::Details){
        out+=tui::header("=== Weapon Details ===")+"\n\n";
        if(player_->equippedWeapon){
            auto* w=player_->equippedWeapon.get();
            out+="  "+w->displayName()+" (EQUIPPED)\n";
            out+="  Type: "+std::string(weaponTypeStr(w->type))+"\n";
            out+="  Element: "+std::string(elementStr(w->element))+"\n";
            out+="  Damage: "+std::to_string(w->damage())+"\n";
            out+="  Tier: "+std::to_string(w->tier)+"/"+std::to_string(w->maxTier)+"\n";
            auto* r=upgrades_->getRecipe(w->id,w->tier);
            if(r) out+="  Next Tier +"+std::to_string(r->damageBonus)+" DMG\n";
        } else out+=Terminal::muted("  No weapon equipped\n");
        out+="\n"+Terminal::muted("[Esc] Back");
    }
    if(!msg_.empty()) out+="\n"+(msgStyle_=="success"?Terminal::success(msg_):Terminal::error(msg_));
    return out;
}

// ═══════════════════════════════════════════════════════════════════════════════
// StatusScreen
// ═══════════════════════════════════════════════════════════════════════════════
StatusScreen::StatusScreen(Player* p):player_(p){}
bool StatusScreen::update(Key key,char /*ch*/){
    return !(key==Key::Escape||key==Key::Backspace);
}
std::string StatusScreen::render() const {
    auto* p=player_;
    std::string out;
    out+=tui::header("=== PLAYER STATUS ===")+"\n\n";
    out+="Name: "+p->name+"\n";
    out+="HP: "+Terminal::hpColor(p->currentHP,p->maxHP,std::to_string(p->currentHP)+"/"+std::to_string(p->maxHP))+"\n";
    out+="Gold: "+Terminal::gold(std::to_string(p->gold))+"\n\n";
    out+=tui::header("--- Combat Stats ---")+"\n";
    out+="Base Attack:  "+std::to_string(p->baseAttack)+"\n";
    out+="Base Defense: "+std::to_string(p->baseDef)+"\n";
    int ta=p->getAttack(),td=p->getDefense();
    out+="Total Attack: "+std::to_string(ta);
    if(ta>p->baseAttack) out+=Terminal::success(" (+"+std::to_string(ta-p->baseAttack)+")");
    out+="\nTotal Defense: "+std::to_string(td);
    if(td>p->baseDef) out+=Terminal::success(" (+"+std::to_string(td-p->baseDef)+")");
    out+="\n\n"+tui::header("--- Equipment ---")+"\n";
    out+="Weapon: ";
    if(p->equippedWeapon){
        auto& w=*p->equippedWeapon;
        out+=w.displayName()+"\n  Type: "+weaponTypeStr(w.type)+"\n  Damage: "+std::to_string(w.damage());
        if(w.element!=Element::None) out+="\n  Element: "+std::string(elementStr(w.element));
    } else out+="None";
    out+="\n\n"+tui::header("--- Active Buffs ---")+"\n";
    if(p->buffs.empty()) out+="None\n";
    else for(auto& b:p->buffs)
        out+=std::string(b.type==BuffType::Attack?"Attack Up":"Defense Up")
            +" ("+std::to_string(b.turnsRemaining)+" turns)\n";
    out+="\n"+Terminal::muted("[Esc] Back");
    return out;
}
