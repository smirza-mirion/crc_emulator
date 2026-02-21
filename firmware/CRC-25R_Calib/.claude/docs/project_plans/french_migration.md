# French Migration Plan — CRC-25R Firmware v4.07a

## 1. Executive Summary

Replace all Spanish localization with French in the CRC-25R calibrator firmware, producing an English+French build. The codebase remains strictly dual-language: array index 0 = English, index 1 = French (formerly Spanish). No third language is added.

**Scope**: 934 UI string pairs, 30 button images, 197 HTML screen files, 8 database text column pairs, hardcoded strings in ~161 menu controller files, and printer output formatting.

---

## 2. Current Localization Architecture

### 2.1 String Storage (LanguageStrings.c)

The localization system uses a flat char array with byte-level offset indexing:

- **`lang_str[]`** (LanguageStrings.c:3-1786): Flat array of individual char literals storing all 934 string pairs interleaved (English then Spanish for each L_* ID). Total size: ~34,177 bytes.
- **`lang_map[]`** (LanguageStrings.c:1789-2681): Array of 934 `LANG_MAP` structs, each containing `unsigned long int off[2]` — byte offsets into `lang_str[]` for English [0] and Spanish [1].

Storage format example (LanguageStrings.c:3-8):
```c
const char lang_str[]={
'R','e','v',' ',     // L_REVI (english) offset 0
'V','e','r',' ',     // L_REVI (spanish) offset 4
'C','O','P','Y','R','I','G','H','T',  // L_COPYRIGHT (english) offset 8
'D','e','r','e','c','h','o',' ','d','e',' ','A','u','t','o','r',  // L_COPYRIGHT (spanish) offset 17
```

### 2.2 String Retrieval

- **`get_amulet_message()`** (DisplayMessage.c:6489): Uses `lang_map[msg_id].off[current.language]` to index into `lang_str[]`
- **`current.language`** (crc.h:223): Runtime language selector — `ENGLISH=0`, `SPANISH=1`
- **Language enum** (crc.h:195-199): `enum language { ENGLISH, SPANISH };`

### 2.3 Source of Truth

**DisplayMessage.c:23-422+** contains a commented-out `amulet_messages[]` array that serves as the **source of truth** for all translations. A build tool reads this and generates `LanguageStrings.c`. Format:
```c
L_REVI, "Rev ", "Ver ",
L_COPYRIGHT, "COPYRIGHT", "Derecho de Autor",
L_UPDATE_SYSTEM, "Update System", "Actualización del Sistema",
```

### 2.4 Database Bilingual Fields

Eight struct field pairs store bilingual text for database records (all `char[40]`):

| Struct Location (crc.h) | English Field | Spanish Field |
|---|---|---|
| Line 1436-1437 | `DataCheckTextEnglish[40]` | `DataCheckTextSpanish[40]` |
| Line 1456-1457 | `ZeroTextEnglish[40]` | `ZeroTextSpanish[40]` |
| Line 1472-1473 | `BackgroundTextEnglish[40]` | `BackgroundTextSpanish[40]` |
| Line 1491-1492 | `VoltageTextEnglish[40]` | `VoltageTextSpanish[40]` |
| Line 1524-1525 | `CalculatedActivityTextEnglish[40]` | `CalculatedActivityTextSpanish[40]` |
| Line 1530-1531 | `MeasuredActivityTextEnglish[40]` | `MeasuredActivityTextSpanish[40]` |
| Line 1534-1535 | `DeviationTextEnglish[40]` | `DeviationTextSpanish[40]` |
| Line 1548-1549 | `ConstancyActivityTextEnglish[40]` | `ConstancyActivityTextSpanish[40]` |

### 2.5 Button Images

30 Spanish-variant button images in `CRCHtml_3.00a/Images/` with `spanish_` prefix:
```
spanish_accuracy_down.png    spanish_accuracy_up.png
spanish_back_big.png         spanish_back_small.png
spanish_background_down.png  spanish_background_up.png
spanish_chamber_volts_down.png spanish_chamber_volts_up.png
spanish_daily_down.png       spanish_daily_up.png
spanish_enhanced_down.png    spanish_enhanced_up.png
spanish_home_big.png         spanish_home_big.png.old
spanish_home_small.png       spanish_home_small.png.old
spanish_inactivate.png       spanish_inactivate2.png
spanish_info_down.png        spanish_info_up.png
spanish_inventory_down.png   spanish_inventory_up.png
spanish_moly_down.png        spanish_moly_up.png
spanish_print_33.png         spanish_print_small.png
spanish_save.png             spanish_setup_33.png
spanish_setup_small.png      spanish_well_33.png
```

