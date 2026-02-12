package domain

import "math/rand"

// Enemy represents a combat opponent.
type Enemy struct {
	ID        string
	Name      string
	Type      MonsterType
	Element   Element
	MaxHP     int
	CurrentHP int
	Attack    int
	Defense   int
	GoldMin   int
	GoldMax   int
}

// MonsterType categorizes enemy difficulty.
type MonsterType int

const (
	MonsterNormal MonsterType = iota
	MonsterElite
	MonsterBoss
)

// String returns the monster type name.
func (t MonsterType) String() string {
	switch t {
	case MonsterElite:
		return "Elite"
	case MonsterBoss:
		return "BOSS"
	default:
		return "Normal"
	}
}

// IsAlive returns true if enemy has HP remaining.
func (e *Enemy) IsAlive() bool {
	return e.CurrentHP > 0
}

// TakeDamage applies damage to the enemy.
// Returns true if enemy died.
func (e *Enemy) TakeDamage(amount int) bool {
	reduced := amount - int(float64(e.Defense)*0.5)
	if reduced < 1 {
		reduced = 1
	}
	e.CurrentHP -= reduced
	if e.CurrentHP < 0 {
		e.CurrentHP = 0
	}
	return e.CurrentHP <= 0
}

// RollGold returns a random gold amount within range.
func (e *Enemy) RollGold() int {
	if e.GoldMax <= e.GoldMin {
		return e.GoldMin
	}
	return e.GoldMin + rand.Intn(e.GoldMax-e.GoldMin+1)
}

// GetAttack returns the enemy's attack value.
func (e *Enemy) GetAttack() int {
	return e.Attack
}

// GetElement returns the enemy's element.
func (e *Enemy) GetElement() Element {
	return e.Element
}
