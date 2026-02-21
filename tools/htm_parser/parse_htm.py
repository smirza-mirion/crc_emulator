#!/usr/bin/env python3
"""
parse_htm.py - Amulet HTM Screen Parser for CRC-25R Emulator

Parses Amulet .htm UI definition files from the CRC-25R calibrator firmware
into JSON screen definitions that the web frontend can render.

The Amulet HTM format uses Java applet classes (StringField, CustomButton,
FunctionButton, RadioButton, BarGraph, Slider, CheckBox, etc.) with absolute
CSS positioning, META REFRESH triggers for reactive updates, and internalRAM
bindings for UART communication with the firmware.

Usage:
    python3 parse_htm.py [--src DIR] [--out DIR] [--copy-assets] [--assets-dest DIR]

    --src           HTM source directory (default: ../../firmware/CRCHtml_3.00a/)
    --out           JSON output directory (default: ./screen_defs/)
    --copy-assets   Copy image assets from CRCHtml_3.00a/Images/ to assets-dest
    --assets-dest   Destination for image assets (default: ../../frontend/public/assets/images/)
"""

import argparse
import json
import os
import re
import shutil
import sys
from collections import OrderedDict
from pathlib import Path


# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

KNOWN_WIDGET_CLASSES = {
    "stringfield.class": "StringField",
    "custombutton.class": "CustomButton",
    "functionbutton.class": "FunctionButton",
    "radiobutton.class": "RadioButton",
    "bargraph.class": "BarGraph",
    "slider.class": "Slider",
    "checkbox.class": "CheckBox",
    "numericfield.class": "NumericField",
    "linegraph.class": "LineGraph",
    "list.class": "List",
    "pwm.class": "Pwm",
}

# Regex patterns compiled once for performance
RE_CSS_RULE = re.compile(
    r"#([\w\-]+)\s*\{([^}]*)\}",
    re.IGNORECASE,
)

RE_CSS_PROP = re.compile(
    r"(position|left|top|width|height)\s*:\s*([^;}\s]+)",
    re.IGNORECASE,
)

# Match an opening APPLET tag, capturing CODE, WIDTH, HEIGHT, NAME.
# Case-insensitive attribute matching; handles ordering variations.
RE_APPLET_OPEN = re.compile(
    r"<APPLET\b([^>]*)>",
    re.IGNORECASE,
)

RE_APPLET_CLOSE = re.compile(
    r"</APPLET\s*>",
    re.IGNORECASE,
)

RE_PARAM = re.compile(
    r'<PARAM\s+NAME\s*=\s*"([^"]*)"\s+VALUE\s*=\s*"([^"]*)"',
    re.IGNORECASE,
)

# Also handle VALUE before NAME (some files may vary)
RE_PARAM_ALT = re.compile(
    r'<PARAM\s+VALUE\s*=\s*"([^"]*)"\s+NAME\s*=\s*"([^"]*)"',
    re.IGNORECASE,
)

RE_DIV_OPEN = re.compile(
    r'<DIV\s+ID\s*=\s*"([^"]*)"',
    re.IGNORECASE,
)

RE_DIV_CLOSE = re.compile(
    r"</DIV",
    re.IGNORECASE,
)

# META REFRESH trigger pattern. The CONTENT attribute spans across newlines.
# We match from <META HTTP-EQUIV="REFRESH" to the closing > accounting for
# multiline content values.
RE_META_REFRESH = re.compile(
    r'<META\s+HTTP-EQUIV\s*=\s*"REFRESH"\s+CONTENT\s*=\s*"((?:[^"\\]|"")*)"[^>]*>',
    re.IGNORECASE | re.DOTALL,
)

# Named IMG elements: <IMG SRC="..." WIDTH="..." HEIGHT="..." INVISIBLE="..." NAME="...">
RE_NAMED_IMG = re.compile(
    r"<IMG\b([^>]*)>",
    re.IGNORECASE,
)

# Generic attribute extractor
RE_ATTR = re.compile(
    r'(\w+)\s*=\s*"([^"]*)"',
    re.IGNORECASE,
)

# META directives (non-REFRESH): Amulet config, Macro, cachedImg, etc.
RE_META_AMULET = re.compile(
    r'<META\s+(?:Name|NAME)\s*=\s*"([^"]*)"\s+CONTENT\s*=\s*"([^"]*)"',
    re.IGNORECASE,
)

# Variable binding patterns
RE_IRAM_BINDING = re.compile(
    r"Amulet:(?:i|I)nternal(?:RAM|Ram|ram)\.(byte|word|string|label|bytes)\((\d+)\)",
    re.IGNORECASE,
)

RE_UART_BINDING = re.compile(
    r"Amulet:UART\.(byte|word)\((\d+)\)",
    re.IGNORECASE,
)


# ---------------------------------------------------------------------------
# CSS Parser
# ---------------------------------------------------------------------------