### 2.6 HTML Screens

197 `.htm` files in `CRCHtml_3.00a/` reference Spanish images via:
```html
<PARAM NAME="upImage" VALUE="Images/spanish_home_big.png">
```
Language switching uses Byte trigger 92: `TRIGGER=0x00` (English), `TRIGGER=0x01` (Spanish).

---

## 3. Buffer Overflow Risk Analysis

### 3.1 Critical Buffer Sizes

French text is typically 15-30% longer than English. The following buffer sizes constrain translations:

| Buffer | Size | Location | Risk Level |
|---|---|---|---|
| `actstr[20]` | 20 chars | MEASUREMENT struct | **HIGH** — very tight for French |
| `acMsg[25]` | 25 chars | Various Amulet*Menu.c | **HIGH** — minimal headroom |
| `m_acTitle[51]` | 51 chars | Screen title buffers | **MEDIUM** — adequate for most |
| `str[2][100]` | 100 chars | AMULET_MESSAGE | **LOW** — generous |
| `*Text*[40]` | 40 chars | Database columns | **MEDIUM** — must verify each |
| Printer lines | 37/70/78 | Printer output | **HIGH** — fixed hardware widths |

### 3.2 Dynamic String Concatenation Patterns

Many menu files build strings via `sprintf`/`strcat` combining localized text with runtime data:
```c
sprintf(acMsg, "%s: %s", get_amulet_message(L_ACTIVITY, buf1), actstr);
```

**Risk**: If French base string is longer than Spanish, the combined string may overflow `acMsg`. Every `sprintf`/`strcat` call using localized strings must be audited.

### 3.3 Abbreviation Strategy

Where French exceeds buffer limits, use standard French abbreviations:
- "Mesure" → "Mes." (for 20-char buffers)
- "Vérification" → "Vérif."
- "Configuration" → "Config."
- "Informations" → "Info."
- "Résultats" → "Rés."
- "Arrière-plan" (background) → "Fond"
- "Précision" (accuracy) → "Préc."

Each abbreviation must be flagged in the QA CSV for human review.

---

## 4. Extended ASCII Support (0x80-0xFF)

### 4.1 Required French Characters

| Character | Code Point | Already Used (Spanish) |
|---|---|---|
| é (e-acute) | 0xE9 | Yes — Spanish uses this |
| è (e-grave) | 0xE8 | No |
| ê (e-circumflex) | 0xEA | No |
| ë (e-diaeresis) | 0xEB | No |
| à (a-grave) | 0xE0 | No |
| â (a-circumflex) | 0xE2 | No |
| ç (c-cedilla) | 0xE7 | No |
| ù (u-grave) | 0xF9 | No |
| û (u-circumflex) | 0xFB | No |
| ô (o-circumflex) | 0xF4 | No |
| î (i-circumflex) | 0xEE | No |
| ï (i-diaeresis) | 0xEF | No |

### 4.2 Verification Required

Spanish already uses extended ASCII (é, á, í, ó, ú, ñ, ü) stored as direct char literals in `lang_str[]`. French needs additional characters not used by Spanish (è, ê, ë, à, â, ç, ù, û, ô, î, ï). Must verify:

1. **Amulet font files** (`.auf` in CRCHtml_3.00a/) contain glyphs for all required French characters
2. **Compiler** handles 0x80-0xFF char literals correctly (already confirmed for Spanish chars)
3. **Printer fonts** support French accented characters
4. **Database** (SQLite) stores/retrieves extended ASCII correctly (already works for Spanish)

### 4.3 Risk Mitigation

If any French character is missing from Amulet fonts, options are:
- Use unaccented equivalent (acceptable for some contexts)
- Modify the .auf font file to add the glyph
- Flag for human decision in QA report

---

## 5. Implementation Phases

### Phase 0: Backup & Branch Setup

**Actions:**
1. Create backup branch: `git branch backup/english-spanish-v4.07a`
2. Create working branch: `git checkout -b feature/french-migration`
3. Push both: `git push -u origin backup/english-spanish-v4.07a && git push -u origin feature/french-migration`

**Verification:** Both branches visible on GitHub. `backup/english-spanish-v4.07a` preserves the complete English+Spanish codebase.

---

### Phase 1: Structural Rename (Spanish → French)

**Goal:** Rename all Spanish identifiers, enum values, struct fields, and database columns to French equivalents without changing any string content yet.

**Files Modified:**

