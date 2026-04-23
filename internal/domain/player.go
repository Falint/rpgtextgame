package domain

// Player represents the player character.
// This is the central mutable entity of the game, holding all persistent
// state that survives between screens and battles.
type Player struct {
	Name       string // Player name (set at creation, displayed in status)
	MaxHP      int    // Maximum hit points — can increase through future progression
	CurrentHP  int    // Current hit points — reduced by damage, restored by healing
	BaseAttack int    // Base attack power before weapon and buff modifiers
	BaseDef    int    // Base defense power before buff modifiers
	Gold       int    // In-game currency for shop purchases and upgrades

	// Equipped weapon (nil = fists)
	// Only one weapon can be equipped at a time.
	// When nil, player fights with fists (BaseAttack only, no element).
	EquippedWeapon *Weapon

	// Inventory holds all items and unequipped weapons.
	// Uses a fixed-size slot array (20 slots by default).
	Inventory *Inventory

	// Active buffs are temporary stat modifications applied during combat.
	// Buffs expire after their TurnsRemaining reaches zero.
	Buffs []Buff
}

// Buff represents a temporary stat modification.
// Applied by consumable items (via ItemService) and ticked each combat round.
type Buff struct {
	Type           BuffType // Which stat is modified (Attack or Defense)
	Value          int      // Modification amount as percentage (e.g., 25 = +25%)
	TurnsRemaining int      // Turns until this buff expires and is removed
}

// BuffType identifies buff effects.
// Used to dispatch buff application in GetAttack() and GetDefense().
type BuffType int

// Buff type constants.
const (
	BuffNone    BuffType = iota // No buff effect — placeholder/sentinel
	BuffAttack                  // Increases attack by Value percent
	BuffDefense                 // Increases defense by Value percent
)

// Combat balance constants.
// These named constants ensure consistent combat math across Player and Enemy.
const (
	DefenseReductionRatio = 0.5 // Defense reduces incoming damage by this fraction of DEF
	DefaultBaseAttack     = 10  // Starting base attack for new players
	MinDamage             = 1   // Minimum damage dealt per hit — prevents zero-damage stalling
)

// PlayerConfig holds configurable player parameters.
// Separated from Player to allow different starting configurations
// without modifying the constructor signature.
type PlayerConfig struct {
	StartingHP   int // Initial and maximum HP
	StartingGold int // Initial gold amount
	BaseDef      int // Starting base defense value
	MaxBuffs     int // Maximum concurrent buff capacity (for slice pre-allocation)
}

// DefaultPlayerConfig returns sensible defaults matching the C version.
// 100 HP, 300 Gold, 10 DEF, up to 3 concurrent buffs.
func DefaultPlayerConfig() PlayerConfig {
	return PlayerConfig{
		StartingHP:   100, // Matches C PLAYER_DEFAULT_HP
		StartingGold: 300, // Matches C PLAYER_DEFAULT_GOLD
		BaseDef:      10,  // Matches C PLAYER_DEFAULT_DEF
		MaxBuffs:     3,   // Pre-allocates buff slice capacity
	}
}

// NewPlayer creates a new player with the given name.
// Initializes all fields from the provided config and creates
// a fresh inventory with MaxInventorySlots (20) capacity.
func NewPlayer(name string, cfg PlayerConfig) *Player {
	return &Player{
		Name:       name,                            // Set player name from parameter
		MaxHP:      cfg.StartingHP,                  // Set both max and current HP to starting value
		CurrentHP:  cfg.StartingHP,                  // Player starts at full health
		BaseAttack: DefaultBaseAttack,               // 10 — matches C constant
		BaseDef:    cfg.BaseDef,                     // 10 — from config
		Gold:       cfg.StartingGold,                // 300 — from config
		Inventory:  NewInventory(MaxInventorySlots), // Create 20-slot inventory
		Buffs:      make([]Buff, 0, cfg.MaxBuffs),   // Pre-allocate buff slice
	}
}

// IsAlive returns true if player has HP remaining.
// Used by battle system to check for game-over condition.
func (p *Player) IsAlive() bool {
	return p.CurrentHP > 0 // Player survives as long as HP is positive
}