def parse_css_positions(content):
    """
    Parse CSS <style> blocks to extract element position mappings.
    Returns dict: css_id -> {left, top, width, height}
    """
    positions = {}

    for match in RE_CSS_RULE.finditer(content):
        css_id = match.group(1)
        props_str = match.group(2)

        props = {}
        for prop_match in RE_CSS_PROP.finditer(props_str):
            key = prop_match.group(1).lower()
            val = prop_match.group(2).strip()
            # Parse numeric values; some may be e.g. "0px" or just "0"
            numeric = re.match(r"^(-?\d+)", val)
            if numeric:
                props[key] = int(numeric.group(1))
            else:
                props[key] = val

        if props:
            positions[css_id] = props

    return positions


# ---------------------------------------------------------------------------
# APPLET / Widget Parser
# ---------------------------------------------------------------------------

def extract_applet_attrs(tag_content):
    """Extract CODE, WIDTH, HEIGHT, NAME from an APPLET tag's attribute string."""
    attrs = {}
    for m in RE_ATTR.finditer(tag_content):
        key = m.group(1).upper()
        attrs[key] = m.group(2)
    return attrs


def extract_params(block):
    """Extract all PARAM NAME/VALUE pairs from an APPLET block."""
    params = OrderedDict()

    for m in RE_PARAM.finditer(block):
        name = m.group(1)
        value = m.group(2)
        params[name] = value

    # Also check alternative ordering
    for m in RE_PARAM_ALT.finditer(block):
        value = m.group(1)
        name = m.group(2)
        if name not in params:
            params[name] = value

    return params


def parse_widgets(content, css_positions):
    """
    Parse all APPLET widgets from the BODY of the HTM file.

    Returns a list of widget dicts with type, name, position, dimensions,
    params, and language variant detection.
    """
    widgets = []

    # We need to track DIV -> APPLET relationships for position resolution.
    # Strategy: find each APPLET block, then look backwards for its parent DIV ID.

    # First, strip HTML comments to avoid parsing commented-out elements.
    # But preserve the structure for position tracking.
    clean = _strip_html_comments(content)

    # Find all DIV+APPLET blocks
    # We iterate through the clean content tracking DIV contexts.
    pos = 0
    div_stack = []

    while pos < len(clean):
        # Check for DIV open
        div_match = RE_DIV_OPEN.search(clean, pos)
        applet_match = RE_APPLET_OPEN.search(clean, pos)

        # Find the earliest match
        next_div_pos = div_match.start() if div_match else len(clean)
        next_applet_pos = applet_match.start() if applet_match else len(clean)

        if next_div_pos < next_applet_pos:
            div_id = div_match.group(1)
            div_stack.append(div_id)
            pos = div_match.end()

            # Check for named IMG inside this DIV before next APPLET or DIV close
            _scan_for_named_imgs(clean, pos, div_id, css_positions, widgets)

        elif next_applet_pos < len(clean):
            # Found an APPLET tag
            applet_attrs = extract_applet_attrs(applet_match.group(1))

            # Find the closing </APPLET>
            close_match = RE_APPLET_CLOSE.search(clean, applet_match.end())
            if close_match:
                applet_block = clean[applet_match.start():close_match.end()]
                params = extract_params(applet_block)

                # Determine widget type
                code = applet_attrs.get("CODE", "").lower()
                widget_type = KNOWN_WIDGET_CLASSES.get(code, code.replace(".class", ""))

                name = applet_attrs.get("NAME", "")
                width = _safe_int(applet_attrs.get("WIDTH", "0"))
                height = _safe_int(applet_attrs.get("HEIGHT", "0"))

                # Resolve position from parent DIV's CSS ID
                parent_div_id = div_stack[-1] if div_stack else None
                x, y = 0, 0
                if parent_div_id and parent_div_id in css_positions:
                    css_props = css_positions[parent_div_id]
                    x = css_props.get("left", 0)
                    y = css_props.get("top", 0)

                # Detect language variant
                is_french = _detect_french_variant(name, params)

                # Extract variable bindings
                bindings = _extract_bindings(params)

                widget = OrderedDict()
                widget["type"] = widget_type
                widget["name"] = name
                widget["x"] = x
                widget["y"] = y
                widget["width"] = width
                widget["height"] = height
                widget["params"] = dict(params)

                if parent_div_id:
                    widget["position_from"] = parent_div_id

                if is_french:
                    widget["language"] = "french"

                if bindings:
                    widget["bindings"] = bindings

                widgets.append(widget)

                pos = close_match.end()
            else:
                # Malformed - no closing tag, skip
                pos = applet_match.end()
        else:
            # No more DIVs or APPLETs
            break

        # Also check for DIV closes to pop the stack
        # Look for any </DIV> between old pos and the match we just processed
        while div_stack:
            close_div = RE_DIV_CLOSE.search(clean, pos)
            next_open_div = RE_DIV_OPEN.search(clean, pos)
            next_open_app = RE_APPLET_OPEN.search(clean, pos)

            close_pos = close_div.start() if close_div else len(clean)
            open_div_pos = next_open_div.start() if next_open_div else len(clean)
            open_app_pos = next_open_app.start() if next_open_app else len(clean)

            if close_pos < open_div_pos and close_pos < open_app_pos:
                div_stack.pop() if div_stack else None
                pos = close_div.end()
            else:
                break

    return widgets