| File | Change | Lines |
|---|---|---|
| `Headers/crc.h` | `SPANISH` → `FRENCH` in enum | 195-199 |
| `Headers/crc.h` | 8x `*TextSpanish` → `*TextFrench` | 1436-1549 |
| `Headers/message.h` | `L_SPANISH` → `L_FRENCH` | 45 |
| `Database.c` | All SQL column names `*TextSpanish` → `*TextFrench` | Multiple |
| `Headers/database.h` | Function signatures if any reference Spanish | Review needed |
| All `.c/.h` files | Any remaining `Spanish`/`spanish`/`SPANISH` identifiers | Grep audit |

**Verification:** Full `grep -ri "spanish"` across all `.c` and `.h` files returns zero hits (excluding image file references and comments explaining the migration).

**Git:** Commit + push after phase completion.

---

### Phase 2: String Translation (DisplayMessage.c — Source of Truth)

**Goal:** Replace all 934 Spanish strings with French translations in the `amulet_messages[]` comment block.

**Process:**
1. Extract all entries from DisplayMessage.c:23-422+ into a working CSV
2. Columns: `L_ID | English | Spanish | French | EnLen | SpLen | FrLen | BufferLimit | Risk`
3. Translate each Spanish string to French
4. Compute risk ranking per entry:
   - **HIGH**: French length > smallest known buffer that uses this string
   - **MEDIUM**: French length > Spanish length but within buffer limits
   - **LOW**: French length <= Spanish length
5. Apply abbreviations where HIGH risk is identified
6. Replace Spanish column in `amulet_messages[]` with French translations
7. Rename comment references from "spanish" to "french"

**Human Oversight Required:**
- All HIGH-risk entries (French longer than buffer allows)
- Medical/regulatory terminology (nuclide names, test procedures, compliance text)
- Abbreviations that may be ambiguous in clinical context
- Entries containing measurement units or formatting codes

**QA CSV Output:** `french_migration_qa_report.csv` with all 934 entries and risk rankings.

**Git:** Commit + push after phase completion.

---

### Phase 3: Regenerate LanguageStrings.c

**Goal:** Rebuild the `lang_str[]` char array and `lang_map[]` offset table from the updated DisplayMessage.c.

**Approach A — Build Tool Available:**
If the original build tool that generates LanguageStrings.c from the amulet_messages[] block is available, run it against the updated DisplayMessage.c.

**Approach B — Manual Regeneration:**
If the build tool is unavailable, write a script to:
1. Parse each `L_ID, "English", "French"` entry from DisplayMessage.c
2. Convert each string to char-by-char format: `'R','e','v',' ',`
3. Compute cumulative byte offsets for `lang_map[]`
4. Handle extended ASCII chars (é, è, ê, etc.) as single-byte 0x80-0xFF literals
5. Output complete LanguageStrings.c with correct offsets

**Verification:**
- Total entries in `lang_map[]` = 934 (matches NUM_AMULET_STRINGS)
- Each offset in `lang_map[]` points to correct string start in `lang_str[]`
- No off-by-one errors in offset computation
- Extended ASCII characters preserved correctly
- Final offset matches total `lang_str[]` size

**Git:** Commit + push after phase completion.

---

### Phase 4: Database Text Fields

**Goal:** Update all hardcoded Spanish text values in Database.c to French.

**Scope:**
- All INSERT statements populating `*TextFrench` columns (formerly `*TextSpanish`)
- Default values for the 8 bilingual field pairs
- Any string literals used in database queries referencing Spanish text

**Buffer Constraint:** All database text fields are `char[40]`. Every French translation must be verified ≤ 39 characters (plus null terminator).

**Verification:**
- `grep -n "TextFrench" Database.c` shows all references are syntactically correct
- No French string exceeds 39 characters
- Database schema matches struct definitions in crc.h

**Git:** Commit + push after phase completion.

---

### Phase 5: Button Images

**Goal:** Create 30 French button images replacing Spanish text with French equivalents.

**Process:**
1. For each `spanish_*.png`, identify the English equivalent image
2. Determine the text rendered on the button
3. Create `french_*.png` with French text, matching exact pixel dimensions
4. Use same font, colors, and layout as English/Spanish originals
5. Place all 30 images in `CRCHtml_3.00a/Images/`

**Image Mapping (30 files):**

