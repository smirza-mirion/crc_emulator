# Database API Reference

## Purpose

`Database.c` (604KB) provides the complete data persistence layer wrapping SQLite3 for the CRC-25R calibrator. All measurement results, calibration data, configuration, test records, and wipe reports are stored in `/data/capintec.db` on the SD card.

## Usage

### Initialization

```
initDB(bool recreate)    // Database.c:189
```
- Called from `main()` at `CalibratorMain_R.c:263`
- Creates `/data` directory if missing
- Opens `capintec.db` (or creates from scratch)
- Sets SQLite pragmas: `foreign_keys=1`, `cache_size=2000`, `page_size=1024`
- Parameter `recreate`: when TRUE, displays Amulet status during rebuild

### Database Handle

- `capDB` - Global `sqlite3*` handle, opened once at startup
- `capStatus` - Global `int*` tracking last SQLite return code

## Key Components

### Helper Functions (Internal API)

All DB operations use these internal wrappers (not exported in header):

| Function | Line | Purpose |
|----------|------|---------|
| `d_command(sql)` | Internal | Execute raw SQL (used for PRAGMA, BEGIN, COMMIT, ROLLBACK) |
| `d_prepare_v2(sql)` | Internal | Prepare a statement with named parameters |
| `d_bind_int(name, val)` | Internal | Bind integer to named parameter |
| `d_bind_int64(name, val)` | Internal | Bind 64-bit integer |
| `d_bind_double(name, val)` | Internal | Bind float/double |
| `d_bind_text(name, val)` | Internal | Bind string |
| `d_step()` | Internal | Execute prepared statement |
| `d_finalize()` | Internal | Finalize/free statement |

### Transaction Control (BookEnd Pattern)

| Function | Line | Purpose |
|----------|------|---------|
| `DB_StartBookEnd()` | `Database.c:13258`, `database.h:198` | Begin transaction (`BEGIN;`) |
| `DB_EndBookEnd()` | `Database.c:13301`, `database.h:200` | Commit or rollback based on `capStatus` |

**Pattern:** Functions accepting `bool bookEnd`:
- `bookEnd=TRUE` - Self-contained transaction (BEGIN...COMMIT/ROLLBACK)
- `bookEnd=FALSE` - Runs within caller-managed transaction

**Example - batching multiple writes:**
```
DB_StartBookEnd();
DB_WriteRecord1(&data1, FALSE);  // No self-transaction
DB_WriteRecord2(&data2, FALSE);  // No self-transaction
DB_EndBookEnd();                 // Commits all or rolls back all
```

## API Reference

### Spectrum Operations

| Function | Header Line | Description |
|----------|-------------|-------------|
| `DB_WriteSpectrum(DB_SPEC*, bookEnd)` | `database.h:25` | Save MCA spectrum data |
| `DB_ReadSpectrum(DB_SPEC*)` | `database.h:26` | Load spectrum by ID |

### Wipe Test Operations

| Function | Header Line | Description |
|----------|-------------|-------------|
| `DB_SearchWipes(WELLWIPESEARCH*, ...)` | `database.h:69` | Search wipes by date range |
| `DB_RetrieveWipe(...)` | `database.h:70` | Load full wipe record |
| `DB_InactivateWipe(...)` | `database.h:71` | Soft-delete a wipe record |

### System Test Operations (Header: `database.h:73-76`)

- `DB_CreateWellSystemTest()` - Save system test result
- `DB_SearchSystemTests()` - Search by date range
- `DB_RetrieveSystemTest()` - Load full record
- `DB_InactivateSystemTest()` - Soft-delete a system test

### Lab Test Operations

| Test Type | Functions | Header Lines |
|-----------|-----------|-------------|
| Schilling | `DB_CreateSchillingTest`, `DB_SearchSchillingTests`, `DB_RetrieveSchillingTest` | `database.h:90-93` |
| Plasma | `DB_CreatePlasmaTest`, `DB_SearchPlasmaTests`, `DB_RetrievePlasmaTest` | `database.h:95-98` |
| RBC | `DB_CreateRBCTest`, `DB_SearchRBCTests`, `DB_RetrieveRBCTest` | `database.h:100-103` |
| MDA | `DB_CreateMDATest`, `DB_SearchMDATests`, `DB_RetrieveMDATest` | `database.h:78-81` |
| Chi | `DB_CreateChiTest`, `DB_SearchChiTests`, `DB_RetrieveChiTest` | `database.h:83-86` |
| Bioassay | `DB_WriteProbeBioAssayTest`, `DB_SearchBioAssayTests`, `DB_RetrieveBioAssayTest` | `database.h:126-129` |
| Thyroid Uptake | `DB_WriteProbeThyroidUptakeTest`, `DB_SearchThyroidUptakeTests` | `database.h:140-143` |
| RBC Survival | `DB_WriteRBCSurvivalTest`, `DB_SearchRBCSurvivalTests` | `database.h:158-160` |

### Chamber Test Operations

| Function | Line | Description |
|----------|------|-------------|
| `DB_CreateChamberDailyTest()` | `Database.c` | Save daily QC test |
| `DB_CreateChamberAutoConstancy()` | `Database.c:13267` | Save auto-constancy result |
| `DB_SearchDailyTests()` | `Database.c:13316` | Search daily tests by date |

### Configuration Operations

| Function | Header Line | Description |
|----------|-------------|-------------|
| `DB_InitializeDetector()` | `database.h` | Write detector config to DB |
| Config read/write | Various | Settings stored in `Config` table as key-value pairs |

### Auto-Calibration Operations

- `DB_WriteAutoCal()` / `DB_ReadAutoCal()` - MCA auto-calibration data
- `DB_SearchAutoCal()` - Search calibration history

## Edge Cases

- **SD card failure:** If `/data` directory cannot be created (`Database.c:209-222`), `flgDirOK` is set FALSE and database operations will fail silently
- **Journal recovery:** On startup, stale `.db-journal` files are deleted if main DB is missing (`Database.c:231`)
- **Temp DB pattern:** New databases are created as `temp.db` first, then renamed to `capintec.db` after schema creation succeeds (`Database.c:239`)
- **Memory allocation:** `sCommand` buffer is `malloc(32768)` for SQL construction (`Database.c:203`) - large queries possible
- **Transaction nesting:** BookEnd pattern does NOT support nested transactions. Calling `DB_StartBookEnd()` twice without `DB_EndBookEnd()` will fail
- **Status checking:** `capStatus` is checked after each operation; ROLLBACK resets it to SQLITE_OK (`Database.c:13292-13294`)

## Dependencies

- **SQLite3:** `sqlite3.c` (4.1MB) - embedded in project, single compilation unit
- **FatFS:** File system access for SD card (`ff.c`, `ff.h`)
- **Headers:** `database.h`, `crc.h` (for struct types), `well.h`, `linearity.h`