def _scan_for_named_imgs(content, start_pos, div_id, css_positions, widgets):
    """
    Scan for named IMG elements within a DIV that are not inside APPLETs.
    These are standalone images like horizontal line separators.
    """
    # Look for IMG tags before the next APPLET or DIV close
    search_end_applet = RE_APPLET_OPEN.search(content, start_pos)
    search_end_div = RE_DIV_CLOSE.search(content, start_pos)

    end_pos = len(content)
    if search_end_applet:
        end_pos = min(end_pos, search_end_applet.start())
    if search_end_div:
        end_pos = min(end_pos, search_end_div.start())

    region = content[start_pos:end_pos]

    for img_match in RE_NAMED_IMG.finditer(region):
        attrs_str = img_match.group(1)
        attrs = {}
        for m in RE_ATTR.finditer(attrs_str):
            attrs[m.group(1).upper()] = m.group(2)

        if "NAME" in attrs and "SRC" in attrs:
            name = attrs["NAME"]
            src = attrs["SRC"]
            width = _safe_int(attrs.get("WIDTH", "0"))
            height = _safe_int(attrs.get("HEIGHT", "0"))
            invisible = attrs.get("INVISIBLE", "FALSE").upper() == "TRUE"

            x, y = 0, 0
            if div_id in css_positions:
                css_props = css_positions[div_id]
                x = css_props.get("left", 0)
                y = css_props.get("top", 0)

            widget = OrderedDict()
            widget["type"] = "Image"
            widget["name"] = name
            widget["x"] = x
            widget["y"] = y
            widget["width"] = width
            widget["height"] = height
            widget["params"] = {"src": src, "invisible": "TRUE" if invisible else "FALSE"}
            widget["position_from"] = div_id

            widgets.append(widget)


def _detect_french_variant(name, params):
    """Detect if a widget is a French language variant."""
    name_lower = name.lower()
    if "french" in name_lower:
        return True

    # Check for french_ prefix in images
    for key in ("upImage", "downImage"):
        val = params.get(key, "")
        if "french_" in val.lower():
            return True

    return False


def _extract_bindings(params):
    """Extract internalRAM and UART variable bindings from widget params."""
    bindings = []

    for key, value in params.items():
        # Check for internalRAM bindings
        for m in RE_IRAM_BINDING.finditer(value):
            var_type = m.group(1).lower()
            index = int(m.group(2))
            bindings.append({
                "source": "internalRAM",
                "type": var_type,
                "index": index,
                "param": key,
            })

        # Check for UART bindings
        for m in RE_UART_BINDING.finditer(value):
            var_type = m.group(1).lower()
            index = int(m.group(2))
            bindings.append({
                "source": "UART",
                "type": var_type,
                "index": index,
                "param": key,
            })

    return bindings if bindings else None


# ---------------------------------------------------------------------------
# META REFRESH Parser
# ---------------------------------------------------------------------------

def parse_meta_refreshes(content):
    """
    Parse META REFRESH triggers from the HEAD section.
    Handles multiline CONTENT attributes with ONVAR/TRIGGER/URL/NAME fields.

    Returns a list of refresh trigger dicts.
    """
    triggers = []

    # Strip HTML comments first
    clean = _strip_html_comments(content)

    # The META REFRESH tags often span multiple lines with the CONTENT attribute
    # containing semicolons and whitespace. We need a more robust approach.
    # Re-join lines and parse the collapsed content.
    collapsed = _collapse_whitespace(clean)

    for match in RE_META_REFRESH.finditer(collapsed):
        content_val = match.group(1).strip()
        trigger = _parse_refresh_content(content_val)
        if trigger:
            triggers.append(trigger)

    return triggers