| Spanish Image | Button Text (English) | French Text | Size Match |
|---|---|---|---|
| `spanish_accuracy_down.png` | Accuracy | Précision | Must match |
| `spanish_accuracy_up.png` | Accuracy | Précision | Must match |
| `spanish_back_big.png` | Back | Retour | Must match |
| `spanish_back_small.png` | Back | Retour | Must match |
| `spanish_background_down.png` | Background | Fond | Must match |
| `spanish_background_up.png` | Background | Fond | Must match |
| `spanish_chamber_volts_down.png` | Chamber Volts | Tension | Must match |
| `spanish_chamber_volts_up.png` | Chamber Volts | Tension | Must match |
| `spanish_daily_down.png` | Daily | Quotidien | Must match |
| `spanish_daily_up.png` | Daily | Quotidien | Must match |
| `spanish_enhanced_down.png` | Enhanced | Amélioré | Must match |
| `spanish_enhanced_up.png` | Enhanced | Amélioré | Must match |
| `spanish_home_big.png` | Home | Accueil | Must match |
| `spanish_home_small.png` | Home | Accueil | Must match |
| `spanish_inactivate.png` | Inactivate | Désactiver | Must match |
| `spanish_inactivate2.png` | Inactivate | Désactiver | Must match |
| `spanish_info_down.png` | Info | Info | Must match |
| `spanish_info_up.png` | Info | Info | Must match |
| `spanish_inventory_down.png` | Inventory | Inventaire | Must match |
| `spanish_inventory_up.png` | Inventory | Inventaire | Must match |
| `spanish_moly_down.png` | Moly | Moly | Must match |
| `spanish_moly_up.png` | Moly | Moly | Must match |
| `spanish_print_33.png` | Print | Imprimer | Must match |
| `spanish_print_small.png` | Print | Imprimer | Must match |
| `spanish_save.png` | Save | Sauver | Must match |
| `spanish_setup_33.png` | Setup | Config. | Must match |
| `spanish_setup_small.png` | Setup | Config. | Must match |
| `spanish_well_33.png` | Well | Puits | Must match |

**Note:** `spanish_home_big.png.old` and `spanish_home_small.png.old` are legacy backups — create `french_` versions of the current `.png` files only.

**Human Oversight Required:** All 30 images need visual review for text fit, readability, and correctness.

**Git:** Commit + push after phase completion.

---

### Phase 6: HTML Screen Updates

**Goal:** Update all 197 `.htm` files to reference `french_` images instead of `spanish_`.

**Changes:**
1. Global find/replace in all `.htm` files: `spanish_` → `french_` in image paths
2. Update comments referencing "Spanish" to "French" where applicable
3. Byte trigger 92 value `0x01` remains unchanged (it means "non-English" language) — only update comments

**Verification:**
- `grep -r "spanish_" CRCHtml_3.00a/*.htm` returns zero hits
- All `french_` image references point to files that exist in Images/
- No broken image paths

**Git:** Commit + push after phase completion.

---

### Phase 7: Hardcoded Strings & Dynamic Concatenation Audit

**Goal:** Find and update any hardcoded Spanish strings outside the L_* localization system, and verify buffer safety for dynamic string assembly.

**7.1 Hardcoded String Audit:**
Search all `.c` files for:
- Spanish words in string literals (e.g., `"Actividad"`, `"Sistema"`, `"Configuración"`)
- Comments containing Spanish that should be updated
- `case SPANISH:` or `if (current.language == SPANISH)` branches

**7.2 Dynamic Concatenation Audit:**
For every `sprintf`/`strcat`/`strcpy` that incorporates a localized string:
1. Identify the destination buffer size
2. Calculate maximum French string length for all L_* IDs used
3. Calculate maximum combined output length (localized text + data values)
4. Flag any case where combined length could exceed buffer

**High-Risk Patterns:**
```c
// Pattern 1: Title + value in small buffer
sprintf(acMsg, "%s: %s", localizedTitle, value);  // acMsg[25]

// Pattern 2: Multiple concatenations
strcat(message, localizedStr1);
strcat(message, " ");
strcat(message, localizedStr2);  // cumulative overflow risk

// Pattern 3: Printer line formatting
sprintf(line, "%-20s %10s", localizedLabel, value);  // fixed-width columns
```

**7.3 Printer Output:**
- Slip/label printer: 37 chars per line — French labels must fit
- USB EPS printer: 70 chars per line
- OKI/LX/USB printers: 78 chars per line

**Git:** Commit + push after phase completion.

---

### Phase 8: Extended ASCII Verification

**Goal:** Confirm all French accented characters render correctly on the Amulet touchscreen and printers.

**Actions:**
1. Inventory all `.auf` font files in `CRCHtml_3.00a/`
2. Check each required French character (see Section 4.1) exists in font glyph set
3. Test character rendering on target hardware (or emulator)
4. Verify printer character set supports French accents
5. Document any missing characters and mitigation strategy

**Git:** Commit + push after phase completion.

---

### Phase 9: QA Report & Final Verification

**Goal:** Produce comprehensive QA report and verify completeness.

**9.1 CSV QA Report (`french_migration_qa_report.csv`):**

