# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2024-01-XX

### Added
- Comprehensive English documentation (README_EN.md)
- CMake build system support
- Makefile for easy building
- GitHub Actions CI/CD pipeline
- Contributing guidelines (CONTRIBUTING.md)
- Changelog file
- .gitignore for common build artifacts
- Doxygen-style API documentation in headers
- Better error code definitions with descriptions

### Changed
- Replaced custom memcpy/memset with standard library functions
- Improved code comments and documentation
- Enhanced type safety with stdint.h types (uint16_t, uint8_t)
- Modernized code style and formatting
- Better NULL pointer checks
- Improved function naming consistency

### Fixed
- Compiler warnings on modern toolchains
- Potential portability issues with custom memory functions
- Missing include guards for MSVC

### Removed
- Custom memory copy/set implementations (use standard library)
- Redundant macro definitions

## [1.0.0] - 2020-04-21

### Added
- Initial release
- TCP-like reliable transmission protocol
- UDP-like unreliable transmission support
- CRC16 checksum verification
- Platform-independent design with hooks
- Support for Linux, RT-Thread, and 8051 platforms
- Automatic retransmission mechanism
- Sequence number based ordering