def _parse_refresh_content(content_val):
    """
    Parse a META REFRESH CONTENT attribute value.

    Formats observed:
    1. Simple timer: "0.0, 0.01; URL=...; NAME=..."
    2. Variable trigger: "0.01;ONVAR=...;TRIGGER=0xFF;URL=...;NAME=..."
    3. Combined: "0.00,0.01;URL=...; NAME=..."

    The URL field contains comma-separated Amulet action chains.
    """
    trigger = OrderedDict()

    # Extract NAME first (always at the end, after last semicolon)
    name_match = re.search(r"NAME\s*=\s*(\S+)", content_val, re.IGNORECASE)
    if name_match:
        trigger["name"] = name_match.group(1).rstrip(";").strip()

    # Extract ONVAR
    onvar_match = re.search(r"ONVAR\s*=\s*(Amulet:[^;]+)", content_val, re.IGNORECASE)
    if onvar_match:
        onvar_raw = onvar_match.group(1).strip()
        # Normalize: strip "Amulet:" prefix and ".value()" suffix for the var path
        var_path = _normalize_var_path(onvar_raw)
        trigger["onVar"] = var_path

    # Extract TRIGGER value
    trigger_match = re.search(r"TRIGGER\s*=\s*(0x[0-9a-fA-F]+|\d+)", content_val, re.IGNORECASE)
    if trigger_match:
        trigger["trigger"] = trigger_match.group(1)

    # Extract interval (the leading numeric value(s))
    interval_match = re.match(r"^\s*([\d.]+(?:\s*,\s*[\d.]+)?)\s*[;,]", content_val)
    if interval_match:
        trigger["interval"] = interval_match.group(1).replace(" ", "")

    # Extract URL actions
    url_match = re.search(r"URL\s*=\s*(.*?)(?:;\s*(?:NAME|ONVAR|TRIGGER)\s*=|$)", content_val, re.IGNORECASE)
    if url_match:
        url_raw = url_match.group(1).strip().rstrip(";").strip()
        actions = _parse_url_actions(url_raw)
        if actions:
            trigger["actions"] = actions

    return trigger if trigger else None


def _normalize_var_path(raw):
    """
    Normalize an Amulet variable reference.
    'Amulet:internalRAM.byte(20).value()' -> 'internalRAM.byte(20)'
    """
    # Remove 'Amulet:' prefix
    path = raw
    if path.lower().startswith("amulet:"):
        path = path[7:]

    # Remove trailing .value(), .setValue(...), etc.
    path = re.sub(r"\.\w+\([^)]*\)\s*$", "", path)

    return path.strip()


def _parse_url_actions(url_raw):
    """
    Parse a URL action chain into individual action dicts.

    URL values are comma-separated Amulet method calls:
    - Amulet:internalRAM.byte(20).setValue(0)
    - Amulet:document.sfChamb.reappear()
    - Amulet:document.sfChamb.forceUpdate()
    - %SetScreen(%%SCREEN_DAILY)
    - %Home, %Back, %Beep
    """
    actions = []

    # Split on commas, but respect parentheses nesting
    parts = _split_action_chain(url_raw)

    for part in parts:
        part = part.strip()
        if not part:
            continue

        action = _parse_single_action(part)
        if action:
            actions.append(action)

    return actions


def _split_action_chain(chain):
    """
    Split a comma-separated action chain, respecting parentheses nesting.
    'Amulet:internalRAM.byte(20).setValue(0),Amulet:document.sfChamb.reappear()'
    -> ['Amulet:internalRAM.byte(20).setValue(0)', 'Amulet:document.sfChamb.reappear()']
    """
    parts = []
    depth = 0
    current = []

    for ch in chain:
        if ch == "(":
            depth += 1
            current.append(ch)
        elif ch == ")":
            depth -= 1
            current.append(ch)
        elif ch == "," and depth == 0:
            parts.append("".join(current))
            current = []
        else:
            current.append(ch)

    if current:
        parts.append("".join(current))

    return parts


