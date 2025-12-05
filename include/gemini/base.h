#pragma once

#ifndef GEMINI_BASE_H
#define GEMINI_BASE_H

/**
 * @brief Macro to suppress "unused parameter" warnings from the compiler.
 * * Useful when a function signature is enforced by an interface but the implementation
 * doesn't use all arguments.
 * @param x The unused variable name.
 */
#define GEMINI_UNUSED(x) (void)(x)

#endif