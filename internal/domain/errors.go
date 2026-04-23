package domain

// ResultCode represents the outcome of domain operations.
// Used instead of Go's generic error interface to provide typed, exhaustive
// error handling specific to game operations. Every domain operation returns
// a ResultCode that callers can switch on for specific error handling.
type ResultCode int

// ResultCode constants define all possible operation outcomes.
// iota auto-increments: Success=0, Fail=1, InventoryFull=2, etc.
const (
	ResultSuccess            ResultCode = iota // Operation completed without error
	ResultFail                                 // Generic operation failure
	ResultInventoryFull                        // No empty inventory slots available
	ResultNotEnoughGold                        // Player lacks sufficient gold for purchase
	ResultNotEnoughMaterials                   // Insufficient materials for upgrade recipe
	ResultItemNotFound                         // Referenced item does not exist in inventory
	ResultInvalidSlot                          // Slot index is out of bounds
	ResultInvalidTarget                        // Target entity is invalid for the operation
	ResultMaxTierReached                       // Weapon is already at maximum upgrade tier
	ResultCannotUpgrade                        // Weapon has no upgrade path defined
)

// String returns a human-readable description of the result code.
// Implements fmt.Stringer for clean error message display in UI.
func (r ResultCode) String() string {
	switch r {
	case ResultSuccess:
		return "Success" // Operation succeeded
	case ResultFail:
		return "Operation failed" // Generic failure message
	case ResultInventoryFull:
		return "Inventory is full" // All 20 slots occupied
	case ResultNotEnoughGold:
		return "Not enough gold" // Gold check failed
	case ResultNotEnoughMaterials:
		return "Not enough materials" // Material check failed for upgrade
	case ResultItemNotFound:
		return "Item not found" // Item ID lookup returned nil
	case ResultInvalidSlot:
		return "Invalid inventory slot" // Index out of [0, MaxInventorySlots) range
	case ResultInvalidTarget:
		return "Invalid target" // Target entity is nil or invalid
	case ResultMaxTierReached:
		return "Already at maximum tier" // Weapon.Tier >= Weapon.MaxTier
	case ResultCannotUpgrade:
		return "Cannot upgrade this item" // No upgrade recipe exists
	default:
		return "Unknown error" // Safety fallback for undefined codes
	}
}

// DomainError wraps a ResultCode with additional context.
// Implements Go's error interface so domain errors can be returned
// from functions that use the standard error return pattern (e.g., SpendGold).
type DomainError struct {
	Code    ResultCode // Typed error code for programmatic handling
	Message string     // Optional human-readable message with additional context
}

// Error implements the error interface.
// Returns the custom message if set, otherwise falls back to the ResultCode's
// default string representation. This allows both specific and generic error reporting.
func (e DomainError) Error() string {
	if e.Message != "" {
		return e.Message // Use custom message when available
	}
	return e.Code.String() // Fall back to the ResultCode's default description
}

// NewError creates a DomainError with the given code and no custom message.
// Used for standard error creation where the ResultCode description is sufficient.
func NewError(code ResultCode) DomainError {
	return DomainError{Code: code} // Message defaults to empty string
}

// NewErrorWithMessage creates a DomainError with custom message.
// Used when additional context is needed beyond the ResultCode description.
func NewErrorWithMessage(code ResultCode, msg string) DomainError {
	return DomainError{Code: code, Message: msg} // Both code and message set
}