def _parse_single_action(part):
    """Parse a single Amulet action into a structured dict."""
    part = part.strip()
    if not part:
        return None

    action = OrderedDict()

    # Macro calls: %SetScreen(...), %SetHTML(...), %SetClear(...), %Home, %Back, %Beep, etc.
    macro_match = re.match(r"^%(\w+)(?:\(([^)]*)\))?$", part)
    if macro_match:
        action["type"] = "macro"
        action["macro"] = macro_match.group(1)
        if macro_match.group(2) is not None:
            action["args"] = macro_match.group(2)
        return action

    # Amulet:document.WIDGET.METHOD(args)
    doc_match = re.match(
        r"^Amulet:document\.(\w+)\.(\w+)\(([^)]*)\)$",
        part, re.IGNORECASE,
    )
    if doc_match:
        action["type"] = doc_match.group(2)
        action["target"] = doc_match.group(1)
        arg = doc_match.group(3).strip()
        if arg:
            action["value"] = _parse_action_value(arg)
        return action

    # Amulet:document.WIDGET.METHOD()  (no args)
    doc_match_noarg = re.match(
        r"^Amulet:document\.(\w+)\.(\w+)\(\)$",
        part, re.IGNORECASE,
    )
    if doc_match_noarg:
        action["type"] = doc_match_noarg.group(2)
        action["target"] = doc_match_noarg.group(1)
        return action

    # Amulet:internalRAM.TYPE(INDEX).METHOD(VALUE)
    iram_match = re.match(
        r"^Amulet:(?:i|I)nternal(?:RAM|Ram|ram)\.(\w+)\((\d+)\)\.(\w+)\(([^)]*)\)$",
        part, re.IGNORECASE,
    )
    if iram_match:
        action["type"] = iram_match.group(3)
        action["target"] = "internalRAM.{}({})".format(iram_match.group(1), iram_match.group(2))
        arg = iram_match.group(4).strip()
        if arg:
            action["value"] = _parse_action_value(arg)
        return action

    # Amulet:UART.TYPE(INDEX).METHOD(VALUE)
    uart_match = re.match(
        r"^Amulet:UART\.(\w+)\((\d+)\)\.(\w+)\(([^)]*)\)$",
        part, re.IGNORECASE,
    )
    if uart_match:
        action["type"] = uart_match.group(3)
        action["target"] = "UART.{}({})".format(uart_match.group(1), uart_match.group(2))
        arg = uart_match.group(4).strip()
        if arg:
            action["value"] = _parse_action_value(arg)
        return action

    # Amulet:NOP()
    if re.match(r"^Amulet:(?:NOP|nop)\(\)$", part, re.IGNORECASE):
        action["type"] = "nop"
        return action

    # Amulet:back()
    if re.match(r"^Amulet:back\(\)$", part, re.IGNORECASE):
        action["type"] = "macro"
        action["macro"] = "Back"
        return action

    # Amulet:nop()
    if re.match(r"^Amulet:nop\(\)$", part, re.IGNORECASE):
        action["type"] = "nop"
        return action

    # Amulet:internal.fileNumber.value() and similar special forms
    special_match = re.match(
        r"^Amulet:(\S+)$",
        part, re.IGNORECASE,
    )
    if special_match:
        action["type"] = "raw"
        action["expression"] = special_match.group(1)
        return action

    # Page navigation: "PageName.htm"
    if part.lower().endswith(".htm"):
        action["type"] = "navigate"
        action["target"] = part
        return action

    # Fallback: raw expression
    action["type"] = "raw"
    action["expression"] = part
    return action


def _parse_action_value(arg):
    """Parse an action argument value, handling hex, decimal, and string literals."""
    arg = arg.strip()

    # Hex value
    if re.match(r"^0x[0-9a-fA-F]+$", arg):
        return arg

    # Integer
    if re.match(r"^-?\d+$", arg):
        return int(arg)

    # Float
    if re.match(r"^-?\d+\.\d+$", arg):
        return float(arg)

    # String literal in single quotes
    sq_match = re.match(r"^'(.*)'$", arg)
    if sq_match:
        return sq_match.group(1)

    # InternalRAM reference passed as argument
    if "internalram" in arg.lower() or "uart" in arg.lower():
        return arg

    return arg


# ---------------------------------------------------------------------------
# Init Actions Parser
# ---------------------------------------------------------------------------

def parse_init_actions(triggers):
    """
    Extract initialization actions from refresh triggers.

    Init triggers are those with NAME containing 'Init' or 'refreshInit',
    or those with interval starting at "0.0" or "0.00" with no ONVAR
    (meaning they fire immediately on page load).
    """
    init_actions = []

    for trigger in triggers:
        name = trigger.get("name", "")
        interval = trigger.get("interval", "")
        has_onvar = "onVar" in trigger

        is_init = False
        if "init" in name.lower():
            is_init = True
        elif not has_onvar and interval and _is_immediate_interval(interval):
            is_init = True

        if is_init and "actions" in trigger:
            for action in trigger["actions"]:
                init_actions.append(action)

    return init_actions


def _is_immediate_interval(interval):
    """Check if an interval is essentially zero (fires immediately)."""
    # Intervals like "0.0", "0.00", "0.0,0.01" are considered immediate
    parts = interval.split(",")
    try:
        first = float(parts[0].strip())
        return first < 0.02
    except (ValueError, IndexError):
        return False


# ---------------------------------------------------------------------------
# Memory Map Comment Parser
# ---------------------------------------------------------------------------

def parse_memory_map(content):
    """
    Parse the memory map comment block at the top of each HTM file.
    These document which byte/word/string RAM indices are used.
    """
    memory_map = {"bytes": {}, "words": {}, "strings": {}}

    # Find the first HTML comment block
    comment_match = re.match(r"\s*<!--(.*?)-->", content, re.DOTALL)
    if not comment_match:
        return memory_map

    comment = comment_match.group(1)
    current_section = None

    for line in comment.split("\n"):
        line = line.strip()
        if not line:
            continue

        # Section headers
        if line.lower().startswith("byte"):
            current_section = "bytes"
            continue
        elif line.lower().startswith("word"):
            current_section = "words"
            continue
        elif line.lower().startswith("string"):
            current_section = "strings"
            continue
        elif line.lower().startswith("href"):
            current_section = None  # Skip href section
            continue

        if current_section is None:
            continue

        # Parse index = description lines
        # Patterns: "20 = Show Chamber Label (TOGGLE)"
        #           "0,1 = Current Nuclide"
        #           "100 = Language   (STATE) English = 0, French = 1"
        idx_match = re.match(r"^([\d,\s]+)\s*[-=]\s*(.+)$", line)
        if idx_match:
            indices_str = idx_match.group(1).strip()
            description = idx_match.group(2).strip()

            for idx_part in indices_str.split(","):
                idx_part = idx_part.strip()
                if idx_part.isdigit():
                    memory_map[current_section][int(idx_part)] = description

    return memory_map


