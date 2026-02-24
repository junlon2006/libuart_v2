# Contributing to libUartCommProtocolV2

Thank you for your interest in contributing! This document provides guidelines for contributing to this project.

## How to Contribute

### Reporting Bugs

- Check if the bug has already been reported in Issues
- Use the bug report template
- Include:
  - Platform and compiler version
  - Minimal reproducible example
  - Expected vs actual behavior

### Suggesting Features

- Open an issue with the feature request template
- Clearly describe the use case
- Explain why this feature would be useful

### Pull Requests

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Make your changes following the coding standards
4. Test your changes thoroughly
5. Commit with clear messages: `git commit -m "Add feature X"`
6. Push to your fork: `git push origin feature/my-feature`
7. Open a Pull Request

## Coding Standards

### C Code Style

- Use C99 standard
- 2-space indentation
- Function names: `snake_case` with `_` prefix for static functions
- Type names: `PascalCase` with typedef
- Constants: `UPPER_SNAKE_CASE`
- Max line length: 100 characters

### Comments

- Use `/* */` for multi-line comments
- Document all public APIs with Doxygen-style comments
- Explain "why" not "what" in implementation comments

### Example

```c
/**
 * @brief Brief description
 * 
 * Detailed description if needed.
 * 
 * @param[in] param1 Description
 * @return Description of return value
 */
int public_function(int param1);

static void _internal_helper(void) {
  /* Implementation */
}
```

## Testing

- Test on multiple platforms if possible (Linux, macOS, embedded)
- Verify both reliable and unreliable transmission modes
- Test edge cases (max payload, timeout, memory allocation failures)

## Commit Messages

Follow conventional commits:

- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `refactor:` Code refactoring
- `test:` Adding tests
- `chore:` Maintenance tasks

Example: `feat: add support for custom CRC algorithms`

## License

By contributing, you agree that your contributions will be licensed under the GPL v2 License.