// GetAttack returns total attack including weapon and buffs.
// Attack calculation chain: BaseAttack + WeaponDamage + BuffPercentage.
// Buff application is multiplicative: atk += atk * buff.Value / 100.
func (p *Player) GetAttack() int {
	atk := p.BaseAttack // Start with base attack (10 by default)
	if p.EquippedWeapon != nil {
		atk += p.EquippedWeapon.Damage() // Add weapon damage (includes tier bonus)
	}

	// Apply attack buffs — each buff adds a percentage of current attack
	for _, buff := range p.Buffs {
		if buff.Type == BuffAttack { // Only process attack-type buffs
			atk += atk * buff.Value / 100 // Multiplicative percentage bonus
		}
	}
	return atk // Total effective attack value
}

// GetDefense returns total defense including buffs.
// Defense calculation: BaseDef + BuffPercentage.
// Uses same multiplicative percentage pattern as GetAttack().
func (p *Player) GetDefense() int {
	def := p.BaseDef // Start with base defense (5 by default)

	// Apply defense buffs — percentage-based increase
	for _, buff := range p.Buffs {
		if buff.Type == BuffDefense { // Only process defense-type buffs
			def += def * buff.Value / 100 // Multiplicative percentage bonus
		}
	}
	return def // Total effective defense value
}

// TakeDamage applies damage considering defense.
// Returns true if player died (HP reached zero).
// Damage formula: reduced = amount - (GetDefense() * 0.5), minimum MinDamage (1).
func (p *Player) TakeDamage(amount int) bool {
	// Calculate damage after defense reduction
	// Defense absorbs 50% of its value from incoming damage
	reduced := amount - int(float64(p.GetDefense())*DefenseReductionRatio)
	// Enforce minimum damage to prevent infinite stalling
	if reduced < MinDamage {
		reduced = MinDamage // At least 1 damage always gets through
	}
	// Apply damage to current HP
	p.CurrentHP -= reduced
	// Clamp HP to zero — prevents negative HP display
	if p.CurrentHP < 0 {
		p.CurrentHP = 0
	}
	// Return true if player HP has reached zero (defeated)
	return p.CurrentHP <= 0
}

// Heal restores HP up to max.
// Called by ItemService when processing healing consumables.
// Prevents over-healing beyond MaxHP.
func (p *Player) Heal(amount int) {
	p.CurrentHP += amount      // Add healing amount
	if p.CurrentHP > p.MaxHP { // Clamp to maximum HP
		p.CurrentHP = p.MaxHP // Prevent over-healing
	}
}

// AddGold adds gold to player.
// Called after defeating enemies (gold reward) or selling items.
func (p *Player) AddGold(amount int) {
	p.Gold += amount // Increment gold balance
}

// SpendGold attempts to spend gold. Returns error if insufficient.
// Uses DomainError (implements error interface) for idiomatic Go error handling.
func (p *Player) SpendGold(amount int) error {
	if p.Gold < amount { // Insufficient funds check
		return NewError(ResultNotEnoughGold) // Return typed domain error
	}
	p.Gold -= amount // Deduct gold
	return nil       // Success — no error
}

// EquipWeapon equips the given weapon.
// Simply sets the EquippedWeapon pointer. The caller is responsible for
// inventory management (removing the weapon from inventory and returning
// the previously equipped weapon).
func (p *Player) EquipWeapon(w *Weapon) {
	p.EquippedWeapon = w // Set new equipped weapon (nil to unequip)
}

// AddBuff adds a temporary buff to the player.
// Appends a new Buff struct to the Buffs slice. Multiple buffs of the
// same type can stack (e.g., two ATK buffs both apply their percentages).
func (p *Player) AddBuff(buffType BuffType, value, duration int) {
	p.Buffs = append(p.Buffs, Buff{
		Type:           buffType, // BuffAttack or BuffDefense
		Value:          value,    // Percentage increase (e.g., 25 for +25%)
		TurnsRemaining: duration, // How many combat turns until expiry
	})
}

// TickBuffs decrements buff durations and removes expired ones.
// Called at the end of each full combat round (after enemy turn).
// Uses the "slice reuse" pattern: active := p.Buffs[:0] to avoid allocation.
// This pattern reuses the underlying array capacity while resetting length to 0.
func (p *Player) TickBuffs() {
	active := p.Buffs[:0] // Reuse underlying array — length 0, same capacity
	for _, buff := range p.Buffs {
		buff.TurnsRemaining--        // Decrement remaining turns
		if buff.TurnsRemaining > 0 { // Still active — keep it
			active = append(active, buff)
		}
		// Expired buffs (TurnsRemaining <= 0) are simply not appended
	}
	p.Buffs = active // Replace slice with filtered results
}