# ---------------------------------------------------------------------------
# Amulet Directives Parser
# ---------------------------------------------------------------------------

def parse_amulet_directives(content):
    """
    Parse META Amulet directives: ReloadInternalRAM, NoClearLCD, Baud, etc.
    """
    directives = []

    # Strip comments
    clean = _strip_html_comments(content)

    for m in RE_META_AMULET.finditer(clean):
        name = m.group(1)
        value = m.group(2)
        if name.lower() == "amulet":
            directives.append(value)

    return directives


# ---------------------------------------------------------------------------
# Static Images Parser
# ---------------------------------------------------------------------------

def parse_static_images(content):
    """
    Parse static (unnamed) IMG elements that serve as backgrounds.
    Only includes images directly in DIVs, not inside APPLETs.
    """
    images = []
    clean = _strip_html_comments(content)

    # Find DIV > IMG patterns (not inside APPLETs)
    body_match = re.search(r"<BODY[^>]*>(.*?)</BODY", clean, re.DOTALL | re.IGNORECASE)
    if not body_match:
        return images

    body = body_match.group(1)

    # Simple approach: find IMG tags not inside APPLET blocks
    # by removing APPLET blocks first
    no_applets = re.sub(r"<APPLET\b.*?</APPLET\s*>", "", body, flags=re.DOTALL | re.IGNORECASE)

    for img_match in RE_NAMED_IMG.finditer(no_applets):
        attrs_str = img_match.group(1)
        attrs = {}
        for m in RE_ATTR.finditer(attrs_str):
            attrs[m.group(1).upper()] = m.group(2)

        if "SRC" in attrs and "NAME" not in attrs:
            # This is a background/static image (no NAME means not individually addressable)
            # Try to find parent DIV ID
            # Look backwards from the IMG position in no_applets
            preceding = no_applets[:img_match.start()]
            parent_div = None
            for div_m in RE_DIV_OPEN.finditer(preceding):
                parent_div = div_m.group(1)

            img_entry = OrderedDict()
            img_entry["src"] = attrs["SRC"]
            img_entry["width"] = _safe_int(attrs.get("WIDTH", "0"))
            img_entry["height"] = _safe_int(attrs.get("HEIGHT", "0"))
            if parent_div:
                img_entry["div_id"] = parent_div
            images.append(img_entry)

    return images


# ---------------------------------------------------------------------------
# Full Screen Parser
# ---------------------------------------------------------------------------

def parse_htm_file(filepath):
    """
    Parse a single Amulet HTM file into a structured screen definition.
    """
    basename = os.path.basename(filepath)
    screen_name = os.path.splitext(basename)[0]

    # Read file with Latin-1 encoding (Amulet uses ISO 8859-1)
    try:
        with open(filepath, "r", encoding="latin-1") as f:
            content = f.read()
    except (IOError, OSError) as e:
        print("  WARNING: Could not read {}: {}".format(filepath, e))
        return None

    # Parse CSS positions
    css_positions = parse_css_positions(content)

    # Parse memory map from comment header
    memory_map = parse_memory_map(content)

    # Parse Amulet directives
    directives = parse_amulet_directives(content)

    # Parse META REFRESH triggers
    refresh_triggers = parse_meta_refreshes(content)

    # Parse widgets
    widgets = parse_widgets(content, css_positions)

    # Parse static images
    static_images = parse_static_images(content)

    # Extract init actions from refresh triggers
    init_actions = parse_init_actions(refresh_triggers)

    # Separate init triggers from ongoing triggers
    ongoing_triggers = []
    for t in refresh_triggers:
        name = t.get("name", "")
        interval = t.get("interval", "")
        has_onvar = "onVar" in t

        is_init = "init" in name.lower() or (
            not has_onvar and interval and _is_immediate_interval(interval)
        )
        if not is_init:
            ongoing_triggers.append(t)

    # Collect all referenced images
    referenced_images = _collect_referenced_images(widgets, static_images)

    # Detect language variants
    has_french = any(w.get("language") == "french" for w in widgets)

    # Build screen definition
    screen = OrderedDict()
    screen["name"] = screen_name
    screen["filename"] = basename
    screen["resolution"] = {"width": 800, "height": 600}

    if directives:
        screen["directives"] = directives

    if memory_map and any(memory_map.values()):
        # Only include non-empty sections
        mm = OrderedDict()
        if memory_map["bytes"]:
            mm["bytes"] = {str(k): v for k, v in sorted(memory_map["bytes"].items())}
        if memory_map["words"]:
            mm["words"] = {str(k): v for k, v in sorted(memory_map["words"].items())}
        if memory_map["strings"]:
            mm["strings"] = {str(k): v for k, v in sorted(memory_map["strings"].items())}
        if mm:
            screen["memoryMap"] = mm

    screen["widgets"] = widgets

    if static_images:
        screen["staticImages"] = static_images

    if ongoing_triggers:
        screen["refreshTriggers"] = ongoing_triggers

    if init_actions:
        screen["initActions"] = init_actions

    if referenced_images:
        screen["referencedImages"] = sorted(set(referenced_images))

    if has_french:
        screen["hasFrenchVariants"] = True

    # Collect all RAM indices used
    ram_usage = _collect_ram_usage(widgets, refresh_triggers)
    if ram_usage:
        screen["ramUsage"] = ram_usage

    return screen


