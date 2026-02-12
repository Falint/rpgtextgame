package domain

// ResultCode represents the outcome of domain operations.
type ResultCode int

const (
	ResultSuccess ResultCode = iota
	ResultFail
	ResultInventoryFull
	ResultNotEnoughGold
	ResultNotEnoughMaterials
	ResultItemNotFound
	ResultInvalidSlot
	ResultInvalidTarget
	ResultMaxTierReached
	ResultCannotUpgrade
)

// String returns a human-readable description.
func (r ResultCode) String() string {
	switch r {
	case ResultSuccess:
		return "Success"
	case ResultFail:
		return "Operation failed"
	case ResultInventoryFull:
		return "Inventory is full"
	case ResultNotEnoughGold:
		return "Not enough gold"
	case ResultNotEnoughMaterials:
		return "Not enough materials"
	case ResultItemNotFound:
		return "Item not found"
	case ResultInvalidSlot:
		return "Invalid inventory slot"
	case ResultInvalidTarget:
		return "Invalid target"
	case ResultMaxTierReached:
		return "Already at maximum tier"
	case ResultCannotUpgrade:
		return "Cannot upgrade this item"
	default:
		return "Unknown error"
	}
}

// DomainError wraps a ResultCode with additional context.
type DomainError struct {
	Code    ResultCode
	Message string
}

// Error implements the error interface.
func (e DomainError) Error() string {
	if e.Message != "" {
		return e.Message
	}
	return e.Code.String()
}

// NewError creates a DomainError with the given code.
func NewError(code ResultCode) DomainError {
	return DomainError{Code: code}
}

// NewErrorWithMessage creates a DomainError with custom message.
func NewErrorWithMessage(code ResultCode, msg string) DomainError {
	return DomainError{Code: code, Message: msg}
}