| Column | Description |
|---|---|
| `L_ID` | Localization constant name |
| `English` | English string (unchanged) |
| `Spanish_Original` | Original Spanish string (for reference) |
| `French` | New French translation |
| `English_Len` | English string length |
| `Spanish_Len` | Original Spanish length |
| `French_Len` | French string length |
| `Min_Buffer` | Smallest buffer this string may occupy |
| `Risk` | HIGH / MEDIUM / LOW |
| `Abbreviated` | Yes/No — was abbreviation applied |
| `Human_Review` | Reason flagged for human review (if any) |
| `Notes` | Additional context |

**9.2 Risk Ranking Criteria:**
- **HIGH**: French length exceeds known buffer limit, OR medical/regulatory term, OR abbreviation applied
- **MEDIUM**: French length > Spanish length but within limits, OR dynamic concatenation context
- **LOW**: French length ≤ Spanish length and no special concerns

**9.3 Final Verification Checklist:**
- [ ] `grep -ri "spanish" *.c *.h` returns zero hits (excluding comments explaining migration)
- [ ] `grep -ri "spanish_" CRCHtml_3.00a/*.htm` returns zero hits
- [ ] All 30 `french_*.png` images exist and match dimensions
- [ ] `lang_map[]` has exactly 934 entries
- [ ] All `lang_map[]` offsets are correct and sequential
- [ ] No `char[40]` database field contains French text > 39 chars
- [ ] No `acMsg[25]` pattern overflows with French text
- [ ] No `actstr[20]` pattern overflows with French text
- [ ] Extended ASCII characters confirmed in Amulet fonts
- [ ] QA CSV report generated and reviewed

**Git:** Commit + push after phase completion.

---

### Phase 10: Final Cleanup & Tagging

**Actions:**
1. Remove `spanish_*.png` image files (replaced by `french_*.png`)
2. Remove `spanish_*.png.old` legacy files
3. Update `CLAUDE.md` to reflect French migration completion
4. Update `.claude/docs/architectural_patterns.md` localization section
5. Final commit + push
6. Tag release: `git tag -a v4.07a-french -m "English+French migration complete"`
7. Push tag: `git push origin v4.07a-french`

---

## 6. Critical Tests

### 6.1 Compile-Time Checks
- Clean compilation with zero warnings related to renamed identifiers
- No unresolved symbols from `SPANISH` → `FRENCH` rename
- String literal encoding correct for all extended ASCII

### 6.2 Runtime Checks (on target hardware or emulator)
- Language toggle switches between English and French correctly
- All 197 screens display French text without truncation
- All 30 French button images render correctly
- Database read/write of French text fields works correctly
- Printer output formats correctly with French text
- Accented characters display correctly on Amulet touchscreen
- No buffer overflows in dynamic string concatenation

### 6.3 Data Integrity Checks
- Existing database records with Spanish text are handled gracefully (migration strategy for live devices)
- New records store French text correctly
- BookEnd transaction pattern still functions with renamed fields

---

## 7. Files Modified Summary

| Category | Files | Count |
|---|---|---|
| Header renames | `crc.h`, `message.h`, `database.h` | 3 |
| String translations | `DisplayMessage.c`, `LanguageStrings.c` | 2 |
| Database operations | `Database.c` | 1 |
| Button images | `french_*.png` (new) | 30 |
| HTML screens | `CRCHtml_3.00a/*.htm` | ~197 |
| Menu controllers | `Amulet*Menu.c` (hardcoded strings) | TBD (audit) |
| Documentation | `CLAUDE.md`, `.claude/docs/*` | 2+ |
| **Estimated total** | | **~235+ files** |

---

## 8. Git Workflow

Every phase ends with:
```bash
git add <changed-files>
git commit -m "Phase N: <description>"
git push origin feature/french-migration
```

After all phases complete:
```bash
git checkout master
git merge feature/french-migration
git push origin master
git tag -a v4.07a-french -m "English+French migration complete"
git push origin v4.07a-french
```

The `backup/english-spanish-v4.07a` branch is preserved permanently on remote for rollback capability.

---

## 9. Human Oversight Requirements

The following items **cannot be fully automated** and require human review:

1. **All French translations** — machine translation of medical device UI requires domain expert validation
2. **Abbreviated strings** — clinical context may make some abbreviations ambiguous
3. **Button image text** — visual layout and readability verification
4. **Regulatory/compliance text** — any strings related to safety, warnings, or regulatory requirements
5. **Extended ASCII rendering** — physical hardware testing of French accented characters
6. **Printer output** — physical print verification for all supported printer types
7. **Database migration** — strategy for devices in the field with existing Spanish data