def _collect_referenced_images(widgets, static_images):
    """Collect all image file paths referenced by widgets and static images."""
    images = []

    for w in widgets:
        params = w.get("params", {})
        for key in ("upImage", "downImage", "src", "SRC"):
            if key in params:
                images.append(params[key])

    for img in static_images:
        if "src" in img:
            images.append(img["src"])

    return images


def _collect_ram_usage(widgets, triggers):
    """
    Collect all internalRAM and UART indices referenced by the screen.
    Useful for the emulator to know which RAM locations this screen reads/writes.
    """
    usage = {
        "internalRAM": {"bytes": [], "words": [], "strings": [], "labels": []},
        "UART": {"bytes": [], "words": []},
    }

    seen = set()

    def _add_binding(source, var_type, index):
        key = (source, var_type, index)
        if key in seen:
            return
        seen.add(key)

        if source == "internalRAM":
            type_key = var_type + "s" if not var_type.endswith("s") else var_type
            if type_key in usage["internalRAM"]:
                usage["internalRAM"][type_key].append(index)
        elif source == "UART":
            type_key = var_type + "s" if not var_type.endswith("s") else var_type
            if type_key in usage["UART"]:
                usage["UART"][type_key].append(index)

    # From widget bindings
    for w in widgets:
        bindings = w.get("bindings")
        if bindings:
            for b in bindings:
                _add_binding(b["source"], b["type"], b["index"])

    # From triggers
    all_text = json.dumps(triggers)
    for m in RE_IRAM_BINDING.finditer(all_text):
        _add_binding("internalRAM", m.group(1).lower(), int(m.group(2)))
    for m in RE_UART_BINDING.finditer(all_text):
        _add_binding("UART", m.group(1).lower(), int(m.group(2)))

    # Sort all lists
    for source in usage.values():
        for key in source:
            source[key] = sorted(set(source[key]))

    # Remove empty entries
    clean_usage = OrderedDict()
    for source_name, source_data in usage.items():
        clean_source = OrderedDict()
        for type_name, indices in source_data.items():
            if indices:
                clean_source[type_name] = indices
        if clean_source:
            clean_usage[source_name] = clean_source

    return clean_usage if clean_usage else None


# ---------------------------------------------------------------------------
# Utility Functions
# ---------------------------------------------------------------------------

def _strip_html_comments(content):
    """
    Remove HTML comments from content, but handle the edge case of
    nested or multiline comments carefully.

    Also handles the <!--DIV... --> pattern where entire DIV blocks are
    commented out.
    """
    return re.sub(r"<!--.*?-->", "", content, flags=re.DOTALL)


def _collapse_whitespace(content):
    """Replace runs of whitespace (including newlines) with a single space."""
    return re.sub(r"\s+", " ", content)


def _safe_int(val):
    """Safely convert a string to int, returning 0 on failure."""
    try:
        return int(val)
    except (ValueError, TypeError):
        return 0


# ---------------------------------------------------------------------------
# Asset Copier
# ---------------------------------------------------------------------------

def copy_assets(src_dir, dest_dir):
    """
    Copy all image assets from the HTM source Images/ directory
    to the frontend assets directory.
    """
    images_src = os.path.join(src_dir, "Images")
    if not os.path.isdir(images_src):
        print("WARNING: Images directory not found at {}".format(images_src))
        return 0

    os.makedirs(dest_dir, exist_ok=True)

    count = 0
    for filename in sorted(os.listdir(images_src)):
        src_path = os.path.join(images_src, filename)
        if os.path.isfile(src_path):
            dest_path = os.path.join(dest_dir, filename)
            shutil.copy2(src_path, dest_path)
            count += 1

    return count


# ---------------------------------------------------------------------------
# Screen Index Generator
# ---------------------------------------------------------------------------

