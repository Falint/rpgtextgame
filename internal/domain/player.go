package domain

// Player represents the player character.
type Player struct {
	Name       string
	MaxHP      int
	CurrentHP  int
	BaseAttack int
	BaseDef    int
	Gold       int

	// Equipped weapon (nil = fists)
	EquippedWeapon *Weapon

	// Inventory
	Inventory *Inventory

	// Active buffs
	Buffs []Buff
}

// Buff represents a temporary stat modification.
type Buff struct {
	Type           BuffType
	Value          int // Percentage or flat value
	TurnsRemaining int
}

// BuffType identifies buff effects.
type BuffType int

const (
	BuffNone BuffType = iota
	BuffAttack
	BuffDefense
)

// Combat balance constants.
const (
	DefenseReductionRatio = 0.5 // Defense reduces incoming damage by this fraction of DEF
	DefaultBaseAttack     = 10  // Starting base attack for new players
	MinDamage             = 1   // Minimum damage dealt per hit
)

// PlayerConfig holds configurable player parameters.
type PlayerConfig struct {
	StartingHP   int
	StartingGold int
	BaseDef      int
	MaxBuffs     int
}

// DefaultPlayerConfig returns sensible defaults.
func DefaultPlayerConfig() PlayerConfig {
	return PlayerConfig{
		StartingHP:   100,
		StartingGold: 150,
		BaseDef:      5,
		MaxBuffs:     3,
	}
}

// NewPlayer creates a new player with the given name.
func NewPlayer(name string, cfg PlayerConfig) *Player {
	return &Player{
		Name:       name,
		MaxHP:      cfg.StartingHP,
		CurrentHP:  cfg.StartingHP,
		BaseAttack: DefaultBaseAttack,
		BaseDef:    cfg.BaseDef,
		Gold:       cfg.StartingGold,
		Inventory:  NewInventory(MaxInventorySlots),
		Buffs:      make([]Buff, 0, cfg.MaxBuffs),
	}
}

// IsAlive returns true if player has HP remaining.
func (p *Player) IsAlive() bool {
	return p.CurrentHP > 0
}

// GetAttack returns total attack including weapon and buffs.
func (p *Player) GetAttack() int {
	atk := p.BaseAttack
	if p.EquippedWeapon != nil {
		atk += p.EquippedWeapon.Damage()
	}

	// Apply attack buffs
	for _, buff := range p.Buffs {
		if buff.Type == BuffAttack {
			atk += atk * buff.Value / 100
		}
	}
	return atk
}

// GetDefense returns total defense including buffs.
func (p *Player) GetDefense() int {
	def := p.BaseDef

	for _, buff := range p.Buffs {
		if buff.Type == BuffDefense {
			def += def * buff.Value / 100
		}
	}
	return def
}

// TakeDamage applies damage considering defense.
// Returns true if player died.
func (p *Player) TakeDamage(amount int) bool {
	reduced := amount - int(float64(p.GetDefense())*DefenseReductionRatio)
	if reduced < MinDamage {
		reduced = MinDamage
	}
	p.CurrentHP -= reduced
	if p.CurrentHP < 0 {
		p.CurrentHP = 0
	}
	return p.CurrentHP <= 0
}

// Heal restores HP up to max.
func (p *Player) Heal(amount int) {
	p.CurrentHP += amount
	if p.CurrentHP > p.MaxHP {
		p.CurrentHP = p.MaxHP
	}
}

// AddGold adds gold to player.
func (p *Player) AddGold(amount int) {
	p.Gold += amount
}

// SpendGold attempts to spend gold. Returns error if insufficient.
func (p *Player) SpendGold(amount int) error {
	if p.Gold < amount {
		return NewError(ResultNotEnoughGold)
	}
	p.Gold -= amount
	return nil
}

// EquipWeapon equips the given weapon.
func (p *Player) EquipWeapon(w *Weapon) {
	p.EquippedWeapon = w
}

// AddBuff adds a temporary buff to the player.
func (p *Player) AddBuff(buffType BuffType, value, duration int) {
	p.Buffs = append(p.Buffs, Buff{
		Type:           buffType,
		Value:          value,
		TurnsRemaining: duration,
	})
}

// TickBuffs decrements buff durations and removes expired ones.
func (p *Player) TickBuffs() {
	active := p.Buffs[:0]
	for _, buff := range p.Buffs {
		buff.TurnsRemaining--
		if buff.TurnsRemaining > 0 {
			active = append(active, buff)
		}
	}
	p.Buffs = active
}
