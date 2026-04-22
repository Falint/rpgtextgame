/**
 * @file element.h
 * @brief Element system for damage calculations
 */

#ifndef ELEMENT_H
#define ELEMENT_H

#include "../types.h"

/**
 * @brief Get string representation of element
 */
const char *element_to_string(Element e);

/**
 * @brief Calculate damage modifier based on attacker vs defender elements
 * @return 2.0 if strong, 0.5 if weak, 1.0 otherwise
 */
float element_get_modifier(Element attacker, Element defender);

/**
 * @brief Get the element that is strong against the given element
 */
Element element_get_counter(Element e);

/**
 * @brief Get the element that is weak against the given element
 */
Element element_get_weakness(Element e);

#endif /* ELEMENT_H */