def generate_screen_index(screen_defs, output_dir):
    """
    Generate an index.json that lists all parsed screens with summary info.
    """
    index = OrderedDict()
    index["totalScreens"] = len(screen_defs)
    index["screens"] = []

    for screen in sorted(screen_defs, key=lambda s: s["name"]):
        entry = OrderedDict()
        entry["name"] = screen["name"]
        entry["filename"] = screen["filename"]
        entry["widgetCount"] = len(screen.get("widgets", []))
        entry["triggerCount"] = len(screen.get("refreshTriggers", []))
        entry["hasFrenchVariants"] = screen.get("hasFrenchVariants", False)

        # List widget types present
        types = set()
        for w in screen.get("widgets", []):
            types.add(w.get("type", "unknown"))
        entry["widgetTypes"] = sorted(types)

        index["screens"].append(entry)

    index_path = os.path.join(output_dir, "index.json")
    with open(index_path, "w", encoding="utf-8") as f:
        json.dump(index, f, indent=2, ensure_ascii=False)

    return index_path


# ---------------------------------------------------------------------------
# Main Entry Point
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Parse Amulet HTM screen files into JSON definitions for the CRC-25R emulator."
    )
    parser.add_argument(
        "--src",
        default=os.path.join(os.path.dirname(__file__), "..", "..", "firmware", "CRCHtml_3.00a"),
        help="HTM source directory (default: ../../firmware/CRCHtml_3.00a/)",
    )
    parser.add_argument(
        "--out",
        default=os.path.join(os.path.dirname(__file__), "screen_defs"),
        help="JSON output directory (default: ./screen_defs/)",
    )
    parser.add_argument(
        "--copy-assets",
        action="store_true",
        help="Copy image assets to frontend assets directory",
    )
    parser.add_argument(
        "--assets-dest",
        default=os.path.join(os.path.dirname(__file__), "..", "..", "frontend", "public", "assets", "images"),
        help="Destination for image assets (default: ../../frontend/public/assets/images/)",
    )

    args = parser.parse_args()

    src_dir = os.path.abspath(args.src)
    out_dir = os.path.abspath(args.out)
    assets_dest = os.path.abspath(args.assets_dest)

    # Validate source directory
    if not os.path.isdir(src_dir):
        print("ERROR: Source directory not found: {}".format(src_dir))
        sys.exit(1)

    # Create output directory
    os.makedirs(out_dir, exist_ok=True)

    # Find all HTM files
    htm_files = sorted(
        [f for f in os.listdir(src_dir) if f.lower().endswith(".htm")],
        key=str.lower,
    )

    if not htm_files:
        print("ERROR: No .htm files found in {}".format(src_dir))
        sys.exit(1)

    print("=" * 70)
    print("Amulet HTM Screen Parser for CRC-25R Emulator")
    print("=" * 70)
    print("Source:  {}".format(src_dir))
    print("Output:  {}".format(out_dir))
    print("Files:   {} .htm files found".format(len(htm_files)))
    print("-" * 70)

    # Parse each file
    screen_defs = []
    total_widgets = 0
    total_triggers = 0
    errors = 0
    french_screens = 0

    for filename in htm_files:
        filepath = os.path.join(src_dir, filename)
        screen_name = os.path.splitext(filename)[0]

        try:
            screen = parse_htm_file(filepath)
        except Exception as e:
            print("  ERROR parsing {}: {}".format(filename, e))
            errors += 1
            continue

        if screen is None:
            errors += 1
            continue

        # Write JSON output
        json_filename = screen_name + ".json"
        json_path = os.path.join(out_dir, json_filename)
        with open(json_path, "w", encoding="utf-8") as f:
            json.dump(screen, f, indent=2, ensure_ascii=False)

        widget_count = len(screen.get("widgets", []))
        trigger_count = len(screen.get("refreshTriggers", []))
        total_widgets += widget_count
        total_triggers += trigger_count

        has_french = screen.get("hasFrenchVariants", False)
        if has_french:
            french_screens += 1

        lang_marker = " [FR]" if has_french else ""
        print("  {:40s} {:3d} widgets, {:3d} triggers{}".format(
            filename, widget_count, trigger_count, lang_marker,
        ))

        screen_defs.append(screen)

    # Generate screen index
    index_path = generate_screen_index(screen_defs, out_dir)

    print("-" * 70)
    print("Parsed:   {} screens".format(len(screen_defs)))
    print("Widgets:  {} total".format(total_widgets))
    print("Triggers: {} total".format(total_triggers))
    print("French:   {} screens with French variants".format(french_screens))
    if errors:
        print("Errors:   {}".format(errors))
    print("Index:    {}".format(index_path))
    print("Output:   {}".format(out_dir))

    # Copy assets if requested
    if args.copy_assets:
        print("-" * 70)
        print("Copying image assets...")
        count = copy_assets(src_dir, assets_dest)
        print("Copied {} files to {}".format(count, assets_dest))

    print("=" * 70)
    print("Done.")

    return 0 if errors == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
