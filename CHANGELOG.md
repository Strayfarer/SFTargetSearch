# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Changed
- Changed component properties to be EditAnywhere instead of EditDefaultsOnly

## [2.0.0] - 2026-05-20
### Added
- Added unit tests
### Changed
- Made query result struct members read-only
### Fixed
- Fixed a bug where with two targets with positive binary rating the one with lower fuzzy score would be reported as best
- Removed ComponentPicker from module dependencies

## [1.0.0] - 2026-05-18

### Added
- TargetComponent
- TargetRegistrationComponent
- TargetQueryDataAsset
- TargetQueryResult
- TargetSearchSubsystem
- GameplayDebuggerCategory_TargetSearch